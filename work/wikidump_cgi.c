#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <sys/time.h>
#include <unicode/uregex.h>

#include <locale.h>
#include <libintl.h>

#include "parser.h"
#include "util.h"
#include "pcre_replace.h"

#include "wikidump_sqlite.h"
#include "wikidump_server_common.h"

static Eina_Bool is_test = EINA_FALSE;

static void test_all_pages_for_sitename(Wiki_Parser_Data *wd);
static int output_file(Wiki_Parser_Data *wd, const char *filename, int raw);

static int output_file(Wiki_Parser_Data *wd, const char *filename, int raw)
{
   FILE *f = NULL;
   int ret = EXIT_FAILURE;

   if(! wd) return ret;
   if(! filename) return ret;
      
   f = fopen(filename, "r");
   if(f) {
      size_t size = 0;
      char *buf = NULL;
      UChar *tmp = NULL;

      fseek(f, 0L, SEEK_END);
      size = ftell(f);
      fseek(f, 0L, SEEK_SET);

      buf = calloc(size+1, sizeof(char));
      if(buf) {
	 fread(buf, sizeof(char), size, f);
	 fclose(f);

	 tmp = u_strdupC(buf);
	 if(tmp) {
	    wd->pagename = filename;
	    ret = output_content(wd, tmp, raw, 0);
	    // free(tmp);
	 }
     	 free(buf);
      }
   }

   return ret;
}

static void test_all_pages_for_sitename2(Wiki_Parser_Data *wd, int level)
{
   Wiki_Sqlite *ws = NULL;
   char sql[] = "select title, uncompress(content) from wiki_pages p, wiki_siteinfo s where lang = %Q and sitename = %Q and siteid = s.id "; // and title like 'Modèle:%%' limit 2000";
   char *qry = NULL;
   int raw = 1;
   int i = 0;

   if(! wd) return;

   ws = wd->data;

   is_test = EINA_TRUE; // no content output
   qry = sqlite3_mprintf(sql, wd->lang, wd->sitename);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, strlen(qry) + 1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 while((r = sqlite3_step(stmt)) == SQLITE_ROW) {
	    const char *title = (const char *) sqlite3_column_text(stmt, 0);
	    const char *buf = sqlite3_column_text(stmt, 1);
	    char *content = NULL;
	    UChar *utf = NULL;
	    Wiki_Parser *wp = NULL;
	    printf("wikimedia=%s&lang=%s&entry=%s\n", wd->sitename, wd->lang, title);
	    if(level > 0 && buf) {
	       Wiki_Dump_Page *wdp = wikidump_page_parser_get(buf, strlen(buf));
	       if(wdp) {
		  if(level > 1) 
		     content = wikidump_page_content_get(wdp);;
		  wikidump_page_free(wdp);
	       } else {
		  printf("can't load content\n");
	       }
	    }
	    if(content) {
	       if(level > 2)
		  utf = u_strdupC(content);
	       free(content);
	    }
	    if(utf) {
	       int error = 0;
	       wd->pagename = title;
	       if(level > 3)
		  wp = wiki_parser_buf_new(wd, utf, u_strlen(utf), &error);
	       free(utf);
	    }
	    if(wp) {
	       int error = 0;
	       if(level > 4) 
		  wiki_parser_convert_xhtml(wp, &error, 0);
	       wiki_parser_free(wp);
	    }
	 }
	 sqlite3_finalize(stmt);
      }

      sqlite3_free(qry);
   }
   printf("release: %d\n", sqlite3_release_memory(sqlite3_memory_used()));
   printf("%ld, %ld\n", sqlite3_memory_used(), sqlite3_memory_highwater(0));
   printf("%ld, %ld\n", sqlite3_memory_used(), sqlite3_memory_highwater(1));
   printf("%ld, %ld\n", sqlite3_memory_used(), sqlite3_memory_highwater(0));
   getchar();
}

static void test_all_pages_for_sitename(Wiki_Parser_Data *wd)
{
   Wiki_Sqlite *ws = NULL;
   char sql[] = "select title from wiki_pages p, wiki_siteinfo s where lang = %Q and sitename = %Q and siteid = s.id limit 2000";
   char *qry = NULL;
   int raw = 1;
   int i = 0;

   if(! wd) return;

   ws = wd->data;

   is_test = EINA_TRUE; // no content output
   qry = sqlite3_mprintf(sql, wd->lang, wd->sitename);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, strlen(qry) + 1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 while((r = sqlite3_step(stmt)) == SQLITE_ROW) {
	    const char *title = (const char *) sqlite3_column_text(stmt, 0);
	    char *buf = NULL;
	    printf("wikimedia=%s&lang=%s&entry=%s\n", wd->sitename, wd->lang, title);
	    buf = output_page(wd, title, raw, 0);
	    if(buf)
	       free(buf);
	    uregex_cache_free();
	    i++;
//	    if(i > 25) break;
	 }
	 sqlite3_finalize(stmt);
      }

      sqlite3_free(qry);
   }
}

extern char **environ;

