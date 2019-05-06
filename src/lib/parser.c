#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <sys/time.h>
#include <unistd.h>

#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/uregex.h>
#include <unicode/ustdio.h>

#include <libintl.h>

#include <libxml/xmlsave.h>
#include <libxml/xpath.h>

#include <curl/curl.h>
#include "wiki_define.h"
#include "parser.h"
#include "parser_extension.h"
#include "parser_tag_extensions.h"
#include "util.h"
#include "entities.h"
#include "pcre_replace.h"
#include "icu_eina_hash.h"

#define WIKI_PARSER_TEMPLATE_USAGE_MAX 200000
#define WIKI_PARSER_REDIRECT_COUNT_MAX 3
#define WIKI_PARSER_TEMPLATES_NOT_FOUND_MAX 1000

static const char WIKI_UNIQ_STRING_FORMAT[] = "\x7fUNIQ%x%x-%s-%08x-QINU\x7f";
static int wiki_parser_initialized = 0;
static Eina_Hash *parser_extension_modules = NULL;
static Eina_Hash *wiki_languages = NULL;

int level_depth = 0;

#define TMPL_BUF_SIZE 1024
typedef struct _TR {
   UChar *content;
   size_t size;
   off_t s;
   off_t e;
} TR;

typedef struct _Wiki_Parser_Find
{
   const UChar *start;
   const UChar *end;
   const UChar *pipe;
   unsigned int curl_count;
} Wiki_Parser_Find;

Eina_Bool wiki_parser_template_first_find2(Wiki_Parser_Find *wpf, const UChar *buf, int num);

static UChar *
wiki_parser_expand_template_or_function(Wiki_Parser *root, const Wiki_Parser_Find *wpf, Wiki_Args *pargs, Eina_Bool *changed);

static UChar *
wiki_parser_match_curl_open_sup_3(Wiki_Parser *root, const Wiki_Parser_Find *wpf, Wiki_Args *args, Eina_Bool *changed);

static UChar *
wiki_parser_match_curl_open_equal_3(Wiki_Parser *root, const Wiki_Parser_Find *wpf, Wiki_Args *args, Eina_Bool *changed);

static UChar *
wiki_parser_template_nowiki_replace(Wiki_Parser *wp, UChar *content);

static UChar *
wiki_parser_template_comment_remove(Wiki_Parser *wp, UChar *content);

static UChar *
_wiki_parser_template_replace_arg_by_val3(Wiki_Parser *root, UChar *buf, Wiki_Args *args, UChar separator, Eina_Bool is_ext);

static Eina_List *
_wiki_parser_parse_args(Wiki_Parser *root, Wiki_Args *pargs, Eina_List *args, Eina_Bool is_ext);

static UChar *
_wiki_parser_template_name_full_get(Wiki_Parser *root, UChar *tname, Eina_Bool redirect);

static Eina_List *wiki_parser_args_sort_uniq(Eina_List *args, Eina_Bool is_ext);
static Eina_Hash *_wiki_parser_args_list_to_hash(Eina_List *args, Eina_Bool is_ext);

int wiki_parser_init(void) 
{
   if(! wiki_parser_initialized) {
      xmlInitParser();
      wiki_parser_extension_builtin_init();
      wiki_parser_extension_magic_words_init();
      wiki_parser_extension_functions_init();
      wiki_parser_extension_stats_init();
      wiki_parser_extension_time_init();
      wiki_languages = wiki_languages_init();
      wiki_lexer_tags_get();
//      xmlInitializePredefinedEntities();
//      xmlSubstituteEntitiesDefault(0);

   }
   wiki_parser_initialized++;
   return wiki_parser_initialized;
}

void wiki_parser_shutdown(void)
{
   if(wiki_parser_initialized > 0) {
      wiki_parser_initialized--;
      if(wiki_parser_initialized == 0) {
	 if(parser_extension_modules) {
	    eina_hash_free(parser_extension_modules);
	    parser_extension_modules = NULL;
	 }
	 if(wiki_languages) {
	    eina_hash_free(wiki_languages);
	    wiki_languages = NULL;
	 }
	 xmlCleanupParser();
	 // Clean memory
         wiki_lexer_tags_free();
	 html_entities_free();
      }
   }
}

static void 
wiki_parser_templates_loaded_free(void *data)
{
   Wiki_Template_Loaded *wtl = data;
   if(wtl) {
      if(wtl->title) 
	 free(wtl->title);
      if(wtl->content)
	 free(wtl->content);
      if(wtl->redirect)
	 free(wtl->redirect);
      free(wtl);
   }
}

static void 
_wiki_parser_nowiki_part_free(void *data)
{
   Wiki_Nowiki_Part *wnp = data;
   if(wnp) {
      if(wnp->args)
	 free(wnp->args);
      if(wnp->content)
	 free(wnp->content);
      free(wnp);
   }
}

void wiki_parser_extension_module_register(const char *name,
      UChar *(*func)(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data))
{
   void *f = NULL;
   UChar *uname = NULL;

   if(! parser_extension_modules)
      parser_extension_modules = icu_eina_hash_new(NULL);
   if(! parser_extension_modules)
      return;
   if(! name) 
      return;

   uname = u_strdupC(name);
   if(! uname)
      return;

   if(! (f = icu_eina_hash_find(parser_extension_modules, uname))) {
      icu_eina_hash_add(parser_extension_modules, uname, func);
   }

   free(uname);
}

const char *wiki_parser_namespace_name_get(const Wiki_Parser_Data *wd, int nsid)
{
   Wiki_Namespace *ns = NULL;
   char nsstr[INT_STR_SIZE];

   if(! wd) return NULL;
   if(! wd->namespaces) return NULL;

   snprintf(nsstr, sizeof(nsstr), "%d", nsid);

   ns = eina_hash_find(wd->namespaces, nsstr);
   if(ns)
      return ns->label;
   else
      return NULL;
}

const Wiki_Lang *wiki_parser_language_infos_get(const char *code)
{
   const Wiki_Lang *lang = NULL;
   if(wiki_languages && code) {
      lang = eina_hash_find(wiki_languages, code);
   }

   return lang;
}

UChar *wiki_parser_intl_get(UChar *msgid, size_t *size)
{
   size_t s = 0;
   
   if(size)
      s = *size;
   else
      return msgid;

   if(msgid) {
      char *tmp = u_strnToUTF8_new(msgid, s);
      if(tmp) {
	 char *msgstr = gettext(tmp);
	 if(msgstr != tmp) {
	    free(msgid);
	    msgid = u_strdupC(msgstr);
	    s = (msgid) ? u_strlen(msgid) : 0;
	 }
	 free(tmp);
      }
   }

   if(size)
      *size = s;

   return msgid;
}

Wiki_File *wiki_parser_file_free(Wiki_File *wf)
{
   if(wf) {
      if(wf->mime)
	 free((char *)wf->mime);
      free(wf);
   }
   return NULL;
}

static UChar *_update_buffer(UChar *buf, size_t size, Eina_List **preplace, size_t *new_size)
{
   Eina_List *l = NULL, *replace = *preplace;
   TR *d = NULL;

   PARSE_TRACE();

   if(! buf) return buf;
   if(size <= 0) return buf;
  
   if(replace && eina_list_count(replace) > 0) {
      Eina_List *t = NULL;
      size_t size_new = 0;
      UChar *tmp = NULL;

      size_new = size;
      EINA_LIST_FOREACH(replace, t, d) {
	 size_new += d->size;
      }

      tmp = calloc((size_new * 1) + 1, sizeof(UChar));
      if(tmp) {
	 off_t last = 0;
	 size_t end;

	 u_memcpy(tmp, buf, size);

	 if(output_log) {
	    EINA_LIST_FOREACH(replace, l, d) {
	       UChar *b = u_strndup(buf + d->s, d->e - d->s);
	       ulog("REPLACE BUF: %d, %d, %d: %S by (%d) %S\n", d->s, d->e, (d->e - d->s), b, d->size, d->content);
	       if(b)
		  free(b);
	    }
	 }
	 EINA_LIST_FOREACH(replace, l, d) {
	    ulog("content: %d, tpl: %d, (%d - %d) = %d , last: %d\n",
		  size, d->size, d->e, d->s, d->e - d->s, last);
	    if(d->e > size || d->size < 0) {
	       uprintf("content: %d, tpl: %d, (%d - %d) = %d , last: %d\n",
	   	     size, d->size, d->e, d->s, d->e - d->s, last);
	       d->e = size; uprintf("before:\n%S\n\nafter:\n%S\n", buf, tmp);
	       exit(10);
	    }
	    ;
	    d->s += last;
	    d->e += last;
	    end = (size + last) - d->e;
	    if(end > 0 && d->e != d->s && (tmp + d->s + d->size + end) < (tmp + size_new))
	       u_memmove(tmp + d->s + d->size, tmp + d->e, end);
	    else if(d->s + d->size < size_new)
	       tmp[d->s + d->size] = '\0';
	    if(d->size > 0 && (tmp + d->s + d->size) < (tmp + size_new)) 
	       u_memcpy(tmp + d->s, d->content, d->size);
	    else if(end == 0) 
	       tmp[d->s] = '\0';
	    if(d->s + d->size + end < size_new)
	       tmp[d->s + d->size + end] = '\0';
	    last += (d->size - (d->e - d->s));
//	    ulog("LAST: %d\nB: %S\nA: %S\n", last, buf, tmp);
	 }

//	 ulog("REPLACE BUF: %p == %p\n", tmp, buf);
	 free(buf);
	 buf = tmp;
	 *new_size = size + last;
	 ulog("NEWSIZE: %d, %d\n", size + last, *new_size);
      }
   }

   if(replace) {
      EINA_LIST_FREE(replace, d) {
	 if(d->content)
       	    free(d->content);
	 free(d);
      }
      replace = NULL;
   }

   *preplace = NULL;

   return buf;
}

