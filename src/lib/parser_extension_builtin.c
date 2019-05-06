#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <math.h>
#include <fenv.h>
#include <stdlib.h>
#include <string.h>
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/utrans.h>
#include <unicode/ustdio.h>

#include <libintl.h>
#include <locale.h>

#include "parser.h"
#include "parser_extension.h"
#include "parser_extension_private.h"
#include "wiki_lang.h"
#include "icu_eina_hash.h"

static UChar *parser_extension_module_not_implemented(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   content = parser_extension_nth_get(root, pargs, args, 0);

   return content;
}

static UChar *parser_extension_module_int(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   content = parser_extension_nth_get(root, pargs, args, 0);
   if(content) {
      char *tmp = u_strToUTF8_new(content);
      const char *msg = NULL;
      if(tmp) {
	 if(strcasecmp(tmp, "Lang") == 0)
	    msg = root->wd->lang;
	 else
	    msg = gettext(tmp);
	 if(msg == tmp) {
	    tmp = strtolower(tmp);
	    msg = gettext(tmp);
	 }
	 if(msg == tmp)
	 uprintf("not found msg: %s for %S with locale = %s\n", msg, content, setlocale(LC_MESSAGES, NULL));
	 free(tmp);
      }
      if(msg && strlen(msg) > 0) {
	 free(content);
	 content = u_strdupC(msg);
      }
   }

   return content;
}

static UChar *parser_extension_module_subst(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;
   Wiki_Template_Arg *arg = NULL;
   Eina_List *l = NULL;
   int i = 0;
   size_t size = 0;

   if(! root) return NULL;
   if(! args) return NULL;
   if(! args->list || eina_list_count(args->list) == 0) return NULL;

   /* TODO: see http://meta.wikimedia.org/wiki/Help:Substitution
    */
   EINA_LIST_FOREACH(args->list, l, arg) {
      if(arg) {
	 if(arg->key)
	    size += arg->key_size;
	 if(arg->have_equal)
	    size += 1;
	 if(arg->val)
	    size += arg->val_size;
      }
      if(i)
	 size += 1;
      i++;
   }

   content = calloc(size + 5, sizeof(UChar *));
   if(content) {
      i = 0;
      u_memcpy(content, ICU_STR_DBL_CURL_OPEN, 2);
      size = 2;
      EINA_LIST_FOREACH(args->list, l, arg) {
	 if(i) {
	    u_memcpy(content + size, ICU_STR_PIPE, 1);
	    size++;
	 }
	 if(arg) {
	    if(arg->key) {
	       u_memcpy(content + size, arg->key, arg->key_size);
	       size += arg->key_size;
	    }
	    if(arg->have_equal) {
	       u_strcat(content + size, ICU_STR_EQUAL_SIGN);
	       size += 1;
	    }
	    if(arg->val) {
	       u_memcpy(content + size, arg->val, arg->val_size);
	       size += arg->val_size;
	    }
	 }
	 i++;
      }
      u_memcpy(content + size, ICU_STR_CURL_CLOSE, 1);
      size++;
      u_memcpy(content + size, ICU_STR_CURL_CLOSE, 1);
      return content;
   }

   ulog("SUBST: %S\n", content);
   return content;
}

static UChar *parser_extension_module_msgnw(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;
   UChar *tmp = NULL;
   UChar entity[10];

   if(! root) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp) {
      Wiki_Template_Loaded *wtl = wiki_parser_template_load(root, tmp, EINA_FALSE);
      if(wtl && wtl->content) 
	 content = u_strdup(wtl->content);

      u_snprintf(entity, sizeof(entity), "&#38;");
      content = icu_replace("&", entity, content, 0);
//      u_snprintf(entity, sizeof(entity), "&#33;");
//      content = icu_replace("!", entity, content, 0);
      u_snprintf(entity, sizeof(entity), "&#60;");
      content = icu_replace("<", entity, content, 0);
      u_snprintf(entity, sizeof(entity), "&#62;");
      content = icu_replace(">", entity, content, 0);
      u_snprintf(entity, sizeof(entity), "&#91;");
      content = icu_replace("\\[", entity, content, 0);
      u_snprintf(entity, sizeof(entity), "&#93;");
      content = icu_replace("\\]", entity, content, 0);
      u_snprintf(entity, sizeof(entity), "&#95;");
      content = icu_replace("_", entity, content, 0);
      u_snprintf(entity, sizeof(entity), "&#124;");
      content = icu_replace("\\|", entity, content, 0);
      u_snprintf(entity, sizeof(entity), "&#39;");
      content = icu_replace("'", entity, content, 0);
      u_snprintf(entity, sizeof(entity), "&#123;");
      content = icu_replace("\\{", entity, content, 0);
      u_snprintf(entity, sizeof(entity), "&#125;");
      content = icu_replace("\\}", entity, content, 0);

      free(tmp);
   }

   return content;
}