static Eina_List *http_query_string_get(void)
{
   Eina_List *l = NULL;
   const char *env = getenv("QUERY_STRING");

   if(env) {
      char ** split = eina_str_split(env, "&", -1);
      if(split) {
	 int i = 0;
	 while(split[i]) {
	    Http_Qry_Arg *arg = calloc(1, sizeof(Http_Qry_Arg));
	    if(arg) {
	       char *p = index(split[i], '=');
	       if(p) {
		  *p = '\0';
		  arg->name = strdup(split[i]);
		  arg->value = strdup(p + 1);
	       } else {
		  arg->name = strdup(split[i]);
	       }
	       l = eina_list_append(l, arg);
	    }
	    i++;
	 }
	 if(split[0]) free(split[0]);
	 free(split);
      }
   }

   return l;
}

int main(int argc, char **argv)
{
   Wiki_Parser_Data *wd = NULL;
   char *wikimedia = NULL;
   char *entry = NULL;
   char *lang = NULL;
   int   raw = 0;
   char *entry_uns = NULL;
   Eina_List *qry = NULL, *l = NULL;
   Http_Qry_Arg *qry_arg = NULL;
   int send = 0, cgi = 0;
   char *page = NULL;

   eina_init();

   wiki_parser_init();
   icu_util_init();

   setlocale(LC_ALL, "C");
   setlocale(LC_NUMERIC, "C");

   bindtextdomain("libmwparser", "/home/fabrice/tmp/wikiparser/data/locales");
   textdomain("libmwparser");
   bind_textdomain_codeset("libmwparser", "UTF-8");

   qry = http_query_string_get();
   if(qry) {
      EINA_LIST_FOREACH(qry, l, qry_arg) {
	 ulog("CGI DATA: %s = %s\n", qry_arg->name, qry_arg->value);
	 if(qry_arg->name) {
	    if(strcmp(qry_arg->name, "wikimedia") == 0 && qry_arg->value) {
	       wikimedia = strdup(qry_arg->value);
	    } else if(strcmp(qry_arg->name, "lang") == 0 && qry_arg->value) {
	       lang = strdup(qry_arg->value);
	    } else if(strcmp(qry_arg->name, "entry") == 0 && qry_arg->value) {
	       entry = strdup(qry_arg->value);
	    } else if(strcmp(qry_arg->name, "raw") == 0 && qry_arg->value) {
	       raw = atoi(qry_arg->value);
	    }
	 }
      }	 
      EINA_LIST_FREE(qry, qry_arg) {
	 if(qry_arg) {
	    if(qry_arg->name) free(qry_arg->name);
	    if(qry_arg->value) free(qry_arg->value);
	    free(qry_arg);
	 }
      }
   }

   if(! wikimedia)
      wikimedia = strdup("fr.wikipedia.org");
   if(wikimedia) {
      char *tmp = url_desencodeC(wikimedia);
      if(tmp) {
	 free(wikimedia);
	 wikimedia = tmp;
      }
   }

   if(! lang) {
      lang = strdup("fr");
   }

   Wiki_Sqlite *ws = wd_sqlite_sitename_lang_new(wikimedia, lang);
   if(ws) {
      wd = wiki_parser_data_init(ws);
      wd->namespaces = wd_sqlite_namespace_load(wd);
      if(setlocale(LC_MESSAGES, wd->lang) == NULL) {
	 if(setlocale(LC_MESSAGES, wd->locale) == NULL)
	    printf("can't set locale to %s or %s\n", lang, wd->locale);
      }
   } else {
      http_send_headers(CONTENT_TYPE_PLAIN);
      printf("<body>%d:Wikimedia '%s:%s' unknown, Sorry.</body>\n", 0, wikimedia, lang);
      goto FINISHDB;
   }

   if(entry && strlen(entry) > 0) {
      entry_uns = url_desencodeC(entry);
   } else
      entry_uns = strdup("Main Page");

   if(getenv("WIKI_DEBUG") && getenv("WIKI_TEST_PAGES")) {
      int level = 0;
      char *var = getenv("WIKI_TEST_PAGES");
      if(var)
	 level = atoi(var);
      test_all_pages_for_sitename2(wd, level);
   } else if(argc == 2) {
      page = output_file(wd, argv[1], raw);
   } else if(entry_uns) {
      page = output_page(wd, entry_uns, raw, 0);
   }

   if(page == NULL) {
      printf("<body>[%s] not found.<br /></body>\n", entry);
      printf("%s, %s, [%s]\n", wikimedia, lang, entry_uns);
   } else {
      printf("%s\n", page);
      free(page);
   }

   wiki_parser_data_free(wd);
FINISHDB:
   wd_sqlite_free(ws);
   getchar();
   wiki_lexer_tags_free();

   if(wikimedia)
      free(wikimedia);
   if(lang)
      free(lang);
   if(entry_uns)
      free(entry_uns);
   if(entry)
      free(entry);


   printf("\n");

   wiki_parser_shutdown();
   icu_util_shutdown();

   eina_shutdown();
   getchar();

   return 0;
}
