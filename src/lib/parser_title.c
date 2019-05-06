#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>

#include <stdio.h>

#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/ustdio.h>

#include "parser.h"
#include "util.h"
#include "pcre_replace.h"

UChar *parser_title_cleanup(UChar *title);

Wiki_Title *parser_title_free(Wiki_Title *wt)
{
   if(wt) {
      if(wt->lang)
	 free(wt->lang);
      if(wt->ns)
	 free(wt->ns);
      if(wt->sitename)
	 free(wt->sitename);
      if(wt->title)
	 free(wt->title);
      if(wt->arg)
	 free(wt->arg);
      free(wt);
   }
   return NULL;
}

static UChar *parser_title_ns_get(const Wiki_Parser_Data *wd, UChar *ns, const char *buf, Eina_Bool *set, int *nsid)
{
   const Wiki_Namespace *wns = NULL;
   char *str = NULL;

   ulog("TITLESPLIT NS: %S + %s\n", ns, buf);
   if(! buf) return ns;

   str = strdup(buf);
   if(! str) return ns;

   str = wiki_parser_utitle_case_set(wd, str);
   if(wd->namespaces && (wns = eina_hash_find(wd->namespaces, str))) {
      if(ns) { 
	 UChar *tmp = calloc(u_strlen(ns) + strlen(wns->label) + 2, sizeof(UChar));
	 if(tmp) {
	    u_sprintf(tmp, "%S:%s", ns, wns->label);
            if(eina_hash_find(wd->namespaces, tmp)) {
               free(ns);
               ns = tmp;
               *set = EINA_TRUE;
	       *nsid = wns->key;
            } else {
               free(tmp);
            }
	 }
      } else {
	 ns = u_strdupC(wns->label);
	 *nsid = wns->key;
         *set = EINA_TRUE;
      }
   }

   free(str);

   return ns;
}

static UChar *parser_title_lang_get(const Wiki_Parser_Data *wd, const char *buf, Eina_Bool *set)
{
   const Wiki_Lang *wl = NULL;
   char *str = NULL;
   UChar *lang = NULL;

   if(! buf) return NULL;

   str = strdup(buf);
   if(str) str = strtolower(str);

   wl = wiki_parser_language_infos_get(str);
   if(wl) {
      lang = u_strdupC(wl->code);
      *set = EINA_TRUE;
   }

   if(str) free(str);

   return lang;
}

static UChar *parser_title_site_get(const Wiki_Parser_Data *wd, const char *buf, Eina_Bool *set)
{
   const char *site = NULL;
   char *str = NULL;
   UChar *sitename = NULL;

   if(! buf) return NULL;

   str = strdup(buf);
   if(str) str = strtolower(str);
   if(str && ((site = eina_hash_find(wd->interwiki, str)))) {
      sitename = u_strdupC(site);
	    ulog("TITLESPLIT SET: %s\n", site);
      *set = EINA_TRUE;
   }

   if(str) free(str);

   return sitename;
}