static UChar *parser_extension_module_ns(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;

   if(! root) return NULL;
   if(! root->wd) return NULL;
   if(! root->wd->namespaces) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp) {
      if(u_islower(tmp[0]))
	 tmp[0] = u_toupper(tmp[0]);
      char *nsstr = u_strToUTF8_new(tmp);
      Wiki_Namespace *ns = eina_hash_find(root->wd->namespaces, nsstr);
      if(ns && ns->label) {
	 content = u_strdupC(ns->label);
	 if(! content)
	    ulog("NAMESPACE: cant dup label: %s\n", ns->label);
	 else
	    ulog("NAMESPACE: %S\n", content);
      } else
	 ulog("NAMESPACE NOT FOUND: %s\n", nsstr);
      if(nsstr)
	 free(nsstr);
      free(tmp);
   }

   return content;
}

static UChar *parser_extension_module_uc(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   content = parser_extension_nth_get(root, pargs, args, 0);
   if(content) {
      UChar *p = content;
      while(p && *p) {
	 if(u_islower(*p))
	    *p = u_toupper(*p);
	 p++;
      }
   }

   return content;
}

static UChar *parser_extension_module_ucfirst(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   content = parser_extension_nth_get(root, pargs, args, 0);
   if(content && u_islower(content[0]))
      content[0] = u_toupper(content[0]);

   return content;
}

static UChar *parser_extension_module_lc(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   content = parser_extension_nth_get(root, pargs, args, 0);
   if(content) {
      UChar *p = content;
      while(p && *p) {
	 if(u_isupper(*p))
	    *p = u_tolower(*p);
	 p++;
      }
   }

   return content;
}

static UChar *parser_extension_module_lcfirst(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   content = parser_extension_nth_get(root, pargs, args, 0);
   if(content && u_isupper(content[0]))
      content[0] = u_tolower(content[0]);

   return content;
}

static UChar *parser_extension_module_fullurl(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL, *qry = NULL;

   if(! root) return NULL;
   if(! root->wd) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   qry = parser_extension_nth_get(root, pargs, args, 1);
   if(tmp && tmp[0] != '\0') {
      int exists = 0;
      char *page = u_strToUTF8_new(tmp);
      char *url = wiki_parser_url_internal_get(root, WIKI_NS_MAIN, page, 0, &exists);
      UChar *url_u = u_strdupC(url);
      if(url_u) {
	 size_t size = ((qry) ? u_strlen(qry) + u_strlen(url_u) + 20: u_strlen(url_u));
	 content = calloc(size + 1, sizeof(UChar));
	 if(content) {
	    if(qry) {
	       u_sprintf(content, "%S&amp;%S", url_u, qry);
//	       content = icu_replace_utf8("&", "&amp;", content, 0);
	    } else
	       u_sprintf(content, "%S", url_u);
	 }
	 free(url_u);
      }
      ulog("FULLURL: page = %s, url = %S\n", page, content);
      if(page)
	 free(page);
      if(url)
	 free(url);

      free(tmp);
   }

   if(qry)
      free(qry);

   return content;
}

static UChar *parser_extension_module_localurl(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   content = parser_extension_module_fullurl(root, pargs, args, data);
   content = icu_replace_utf8("^http:\\/\\/([^/]*?)/", "/", content, 0);
   content = icu_replace("(\\[|\\])", ICU_STR_EMPTY, content, 0);

   return content;
}

static UChar *parser_extension_module_filepath(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL, *nowiki = NULL;;

   if(! root) return NULL;
   if(! root->wd) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   nowiki = parser_extension_nth_get(root, pargs, args, 1);
   if(tmp) {
      char *file = u_strToUTF8_new(tmp);
      char *url = wiki_parser_url_image_get(root->wd, file); 
      content = u_strdupC(url);
      if(content && icu_search("^nowiki$", nowiki, 0)) {
	 // TODO : add nowiki tag
      }
      if(url) 
	 free(url);
      if(file) 
	 free(file);
      free(tmp);
   }

   if(nowiki)
      free(nowiki);

   return content;
}

