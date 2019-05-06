#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <microhttpd.h>

#include <locale.h>
#include <libintl.h>

#include <Eina.h>

#include "parser.h"
#include "util.h"
#include "pcre_replace.h"
#include "wikidump_sqlite.h"
#include "wikidump_server_common.h"

#define PORT 8888

Eina_List *wikimedias = NULL;
Wiki_Parser_Data *commons = NULL;

static int
http_send_page(struct MHD_Connection *connection, const char *url, 
      const char *page, size_t size, int raw, int status_code)
{
   int ret;
   struct MHD_Response *response;
   const char html[] = "text/html; charset=UTF-8";
   const char *type;

   if(! page)
      return MHD_NO;

   type = html;
   response = MHD_create_response_from_data(size, (void *) page, 
         MHD_YES, MHD_NO);
   // response = MHD_create_response_from_buffer(size, (void *) page, 
   //      MHD_RESPMEM_MUST_FREE);
   if(! response)
      return MHD_NO;

   if(url && raw == WIKI_OUTPUT_RAW) {
      char *p = rindex(url, '.');
      if(p) {
         if(strncmp(p, ".css", 4) == 0) {
            type = "text/css; charset=UTF-8";
         } else if(strncmp(p, ".txt", 4) == 0) {
            type = "text/plain; charset=UTF-8";
         } else if(strncmp(p, ".js", 3) == 0) {
            type = "application/javascript; charset=UTF-8";
         } else if(strncmp(p, ".ogg", 4) == 0) {
            type = "application/ogg; charset=UTF-8";
         } else if(strncmp(p, ".gif", 4) == 0) {
            type = "Content-type", "image/gif";
         } else if(strncmp(p, ".png", 4) == 0) {
            type = "image/png";
         } else if(strncmp(p, ".svg", 4) == 0) {
            type = "image/svg";
         } else if(strncmp(p, ".jpeg", 5) == 0) {
            type = "image/jpeg";
         } else if(raw == WIKI_OUTPUT_XML)
            type = "application/xml; charset=utf-8";
      } else {
         if(raw == WIKI_OUTPUT_PAGE_DUMP && strncmp(page, "<?xml ", 6) == 0)
            type = "application/xml; charset=utf-8";
         else if(raw == WIKI_OUTPUT_XML && strncmp(page, "<?xml ", 6) == 0)
            type = "application/xhtml+xml; charset=utf-8";
      }
   } else if(raw != WIKI_OUTPUT_DEFAULT)
      type = "application/xml; charset=utf-8";

   MHD_add_response_header(response, "Content-type", type);

   ret = MHD_queue_response(connection, status_code, response);
   MHD_destroy_response (response);

   return ret;
}

static int
http_page_static_send(struct MHD_Connection *connection, const char *url)
{
   char *path = NULL;
   static char *dir = NULL;
   char *page = NULL;
   int ret = MHD_NO;
   size_t size = 0;
   
   if(dir == NULL) {
      dir = getenv("WIKIDUMP_STATIC_DATA");
   }

   if(dir == NULL)
      return ret;

   if(url) {
      UChar *tmp = u_strdupC(url);
      if(tmp) {
        tmp = icu_replace_utf8("^/static/", "", tmp, 0);
        tmp = icu_replace_utf8("\\.\\.", "", tmp, 0);
        if(tmp) {
           path = u_strToUTF8_new(tmp);
           free(tmp);
        }
      }
   }

   if(path) {
      char *file = NULL;
      asprintf(&file, "%s/%s", dir, path);
      if(file) {
         struct stat s;
         stat(file, &s);
         if(S_ISREG(s.st_mode)) {
            FILE *f = fopen(file, "r");
            if(f) {
               page = calloc(s.st_size + 1, sizeof(char));
               if(page) {
                  size = fread(page, sizeof(char), s.st_size, f);
               }
               fclose(f);
            }
         }
         free(file);
      }
      free(path);
   }


   if(page)
      ret = http_send_page(connection, url, page, size, 1, MHD_HTTP_OK);

   return ret;
}

static void
http_response_completed(void *cls, struct MHD_Connection *connection,
      void **con_cls, enum MHD_RequestTerminationCode toe)
{
   Wiki_Parser_Data *wpd = (Wiki_Parser_Data *) *con_cls;

   if(wpd == NULL)
      return;

   wiki_parser_data_free(wpd);
   *con_cls = NULL;
}