static UChar *wiki_parser_tag_name_remove(UChar *content, const char *tag)
{
   const UChar *p = NULL;
   char *reg = NULL;
   size_t skip = 0;

   PARSE_TRACE();

   if(! content) return NULL;
   if(! tag) return content;

   asprintf(&reg, "</?(%s)[^>]*>", tag);
   if(! reg) return content;

   while((p = icu_search(reg, content + skip, UREGEX_CASE_INSENSITIVE))) {
      const UChar *no = NULL, *ne = NULL;
      if((no = icu_search_size("<nowiki[^/>]*>", content + skip, p - (content + skip), UREGEX_CASE_INSENSITIVE))) {
	 ne = icu_search("</nowiki>", no, UREGEX_CASE_INSENSITIVE);
	 if(ne) {
	    skip = (ne - content) + 1;
	 } else {
	    skip = u_strlen(content);
	 }
      }
      if(no == NULL) {
	 const UChar *e = icu_search(">", p, UREGEX_CASE_INSENSITIVE);
	 if(e) {
	    UChar *d = content + (p - content);
	    e++;
	    if(*(d + 1) != '/') {
	       while(u_isWhitespace(*e))
		  e++;
	    }
	    u_memmove(d, e, u_strlen(e) + 1);
	 }
	 skip = 0;
      }
   }

   free(reg);

   return content;
}

static UChar *wiki_parser_tag_and_content_remove(UChar *content, const char *tag)
{
   const UChar *p = NULL;
   char *reg_open = NULL, *reg_close = NULL;
   size_t skip = 0;

   PARSE_TRACE();

   if(! content) return NULL;
   if(! tag) return content;

   asprintf(&reg_open, "<(%s)[^>]*>", tag);
   if(! reg_open) return content;

   asprintf(&reg_close, "</(%s)[^>]*>", tag);
   if(! reg_close) {
      free(reg_open);
      return content;
   }

   while((p = icu_search(reg_open, content + skip, UREGEX_CASE_INSENSITIVE))) {
      const UChar *no = NULL, *ne = NULL;
      if((no = icu_search_size("<nowiki[^/>]*>", content + skip, p - (content + skip), UREGEX_CASE_INSENSITIVE))) {
	 ne = icu_search("</nowiki>", no, UREGEX_CASE_INSENSITIVE);
	 if(ne) {
	    skip = (ne - content) + 1;
	 } else {
	    skip = u_strlen(content);
	 }
      }
      if(no == NULL) {
	 const UChar *e = icu_search(reg_close, p, UREGEX_CASE_INSENSITIVE);
	 if(e) {
	    UChar *d = content + (p - content);
//	    ulog("REMOVE: %S\n", d);
	    e = icu_search(">", e, 0);
	    e++;
	    u_memmove(d, e, u_strlen(e) + 1);
	 } else {
	    content[p - content] = '\0';
	 }
	 skip = 0;
      }
   }

   free(reg_close);
   free(reg_open);

   return content;
}

static UChar *wiki_parser_tag_content_get(UChar *content, const char *tag)
{
   const UChar *p = NULL;
   char *reg_open = NULL, *reg_close = NULL;
   size_t skip = 0;

   PARSE_TRACE();

   if(! content) return NULL;
   if(! tag) return content;

   asprintf(&reg_open, "<(%s)[^>]*>", tag);
   if(! reg_open) return content;

   asprintf(&reg_close, "</(%s)[^>]*>", tag);
   if(! reg_close) {
      free(reg_open);
      return content;
   }

   while((p = icu_search(reg_open, content + skip, UREGEX_CASE_INSENSITIVE))) {
      const UChar *no = NULL, *ne = NULL;
      if((no = icu_search_size("<nowiki[^/>]*>", content + skip, p - (content + skip), UREGEX_CASE_INSENSITIVE))) {
	 ne = icu_search("</nowiki>", no, UREGEX_CASE_INSENSITIVE);
	 if(ne) {
	    skip = (ne - content) + 1;
	 } else {
	    skip = u_strlen(content);
	 }
      }
      if(no == NULL) {
	 const UChar *e = icu_search(reg_close, p, UREGEX_CASE_INSENSITIVE);
	 p = icu_search(">", p, 0);
	 p++;
	 if(e) {
	    u_memmove(content, p, e - p);
	    content[e - p] = '\0';
	 } else {
	    u_memmove(content, p, u_strlen(p) + 1);
	 }
	 skip = 0;
      }
   }
   free(reg_close);
   free(reg_open);

   return content;
}


/* 
 * TODO: remove comment test case:
 * wikimedia=fr.wikipedia.org&lang=fr&entry=Modèle:ATC&raw=0
 */
static UChar *wiki_parser_template_comment_remove(Wiki_Parser *wp, UChar *content)
{
   const UChar *s = NULL;
   UChar *p = NULL;
   UChar open[10], close[10];
   size_t skip = 0, len = 0;;

   PARSE_TRACE();

   if(! wp) return NULL;
   if(! wp->wd) return NULL;
   if(! content) return NULL;

   u_sprintf(open, "%s", "<!--");
   u_sprintf(close, "%s", "-->");

   len = u_strlen(content);

   while((s = u_strstr(content + skip, open))) {
      const UChar *e = u_strstr(s + 4, close);
      p = content + (s - content);
      if(e) {
	 e += 3;
	 u_memmove(p, e, (len - (e - content)) + 1);
	 skip = p - content;
      } else
	 *p = '\0';
   }

   return content;

}

static UChar *wiki_parser_template_static(Wiki_Parser *wp, UChar *content)
{
   const UChar *p = NULL, *nc = NULL;
   size_t skip = 0;
   PARSE_TRACE();

   if(! wp) return NULL;
   if(! wp->wd) return NULL;
   if(! content) return NULL;

   while((p = icu_search("__(NOTOC|FORCETOC|TOC|NOEDITSECTION|NEWSECTIONLINK|NONEWSECTIONLINK|NOGALLERY|HIDDENCAT|NOCONTENTCONVERT|NOCC|NOTITLECONVERT|NOTC|START|END|INDEX|NOINDEX|STATICREDIRECT)__", content + skip, 0))) {
      if(icu_search("^__NOTOC__", p, 0)) {
	 wp->toc = EINA_FALSE;
      } else if(icu_search("^__FORCETOC__", p, 0)) {
	 wp->toc = EINA_TRUE;
      } else if(icu_search("^__TOC__", p, 0)) {
	 size_t r = u_strlen(p + 7);
	 wp->toc = EINA_FALSE;
	 if(r > 0) {
	    UChar *tmp = NULL, *toc = NULL;
	    toc = u_strdupC("<tableofcontent />");
	    if(toc)
	       tmp = calloc((p - content) + r +  20, sizeof(UChar *));
	    if(tmp) {
	       u_strncat(tmp, content, (p - content));
	       u_strcat(tmp, toc);
	       u_strcat(tmp, p + 7);
	       free(toc);
	       free(content);
	       content = tmp;
	       continue;
	    } else {
	       if(toc)
		  free(toc);
	    }
	 } else {
	    *(content + (p - content)) = '\0';
	    continue;
	 }
	 // content = icu_replace_utf8("__TOC__", "<tableofcontent />", icontent, 0);
      }
      nc = icu_search("__", p + 2, 0);
      u_memmove(content + (p - content), nc + 2, u_strlen(nc + 2) + 1);
   }

   return content;
}

static Eina_Hash *_wiki_parser_args_list_to_hash(Eina_List *args, Eina_Bool is_ext)
{
   Eina_List *l = NULL;
   Eina_Hash *uniq = NULL;
   Wiki_Template_Arg *arg = NULL;
   UChar idx[255];
   char tmp[255];
   int i;

   PARSE_TRACE();

   if(! args) return NULL;

   uniq = icu_eina_hash_new(NULL);
   if(! uniq) return NULL;

   i = 0;
   EINA_LIST_FOREACH(args, l, arg) {
      if(arg) {
	 if(arg->key && arg->have_equal) {
	    UChar *tmp = u_strndup(arg->key, arg->key_size);
	    tmp = trimn_u(tmp, arg->key_size, NULL);
	    icu_eina_hash_add(uniq, tmp, arg);
	    free(tmp);
	 } else {
	    do {
	       i++;
	       sprintf(tmp, "%d", i);
	       u_uastrcpy(idx, tmp);
	    } while(icu_eina_hash_find(uniq, idx));
	    icu_eina_hash_add(uniq, idx, arg);
	 }
      }
   }

   return uniq;
}

static Eina_List *wiki_parser_args_sort_uniq(Eina_List *args, Eina_Bool is_ext)
{
   Eina_List *l = NULL, *n = NULL;
   Eina_Hash *uniq = NULL;
   Wiki_Template_Arg *arg = NULL;
   UChar idx[255], *p = NULL;
   char tmp[255];
   int i;

   PARSE_TRACE();

   if(! args) return NULL;
   if(is_ext) return args;

   uniq = icu_eina_hash_new(NULL);
   if(! uniq) return args;

   i = 0;
   EINA_LIST_FOREACH_SAFE(args, l, n, arg) {
      if(arg) {
	 if(is_ext == EINA_FALSE) {
	    p = NULL;
	    if(arg->key && arg->have_equal)
	       p = arg->key;
	    else if(arg->have_equal) {
	       /* TODO: should merge arg->val to previous arg ? */
	       args = eina_list_remove(args, arg);
	       if(arg->val)
		  free(arg->val);
	       free(arg);
//	       ulog("AFTER PARSER ARG REMOVE = %S val = %S\n", arg->key, arg->val);
	       continue;
	    } else {
	       do {
		  i++;
		  sprintf(tmp, "%d", i);
		  u_uastrcpy(idx, tmp);
	       } while(icu_eina_hash_find(uniq, idx));
	       p = idx;
	    }
	    if(p) {
	       Wiki_Template_Arg *a = icu_eina_hash_find(uniq, p);
	       if(a) {
//		  ulog("AFTER PARSER ARG REPLACE key = %S,  %S by %S\n", p, a->val, arg->val);
		  args = eina_list_remove(args, a);
		  if(a->key) free(a->key);
		  if(a->val) free(a->val);
		  free(a);
		  icu_eina_hash_modify(uniq, p, arg);
	       } else {
//		  ulog("NEW KEY: %S, %p\n", p, arg);
		  icu_eina_hash_add(uniq, p, arg);
	       }
	    }
	 } else { 
	    do {
	       i++;
	       sprintf(tmp, "%d", i);
	       u_uastrcpy(idx, tmp);
	    } while(icu_eina_hash_find(uniq, idx));
	    icu_eina_hash_add(uniq, idx, arg);
	 }
      }
   }

   if(uniq)
      eina_hash_free(uniq);

   return args;
}