Wiki_Title *parser_title_part(const Wiki_Parser_Data *wd, const UChar *pagename)
{
   Wiki_Title *wt = NULL;
   UChar *lang = NULL, *ns = NULL, *sitename = NULL, *title = NULL;
   const UChar *p = NULL;
   char *utf = NULL;

   if(! wd) return NULL;
   if(! pagename) return NULL;

   wt = calloc(1, sizeof(Wiki_Title));
   if(! wt) return NULL;

   wt->lang_first = EINA_FALSE;
   wt->intext = EINA_FALSE;

   if((p = u_strstr(pagename, ICU_STR_HASH))) {
      wt->arg = u_strdup(p);
   }

   if(p) 
      utf = u_strnToUTF8_new(pagename, p - pagename);
   else
      utf = u_strToUTF8_new(pagename);

   if(utf && strstr(utf, ":")) {
      unsigned int cnt = 0;
      char **split = eina_str_split_full(utf, ":", -1, &cnt);
      if(split) {
	 int i = 0;
	 char *utitle = calloc(strlen(utf) + 1, sizeof(char));
	 while(i < cnt) {
	    Eina_Bool set = EINA_FALSE;

	    if(i == 0 && split[i][0] == '\0') {
	       wt->intext = EINA_TRUE;
	       i++;
	       continue;
	    }
	    if(! lang)
	       ns = parser_title_ns_get(wd, ns, split[i], &set, &wt->nsid);

	    if(! ns && cnt > 1) {
	       if(set == EINA_FALSE && ! lang) {
		  lang = parser_title_lang_get(wd, split[i], &set);
		  if(lang && ! ns && ! sitename)
		     wt->lang_first = EINA_TRUE;
	       } 

	       if(set == EINA_FALSE && ! sitename && wd->interwiki) {
		  sitename = parser_title_site_get(wd, split[i], &set);
	       }
	    }

	    if(set == EINA_FALSE) {
	       if(utitle) {
		  if(utitle[0] != '\0')
		     strcat(utitle, ":");
		  strcat(utitle, split[i]);
	       }
	    }
	    i++;
	 }
	 if(utitle) {
	    title = trim_u(u_strdupC(utitle));
	    free(utitle);
	 }
	 if(split[0])
	    free(split[0]);
	 free(split);
      }
      free(utf);
   } else if(utf) {
      title = u_strdupC(utf);
      free(utf);
   }

   ns = wiki_parser_title_case_set(wd, ns);
   title = wiki_parser_title_case_set(wd, title);

   wt->lang = lang;
   wt->sitename = sitename;
   wt->ns = ns;
   if(wt->ns && ! title) {
      title = ns;
      wt->ns = NULL;
   }

   wt->title = parser_title_cleanup(title);
   wt->title = trim_u(wt->title);

   ulog("TITLESPLITPART: %S => intext = %d, lang_first = %d, lang = %S, sitename = %S, ns = %S, title = %S, arg = %S\n",
	pagename, wt->intext, wt->lang_first, wt->lang, wt->sitename, wt->ns, wt->title, wt->arg);

   return wt;
}

UChar *parser_title_cleanup(UChar *title)
{
   UChar *c = title, *p = title;
   size_t size = 0;

   if(! title) return NULL;

   size = u_strlen(title);
   while(*p) {
      if(*p == '_')
	 *p = ' ';
      p++;
   }
   p= title;
   while(*p) {
      if(*p == '/' || *p == ' ') {
	 int cnt = 0;
	 c = p;
	 while(*p == *c) {
	    cnt++;
	    p++;
	 }
	 if(cnt > 1) {
	    u_memmove(c + 1, p, size - (p - title) + 1);
	    size -= (cnt - 1);
	 }
	 p--;
      }
      p++;
   }

   return title;
}

UChar *parser_title_with_namespace(Wiki_Title *wt)
{
   UChar *content = NULL;
   size_t size = 0;

   if(wt) {
      if(wt->ns)
	 size = u_strlen(wt->ns) + 1;
      if(wt->title)
	 size += u_strlen(wt->title) + 1;
      if(wt->arg) 
	 size += u_strlen(wt->arg);
      content = calloc(size + 1, sizeof(UChar));
      if(content) {
	 if(wt->ns) {
	    u_strcat(content, wt->ns);
	    u_strcat(content, ICU_STR_COLON);
	 }
	 if(wt->title)
	    u_strcat(content, wt->title);
	 if(wt->arg)
	    u_strcat(content, wt->arg);
      }
   }

   return content;
}