http_answer_reponse(void *cls, struct MHD_Connection *connection, 
      const char *url, const char *method, const char *version,
      const char *upload_data, size_t *upload_size, void **data)
{
   int ret = MHD_NO;
   Wiki_Parser_Data *wpd = NULL;
   Wiki_Sqlite *ws = cls;
   struct MHD_Response *response = NULL;
   char *page = NULL;
   const char *wikimedia = NULL, *lang = NULL, *entry = NULL, *search = NULL;
   char *host = NULL;
   int raw = 0, status_code = 0;
   const union MHD_ConnectionInfo *info = NULL;
   int mode = WIKI_OUTPUT_XML;
   
   entry = getenv("WIKI_DEFAULT_MODE");
   if(entry) {
      mode = atoi(entry);
      entry = NULL;
   }
   if(mode < 0 || mode >= WIKI_OUTPUT_MODE_INVALID)
      mode = WIKI_OUTPUT_XML;


   info = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
   if(info) {
      struct sockaddr_in *addr = info->client_addr;
      if(addr)
	 asprintf(&host, "%s", inet_ntoa(addr->sin_addr));
   }

   if(url && strncmp("/static/", url, 8) == 0) {
      ret = http_page_static_send(connection, url);
      status_code = (ret == MHD_NO) ? MHD_HTTP_NOT_FOUND : MHD_HTTP_OK;
      printf("Request from %s: %s => %d\n", host, url, status_code);
      entry = url;
      // return ret;
      if(status_code == MHD_HTTP_OK) {
         if(host)
            free(host);
        return ret; 
      }
   } else {
      wikimedia = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "wikimedia");
      lang = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "lang");
      entry = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "raw");
      if(entry) 
         raw = atoi(entry);
      if(raw < 1 || raw >= WIKI_OUTPUT_MODE_INVALID)
         raw = mode;

      entry = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "entry");
      search = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "search");
   }

   if(wikimedia == NULL && wikimedias) {
      Wiki_Media *wm = eina_list_nth(wikimedias, 0);
      if(wm) {
         wikimedia = wm->sitename;
         lang = wm->lang;
      }
      
   }

   if(entry == NULL || strcmp(entry, "") == 0) {
      const char *title = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "title");
      entry = (title) ? title : "Main Page";
   }

   wpd = wiki_parser_data_init(ws);
   if(wpd) {
      Eina_List *l;
      Wiki_Media *wm;
      char *tmp = NULL;
      EINA_LIST_FOREACH(wikimedias, l, wm) {
         if(! wikimedia || strcmp(wikimedia, wm->sitename) == 0) {
            wpd->siteid = wm->siteid;
            wpd->sitename = wm->sitename;
            wpd->lang = (lang) ? lang : wm->lang;
            wpd->locale = wm->locale;
            wpd->localname = wm->localname;
            wpd->base = wm->base;
	    wpd->default_output_mode = mode;
            wpd->commons = commons;
            if(wm->case_used) {
               if(strcmp(wm->case_used, "first-letter") == 0)
                  wpd->wcase = WIKI_FIRST_LETTER;
               else if(strcmp(wm->case_used, "case-sensitive") == 0)
                  wpd->wcase = WIKI_SENSITIVE;
               else
                  wpd->wcase = WIKI_CASE_UNKNOW;
            }
            wpd->namespaces = wd_sqlite_namespace_load(wpd);
            wpd->output_mode = raw;

	    if(setlocale(LC_MESSAGES, lang) == NULL) {
	       if(setlocale(LC_MESSAGES, wm->locale) == NULL)
		  printf("can't set locale to %s or %s\n", lang, wm->locale);
	    }
	    if(setlocale(LC_TIME, lang) == NULL) {
	       if(setlocale(LC_TIME, wm->locale) == NULL)
		  printf("can't set locale to %s or %s\n", lang, wm->locale);
	    }
            break;
         }
      }
      if((tmp = getenv("WIKIDUMP_NO_PAGE_EXISTS")) != NULL) {
         if(strcmp(tmp, "1") == 0) {
            wpd->no_page_exists = EINA_TRUE;
         }
      } 
   }

   if(search) {
      char *tmp = NULL;
      UChar *content = u_strdupC("<div style=\"display:none;\">Search:</div>");
      wpd->search = search;
      asprintf(&tmp, "search: %s", search);
      wpd->pagename = (tmp) ? tmp : "search:";
      printf("Request from %s: ?wikimedia=%s&lang=%s&entry=%s&raw=%d&search=%s",
	    host, wikimedia, lang, entry, raw, search);
      if(content) {
         page = output_content(wpd, content, raw, 0);
         free(content);
      }
   } else if (entry && strcmp(entry, "mwparser") == 0) {
      FILE *f = fopen("/tmp/wiki", "r");
      if(f) {
	 UChar *content = NULL;
	 char *buf = NULL;
	 size_t s = 0;
	 fseek(f, 0L, SEEK_END);
	 s = ftell(f);
	 fseek(f, 0L, SEEK_SET);
	 buf = calloc(s+1, sizeof(char));
	 if(buf) {
	    fread(buf, s, sizeof(char), f);
	    fclose(f);
	    content = u_strdupC(buf);
	    if(content) {
	       if(! wpd->pagename) 
		  wpd->pagename = entry;
	       page = output_content(wpd, content, raw, 0);
	       free(content);
	    }
	    free(buf);
	 }
      }
   } else {
      printf("Request from %s: ?wikimedia=%s&lang=%s&entry=%s&raw=%d",
	    host, wikimedia, lang, entry, raw);
      page = output_page(wpd, entry, raw, 0);
   }
   if(! page) {
      UChar *content = u_strdupC(WIKI_PAGE_404);
      if(content) {
         if(! wpd->pagename) 
            wpd->pagename = entry;
         page = output_content(wpd, content, raw, 0);
         free(content);
      }
      status_code = MHD_HTTP_NOT_FOUND;
   } else {
      status_code = MHD_HTTP_OK;
      if(raw == 1 && strstr(entry, ".css")) {
	 char *r = NULL;
	 asprintf(&r, "@import url(\"?wikimedia=%s&lang=%s&raw=1&entry=", wikimedia, lang);
	 if(r) {
	    page = pcre_replace("/@import url\\(\".*\\?title=/", r, page);
	    page = pcre_replace("/http:\\/\\//", "/static/", page);
	    free(r);
	 }
      }
   }
   printf(" => %d\n", status_code);
   if(page) {
      ret = http_send_page(connection, entry, page, strlen(page), raw, status_code);
   }
   
   if(host)
      free(host);

   wiki_parser_data_free(wpd);

   uregex_cache_free();

   return ret;
}