static Eina_List *_wiki_parser_parse_args(Wiki_Parser *root, Wiki_Args *pargs, Eina_List *args, Eina_Bool is_ext)
{
   Eina_List *l = NULL;
   Wiki_Template_Arg *arg = NULL;
   int error = 0;

   PARSE_TRACE();

   if(! root) return args;
   if(! args) return args;

#if 0
   if(pargs && pargs->list) {
      EINA_LIST_FOREACH(pargs->list, l, arg) {
	 if(arg)
	    ulog("PARGS: equal: %d, key: %S, val: %S\n", arg->have_equal, arg->key, arg->val);
      }
   } else {
      ulog("No parg\n");
   }
#endif

   EINA_LIST_FOREACH(args, l, arg) {
      if(arg) {
	 size_t size = 0;
	 if(arg->key && u_strstr(arg->key, ICU_STR_CURL_OPEN))  {
	    UChar *key = NULL;
	    if(is_ext == EINA_FALSE)
	       key = wiki_parser_parse_part(root, arg->key, arg->key_size, pargs, &error);
	    else
	       key = _wiki_parser_template_replace_arg_by_val3(root, arg->key, pargs, '|', 0);

	    arg->key = NULL;
	    size = 0;

	    if(key) {
	       arg->key = key;
	       if(arg->have_equal)
		  arg->key = trimn_u(arg->key, u_strlen(key), &size);
	       else
		  size = u_strlen(key);
            }
	    arg->key_size = size;;
	 } else if(arg->have_equal) {
	    size = arg->key_size;
	    arg->key = trimn_u(arg->key, size, &arg->key_size);
	 }

	 if(arg->val && u_strstr(arg->val, ICU_STR_CURL_OPEN)) {
	    UChar *val = NULL;
	    if(is_ext == EINA_FALSE)
	       val = wiki_parser_parse_part(root, arg->val, arg->val_size, pargs, &error);
	    else
	       val = _wiki_parser_template_replace_arg_by_val3(root, arg->val, pargs, '|', 0);

	    arg->val = NULL;
	    size = 0;

	    if(val) {
	       arg->val = val;
	       arg->val = trimn_u(arg->val, u_strlen(val), &size);
            }
	    arg->val_size = size;
	 } else if(arg->have_equal) {
	    size = arg->val_size;
	    arg->val = trimn_u(arg->val, size, &arg->val_size);
	 }
      }
   }

   return args;
}

Wiki_Args *wiki_parser_wiki_args_free(Wiki_Args *args)
{
   if(args) {
      if(args->hash)
	 eina_hash_free(args->hash);
      if(args->list) {
	 Wiki_Template_Arg *arg;
	 EINA_LIST_FREE(args->list, arg) {
	    if(arg) {
	       if(arg->key)
		  free(arg->key);
	       if(arg->val)
		  free(arg->val);
	       free(arg);
	    }
	 };
      }
      free(args);
   }

   return NULL;
}

Wiki_Args *wiki_parser_wiki_args_get(Wiki_Parser *root, Wiki_Args *pargs, UChar *buf, Eina_Bool is_expr)
{
   Wiki_Args *args = NULL;

   args = calloc(1, sizeof(Wiki_Args));
   if(! args) return NULL;

   args->list = wiki_parser_template_args_list_get2(buf, is_expr);
   args->list = _wiki_parser_parse_args(root, pargs, args->list, is_expr);
   args->list = wiki_parser_args_sort_uniq(args->list, is_expr);
   if(args->list) {
      args->hash = _wiki_parser_args_list_to_hash(args->list, is_expr);
   }
   return args;
}

static UChar *
wiki_parser_variable_value_get(Wiki_Parser *root, const Wiki_Parser_Find *wpf, Wiki_Args *args)
{
   UChar *name = NULL, *content = NULL;
   Wiki_Template_Arg *arg = NULL;
   Eina_Bool val_set = EINA_FALSE;

   PARSE_TRACE();

   if(! root) return NULL;
   if(! wpf) return NULL;

   if(wpf->curl_count != 3) return NULL;
   name = u_strndup(wpf->start + 3, ((wpf->pipe) ? wpf->pipe : wpf->end - 3) - (wpf->start + 3));
   name = trimn_u(name, ((wpf->pipe) ? wpf->pipe : wpf->end - 3) - (wpf->start + 3), NULL);

   if(name && args && args->hash) {
      arg = icu_eina_hash_find(args->hash, name);
      if(arg) {
	 ulog("NAMED: %S => %S\n", arg->key, arg->val);
	 if(arg->have_equal) // named parameter
	    content = u_strndup(arg->val, arg->val_size);
	 else // numbered parameter
	    content = u_strndup(arg->key, arg->key_size);
	 val_set = EINA_TRUE;
      }
   }

   if(val_set == EINA_FALSE) { // default value
      if(wpf->pipe) {
	 content = u_strndup(wpf->pipe + 1, (wpf->end - 3) - (wpf->pipe  + 1));
      } else {
	 content = u_strndup(wpf->start, wpf->end - wpf->start);
      }
   }

   if(name && content && u_strstr(content, ICU_STR_TRI_CURL_OPEN)) {
      char *tmp = u_strToUTF8_new(name);
      if(tmp) {
	 char *reg = NULL, *replace = NULL;
	 tmp = pcre_replace("/(\\{|\\}|\\[|\\]|\\+|\\*|\\?|\\(|\\)|\\|)/", "\\\\\1", tmp);
	 asprintf(&replace, "&#123;&#123;&#123;%s&#125;&#125;&#125;", tmp);
	 asprintf(&reg, "\\{\\{\\{[ \n\r\t]?(%s)[ \n\r\t]?\\}\\}\\}", tmp);
	 ulog("MASK: %S\n", content);
	 if(reg) {
	    content = icu_replace_utf8(reg, replace, content, 0);
	    free(reg);
	 }
	 if(replace)
	    free(replace);
	 free(tmp);
      }
   }

   ulog("REPLACE BY:%d:%s:%s, %S => %S\n", (arg) ? arg->have_equal : 0, (arg && arg->have_equal) ? "val" : "key", (wpf->pipe) ? "have pipe" : "no pipe", name, content);

   if(name)
      free(name);

   return content;
}

static UChar *_wiki_parser_template_replace_arg_by_val3(Wiki_Parser *root, UChar *buf, Wiki_Args *args, UChar separator, Eina_Bool is_ext)
{
   Wiki_Parser_Find wpf;
   Eina_List *replace = NULL;
   Eina_Bool have_replace = EINA_TRUE;
   size_t skip = 0i, size = 0;

   PARSE_TRACE();

   if(! root) return NULL;
   if(! buf) return NULL;

   size = u_strlen(buf);
   do {
      while(wiki_parser_template_first_find2(&wpf, buf + skip, 3) == EINA_TRUE) {
	 UChar *content = NULL;
	 Eina_Bool changed = EINA_FALSE;

	 if(wpf.curl_count > 3) {
	    content = wiki_parser_match_curl_open_sup_3(root, &wpf, args, &changed);
	 } else if(wpf.curl_count == 3) {
	    content = wiki_parser_match_curl_open_equal_3(root, &wpf, args, &changed);
	 }

	 if(wpf.start && changed == EINA_TRUE) {
	    TR *d = malloc(sizeof(TR));
	    if(d) {
	       d->content = content;
	       d->size = (content) ? u_strlen(content) : 0;
	       d->s = wpf.start - buf;
	       d->e = (wpf.end) ? wpf.end - buf : u_strlen(wpf.start) + 1;
	       replace = eina_list_append(replace, d);
	       content = NULL;
	       if(d->e && d->e > size) {
		  uprintf("TRY REPLACE: from %d to %d (%d or %d)width (%p) : %S by %S\n", d->s, d->e, size, u_strlen(buf), wpf.end, buf, content);
		  exit(3);
	       }
	    }
	 }

      	 if(content) {
	    free(content);
	    content = NULL;
	 }

	 if(wpf.end)
	    skip = (wpf.end - buf);
	 else
	    break;
	 ulog("skip: %d, %d\n", skip, size);
      }

      if(replace && eina_list_count(replace) > 0) { 
	 ulog("BEFORE UPDATE: %S\n", buf);
	 buf = _update_buffer(buf, size, &replace, &size);
	 skip = 0;
	 ulog("MODTO:\n%S\n", buf);
      } else {
	 if(! wpf.end || wpf.end < buf + size)
	    have_replace = EINA_FALSE;
	 else
	    skip = (wpf.end - buf);
	 if(replace) {
	    TR *d;
	    EINA_LIST_FREE(replace, d) {
	       if(d) {
		  if(d->content)
		     free(d->content);
		  free(d);
	       }
	    };
	 }
      }

   } while(have_replace == EINA_TRUE);

   return buf;
}

