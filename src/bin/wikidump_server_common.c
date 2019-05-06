#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <sys/time.h>
#include <unicode/uregex.h>
#include <unicode/utypes.h>

#include <Eina.h>

#include "util.h"
#include "parser.h"
#include "pcre_replace.h"

#include "wikidump_sqlite.h"
#include "wikidump_server_common.h"
#include "wikipedia_ui.h"

char *
wikidump_image_url_get(const Wiki_Parser_Data *wd, const char *title);
static char *
wikidump_server_url_get(Wiki_Parser_Data *wd, const char *title, const char *sitename, const char *lang);
// static void http_send_headers(int content_type);

Eina_Bool
wikidump_page_exists(const Wiki_Parser_Data *wd, const char *title)
{
   const Wiki_Parser_Data *commons = NULL;
   Wiki_Sqlite *ws = NULL;
   Eina_Bool ret = EINA_FALSE;

   if(! wd) return ret;
   if(! title) return ret;

   ret = wd_sqlite_page_exists(wd, title);

   if(ret == EINA_FALSE && wd->commons) {
      char *ns = NULL;
      const char *colon = strstr(title, ":");
      commons = wd->commons;
      if(colon) 
	 ns = strndup(title, colon - title);
      if(ns) {
	 const Wiki_Namespace *wn = eina_hash_find(wd->namespaces, ns);
	 if(wn && (wn->key == WIKI_NS_FILE || wn->key == WIKI_NS_MEDIA)) {
	    const char *nsfile = wikidump_namespace_name_get(commons->namespaces, WIKI_NS_FILE);
	    char *t = NULL;
	    if(nsfile)
	       asprintf(&t, "%s%s", nsfile, colon);
	    if(t) {
	       ret = wd_sqlite_page_exists(commons, t);
	       free(t);
	    }
	 }
	 free(ns);
      }
   }

   return ret;
}

UChar *
wikidump_image_to_desc_add(Wiki_Parser_Data *wd, const char *file, UChar *content)
{
   const Wiki_Parser_Data *commons = NULL;
   Wiki_Sqlite *ws = NULL;

   Wiki_File *wf = NULL;
   int w = 0, h = 0;
   UChar *tmp = NULL;
   char *url = NULL, *text = NULL;
   const char *img ="\
{{#ifeq:%d|%d\n\
  |[[%s|link=%s]]\n\
  |{{#ifeq:%d|%d\n\
     |[[%s|link=%s]]\n\
     |[[%s|link=%s|%dx%dpx]]\n\
   }}\n\
}}\n\
\n\
{{#ifexpr: %d != %d\n\
  | Size of this preview: %d x %d pixels\n\
  | No higher resolution available.\n\
}}<br />\n\
[%s Full resolution] (%d x %d pixels, file size: %d, MIME type: %s)\n\
\n\
\n";

   if(! wd) return content;
   if(! file) return content;
   if(! content) return content;

   url = wikidump_image_url_get(wd, file);
   if(! url)
      return content;

   ws = (Wiki_Sqlite *) wd->data;
   commons = wd->commons;
   if(commons) {
      if(commons->siteid) {
	 char *f = NULL;
	 const char *ns = wikidump_namespace_name_get(commons->namespaces, WIKI_NS_FILE);
	 if(ns) {
	    const char *colon = strstr(file, ":");
	    if(colon) {
	       asprintf(&f, "%s%s", ns, colon);
	    }
	 }
	 if(f) {
	    if(wd->page_file_info_get) 
	       wf = wd->page_file_info_get(commons, f);
	    free(f);
	 }
      }
   }

   if(! wf && wd->page_file_info_get) {
      wf = wd->page_file_info_get(wd, file);
   }

   if(wf) {
      if(wf) {
	 if(wf->width > wf->height && wf->width > 800) {
	    w = 800;
     	    h = wf->height / ((double) wf->width / 800.0);
	 } else if(wf->width < wf->height && wf->height > 600) {
	    w = wf->width / ((double) wf->height / 600.0);
	    h = 600;
	 } else { 
	    w = wf->width;
	    h = wf->height;
	 }
      }
      asprintf(&text, img, 
     	    w, wf->width, file, url,
	    h, wf->height, file, url,
	    file, url, w, h,
	    w, wf->width,
	    w, h,
	    url, wf->width, wf->height, wf->size, wf->mime);
      wiki_parser_file_free(wf);
   }

   if(! text) {
      asprintf(&text, "[[%s|link=%s]]\n\n", file, url);
   }

   if(text) {
      tmp = calloc(strlen(text) + u_strlen(content) + 10, sizeof(UChar));
      if(tmp) {
	 u_sprintf(tmp, "%s\n%S", text, content);
	 free(content);
	 content = tmp;
      }
      free(text);
   }

   free(url);
   return content;
}