int 
main(int argc, char *argv[])
{
   Wiki_Sqlite *ws = NULL;
   struct MHD_Daemon *daemon = NULL;

   eina_init();
   eina_threads_init();
   icu_util_init();
   wiki_parser_init();

   setlocale(LC_ALL, "");
   setlocale(LC_NUMERIC, "C");
   bindtextdomain("libmwparser", "/home/fabrice/tmp/wikiparser/data/locales");
   textdomain("libmwparser");
   bind_textdomain_codeset("libmwparser", "UTF-8");

   ws = wd_sqlite_open_new();
   if(ws) {
      Wiki_Parser_Data *wpd = wiki_parser_data_init(ws);
      if(wpd) {
         wikimedias = wd_sqlite_wikimedia_list_get(wpd);
	 if(wikimedias) {
	    Eina_List *l;
	    Wiki_Media *wm;
	    EINA_LIST_FOREACH(wikimedias, l, wm) {
	       if(strcmp("commons.wikimedia.org", wm->sitename) == 0) {
		  wpd->siteid = wm->siteid;
		  wpd->sitename = wm->sitename;
		  wpd->lang = wm->lang;
		  wpd->locale = wm->locale;
		  wpd->localname = wm->localname;
		  wpd->base = wm->base;
		  wpd->default_output_mode = WIKI_OUTPUT_XML;
		  //wpd->used_case = wm->case_used;
		  if(wm->case_used) {
		     if(strcmp(wm->case_used, "first-letter") == 0)
			wpd->wcase = WIKI_FIRST_LETTER;
		     else if(strcmp(wm->case_used, "case-sensitive") == 0)
			wpd->wcase = WIKI_SENSITIVE;
		     else
			wpd->wcase = WIKI_CASE_UNKNOW;
		  }
		  wpd->namespaces = wd_sqlite_namespace_load(wpd);
		  wpd->output_mode = WIKI_OUTPUT_XML;
		  commons = wpd;

		  break;
	       }
	    }
	 }
	 if(! commons)
	    wiki_parser_data_free(wpd);
      }
      
      daemon = MHD_start_daemon(
	    MHD_USE_THREAD_PER_CONNECTION,
//	    MHD_USE_SELECT_INTERNALLY, 
	    PORT, 
            NULL, NULL, 
            &http_answer_reponse, ws, 
            MHD_OPTION_CONNECTION_LIMIT, 5,
            MHD_OPTION_NOTIFY_COMPLETED, http_response_completed, ws,
            MHD_OPTION_END);

      if(daemon) {
         getchar();

         MHD_stop_daemon(daemon);
      }

      if(commons)
	 wiki_parser_data_free(commons);
      wd_sqlite_free(ws);
   }

   wiki_parser_shutdown();
   icu_util_shutdown();
   eina_threads_shutdown();
   eina_shutdown();

   return 0;
}