UChar *_wiki_parser_template_name_full_get(Wiki_Parser *root, UChar *name, Eina_Bool redirect)
{
   UChar *title = NULL;
   Wiki_Title *wt = NULL;
   const Wiki_Namespace *wns = NULL;

   PARSE_TRACE();
   if(! root) return title;
   if(! name) return title;

   if(icu_search("\\.\\.", name, 0)) {
      char *str = u_strToUTF8_new(name);
      if(str) {
	 char *path = parser_title_rel2abs(root->wd, str, wiki_parser_fullpagename_get(root));
	 if(path) {
	    UChar *tmp = u_strdupC(path);
	    if(tmp) {
	       wt = parser_title_part(root->wd, tmp);
	       free(tmp);
	    }
	    free(path);
	 }
	 free(str);
      }
   } else
      wt = parser_title_part(root->wd, name);
   if(! wt) return NULL;

   if(wt->ns && ! wt->title) {
      wt->title = wt->ns;
      wt->ns = NULL;
   }
   if(wt->ns) {
      char *ns = u_strToUTF8_new(wt->ns);
      if(ns) {
	 wns = eina_hash_find(root->wd->namespaces, ns);
	 free(ns);
      }
   }

   if(! wns)
      wns = eina_hash_find(root->wd->namespaces, "0");

   if((wns && wns->key == WIKI_NS_TEMPLATE) || redirect == EINA_TRUE) {
      title = parser_title_with_namespace(wt);
   } else if((wns && wns->key != WIKI_NS_MAIN && wt->title)) { // || (wt->ns && ! wt->title)) {
      title = parser_title_with_namespace(wt);
   } else if(wt->title) {
      if(wt->title[0] == '/') {
	 title = calloc(strlen(wiki_parser_fullpagename_get(root)) 
	       + u_strlen(wt->title) + 1, sizeof(UChar));
	 if(title)
	    u_sprintf(title, "%s%S", 
		  wiki_parser_fullpagename_get(root), wt->title);
      } else if(wt->intext == EINA_TRUE) {
	 title = u_strdup(wt->title);
      } else {
	 const char *template = wiki_parser_namespace_name_get(root->wd, WIKI_NS_TEMPLATE);
	 if(template) 
	    title = calloc(strlen(template) + u_strlen(wt->title) + 2, 
		  sizeof(UChar));
	 if(title)
	    u_sprintf(title, "%s:%S", template, wt->title);
      }
   }

   ulog("GET TEMPLATE REAL NAME: %S => %S [%S:%S]\n", name, title, wt->ns, wt->title);
   parser_title_free(wt);

   return title;

}

Wiki_Template_Loaded* wiki_parser_template_load(Wiki_Parser *root, UChar *tname, Eina_Bool redirect)
{
   Wiki_Template_Loaded *wtl = NULL;
   UChar *title = NULL;
   char *title_str = NULL;
   size_t size = 0;
   struct timeval ts, te, tr;
   gettimeofday(&ts, NULL);

   PARSE_TRACE();

   if(! root) return wtl;
   if(! root->templates) return wtl;
   if(! tname) return wtl;

   title_str = u_strToUTF8_new(tname);
   if(! title_str) return NULL;

   wtl = icu_eina_hash_find(root->templates, tname);
   if(! wtl) {
      title = _wiki_parser_template_name_full_get(root, tname, redirect);

      if(title) {
	 char *t = u_strToUTF8_new(title);
	 if((wtl = icu_eina_hash_find(root->templates, title)) == NULL) {
	    ulog("TEMPLATE LOAD: %S, current : %S\n", title, root->current_template);
	    wtl = calloc(1, sizeof(Wiki_Template_Loaded));
	    wtl->content = NULL;
	    wtl->usage_count = 0;
	    wtl->is_template = EINA_FALSE;
	    wtl->in_error = EINA_FALSE;
	    char *tpl_content = root->wd->template_content_get(root->wd, t, &size);
	    if(tpl_content != NULL) {
	       wtl->content = u_strdupC(tpl_content);
	       wtl->title = u_strdup(title);
	       icu_eina_hash_add(root->templates, title, wtl);
	       root->current_template = wtl->title;
	       root->is_template = EINA_TRUE;
	       wtl->is_template = EINA_TRUE;
	    } else {
	       ulog("TEMPLATE NOT FOUND:%d: '%S'\n", __LINE__, title);
	       root->is_template = EINA_FALSE;
	       wtl->title = u_strdup(title);
	       icu_eina_hash_add(root->templates, title, wtl);
	       root->current_template = wtl->title;
	       root->templates_not_found++;
	    }
	    if(tpl_content)
	       free(tpl_content);
	    
	 }
	 free(t);
      } else {
	 ulog("TEMPLATE NOT FOUND:%d: %S\n", __LINE__, tname);
	 root->is_template = EINA_FALSE;
	 root->templates_not_found++;
      }
   }

   if(title)
      free(title);
   if(title_str)
      free(title_str);

   if(wtl)
      root->is_template = wtl->is_template;

   gettimeofday(&te, NULL);
   timersub(&te, &ts, &tr);
   timeradd(&root->time_load, &tr, &ts);
   root->time_load.tv_sec = ts.tv_sec;
   root->time_load.tv_usec = ts.tv_usec;

   return wtl;
}

static void wiki_parser_template_error_set(Wiki_Parser *root, Wiki_Template_Loaded *wtl, int error)
{
   const char *msg = NULL;

   PARSE_TRACE();

   if(! root) return;
   if(! wtl) return;
   if(wtl->in_error == EINA_TRUE) return;

   switch(error) {
      case WIKI_PARSER_ERROR_TEMPLATE_LOOP:
	 msg = "<span class=\"error\">Template <b>loop</b> detected in : <b>%S</b></span>"; 
	 break;
      case WIKI_PARSER_MAX_TEMPLATE_COUNT:
	 msg = "<span class=\"error\">Template max <b>usage</b> count in : <b>%S</b></span>"; 
	 break;
      case WIKI_PARSER_MAX_REDIRECT_COUNT:
	 msg = "<span class=\"error\">Template max <b>redirect</b> in : <b>%S</b></span>"; 
	 break;
      default:
	 msg = "<span class=\"error\">Unknow error in template: <b>%S</b></span>";
   }

   if(wtl->content) {
      free(wtl->content);
      wtl->content = NULL;
   }

   if(msg && wtl->title) {
      wtl->content = calloc(strlen(msg) + u_strlen(wtl->title) + 1, sizeof(UChar));
      if(wtl->content)
	 u_sprintf(wtl->content, msg, wtl->title);
   }

   wtl->in_error = EINA_FALSE;
}

UChar *wiki_parser_template_add(Wiki_Parser *root, UChar *tname, Wiki_Args *pargs, Wiki_Args *args, Eina_Bool redirect, Eina_Bool parse)
{
   Wiki_Template_Loaded *wtl = NULL;
   UChar *content = NULL;
   UChar dbt[256];
   u_uastrcpy(dbt, "Template:Overlay/main");
//   u_uastrcpy(dbt, "Portail:Félins/Cadre");

   PARSE_TRACE();

   if(!tname) return NULL;

   wtl = wiki_parser_template_load(root, tname, redirect);

   if(wtl && (wtl->content || wtl->redirect)) {
      struct timeval tv1, tv2, tv3;
      gettimeofday(&tv1, NULL);
      wtl->usage_count++;
      if(wtl->usage_count > WIKI_PARSER_TEMPLATE_USAGE_MAX) {
	 ulog("MAX TEMPLATE USAGE FOR: %S = %d\n", wtl->title, wtl->usage_count);
	 wiki_parser_template_error_set(root, wtl, WIKI_PARSER_MAX_TEMPLATE_COUNT);
      }

      if(root->templates_call && eina_array_count_get(root->templates_call) > 0) {
	 Wiki_Template_Loaded *prev = NULL;
	 int i = 0, cnt = 0;
	 for(i = 0; i < eina_array_count_get(root->templates_call); i++) {
	    prev = eina_array_data_get(root->templates_call, i);
	    if(prev == wtl) {
	       cnt++;
	       if(cnt > 6)
		  wiki_parser_template_error_set(root, wtl, WIKI_PARSER_ERROR_TEMPLATE_LOOP);
	    }
	 } 
      }

      if(root->templates_call)
	 eina_array_push(root->templates_call, wtl);

      if(wtl->usage_count == 1) {
	 wtl->content = wiki_parser_template_comment_remove(root, wtl->content);
   	 wtl->content = wiki_parser_tag_content_get(wtl->content, "onlyinclude");
   	 wtl->content = wiki_parser_tag_and_content_remove(wtl->content, "noinclude");
	 wtl->content = wiki_parser_tag_name_remove(wtl->content, "includeonly");
	 wtl->content = wiki_parser_template_nowiki_replace(root, wtl->content);
	 if(u_strcmp(wtl->title, dbt) == 0) {
	    uprintf("BEFORE STATIC: %S \n", wtl->content);
	 }
	 wtl->content = wiki_parser_template_static(root, wtl->content);
	 wtl->content_size = (wtl->content) ? u_strlen(wtl->content) : 0;

	 if(wtl->content && redirect == EINA_FALSE &&
	       icu_search("^([ \n]*)#redirect", wtl->content, UREGEX_CASE_INSENSITIVE)) {
	    ulog("REDIRECT MATCH: %S => %S\n", tname, wtl->content);
	    wtl->content = icu_replace_utf8("^[^\\[]*\\[\\[([^]]*)\\]\\](.*)", "$1", trim_u(wtl->content), 0);
	    wtl->content = icu_replace("\\n.*", ICU_STR_EMPTY, wtl->content, 0);
	    wtl->content = icu_replace("|.*", ICU_STR_EMPTY, wtl->content, 0);
	    ulog("REDIRECT RES: %S\n", wtl->content);
	    if(wtl->content) {
	       wtl->redirect = wtl->content;
	       wtl->content = NULL;
	       wtl->content_size = 0;
	    }
	 }
      }

      if(wtl->redirect && redirect == EINA_FALSE) {
	 if(wtl->redirect_count >= WIKI_PARSER_REDIRECT_COUNT_MAX) {
	    wiki_parser_template_error_set(root, wtl, WIKI_PARSER_MAX_REDIRECT_COUNT);
	 } else {
	    wtl->redirect_count++;
	    content = wiki_parser_template_add(root, wtl->redirect, pargs, args, EINA_TRUE, parse);
	    if(root->is_template == EINA_TRUE)
	       wtl->redirect_count--;
	    else {
	       if(content)
		  free(content);
	       content = NULL;
	    }
	 }
      } else if(wtl->content) {
	 int error = 0;
	 content = u_strndup(wtl->content, wtl->content_size);
	 if(parse == EINA_TRUE)
	    content = wiki_parser_parse_part(root, content, wtl->content_size, args, &error);
      }

      ulog("IN TEMPLATE : %S, count: %d, redirect: %d\n", wtl->title, wtl->usage_count, wtl->redirect_count);

      if(root->templates_call)
	 eina_array_pop(root->templates_call);

      if(! wtl->redirect) {
	 gettimeofday(&tv2, NULL);
	 timersub(&tv2, &tv1, &tv3);
	 timeradd(&tv3, &wtl->tv, &tv1);
      }
      wtl->tv.tv_sec = tv1.tv_sec; 
      wtl->tv.tv_usec = tv1.tv_usec; 
   } else if (! wtl) {
      ulog("TEMPLATE_NOT_FOUND: %S\n", tname);
      root->templates_not_found++;
      root->is_template = EINA_FALSE;
      if(wtl) {
	 content = calloc(u_strlen(wtl->title) + 5, sizeof(UChar));
	 u_sprintf(content, "[[%S]]", wtl->title);
      } else {
	 content = calloc(u_strlen(tname) + 5, sizeof(UChar));
	 u_sprintf(content, "[[%S]]", tname);
      }
   }

   if(wtl && wtl->redirect_count > 0)
      wtl->redirect_count--;

   return content;
}