static UChar *parser_extension_module_urlencode(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp;

   if(! root) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp) {
      content = url_encode(tmp);
      free(tmp);
   }

   return content;
}

static UChar *parser_extension_module_anchorencode(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   tmp = trim_u(parser_extension_nth_get(root, pargs, args, 0));
   if(tmp) {
      char *tmpc = NULL, *enc = NULL;
      tmpc = u_strToUTF8_new(tmp);
      if(tmpc) {
	 enc = wiki_parser_anchor_encode(tmpc);
	 if(enc) {
	    content = u_strdupC(enc);
	    free(enc);
	 }
	 free(tmpc);
      }
      free(tmp);
   }

   return content;
}

static UChar *parser_extension_module_time(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp && tmp[0] != '\0') {
      UChar *date_iso = NULL, *date_rfc = NULL;
      size_t size = u_strlen(tmp);
      size_t tmp_size = size;
      date_iso = u_strdupC("%Y-%m-%dT%H:%M:%S+00:00");
      date_rfc = u_strdupC("%a, %-e %b %Y %H:%M:%S +0000");
      const UChar *p = tmp;
      while(p < tmp + size) {
	 if(*p == 'c') {
            if(date_iso)
               size += u_strlen(date_iso);
         }
	 if(*p == 'r')
            if(date_rfc)
               size += u_strlen(date_rfc);
	 p++;
      }
      p = tmp;
      size *= 3;
      UChar *buf = calloc(size + 1, sizeof(UChar));
      UChar *t = buf;
      while(p < (tmp + tmp_size)) {
	 switch(*p) {
	    case 'Y':
	    case 'y':
	    case 'm':
	    case 'd':
	    case 'H':
	    case 'w':
	    case 'W':
	       *t = '%'; t++; *t = *p ; t++; break;
	    case 'i':
	       *t = '%'; t++; *t = 'M'; t++; break;
	    case 's':
	       *t = '%'; t++; *t = 'S'; t++; break;
	    case 'U':
	       *t = '%'; t++; *t = 's'; t++; break;
	    case 'a':
	       *t = '%'; t++; *t = 'P'; t++; break;
	    case 'A':
	       *t = '%'; t++; *t = 'p'; t++; break;
	    case 'M':
	       *t = '%'; t++; *t = 'b'; t++; break;
	    case 'F':
	       *t = '%'; t++; *t = 'B'; t++; break;
	    case 'D':
	       *t = '%'; t++; *t = 'a'; t++; break;
	    case 'l':
	       *t = '%'; t++; *t = 'A'; t++; break;
	    case 'h':
	       *t = '%'; t++; *t = 'I'; t++; break;
	    case 'g':
	       *t = '%'; t++; *t = 'l'; t++; break;
	    case 'z':
	       *t = '%'; t++; *t = 'j'; t++; break;
	    case 'N':
	       *t = '%'; t++; *t = 'w'; t++; break;
	    case 'G':
	       *t = '%'; t++; *t = '-'; t++; *t = 'H'; t++; break;
	    case 'j':
	       *t = '%'; t++; *t = '-'; t++; *t = 'e'; t++; break;
	    case 'n':
	       *t = '%'; t++; *t = '-'; t++; *t = 'm'; t++; break;
	    case 'c':
	       u_strcat(buf, date_iso); t = buf + u_strlen(buf); break;
	    case 'r':
	       u_strcat(buf, date_rfc); t = buf + u_strlen(buf); break;
	    case '"':
	       p++;
	       while(*p && *p != '"') {
		  *t = *p; t++;
		  p++;
	       }
	       break;
	    default:
	       *t = *p;
	       t++;
	       break;
	 }
	 p++;
      }

      if(date_iso)
         free(date_iso);
      if(date_rfc)
         free(date_rfc);

      ulog("PARSER TIME: %S\n", buf);
      time_t tt = time(NULL);
      const struct tm *tmp_tm;
      tmp_tm = localtime(&tt);
      char *buft = calloc(1024 +1, sizeof(char));
      char *bufa = u_strToUTF8_new(buf);
      if(bufa) {
	 strftime(buft, 1024, bufa, tmp_tm);
	 ulog("PARSER TIME: %s\n", buft);
	 content = u_strdupC(buft);
	 free(bufa);
      }
      free(buft);
      free(buf);
      free(tmp);
   }

   return content;
}