char *parser_title_rel2abs(const Wiki_Parser_Data *wd, const char *title_str, const char *path_str)
{
   char *content = NULL;
   char *tmp = NULL;
   const char *title = title_str, *path = path_str;

   ulog("rel2abs: %s, %s\n", title, path);

   if(title && index(title, ':')) {
      content = strdup(title);
   } else if(title && path) {
      char *s = NULL;
      asprintf(&s, "/^%s/", path);
      if(s) {
	 tmp = strdup(title);
	 if(tmp) 
	    tmp = pcre_replace(s, "", tmp);
	 free(s);
      }
   }
   if(tmp && path) {
      char **relative = NULL, **absolute = NULL;
      relative = eina_str_split(tmp, "/", -1);
      absolute = eina_str_split(path, "/", -1);
      if(absolute && relative) {
	 int cnt = 0, i = 0, len;
	 int cnt_path = 0;

	 len = (strlen(path) + strlen(title)) * 2;
	 while(absolute[cnt]) 
	    cnt++;

      	 cnt_path = cnt;
	 while(relative[i]) {
	    // if(i == 0 && strcmp(relative[i], "") == 0 && cnt > 0) 
	    //   cnt--;

	    if(strcmp(relative[i], "..") == 0 && cnt > 0) {
     	       cnt--;
	    }
	    i++;
	 }

      	 ulog("rel2abs: %d, %d\n", cnt_path, cnt);
	 i = 0;
	 if(cnt_path != cnt) {
	    char *dst = calloc(len + 2, sizeof(char));
	    if(dst) {
	       while(cnt > 0) {
		  if(i > 0)
		     strcat(dst, "/");
		  strcat(dst, absolute[i]);
		  cnt--;
		  i++;
	       }

     	       if(strlen(dst) == 0 && relative[0] && strcmp(relative[0], "") == 0)
		  strcat(dst, path);

     	       i = 0;
	       while(relative[i]) {
		  if(strlen(relative[i]) > 0 && ! (strcmp(relative[i], "..") == 0 || strcmp(relative[i], ".") == 0 )) {
		     if(len < strlen(dst) + strlen(relative[i]) + 2) {
			len *= 2;
			dst = realloc(dst, len);
			if(! dst) break;
		     }
		     if(strlen(dst))
			strcat(dst, "/");
		     strcat(dst, relative[i]);
		  }
		  i++;
	       }

	       if(dst) {
		  content = dst;
	       }
	    }
	 } else {
	    content = calloc(len + 2, sizeof(char));
	    if(content) {
	       strcat(content, path);
	       while(relative[i]) {
		  if(strlen(relative[i]) > 0 && strcmp(relative[i], ".")) {
		  ulog("rel2abs add: %s\n", relative[i]);
		     strcat(content, "/");
		     strcat(content, relative[i]);
		  }
		  i++;
	       }
	    }
	 }
      }

      if(absolute) {
	 free(absolute[0]);
	 free(absolute);
      }
      if(relative) {
	 free(relative[0]);
	 free(relative);
      }
   }

   if(tmp)
      free(tmp);

   ulog("rel2abs res: %s\n", content);
   return content;
}

char *wiki_parser_anchor_encode(const char *url)
{
   char *str = NULL;

   if(! url) return str;

   str = strdup(url);
   if(str) {
      str = pcre_replace("/\n/", " ", str);
      str = pcre_replace("/^ */", "", str);
      str = pcre_replace("/ *$/", "", str);
      str = pcre_replace("/( |\\/)/", "_", str);
      str = pcre_replace("/&/", "%#38;%#38;", str);
      if(str) {
	 char *enc = url_encodeC(str);
	 if(enc) {
	    enc = pcre_replace("/%5F/", "_", enc);
	    enc = pcre_replace("/%/", ".", enc);
	    free(str);
	    str = enc;
	 }
      }
   }

   ulog("ANCHOR ENC: %s => %s\n", url, str);

   return str;
}

UChar *wiki_parser_title_case_set(const Wiki_Parser_Data *wd, UChar *str)
{
   PARSE_TRACE();

   if(! wd) return str;
   if(! str) return str;

   if(wd->wcase == WIKI_FIRST_LETTER) {
      UChar *p = str, *s = NULL;
      if(! u_isupper(str[0]))
	 str[0] = u_toupper(str[0]);
      while(0 && (s = (UChar *)icu_search("( |')", p, 0))) {
         s++;
         if(u_isupper(*s))
            *s = u_tolower(*s);
         p = s;
      } 
   }

   return str;
}

char *wiki_parser_utitle_case_set(const Wiki_Parser_Data *wd, char *str)
{
   PARSE_TRACE();

   if(! wd) return str;
   if(! str) return str;

   if(wd->wcase == WIKI_FIRST_LETTER) {
      UChar *tmp = u_strdupC(str);
      if(tmp) {
	 tmp = wiki_parser_title_case_set(wd, tmp);
	 if(tmp) {
	    free(str);
	    str = u_strToUTF8_new(tmp);
	    free(tmp);
	 }
      }
   }

   return str;
}