UChar *wiki_parser_uniq_string_get(Wiki_Parser *wp, const char *tagname)
{
   UChar *str = NULL;
   size_t size = 0;
   int r1, r2;
  
   if(! wp) return str;
   if(! tagname) return str;

   srandom(getpid());

   r1 = random();
   r2 = random();

   size = strlen(tagname);
   str = calloc(200 + size + 1, sizeof(UChar));
   do {
      if(str) {
	 wp->nowiki_num++;
	 u_sprintf(str, WIKI_UNIQ_STRING_FORMAT, r1, r2, tagname, wp->nowiki_num);
      }
   } while(str && icu_eina_hash_find(wp->nowiki, str));

   return str;
}

UChar *
wiki_parser_template_nowiki_replace(Wiki_Parser *wp, UChar *content)
{
   UChar entity[50];

   PARSE_TRACE();

   if(! wp) return content;
   if(! content) return NULL;

//   content = icu_replace("<nowiki[^>]*/>", ICU_STR_EMPTY, content, UREGEX_CASE_INSENSITIVE);
   u_sprintf(entity, "%s", "</$1>");
   content = icu_replace("<([^>]*)\\\\>", entity, content, UREGEX_CASE_INSENSITIVE);
   content = icu_replace("<\\\\([^>]*)>", entity, content, UREGEX_CASE_INSENSITIVE);
   u_sprintf(entity, "%s", "<$1/>");
   content = icu_replace("</(br|hr)>", entity, content, UREGEX_CASE_INSENSITIVE);

   if(content) {
      const Eina_Hash *tags = NULL;
      const UChar *p = content, *s = NULL, *e = NULL;
      size_t fsize = 0, size = 0;

      tags = wiki_lexer_tags_get();
      const UChar *close_tag = ICU_STR_CLOSE_TAG;


      fsize = u_strlen(content);
      size = fsize;

      while(p && *p) {
	 if(*p == '<') { 
//      size_t len = u_strlen(ptr);
	    s = p + 1;
	    if(tags) {
	       e = s;
	       while(e && *e
		     && ! ( u_isWhitespace(*e) || *e == '/' || *e == '>'))
		  e++;
	       if(s && *e) {
		  char *tag = u_strnToUTF8_new(s, e - s);
		  const Wiki_Parser_Tag_Extension *te = NULL;
		  if(tag && (te = eina_hash_find(tags, tag)) 
			&& te->type == WIKI_PARSER_TAG_CONTENT_PARSE_NONE) {
		     UChar *tu = u_strdupC(tag);
		     const UChar *sa = e;
		     Wiki_Nowiki_Part *wnp = calloc(1, sizeof(Wiki_Nowiki_Part));
		     if(wnp) {
			wnp->tagname = te->name;
			wnp->tagname_size = e - s;
		     }
		     if(u_isWhitespace(*e)) {
			while(*e && ! (*e == '/' || *e == '>'))
			   e++;
		     }
		     if(e > sa && wnp) {
			wnp->args = u_strndup(sa, e - sa);
			wnp->args_size = e - sa;
		     }
		     ulog("Found tag %s with '%C'\n", tag, *e);
		     if(tu && *e != '/') {
			const UChar *sc = e + 1;
			size_t l = u_strlen(tu);
			int skip = 0;
			while((e = u_strstr(p + skip, close_tag)) 
			      && u_strncmp(e + 2, tu, l) != 0)
			   skip = (e - p) + 1;
			if(e && u_strncmp(e + 2, tu, l) == 0) {
			   p = u_strstr(e, ICU_STR_ANGL_CLOSE);
			   if(p && wnp) {
			      wnp->content = u_strndup(sc, e - sc);
			      wnp->content_size = e - sc;
			      p++;
			   }
			}
		     } else  {
			p = u_strstr(e, ICU_STR_ANGL_CLOSE);
			if(p)
			   p++;
		     }
		     if(tu)
			free(tu);
		     if(wnp) {
			if(wp->nowiki && p) {
			   UChar *key = wiki_parser_uniq_string_get(wp, wnp->tagname);
			   if(key) {
			      size_t klen = u_strlen(key), read = (s - 1) - content, part = p - (s - 1);
			      size_t nsize = 0, last = (size - (p - content) + 1);
			      icu_eina_hash_add(wp->nowiki, key, wnp);
			      if(0) {
				 uprintf("NOWIKI_PART: tag = %s, key = %S, args = %d : %d, content = %d : %d\n", wnp->tagname, key, (wnp->args) ? u_strlen(wnp->args) : 0, wnp->args_size, (wnp->content) ? u_strlen(wnp->content) : 0, wnp->content_size);
				 uprintf("\targs = %S, content = %S\n", wnp->args, wnp->content);
			      }
			      if(size + klen - part <= fsize) {
				 nsize = size + klen - part;
//				 printf("in place buf size: %d, new size: %d, fsize: %d, size: %d, klen: %d, read: %d, part: %d, last: %d\n", wp->size, nsize, fsize, size, klen, read, part, last);
				 size = nsize;
			      } else {
				 nsize = size + klen - part;
//				 printf("grow buf size: %d, new size: %d, fsize: %d, size: %d, klen: %d, read: %d, part: %d, last: %d\n", wp->size, nsize, fsize, size, klen, read, part, last);
				 content = realloc(content, nsize * sizeof(UChar) + 1);
				 content[nsize] = '\0';
				 if(! content)
				    return content;

				 fsize = size = nsize;
			      }

			      UChar *d = content + read;
//				 uprintf("last part: %d, %S\n", u_strlen(d + part), d + part);
			      if(klen > part) {
				 u_memmove(d + klen, d + part, last);
				 u_memcpy(d, key, klen);
			      } else if(klen < part) {
				 u_memcpy(d, key, klen);
				 u_memmove(d + klen, d + part, last);
			      } else {
				 u_memcpy(d, key, klen);
			      }

			      p = content + read;
			      if(0 && u_strlen(content) != size) {
				 uprintf("exit by size: %d, %S\n", u_strlen(content), content);
				 exit(0);
			      }

			      free(key);
			   } else
			      _wiki_parser_nowiki_part_free(wnp);
			} else {
			   _wiki_parser_nowiki_part_free(wnp);
			}
		     }
		  }
		  if(tag)
		     free(tag); 
	       } 
	    }
	 }
	 if(0 && p && ((p == content && *p == ' ') || (*p == '\n' && *(p + 1) == ' '))) {
	    const UChar *c = p + 1;
	    if(p != content)
	       p++;
	    while(*c == ' ') 
	       c++;
	    if(*c == '{' || *c == '|') {
	       u_memmove(content + (p - content), c, u_strlen(c) + 1);
	       size -= (c - p);
	    }
	 }
	 if(p)
	    p++;
      }
   }

   return content;
}

#define DOUBLE_SQUARE_MUST_BE_CLOSED \
   if(*p && *p == '[') { \
      int scnt = 0; \
      const UChar *sc = p; \
      while(*sc == '[' && scnt < 2) { \
	 scnt++; sc++; \
      } \
      p = sc - 1; \
      if(scnt == 2) {\
	 while(*sc && scnt > 0) { \
	    if(*sc == ']' && *(sc + 1) == ']') { \
	       scnt -= 2; sc++; \
	    } else if(*sc == '[' && *(sc + 1) == '[') { \
	       scnt += 2; sc++; \
	    } \
	    sc++; \
	 } \
	 if(scnt == 0) { \
	    p = sc - 1; \
	 } \
      } \
   }