static UChar *parser_extension_module_plural(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   int plural = 0;

   if(! root) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp) {
      plural= (int) number_read_u(tmp);
      plural %= 10;
      if(plural < 1)
	 plural = 1;

      free(tmp);
      tmp = NULL;
   }

   tmp = parser_extension_nth_get(root, pargs, args, plural);
   if(tmp) {
      content = tmp;
   }

   return content;
}

static UChar *parser_extension_module_lang(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *lang = NULL, *tmp = NULL;
   char tag[] = "<span class=\"lang-%S\" xml:lang=\"%S\" lang=\"%S\">%S</span>";

   if(! root) return NULL;

   lang = parser_extension_nth_get(root, pargs, args, 0);
   tmp = parser_extension_nth_get(root, pargs, args, 1);
   if(tmp && lang) {
      content = calloc((u_strlen(lang) * 3) + u_strlen(tmp) + strlen(tag) + 1,
	    sizeof(UChar));
      u_sprintf(content, tag, lang, lang, lang, tmp);
   }

   if(lang)
      free(lang);
   if(tmp)
      free(tmp);

   return content;
}

static UChar *parser_extension_module_language(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   const Wiki_Lang *wl = NULL;

   if(! root) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp) {
      char *lang = u_strToUTF8_new(tmp);
      if(lang) {
         wl = wiki_parser_language_infos_get(lang);
         if(wl && wl->local)
            content = u_strdupC(wl->local);
         else if(wl)
            content = u_strdupC(wl->english);
         else
            content = u_strdup(tmp);
         free(lang);
      }
      free(tmp);
   }

   return content;
}

static UChar *parser_extension_module_tag(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL, *tag = NULL, *tag_content = NULL;
   Wiki_Nowiki_Part *wnp = NULL;
   const Wiki_Parser_Tag_Extension *te = NULL;
   const Eina_Hash *tags = wiki_lexer_tags_get();
   int i = 0;

   if(! root) return NULL;
   if(! args) return NULL;
   if(! args->list) return NULL;

   ulog("CHECK: %s\n", __FUNCTION__);
   for(i = 0; i < eina_list_count(args->list); i++) {
      if(i == 0) {
	 tag = parser_extension_nth_get(root, pargs, args, i);
	 if(tag) {
	    char *tagname = u_strToUTF8_new(tag);
	    if(tagname) {
	       if(tags && root->nowiki)
		  te = eina_hash_find(tags, tagname);
	       if(te && te->type == WIKI_PARSER_TAG_CONTENT_PARSE_NONE) {
		  wnp = calloc(1, sizeof(Wiki_Nowiki_Part));
		  if(wnp) {
		     wnp->tagname = te->name;
		     wnp->tagname_size = strlen(te->name);
		  }
	       }
	       free(tagname);
	    }
	    if(! wnp) {
	       content = calloc(u_strlen(tag) + 4, sizeof(UChar));
	       if(content) {
		  u_strcat(content, ICU_STR_ANGL_OPEN);
		  u_strcat(content, tag);
	       }
	    }
	 }
      } else if(i == 1) {
	 tag_content = parser_extension_nth_get(root, pargs, args, i);
	 if(wnp) {
	    wnp->content = tag_content;
	    wnp->content_size = (tag_content) ? u_strlen(tag_content) : 0;
	    tag_content = NULL;
	 }
      } else {
	 tmp = parser_extension_nth_get(root, pargs, args, i);
	 if(content && tmp) {
	    UChar *n = calloc(u_strlen(content) + u_strlen(tmp) + 2, sizeof(UChar));
	    if(n) {
	       u_strcat(n, content);
	       u_strcat(n, ICU_STR_SPACE);
	       u_strcat(n, tmp);
	       free(content);
	       content = n;
	    }
	 } else if(wnp && tmp) {
	    size_t stmp = u_strlen(tmp);
	    UChar *n = calloc(wnp->args_size + stmp + 1, sizeof(UChar));
	    if(n) {
	       if(wnp->args)
		  u_strncat(n, wnp->args, wnp->args_size);
	       u_strncat(n, tmp, stmp);
	       wnp->args_size += stmp;
	       wnp->args = n;
	    }
	 }
	 if(tmp)
	    free(tmp);
      }

      if(! content && ! wnp) 
	 break;
   }

   if(wnp) {
      UChar *hash = NULL;
      hash = wiki_parser_uniq_string_get(root, wnp->tagname);
      if(hash && root->nowiki) {
	 icu_eina_hash_add(root->nowiki, hash, wnp);
	 content = hash; 
      } else {
	 if(hash)
	    free(hash);
	 if(wnp->args)
	    free(wnp->args);
	 if(wnp->content)
	    free(wnp->content);
	 free(wnp);

      }
   } else if(content) {
      UChar *n = NULL;
      size_t size = u_strlen(content);
      if(tag_content)
	 size += u_strlen(tag_content);
      size += u_strlen(tag) + 10;
      
      n = calloc(size + 1, sizeof(UChar));
      if(n) {
	 u_strcat(n, content);
	 u_strcat(n, ICU_STR_ANGL_CLOSE);
	 if(tag_content)
	    u_strcat(n, tag_content);
	 u_strcat(n, ICU_STR_ANGL_OPEN);
   	 u_strcat(n, ICU_STR_SLASH);
	 u_strcat(n, tag);
	 u_strcat(n, ICU_STR_ANGL_CLOSE);
	 free(content);
	 content = n;
      } else {
	 free(content);
	 content = NULL;
      } 
   }

   if(tag_content)
       free(tag_content);
   if(tag)
      free(tag);

   return content;
}