Wiki_File *
wikidump_image_info_get(const Wiki_Parser_Data *wd, const char *title)
{
   const Wiki_Parser_Data *commons = NULL;
   Wiki_Dump_Page *wdp = NULL;
   Wiki_Sqlite *ws = NULL;
   Wiki_File *wf = NULL;
   char *file = NULL, *text = NULL;
   const char *ns = NULL;

   if(! wd) return wf;
   if(! title) return wf;

   ws = (Wiki_Sqlite *) wd->data;
   commons = wd->commons;

   if(commons) {
      if(commons->siteid) {
	 ns = wikidump_namespace_name_get(commons->namespaces, WIKI_NS_FILE);
	 if(ns) {
	    const char *colon = strstr(title, ":");
	    if(colon) {
	       asprintf(&file, "%s%s", ns, colon);
	    }
	 }
	 if(file && (wdp = wd_sqlite_page_load(commons, file)) != NULL) {
	    text = wikidump_page_content_get(wdp);
	 } else if(file) {
	    free(file);
	    file = NULL;
	    ns = NULL;
	 }
	 if(wdp)
	    wdp = wikidump_page_free(wdp);
      }
   }

   /*	set base to commons */
   /* If not exists, load page content from current sitename */
   /*	set base to sitename */
   if(! file) {
      file = strdup(title);
      if(file && (wdp = wd_sqlite_page_load(wd, file)) != NULL) {
	 text = wikidump_page_content_get(wdp);
      }
      if(wdp)
	 wdp = wikidump_page_free(wdp);
   }
   
   /* If image is found: check for redirect */
   /* if redirect check if page exist in current wd */
   if(text) {
      if(strcasestr(text, "redirect")) {
	 ulog("REDIRECT: %s\n", text);
	 text = pcre_replace("/[^\\[]*\\[\\[([^\\]|\\[]*).*/", "\\1", text);
	 text = pcre_replace("/\n.*/", "", text);
	 if(ns)
	    text = pcre_replace("/^[^:]*/", ns, text);
	 ulog("REDIRECT %s TO %s\n", file, text);
	 free(file);
	 file = text;
	 text = NULL;
      } else
	 free(text);
   } else {
      free(file);
      file = NULL;
   }

   if(ns)
      wf = wd_sqlite_image_info_get(commons, file);
   else
      wf = wd_sqlite_image_info_get(wd, file);

   if(! wf) {
      ulog("Wiki_File not found for %s\n", file);
   }

   if(file) 
      free(file);

   return wf;
}