Eina_List *wiki_parser_template_args_list_get2(UChar *buf, Eina_Bool expr)
{
   Eina_List *l = NULL; 
   Wiki_Template_Arg *arg = NULL;
   const UChar *s = NULL, *p = NULL;
   size_t size = 0, len = 0;
   off_t  offset = 0;
   const char spipe[] = "|", scolon[] = ":";
   const char *search = spipe;
   int in_sub_tag = 0, in_extern_link = 0, in_tag = 0;
   Eina_Bool set_val = EINA_FALSE;

   PARSE_TRACE();

   if(! buf) return NULL;

   if(expr == EINA_TRUE) search = scolon;

   p = buf;
   len = u_strlen(buf);
   while(p && p < (buf + len)) {
//      ulog("CHECK: %d, %C\n", in_sub_tag, *p);
      switch(*p) {
	 case ' ':
	    if((expr || set_val) && size == 0) {
	       offset++;
	    } else
	       size++;
	    break;
	 case '<':
	    /* 1 - skip open even if not know
	     * 2 - default
	     */
	    in_tag = 0;
	    if(! in_sub_tag && *(p + 1) && u_isalpha(*(p + 1))) {
	       in_tag++;
	       size++;
	    } else {
	       size++;
	    }
	    
	    break;
	 case '>':
	    if(! in_sub_tag && in_tag)
	       in_tag--;
	    size++;
	    break;
	 case '[':
	    s = p;
	    DOUBLE_SQUARE_MUST_BE_CLOSED;
	    if(s == p && icu_search("^\\[[a-z]+://", p , 0)) {
	       while(*p && ! (*p == ']' || *p == ' '))
		  p++;
	       if(! *p)
		  p = s;
	    }
	    size += (p - s) + 1;
	    break;
	 case '{':
	    if(*(p + 1) == '{') {
	       Wiki_Parser_Find wpf;
	       wiki_parser_template_first_find2(&wpf, p, 2);
	       if(wpf.start && wpf.end) {
		  size += wpf.end - wpf.start;
		  p = (wpf.end - 1);
	       } else 
		  size++;
	    } else if(*(p + 1) == '|' && (
		     ((p - 1) >= buf && *(p - 1) == '\n') 
		     || (expr == EINA_TRUE && *(p - 1) == ':'))) {
	       in_sub_tag += 2;
	       size += 2;
	       p++;
	    } else
	       size++;
	    break;
	 case '|':
	    in_extern_link = 0;
	    in_tag = 0;
	    if(in_sub_tag) {
	       if(*(p + 1) == '}' && (p == buf || *(p - 1) == '\n')) {
		  in_sub_tag -= 2;
		  size += 2;
		  p++;
	       } else
		  size++;
	    } else {
	       if(set_val) {
		  arg->val = u_strndup(buf + offset, size);
		  arg->val = trimn_u(arg->val, size, &arg->val_size);
		  set_val = EINA_FALSE;
	       } else if(arg) {
		  arg->key = u_strndup(buf + offset, size);
		  arg->key_size = (arg->key) ? size : 0;
	       }
	       arg = calloc(1, sizeof(Wiki_Template_Arg));
	       offset = (p - buf) + 1;
	       size = 0;
	       l = eina_list_append(l, arg);
	    }
	    break;
	 case '=':
	    if(in_extern_link || in_tag || in_sub_tag || ! arg || set_val)
	       size++;
	    else {
	       arg->key = u_strndup(buf + offset, size);
	       arg->key = trimn_u(arg->key, size, &arg->key_size);
	       arg->have_equal = 1;
	       size = 0;
	       offset = (p - buf) + 1;
	       set_val = EINA_TRUE;
	    }
	    break;
	 case ':':
	    if(search == scolon) {
	       ulog("COLON: %S\n", p);
	       arg = calloc(1, sizeof(Wiki_Template_Arg));
	       offset = (p - buf) + 1;
	       size = 0;
	       l = eina_list_append(l, arg);
	       search = spipe;
	    } else
	       size++;
	    break;
	 default:
	    size++;
      }
      p++;
   }

   if(arg && (offset + size) <= len) {
      if(set_val && ! arg->val) {
	 arg->val = u_strndup(buf + offset, size);
	 arg->val = trimn_u(arg->val, size, &arg->val_size);
      } else if(! arg->key) {
	 arg->key = u_strndup(buf + offset, size);
	 arg->key_size = (arg->key) ? size : 0;
      }
   }

   if((0 || output_log) && l) {
      Eina_List *n;
      EINA_LIST_FOREACH(l, n, arg) {
	 if(arg) {
	    uprintf("ARGS: expr: %d, have_equal = %d, key = %S, val = %S\n",
		  expr, arg->have_equal, arg->key, arg->val);
	 }
      }
   }

//   printf("EGET2: %d\n", time(NULL));
   return l;
}

Eina_Bool
wiki_parser_template_first_find2(Wiki_Parser_Find *wpf, const UChar *buf, int num)
{
   const UChar *s = NULL, *e = NULL, *pipe = NULL;
   const UChar *p = NULL;
   const UChar *topen;
   int curl_open_all = 0, curl_cnt = 0, curl_open_cnt = 0, curl_close_cnt = 0;
   int last_close = 0;

   PARSE_TRACE();

   if(! wpf) return EINA_FALSE;
   if(! buf) return EINA_FALSE;

   wpf->start = NULL;
   wpf->end = NULL;
   wpf->pipe = NULL;
   wpf->curl_count = 0;

   if(num == 3) 
      topen = ICU_STR_TRI_CURL_OPEN;
   else
      topen = ICU_STR_DBL_CURL_OPEN;
   s = u_strstr(buf, topen);

   curl_open_all = 0;
   curl_cnt = 0;
   e = NULL;
   if(s) {
      int open_tab = 0, last_open = 0;
      p = s;
      while(*p == '{') {
	 p++;
	 curl_open_all++;
      }

      curl_cnt = curl_open_all;
      curl_open_cnt = 0;
      while(p && *p && e == NULL) {
	 if(*p == '{') {
	    int n = 0;
	    last_open = 0;
	    while(*p == '{') {
	       n++;
	       p++;
	    }
	    last_open = n;
	    if(n > 1) {
	       curl_open_cnt += n;
	    } else if(n == 1 && *p == '|' && ((p - 2) == s || *(p - 2) == '\n')) {
	       ulog("OPEN_TAB\n");
	       open_tab = 1;
	       p++;
	    }
	 } else if(*p == '{')
	    p++;
	 if(*p == '}' && *(p + 1) == '}') {
	    curl_close_cnt = 0;
	    last_close = 0;
	    while(*p == '}') {
	       last_close++;
	       last_open--;
	       if(curl_open_cnt > 0)
		  curl_open_cnt--;
	       else
		  curl_close_cnt++;
	       p++;
	    }

	    if(last_open == 1 && curl_close_cnt == 0 && curl_open_cnt) {
//	       uprintf("Acurl_cnt_all : %d, curl_cnt : %d, last open: %d, last close: %d => curl_open_cnt: %d, curl_close_cnt: %d\n", curl_open_all, curl_cnt, last_open, last_close, curl_open_cnt, curl_close_cnt);
	       curl_open_cnt--;
	    }
	    if(last_open == -1 && last_close >= 3 && curl_open_cnt) {
//	       uprintf("Bcurl_cnt_all : %d, curl_cnt : %d, last open: %d, last close: %d => curl_open_cnt: %d, curl_close_cnt: %d\n", curl_open_all, curl_cnt, last_open, last_close, curl_open_cnt, curl_close_cnt);
	       curl_open_cnt++;
	    }

	    if(open_tab && curl_open_cnt == 0 && curl_close_cnt >= 3) {
	       curl_close_cnt--;
	       open_tab = 0;
	    }

	    if(curl_open_cnt == 0 && curl_close_cnt > 1) {
	       int n = curl_close_cnt - curl_cnt;
//	       uprintf("curl all: %d, curl open: %d, curl close: %d, curl cnt: %d => %d, %d\n", curl_open_all, curl_open_cnt, curl_close_cnt, curl_cnt, n, 0);
	       if(curl_close_cnt <= curl_cnt)
		  curl_cnt -= curl_close_cnt;
	       else {
		  p -= n;
		  curl_cnt = 0;
	       }
	       if(curl_cnt == 1) {
//		  printf("FORCE BREAK\n");
		  if(curl_open_all > 2) {
		     s++;
		     curl_open_all--;
		  } else
		     p--;
		  curl_cnt = 0;
		  e = p;
		  break;
	       }
	    } 
	 } 
	 if(! pipe && *p == '|' && curl_open_cnt == 0 && (curl_cnt == 2 || curl_cnt == 3)) {
	    pipe = p;
	 }
	 if(curl_cnt == 0) {
	    e = p ;
	 } else {
	    DOUBLE_SQUARE_MUST_BE_CLOSED;
	    if(*p && ! (*p == '{'/* && *(p + 1) == '{' */))
	       p++;
	 }
//   uprintf("TEMPLATE TRY: %C, curl_open_all = %d, curl_cnt = %d, curl_open_cnt = %d, curl_close_cnt = %d\n", *p, curl_open_all, curl_cnt, curl_open_cnt, curl_close_cnt);
      }
   }

   if(curl_open_all == 4) {
      curl_open_cnt = 2;
   } else if(curl_open_all == 5) {
//      uprintf("CURL: curl_open_all = %d, curl_cnt = %d, curl_open_cnt = %d, curl_close_cnt = %d\n", curl_open_all, curl_cnt, curl_open_cnt, curl_close_cnt);
      if(curl_close_cnt == 3) {
	 curl_open_cnt = 3;
      } else
	 curl_open_cnt = 2;
   } else if(curl_open_all > 5) {
//      uprintf("CURL: curl_open_all = %d, curl_cnt = %d, curl_open_cnt = %d, curl_close_cnt = %d\n", curl_open_all, curl_cnt, curl_open_cnt, curl_close_cnt);

      if(curl_close_cnt > 1 && curl_close_cnt < 4)
	 curl_open_cnt = curl_close_cnt;
      else
	 curl_open_cnt = 3; // curl_open_all;
   } else {
      curl_open_cnt = curl_open_all;
   }

   ulog("TEMPLATE TRY: curl_open_all = %d, curl_cnt = %d, curl_open_cnt = %d, curl_close_cnt = %d\n", curl_open_all, curl_cnt, curl_open_cnt, curl_close_cnt);
   if(s && ! e) {
      ulog("UNMATCH: %S\n", s);
   }

   if(s && e && curl_open_cnt > 0) {
      if(output_log) {
	 UChar *tmp = NULL;
	 if(e) 
	    tmp = u_strndup(s, e - s);
	 else
	    tmp = u_strdup(s);
	 if(tmp) {
	    tmp = icu_replace_utf8("\n", "\\\\n", tmp, 0);
	    if(pipe) {
	       UChar *p = u_strndup(s, pipe - s);
	       if(p) {
		  p = icu_replace_utf8("\n", "\\\\n", p, 0);
		  ulog("FIND TEMPLATE: %S (before pipe: %S)\n", tmp, p);
		  free(p);
	       } else {
		  ulog("FIND TEMPLATE: %S (before pipe: %S)\n", tmp, p);
	       }
	    } else
	       ulog("FIND TEMPLATE: %S\n", tmp);
	    free(tmp);
	 }
      }
      wpf->start = s;
      wpf->end = e;
      wpf->pipe = pipe;
      wpf->curl_count = curl_open_cnt;
      return EINA_TRUE;
   } else if(s) {
      wpf->end = s + 1; // curl_open_cnt;
      return EINA_TRUE;
   }

   return EINA_FALSE;
}