static UChar *parser_extension_module_padding(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL, *str = NULL;
   UChar *name = data;
   UChar padright[10], padleft[10];
   int pad = 0;
   size_t nsize = 0;

   if(! root) return NULL;
   if(! name) return NULL;

   u_sprintf(padright, "padright");
   u_sprintf(padleft, "padleft");

   str = parser_extension_nth_get(root, pargs, args, 0);
   
   tmp = parser_extension_nth_get(root, pargs, args, 1);
   if(tmp) {
      pad = (int) number_read_u(tmp);
      if(isnan(pad)) {
	 pad = 0; // u_strlen(tmp);
      } else if(str && pad < 0)
	 pad = 0;
      if(pad > 498)
	 pad = 498;
      free(tmp);
      tmp = NULL;
   }

   tmp = parser_extension_nth_get(root, pargs, args, 2);
   if(tmp) {
      if(! str && pad < 0)
	 pad += u_strlen(tmp);
      if(pad < 0)
	 pad = 0;
      if(pad >= 0 && u_strlen(tmp) > pad)
	 tmp[pad] = '\0';
   } else {
      tmp = u_strdupC("0");
   }

   if(str && pad > 0) {
      nsize = u_strlen(str);
      if(nsize < pad) {
	 content = calloc((pad * (u_strlen(tmp) + 1)) + 2, sizeof(UChar));
	 if(tmp) {
	    int i = (pad / u_strlen(tmp)) + 1;
	    while(i > 0) {
	       u_strcat(content, tmp);
	       i--;
	       ulog("PADSIZE: %d\n", u_strlen(content));
	    }
	 }
	 content[pad] = '\0';

	 if(u_strcmp(name, padright) == 0) 
	    u_memcpy(content, str, u_strlen(str));
	 else
	    u_memcpy(content + u_strlen(content) - nsize, str, nsize);
      } else {
	 content = u_strdup(str);
      }

   } else if(! str && tmp) {
      nsize = u_strlen(tmp);
      if(u_strcmp(name, padright) == 0) {
	 if(nsize >= pad)
	    content = u_strndup(tmp, pad);
	 else if(nsize > 0) {
	    int npad = (pad / nsize) + 1;
	    content = calloc((nsize * npad) + 1, sizeof(UChar));
	    if(content) {
	       while(npad > 0) {
		  u_strcat(content, tmp);
		  npad--;
	       }
	       content[pad] = '\0';
	    }
	 }
      } else {
	 if(nsize >= pad)
	    content = u_strndup(tmp, pad);
	 else if(nsize > 0) {
	    int npad = (pad / nsize) + 1;
	    content = calloc((nsize * npad) + 1, sizeof(UChar));
	    if(content) {
	       while(npad > 0) {
		  u_strcat(content, tmp);
		  npad--;
	       }
	       content[pad] = '\0';
	    }
	 }
      }
   } else if(str) {
      content = u_strdup(str);
   }
   
   ulog("PADDING: %S, %S, %S, %d, %d => %S\n", name, str, tmp, pad, nsize, content);

   if(str)
      free(str);

   if(tmp) 
      free(tmp);

   return content;
}