char *
wikidump_image_url_get(const Wiki_Parser_Data *wd, const char *title)
{
   const Wiki_Parser_Data *commons = NULL;
   Wiki_Dump_Page *wdp = NULL;
   Wiki_Sqlite *ws = NULL;
   char *url = NULL, *hash = NULL, *tmp = NULL, *file = NULL;
   char *text = NULL, *base = NULL, *colon = NULL;

   if(getenv("WIKIDUMP_NO_IMAGE")) {
      url = strdup("/static/404.png");
      return url;
   }

   if(! wd) return url;
   if(! title) return url;

   ws = (Wiki_Sqlite *) wd->data;
   commons = wd->commons;

   /* Most images are in commons so load page content from commons :
    *	need to change wd->siteid and wd->sitename */
   if(! file && commons) {
      if(commons->siteid) {
	 const char *ns = wikidump_namespace_name_get(commons->namespaces, WIKI_NS_FILE);
	 if(ns) {
	    const char *colon = strstr(title, ":");
	    if(colon) {
	       asprintf(&file, "%s%s", ns, colon);
	    }
	 }
	 if(file && (wdp = wd_sqlite_page_load(commons, file)) != NULL) {
	    text = wikidump_page_content_get(wdp);
	    base = strdup("wikipedia/commons");
	 } else if(file) {
	    free(file);
	    file = NULL;
	 }
	 if(wdp) {
	    wdp = wikidump_page_free(wdp);
	 }
      }
   }

   /*	set base to commons */
   /* If not exists, load page content from current sitename */
   /*	set base to sitename */
   if(! file) {
      file = strdup(title);
      if(file && (wdp = wd_sqlite_page_load(wd, file)) != NULL) {
	 text = wikidump_page_content_get(wdp);
	 tmp = strdup(wd->sitename);
	 if(tmp) {
	    tmp = pcre_replace("/.*\\.([^.]*)\\..*/", "\\1", tmp);
	    if(tmp) {
	       asprintf(&base, "%s/%s", tmp, wd->lang);
	       free(tmp);
	       tmp = NULL;
	    }
	 }
      }
      if(! base && file) {
	 free(file);
	 file = NULL;
      }
      if(wdp) {
	 wdp = wikidump_page_free(wdp);
      }
   }
   
   /* If image is found: check for redirect */
   /* if redirect check if page exist in current wd */
   if(text) {
      if(strcasestr(text, "redirect")) {
	 ulog("REDIRECT: %s\n", text);
	 text = pcre_replace("/[^\\[]*\\[\\[([^\\]|\\[]*).*/", "\\1", text);
	 text = pcre_replace("/\n.*/", "", text);
	 ulog("REDIRECT TO: %s\n", text);
	 free(file);
	 file = text;
	 text = NULL;
      } else
	 free(text);
   } else {
      free(file);
      file = NULL;
   }

   if(file) {
      colon = strstr(file, ":");
      if(colon)
	 tmp = strdup(colon + 1);
      else
	 tmp = strdup(file);
   }
   if(tmp) {
      tmp = pcre_replace("/^ +/", "", tmp);
      tmp = pcre_replace("/ /", "_", tmp);
      hash = get_hash_path(tmp, 2);
      free(tmp);
      tmp = NULL;
   }

   if(file)
      colon = strstr(file, ":");
   if(colon)
      tmp = url_encodeC(colon + 1);
   if(tmp) {
      size_t size = strlen(tmp);
      if(wd->image_url)
	 size += strlen(wd->image_url);
      if(base)
	 size += strlen(base);
      if(hash)
	 size += strlen(hash);
      if(size > 0)
	 url = calloc(size + 10, sizeof(char));
      if(url) {
	 if(wd->image_url) {
	    strcpy(url, wd->image_url);
	    strcat(url, "/");
	 }
	 if(base) {
	    strcat(url, base);
	    strcat(url, "/");
	 }
	 if(hash) {
	    strcat(url, hash);
	    strcat(url, "/");
	 }
	 strcat(url, tmp);
      }
      free(tmp);
   }
   
   if(url) {
      url = pcre_replace("/( |\\+)/", "_", url);
      url = pcre_replace("/%20/", "_", url);
      url = pcre_replace("/%2E/", ".", url);
   } else {
      ulog("IMGAGE NOT FOUND => %s\n", title);
   }
//   printf("Image : %s\nurl: %s\n", file, url);

   if(hash)
      free(hash);
   if(base)
      free(base);
   if(file)
      free(file);

   if(! url) {
      url = strdup("/static/404.png");
   }

   return url;
}

void http_send_headers(int content_type)
{
   const char *type = NULL;
   switch(content_type) {
      case CONTENT_TYPE_CSS:
	 type = "text/css";
	 break;
      case CONTENT_TYPE_JS:
	 type = "application/javascript";
	 break;
      case CONTENT_TYPE_PLAIN:
	 type = "text/plain";
	 break;
      case CONTENT_TYPE_XML:
	 type = "application/xml";
	 break;
      default:
	 type = "text/html";
   }

   printf("Content-type: %s; charset=UTF-8\r\n", type);
   printf("\r\n");
}

static char *
wikidump_server_url_get(Wiki_Parser_Data *wd, const char *title, const char *sitename, const char *lang)
{
   const char *t = title;
   const char *site = sitename;
   char *url = NULL;
   if(! wd) return url;
   
   if(! title)
      t = "";
   if(! sitename)
      site = wd->sitename;
   if(lang) {
      asprintf(&url, "/%s?wikimedia=%s&lang=%s&entry=%s", wd->scriptpath,
	    site, lang, title);
   } else {
      asprintf(&url, "/%s?wikimedia=%s&entry=%s", wd->scriptpath,
	    site, title);
   }

   return url;
}

/*
 * Title redirect can contains unicode direction mark. See :
 * http://www.mediawiki.org/w/index.php?title=AMP_package&oldid=304694&action=raw
 */