static UChar *
wiki_parser_match_curl_open_sup_3(Wiki_Parser *root, const Wiki_Parser_Find *wpf, Wiki_Args *args, Eina_Bool *changed)
{
   UChar *tmp = NULL, *content = NULL;
   const UChar *s = NULL, *e = NULL;
   int error = 0;

   PARSE_TRACE();

   if(! root) return NULL;
   if(! wpf) return NULL;

   s = wpf->start + 3;
   if(wpf->end) {
      e = wpf->end - 3;
      content = u_strndup(s, e - s);
   } else {
      content = u_strdup(wpf->start);
   }
   if(content) {
      tmp = u_strdup(content);
      content = wiki_parser_parse_part(root, content, wpf->end - wpf->start, args, &error);
      if(tmp && content) {
	 if(u_strcmp(tmp, content) != 0) {
	    *changed = EINA_TRUE;
	 }
      }
      if(tmp)
	 free(tmp);
   }

   return content;
}

static UChar *
wiki_parser_match_curl_open_equal_3(Wiki_Parser *root, const Wiki_Parser_Find *wpf, Wiki_Args *args, Eina_Bool *changed)
{
   UChar *tmp = NULL, *content = NULL;
   const UChar *e = NULL;
   int error = 0;

   PARSE_TRACE();

   if(! root) return NULL;
   if(! wpf) return NULL;

   e = (wpf->end) ? wpf->end - 3 : wpf->start + u_strlen(wpf->start) + 1;

   if(u_strFindFirst(wpf->start + 3, e - (wpf->start + 3), ICU_STR_DBL_CURL_OPEN, -1)) {
      UChar *t = u_strndup(wpf->start + 3, e - (wpf->start + 3));
      if(t) {
	 t = wiki_parser_parse_part(root, t, u_strlen(t), args, &error);
	 // t = trim_u(t);
	 if(t) {
	    tmp = calloc(u_strlen(t) + 7, sizeof(UChar));
	    if(tmp) {
	       u_sprintf(tmp, "{{{%S}}}", t);
	    }
	 }
	 if(t)
	    free(t);
	 if(tmp) {
	    Wiki_Parser_Find w;
	    w.start = tmp;
	    w.end = w.start + u_strlen(tmp);
	    w.pipe = u_strFindFirst(tmp, -1, ICU_STR_PIPE, -1);
	    w.curl_count = 3;
	    content = wiki_parser_variable_value_get(root, &w, args);

	    if((content && u_strcmp(content, tmp) != 0) || !content)
	       *changed = EINA_TRUE;
	    free(tmp);
	    return content;
	 }
      }
   }

   tmp = u_strndup(wpf->start, wpf->end - wpf->start);
   content = wiki_parser_variable_value_get(root, wpf, args);
   if(content) {
      if(content && tmp && u_strcmp(tmp, content) != 0) 
	    *changed = EINA_TRUE;
   } else
      *changed = EINA_TRUE;

   if(tmp)
      free(tmp);

   return content;
}

static void *
wiki_parser_title_is_extension(Wiki_Parser *wp, const UChar *title, UChar **extension, Eina_Bool have_pipe)
{
   Eina_Bool is_ext = EINA_FALSE;
   UChar *ext_name = NULL;
   void *parser_function = NULL;

   PARSE_TRACE();

   *extension = NULL;
   if(! parser_extension_modules) {
      return NULL;
   }

   UChar *c = u_strstr(title, ICU_STR_COLON);
   if(c && c > title) {
      ext_name = u_strndup(title, c - title);
   } else if (have_pipe == EINA_FALSE) {
      ext_name = u_strdup(title);
   }

   if(wiki_languages && ext_name && wp->wd->lang) {
      size_t size = (c) ? ((c + 1) - title) : u_strlen(ext_name);
      ext_name = wiki_parser_intl_get(ext_name, &size);
   }

   if(ext_name) {
      parser_function = icu_eina_hash_find(parser_extension_modules, ext_name);
      if(parser_function)
	 is_ext = EINA_TRUE;
      else {
	 ext_name = u_strtolower(ext_name);
	 parser_function = icu_eina_hash_find(parser_extension_modules, ext_name);
	 if(parser_function)
	    is_ext = EINA_TRUE;
      }
   }
   if(is_ext == EINA_FALSE && ext_name) {
      free(ext_name);
      ext_name = NULL;
   }

   *extension = ext_name;
   return parser_function;
}

static UChar *
wiki_parser_title_expand(Wiki_Parser *root, const Wiki_Parser_Find *wpf, UChar *title, Wiki_Args *args, UChar **set_params)
{
   int error = 0;
   UChar *tmp = NULL;

   PARSE_TRACE();

   if(! root) return NULL;
   if(! wpf) return NULL;
   if(! title) return NULL;

   tmp = wiki_parser_parse_part(root, title, u_strlen(title), args, &error);
   if(tmp) {
      title = tmp;
      ulog("TEMPLATE TITLE 2: %S\n", title);
      title = icu_replace("\\n", ICU_STR_EMPTY, title, 0);
      title = icu_replace("_", ICU_STR_SPACE, title, 0);
      title = trim_u(title);
      if(title && title[0] != '[') {
	 const UChar *arg = NULL;
	 if((arg = icu_search("(\\n)", title, 0))) {
	    title[arg - title] = '\0';
	 }
	 title = trim_u(title);
      }
   }

   return title;
}

static UChar *
wiki_parser_expand_template_or_function(Wiki_Parser *root, const Wiki_Parser_Find *wpf, Wiki_Args *pargs, Eina_Bool *changed)
{
   UChar *(*parser_function)(Wiki_Parser *, Wiki_Args *, Wiki_Args *, void *) = NULL;
   UChar *ext_name = NULL;
   Eina_Bool have_pipe = EINA_FALSE;
   UChar *content = NULL;
   Wiki_Args *args = NULL;
   const UChar *s = NULL, *e = NULL, *pipe = NULL;

   PARSE_TRACE();

   if(! root) return NULL;
   if(! wpf) return NULL;

   s = wpf->start;
   e = (wpf->end) ? wpf->end - 2 : s + u_strlen(s) + 1;

   UChar *title = NULL, *params = NULL;

   if(wpf->pipe == NULL) 
      pipe = e;
   else {
      pipe = wpf->pipe;
      have_pipe = EINA_TRUE;
   }

   title = u_strndup(s + 2, pipe - (s + 2));
   title = trimn_u(title, pipe - (s + 2), NULL);
   title = icu_replace(" {2,}", ICU_STR_SPACE, title, 0);

   ulog("TEMPLATE TITLE 1: %S\n", title);

   if(title && parser_extension_modules && title[0] == '#') { //icu_search("^#?[a-zA-Z]+:", title, 0)) {
      parser_function = wiki_parser_title_is_extension(root, title, &ext_name, have_pipe);
   }

   if(! ext_name && title && u_strstr(title, ICU_STR_DBL_CURL_OPEN)) {
      // title = icu_replace("(\n|_)", ICU_STR_SPACE, title, 0);
      // title = trim_u(title);
      title = wiki_parser_title_expand(root, wpf, title, pargs, &params);
   } 

   if(! ext_name && title && parser_extension_modules && ! content) {
      parser_function = wiki_parser_title_is_extension(root, title, &ext_name, have_pipe);
   }

   if(title && ! params && ! content) {
      if(ext_name) {
	 const UChar *colon = u_strstr(title, ICU_STR_COLON);
	 size_t sp = 0;
	 if(colon) 
	    sp = u_strlen(colon);
	 if(pipe < e)
	    sp += e - pipe;
	 params = calloc(sp + 50, sizeof(UChar));
	 if(params) {
	    u_strcat(params, ICU_STR_PRE);
	    if(colon) 
	       u_strcat(params, colon);
	    if(pipe < e)
	       u_strncat(params, pipe, e - pipe);
	 }
      } else if(pipe < e) {
	 params = calloc((e - s) + 50, sizeof(UChar));
	 if(params) {
	    u_strcat(params, ICU_STR_PRE);
	    u_strncat(params, pipe, e - pipe);
	 }
      }
   }

   if(s && params) {
      ulog("PARAMS: %S\n", params);
      args = wiki_parser_wiki_args_get(root, pargs, params, (ext_name) ? EINA_TRUE: EINA_FALSE);
   }

   if(params)
      free(params);

   if(title && ! content) {
      ulog("TITLE TRY:%d: %S\n", root->level, title);
      if(parser_function) {
	 ulog("PARSER EXT BUILTIN: %S\n", ext_name);
	 content = parser_function(root, pargs, args, ext_name);
	 ulog("PARSER EXT BUILTIN RES: %S, %S\n", ext_name, content);
	 root->is_template = EINA_TRUE;
      } else if(! content && ! ext_name) {
	 title = icu_replace("(\n|_)", ICU_STR_SPACE, title, 0);
	 title = trim_u(title);
	 if(title) {
	    if (title[0] == '[') {
	       content = u_strndup(s + 2, e - (s + 2));
	    } else if (title[0] == '{') {
	       content = u_strdup(title);
	    } else if (0 && title[0] == '#') {
	       content = u_strdup(title + 1);
	    } else {
	       content = wiki_parser_template_add(root, title, pargs, args, EINA_FALSE, EINA_TRUE);
	    }
	 }
      }

      if(content) {
	 UChar *p = content;
	 size_t size = u_strlen(content);
	 if(size > 0) {
	    if((*p == '{' && *(p + 1) == '|' && *(p + 2) != '}') 
		  || *p == '*' 
		  || (*p == '#')
		  || *p == ':' 
		  || *p == ';') {
	       UChar *nw = NULL, *tmp =  NULL;
	       nw = u_strndupC("<nowiki />\n", 11);
	       if(nw)
		  tmp = calloc(size + 13, sizeof(UChar));
	       if(tmp) {
		  ulog("FORCE NOWKI BEFORE \\n\n");
		  u_memcpy(tmp, nw, 11);
		  u_memcpy(tmp + 11, content, size);
		  free(content);
		  content = tmp;
	       }
	       if(nw)
		  free(nw);
	    }
	 }
      }

      args = wiki_parser_wiki_args_free(args);
   }

   *changed = EINA_TRUE;

   if(ext_name)
      free(ext_name);

   if(title)
      free(title);

   return content;
}