static UChar *parser_extension_module_formatnum(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   static UChar buf[1024];
   UFILE *in = NULL, *out = NULL;
   int rev = 0;

   if(! root) return NULL;
   if(! args) return NULL;

//   memset(buf, 0x0, sizeof(UChar) * 1024);
   tmp = parser_extension_nth_get(root, pargs, args, 0);
   
   tmp = icu_replace(",", ICU_STR_EMPTY, tmp, 0);
   ulog("FORMATNUM: %S\n", tmp);
   return tmp;
   
   if(tmp && u_strlen(tmp) > 0) {
      UChar *arg = parser_extension_nth_get(root, pargs, args, 1);
      if(arg) {
	 in = u_fstropen(tmp, u_strlen(tmp), root->wd->locale);
   	 out = u_fstropen(buf, 1023, "en_US.UTF-8");
	 if(in && out) {
	    double r = 0.0, c = 0.0;
	    // u_frewind(in);
	    u_fscanf(in, "%lf %f", &r, &c);
	    ulog("FORMATNUM READ: %g %g\n", r, c);
	    u_fprintf(out, "%g", r);
	    rev = 1;
	 }
	 
	 free(arg);
      } else {
       	 out = u_fstropen(buf, 1023, root->wd->locale);
	 if(out) {
	    double r = number_read_u(tmp);
	    ulog("FORMATNUM isnan: %d\n", isnan(r));
	    if(0 && isnan(r))
	       u_fprintf(out, "%S", tmp);
	    else
	       u_fprintf(out, "%g", r);
	 }
      }
      free(tmp);
   }

   if(in)
      u_fclose(in);
   if(out) {
      u_fflush(out);
      u_frewind(out);
   }

   content = u_strdup(buf);
   if(rev && content) {
      content = icu_replace(",", ICU_STR_EMPTY, content, 0);
   }

   ulog("FORMATNUM RES: %S\n", content);
   return content;
}

static UChar *parser_extension_module_formatdate(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   /* TODO: Need to parse date first */
   content = parser_extension_nth_get(root, pargs, args, 0);

   return content;
}

void wiki_parser_extension_builtin_init(void) {
   wiki_parser_extension_module_register("ns", parser_extension_module_ns);
   wiki_parser_extension_module_register("uc", parser_extension_module_uc);
   wiki_parser_extension_module_register("ucfirst", parser_extension_module_ucfirst);
   wiki_parser_extension_module_register("lc", parser_extension_module_lc);
   wiki_parser_extension_module_register("lcfirst", parser_extension_module_lcfirst);
   wiki_parser_extension_module_register("localurl", parser_extension_module_localurl);
   wiki_parser_extension_module_register("fullurl", parser_extension_module_fullurl);
   wiki_parser_extension_module_register("filepath", parser_extension_module_filepath);
   wiki_parser_extension_module_register("urlencode", parser_extension_module_urlencode);
   wiki_parser_extension_module_register("anchorencode", parser_extension_module_anchorencode);
   wiki_parser_extension_module_register("lang", parser_extension_module_lang);
   wiki_parser_extension_module_register("#language", parser_extension_module_language);
   wiki_parser_extension_module_register("#time", parser_extension_module_time);
   wiki_parser_extension_module_register("#timel", parser_extension_module_time);
   wiki_parser_extension_module_register("#tag", parser_extension_module_tag);
   wiki_parser_extension_module_register("int", parser_extension_module_int);
   wiki_parser_extension_module_register("gender", parser_extension_module_not_implemented);
   wiki_parser_extension_module_register("formatnum", parser_extension_module_formatnum);
   wiki_parser_extension_module_register("dateformat", parser_extension_module_not_implemented);
   wiki_parser_extension_module_register("padleft", parser_extension_module_padding);
   wiki_parser_extension_module_register("padright", parser_extension_module_padding);
   wiki_parser_extension_module_register("plural", parser_extension_module_plural);
   wiki_parser_extension_module_register("groupconvert", parser_extension_module_not_implemented);
   wiki_parser_extension_module_register("msgnw", parser_extension_module_msgnw);
   wiki_parser_extension_module_register("subst", parser_extension_module_null);
   wiki_parser_extension_module_register("safesubst", parser_extension_module_subst);
   wiki_parser_extension_module_register("#dateformat", parser_extension_module_formatdate);
   wiki_parser_extension_module_register("#formatdate", parser_extension_module_formatdate);
}