static UChar *title_cleanup(UChar *title)
{
   UChar *tmp = NULL;

   if(! title) return NULL;
   tmp = u_strdup(title);
   if(tmp) {
      UChar *p = title;
      int pos = 0;
      while(*p) {
	 if(u_isprint(*p)) {
	    tmp[pos] = *p;
	    pos++;
	 }
	 p++;
      }

      tmp[pos] = '\0';

      free(title);
      title = tmp;
   }

   return title;
}

char *output_content(Wiki_Parser_Data *wd, UChar *content, int raw, int redirect)
{
   Wiki_Parser *parser = NULL;
   char *html = NULL;
   int error = 0;

   if(! content)
      return html;

   if(raw == 1)
      html = u_strToUTF8_new(content);
   else {
      parser = wiki_parser_buf_new(wd, content, u_strlen(content), &error);
      if(! parser)
	 return html;

      if(raw == 2)
	 html = wiki_parser_html_page_source_get(parser);
      else if(raw == 3)
	 html = wiki_parser_html_page_template_included_get(parser);
      else if(raw == WIKI_OUTPUT_PARSED_ROOT)
	 html = wiki_parser_parsed_root_get(parser);
      else {
	 /* TODO: redirect interwiki will fail */
	 if(! redirect && icu_search("^([ \\n]*)#redirect", parser->buf, UREGEX_CASE_INSENSITIVE)) {
	    UChar *tmp = trim_u(u_strdup(parser->buf));
	    tmp = icu_replace_utf8("^[^\\[]*\\[\\[([^\\]|\\[]*).*", "$1", tmp, 0);
	    tmp = icu_replace("\\n.*", ICU_STR_EMPTY, tmp, 0);
	    ulog("REDIRECT MAIN: ==%S==\n", tmp);
	    if(tmp) {
	       char *pname = u_strToUTF8_new(tmp);
	       wiki_parser_free(parser);
	       if(pname) {
		  html = output_page(wd, pname, raw, 1);
		  free(pname);
	       }
	       free(tmp);
	       return html;
	    }
	 }
         if(raw == 5)
            html = wiki_parser_xml_page_default_get(parser);
         else
            html = wiki_parser_html_page_default_get(parser);
      }

      wiki_parser_free(parser);

   }

   return html;
}