UChar *
wiki_parser_template_replace(Wiki_Parser *root, Wiki_Args *pargs, UChar *buf, size_t buf_size, size_t *new_size)
{
   Wiki_Parser_Find wpf;
   Eina_List *tr = NULL;
   Eina_Bool have_replace = EINA_TRUE;
   size_t skip = 0, size = buf_size;

   PARSE_TRACE();

   if(! new_size) exit(1);
   *new_size = 0;

   if(! buf) return NULL;
   if(! root || ! root->wd || buf_size <= 0) {
      free(buf);
      return NULL;
   }

   root->level++;
   level_depth = root->level;
   ulog("LEVEL %d ENTER\n", root->level);

   do {
      if(root->level > 41) {
	 free(buf);
	 buf = u_strdupC("<span class=\"error\">Expansion depth limit exceeded</span>");
	 size = (buf) ? u_strlen(buf) : 0;
      };

      while(wiki_parser_template_first_find2(&wpf, buf + skip, 2) == EINA_TRUE) {
	 UChar *content = NULL;
	 Eina_Bool changed = EINA_FALSE;

	 if(wpf.curl_count > 3) {
	    content = wiki_parser_match_curl_open_sup_3(root, &wpf, pargs, &changed);
	 } else if(wpf.curl_count == 3) {
	    content = wiki_parser_match_curl_open_equal_3(root, &wpf, pargs, &changed);
	 } else if(wpf.curl_count == 2) {
	    content = wiki_parser_expand_template_or_function(root, &wpf, pargs, &changed);
	 }

      	 if(wpf.start && changed == EINA_TRUE) {
	    TR *d = malloc(sizeof(TR));
	    if(d) {
	       d->content = content;
	       d->size = (content) ? u_strlen(content) : 0;
	       d->s = wpf.start - buf;
	       d->e = (wpf.end) ? wpf.end - buf : u_strlen(wpf.start) + 1;
	       tr = eina_list_append(tr, d);
	       content = NULL;
	       if(d->e && d->e > size) {
		  uprintf("TRY REPLACE: from %d to %d (%d or %d)width (%p) : %S by %S\n", d->s, d->e, size, u_strlen(buf), wpf.end, buf, content);
		  exit(3);
	       }
	    }
	 }

	 if(content) {
	    free(content);
	    content = NULL;
	 }

	 if(wpf.end)
	    skip = (wpf.end - buf);
	 else
	    break;
       	 ulog("skip: %d, %d\n", skip, size);
      }

      if(tr && eina_list_count(tr) > 0 
	 && root->templates_not_found < WIKI_PARSER_TEMPLATES_NOT_FOUND_MAX) {
	 ulog("BEFORE UPDATE: %S\n", buf);
	 buf = _update_buffer(buf, size, &tr, &size);
	 skip = 0;
	 ulog("MODTO:\n%S\n", buf);
	 ulog("LOOP: level = %d\n", root->level);
      } else {
	 if(! wpf.end || wpf.end < buf + size)
	    have_replace = EINA_FALSE;
	 else
	    skip = (wpf.end - buf);
	 if(tr) {
	    TR *d;
	    EINA_LIST_FREE(tr, d) {
	       if(d) {
		  if(d->content)
		     free(d->content);
		  free(d);
	       }
	    };
	 }
      }

   } while(have_replace == EINA_TRUE);

   *new_size = size;

   ulog("LEVEL %d QUIT\n", root->level);
   root->level--;
   level_depth = root->level;

   return buf;
}

Wiki_Parser *wiki_parser_add(const Wiki_Parser_Data *wd)
{
   PARSE_TRACE();

   Wiki_Parser *parser = calloc(1, sizeof(Wiki_Parser));
   if(parser) {
      parser->wd = wd;
      gettimeofday(&parser->time_start, NULL);
      gettimeofday(&parser->time_parse, NULL);
      gettimeofday(&parser->time_xhtml, NULL);
      parser->time_rval.tv_sec = 0;
      parser->time_rval.tv_usec = 0;
      return parser;
   } else
      return NULL;
}

Wiki_Parser *wiki_parser_free(Wiki_Parser *wp)
{
   PARSE_TRACE();

   if(wp) {
      if(wp->lexer)
	 wiki_lexer_free(wp->lexer);
      if(wp->root) {
	 xmlUnlinkNode(wp->root);
	 xmlFreeNode(wp->root);
      }
      if(wp->xml) 
	 xmlFreeDoc(wp->xml);
      if(wp->buf)
	 free(wp->buf);
      if(wp->source)
	 free(wp->source);
      if(wp->fullpagename)
	 free(wp->fullpagename);

      if(wp->templates_call) {
         while(eina_array_count_get(wp->templates_call) > 0)
            eina_array_pop(wp->templates_call);
	 eina_array_free(wp->templates_call);
         wp->templates_call = NULL;
      }
      if(wp->templates) {
	 eina_hash_free(wp->templates);
	 wp->templates = NULL;
      }
      if(wp->nowiki) {
	 eina_hash_free(wp->nowiki);
	 wp->templates = NULL;
      }
      if(wp->langs) {
	 xmlNodePtr n = NULL;
	 EINA_LIST_FREE(wp->langs, n) {
	    if(n)
	       xmlFreeNode(n);
	 };
	 wp->langs = NULL;
      }
      if(wp->categories) {
	 xmlNodePtr n = NULL;
	 EINA_LIST_FREE(wp->categories, n) {
	    if(n)
	       xmlFreeNode(n);
	 }
	 wp->categories = NULL;
      }

      free(wp);
   }

   return NULL;
}

Wiki_Parser *wiki_parser_buf_full_new(const Wiki_Parser_Data *wd, const UChar *buf, size_t size, Wiki_Args *args, Eina_Bool is_include, int *error)
{
   Wiki_Parser *parser = NULL;
   int have_template = 0;

   PARSE_TRACE();
   if(! wd) return NULL;
   if(size < 0) return NULL;

   *error = WIKI_PARSER_ERROR_NONE;

   parser = wiki_parser_add(wd);
   if(! parser) {
      *error = WIKI_PARSER_ERROR_ALLOCATE_PARSER;
      return NULL;
   }

   if(parser->templates == NULL) 
      parser->templates = icu_eina_hash_new(wiki_parser_templates_loaded_free);
   if(parser->nowiki == NULL) {
      parser->nowiki = icu_eina_hash_new(_wiki_parser_nowiki_part_free);
      parser->nowiki_num = 0;
   }

   parser->templates_call = eina_array_new(10);

   if(! buf) return parser;

   parser->buf = u_strndup(buf, size);

   if(is_include == EINA_FALSE) {
      parser->toc = EINA_TRUE;
      parser->source = u_strndup(buf, size);
      ulog("FIRST:\n%S\n", parser->buf);
      parser->buf = wiki_parser_template_comment_remove(parser, parser->buf);
      parser->buf = wiki_parser_tag_and_content_remove(parser->buf, "includeonly");
      parser->buf = wiki_parser_tag_name_remove(parser->buf, "noinclude|onlyinclude");
      parser->buf = wiki_parser_template_nowiki_replace(parser, parser->buf);
      ulog("INCLUDEONLY RM:\n%S\n", parser->buf);
   }

   parser->buf = wiki_parser_template_static(parser, parser->buf);
   ulog("STATIC:\n%S\n", parser->buf);

   if(parser->buf == NULL) return parser;
   parser->size = u_strlen(parser->buf);

   if(u_strstr(parser->buf, ICU_STR_CURL_OPEN)) 
      have_template = 1;

   if(have_template) {
      size_t s = parser->size;
      parser->buf = wiki_parser_template_replace(parser, args, parser->buf, s, &parser->size);
      parser->buf = icu_replace("<nowiki />", ICU_STR_EMPTY, parser->buf, 0);
      parser->size = (parser->buf) ? u_strlen(parser->buf) : 0;
   }

   ulog("PARSER RESULT: %S\n", parser->buf);

   gettimeofday(&parser->time_parse, NULL);
   return parser;
}

Wiki_Parser *wiki_parser_buf_new(const Wiki_Parser_Data *wd, const UChar *buf, size_t size, int *error)
{
   PARSE_TRACE();

   return wiki_parser_buf_full_new(wd, buf, size, NULL, 0, error);
}

UChar *wiki_parser_parse_part(Wiki_Parser *root, UChar *buf, size_t size, Wiki_Args *args, int *error)
{
   UChar *content = NULL;
   size_t s = 0;

   PARSE_TRACE();

   if(! root) return NULL;
   if(! root->wd) return NULL;
   if(! buf || size <= 0) return NULL;

#if 0
   if(args && args->list) {
      Wiki_Template_Arg *arg;
      Eina_List *l;
      EINA_LIST_FOREACH(args->list, l, arg) {
	 if(arg) {
	    ulog("SUBPART have equal: %d, key: %S, val: %S\n", arg->have_equal, arg->key, arg->val);
	 } else {
	    ulog("SUBPART have args but no arg\n");
	 }
      }
   } else {
      ulog("SUBPART have no args\n");
   }
#endif
   content = wiki_parser_template_replace(root, args, buf, size, &s);

   return content;
}