char *output_page(Wiki_Parser_Data *wd, const char *pagename, int raw, int redirect)
{
   Wiki_Dump_Page *wdp = NULL;
   Wiki_Sqlite *ws = NULL;
   char *pname = NULL, *text = NULL, *ret = NULL;
   char *re = NULL; 
   const char *ns = NULL;
   UChar *content = NULL;
   Wiki_Title *wt = NULL;
   UChar *title = NULL;

   if(! pagename) return ret;
   if(! wd) return ret;
   if(! wd->data) return ret;

   ws = wd->data;

   if(pagename) {
      char *tmp = url_desencodeC(pagename);
      if(tmp) {
         title = u_strdupC(tmp);
         free(tmp);
      } else
         return ret;
   }
   title = title_cleanup(title);
   /* TODO: go to page anchor */
   title = icu_replace("#.*", ICU_STR_EMPTY, title, 0);
   if(title) {
      wt = parser_title_part(wd, title);
      if(wt) {
	 free(title);
	 title = parser_title_with_namespace(wt);
	 if(title) {
	    pname = u_strToUTF8_new(title);
	 }
      }
      if(title)
	 free(title);
   }

   if(! pname) {
      if(wt)
         parser_title_free(wt);
      return ret;
   }

   pname = pcre_replace("/(&nbsp;|_)/", " ", pname);
   pname = pcre_replace("/ +/", " ", pname);
   wdp = wd_sqlite_page_load(wd, pname);
   if(wdp == NULL) 
   {
      int failed = 1;
      if(wt) {
	 wd->ns = wikidump_title_namespace_get(wd->namespaces, pname);
	 if(wd->ns == WIKI_NS_FILE && strcmp(wd->sitename, "commons.wikimedia.org")) {
	    uprintf("Location: http://%s/%s?wikimedia=commons.wikimedia.org&lang=en&entry=File:%S\r\n\r\n", wd->servername, wd->scriptpath, wt->title);
	    // exit(0);
	 } else if (wd->ns == WIKI_NS_MAIN && wd->namespaces) {
	    const char *nscat = wikidump_namespace_name_get(wd->namespaces, WIKI_NS_PROJECT);
	    const char *colon = strstr(pname, ":");
	    if(nscat && colon) {
	       char *rname = NULL;
	       asprintf(&rname, "%s%s", nscat, colon);
//	       printf("\nRNAME: %s\n", rname);
	       if(rname) {
		  if((wdp = wd_sqlite_page_load(wd, rname)) != NULL) 
		     failed = 0;
		  free(rname);
	       }
	    }
	    if(failed)
	       nscat = wikidump_namespace_name_get(wd->namespaces, WIKI_NS_HELP);
	    if(nscat && colon) {
	       char *rname = NULL;
	       asprintf(&rname, "%s%s", nscat, colon);
//	       printf("RNAME: %s\n", rname);
	       if(rname) {
		  if((wdp = wd_sqlite_page_load(wd, rname)) != NULL) 
		     failed = 0;
		  free(rname);
	       }
	    }
	    
	 }
      }
      if(failed) {
	 ulog("Error loading page\n");
	 free(pname);
         if(wt)
            parser_title_free(wt);
	 return ret;
      }
   }

   if(redirect && wd->data_page) {
      wikidump_page_free(wd->data_page);
   }
   wd->data_page = wdp;

   if(wt)
      parser_title_free(wt);

   if(raw == 4) {
      text = wikidump_page_buffer_get(wdp);
      if(! text) {
	 printf("Failed to get raw page\n");
      } else
         text = pcre_replace("/^/", "<?xml version=\"1.0\" ?>\n", text);
      free(pname);
      return text;
   }

   text = wikidump_page_content_get(wdp);
   if(! text) {
      text = strdup(" ");
      if(! text) {
	 ulog("Failed to get content from wdp\n");
	 free(pname);
	 return ret;
      }
   }

   content = u_strdupC(text);
   if(! content) {
      ulog("Failed to get content from text: '%s', '%S'\n", text, content);
      free(pname);
      free(text);
      return ret;
   }
   free(text);

   wd->ns = wikidump_title_namespace_get(wd->namespaces, pname);
   if(raw != 1 && (wd->ns == WIKI_NS_FILE || wd->ns == WIKI_NS_MEDIA)) {
      content = wikidump_image_to_desc_add(wd, pname, content);
   }

   ns = wikidump_namespace_name_get(wd->namespaces, wd->ns);

   if(ns) {
      re = calloc(strlen(ns) + 5, sizeof(char));
      if(re) {
	 sprintf(re, "/^%s:/", ns);
	 pname = pcre_replace(re, "", pname);
	 free(re);
      }
   }

   wd->pagename = pname;

   ret = output_content(wd, content, raw, redirect);

   wd->pagename = NULL;
   free(pname);
   free(content);

   return ret;
}

static void wiki_parser_data_static_set(Wiki_Parser_Data *wd)
{
   if(! wd) return;

   if(wd->locale) {
      setenv("LANG", wd->locale, 1);
      setlocale(LC_TIME, wd->locale);
   }
   
   wd->servername = "localhost";
   wd->port = 80;
   wd->proto = "http";
   wd->image_url = "http://upload.wikimedia.org/";

   wd->template_content_get = wd_sqlite_template_content_get;
   wd->image_url_get = wikidump_image_url_get;
   wd->page_exists = wikidump_page_exists;
   wd->page_exists_interwiki = wd_sqlite_page_exists_interwiki;
   wd->page_nsid_get = wd_sqlite_page_namespace_id_get;
   wd->category_pages_get = wd_sqlite_category_pages_list_get;
   wd->search_pages_get = wd_sqlite_search_pages_list_get;
   wd->sitename_from_base = wd_sqlite_sitename_from_base;
   wd->stats_get = wd_sqlite_stats_get;
   wd->stats_npages_in_category = wd_sqlite_stats_npages_in_category;
   wd->base_origin = wd_sqlite_base_origin_get;

   wd->page_revision_id_get = wd_sqlite_page_revision_id_get;
   wd->page_revision_timestamp_get = wd_sqlite_page_revision_timestamp_get;
   wd->page_revision_comment_get = wd_sqlite_page_revision_comment_get;
   wd->page_contributor_username_get = wd_sqlite_page_contributor_username_get;
   wd->page_contributor_id_get = wd_sqlite_page_contributor_id_get;
   wd->page_file_info_get = wikidump_image_info_get;

   wd->interwiki = eina_hash_string_superfast_new(NULL);

   eina_hash_add(wd->interwiki, "c", "commons.wikimedia.org");
   eina_hash_add(wd->interwiki, "w", "wikipedia.org");
   eina_hash_add(wd->interwiki, "b", "wikibooks.org");
   eina_hash_add(wd->interwiki, "v", "wikiversity.org");
   eina_hash_add(wd->interwiki, "s", "wikisource.org");
   eina_hash_add(wd->interwiki, "m", "meta.wikimedia.org");
   eina_hash_add(wd->interwiki, "q", "wikiquote.org");
   eina_hash_add(wd->interwiki, "n", "wikinews.org");
   eina_hash_add(wd->interwiki, "mw", "www.mediawiki.org");
   eina_hash_add(wd->interwiki, "wikt", "wiktionary.org");
   eina_hash_add(wd->interwiki, "wmf", "foundation.wikimedia.org");
   eina_hash_add(wd->interwiki, "commons",     "commons.wikimedia.org");
   eina_hash_add(wd->interwiki, "meta",        "meta.wikimedia.org");
   eina_hash_add(wd->interwiki, "species",     "wikispecies.org");
   eina_hash_add(wd->interwiki, "wikipedia",   "wikipedia.org");
   eina_hash_add(wd->interwiki, "wikibooks",   "wikibooks.org");
   eina_hash_add(wd->interwiki, "wikiversity", "wikiversity.org");
   eina_hash_add(wd->interwiki, "wikisource",  "wikisource.org");
   eina_hash_add(wd->interwiki, "metawiki",    "meta.wikimedia.org");
   eina_hash_add(wd->interwiki, "wikiquote",   "wikiquote.org");
   eina_hash_add(wd->interwiki, "wikinews",    "wikinews.org");
   eina_hash_add(wd->interwiki, "mediawiki",   "www.mediawiki.org");

}

Wiki_Parser_Data *wiki_parser_data_init(Wiki_Sqlite *ws)
{
   Wiki_Parser_Data *wd = NULL;

   if(! ws) return wd;

   wd = calloc(1, sizeof(Wiki_Parser_Data));
   if(! wd) return wd;

   wd->siteid = ws->siteid;
   wd->scriptpath = "cgi-bin/wikidump_cgi";
   wd->locale = ws->locale;
   wd->base = ws->base;
   wd->sitename = ws->sitename;
   wd->localname = ws->localname;
   wd->lang = ws->lang;
   wd->data = ws;
   if(ws->case_used) {
      if(strcmp(ws->case_used, "first-letter") == 0)
	 wd->wcase = WIKI_FIRST_LETTER;
      else if(strcmp(ws->case_used, "case-sensitive") == 0)
	 wd->wcase = WIKI_SENSITIVE;
      else
	 wd->wcase = WIKI_CASE_UNKNOW;
   }

   wiki_parser_data_static_set(wd);
   wd->wikilist = wd_sqlite_wikimedia_list_get(wd);

//   wd->namespaces = wd_sqlite_namespace_load(wd);

   return wd;
}

void *wiki_parser_data_free(Wiki_Parser_Data *wd)
{
   void *data = NULL;

   if(wd) {
      if(wd->wikilist) {
	 Wiki_Media *wm;
	 EINA_LIST_FREE(wd->wikilist, wm) {
	    if(wm) {
	       if(wm->sitename) eina_stringshare_del(wm->sitename);
	       if(wm->lang) eina_stringshare_del(wm->lang);
	       if(wm->localname) eina_stringshare_del(wm->localname);
	       if(wm->locale) eina_stringshare_del(wm->locale);
	       if(wm->base) eina_stringshare_del(wm->base);
	       if(wm->case_used) eina_stringshare_del(wm->case_used);
	    }
	    free(wm);
	 }
      }
      if(wd->namespaces) {
	 eina_hash_foreach(wd->namespaces, wikidump_namespaces_del, NULL);
	 eina_hash_free(wd->namespaces);
      }
      if(wd->interwiki) {
	 eina_hash_free(wd->interwiki);
      }
      if(wd->data_page) {
	 Wiki_Dump_Page *wdp = wd->data_page;
	 wikidump_page_free(wdp);
      }
      if(wd->data) {
	 data = wd->data;
      }

      free(wd);
   }

   return data;
}
