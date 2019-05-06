#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/ustdio.h>

#include "parser_extension.h"
#include "parser_extension_private.h"

static UChar *parser_extension_module_titleparts(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *title = NULL, *buf = NULL;
   int num = 0;
   int start = 0;

   if(! root) return NULL;
   if(! args) return NULL;

   title = parser_extension_nth_get(root, pargs, args, 0);
   if(! title) return NULL;

   buf = parser_extension_nth_get(root, pargs, args, 1);
   if(buf) {
      char *tmp = u_strToUTF8_new(buf);
      if(tmp) {
	 num = atoi(tmp);
	 free(tmp);
      }
      free(buf);
   }

   buf = parser_extension_nth_get(root, pargs, args, 2);
   if(buf) {
      char *tmp = u_strToUTF8_new(buf);
      if(tmp) {
	 start = atoi(tmp);
	 free(tmp);
      }
      free(buf);
   }

   if(title) {
      char *path = u_strToUTF8_new(title);
      if(path) {
	 char **abs = NULL;
	 unsigned int cnt = 0;
	 abs = eina_str_split_full(path, "/", -1, &cnt);
	 ulog("PATH: %d, %s\n", cnt, path);
	 if(abs) {
	    if(start < 0)
	       start = cnt + start;
	    else if(start > cnt) 
	       start = cnt;
	    else if(start > 0)
	       start--;

	    if(num < 0) 
	       cnt += num;
	    else if(num && start + num < cnt)
	       cnt = start + num;

	    if(cnt > 0) {
	       int i = start;
	       *path = '\0';
	       while(i < cnt && abs[i]) {
		  if(i > start) 
		     strcat(path, "/");
		  strcat(path, abs[i]);
		  i++;
	       }
	    }
	    if(abs[0]) 
	       free(abs[0]);
	    if(abs)
	       free(abs);
	 }
      }
      if(path) {
	 content = u_strdupC(path);
	 free(path);
      }
   }
   ulog("TITLEPARTS: %S => %S\n", title, content);
   free(title);

   return content;
}

static UChar *parser_extension_module_if(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   size_t size = 0, nsize = 0;

   if(! root) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp) {
      size = u_strlen(tmp);
      tmp = trimn_u(tmp, size, &nsize);
   }
   ulog("TESTIF: '%S'\n", tmp);
   if(tmp && nsize > 0) {
      content = parser_extension_nth_merge_get(root, pargs, args, 1);
   } else {
      content = parser_extension_nth_merge_get(root, pargs, args, 2);
   }

   if(tmp)
      free(tmp);

   return trimn_special_u(parser_extension_indent_remove(content));
}

static UChar *parser_extension_module_ifeq(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *a = NULL, *b = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   a = parser_extension_nth_get(root, pargs, args, 0);
   b = parser_extension_nth_get(root, pargs, args, 1);

   a = trim_u(a);
   b = trim_u(b);

   if(a && b) {
      ulog("IFEQ: %S = %S\n", a, b);
      if(parser_extension_content_is_number(a) == EINA_FALSE
	    || parser_extension_content_is_number(b) == EINA_FALSE) {
	 if(u_strcmp(a, b) == 0) {
	    content = parser_extension_nth_merge_get(root, pargs, args, 2);
	 } else  
	    content = parser_extension_nth_merge_get(root, pargs, args, 3);
      } else {
	 double na, nb;
	 na = number_read_u(a);
	 nb = number_read_u(b);
	 if(! isnan(na) && ! isnan(nb) && na == nb)
	    content = parser_extension_nth_merge_get(root, pargs, args, 2);
	 else
	    content = parser_extension_nth_merge_get(root, pargs, args, 3);
      }
   } else if((a && !b) || (!a && b)) {
      content = parser_extension_nth_merge_get(root, pargs, args, 3);
   } else
      content = parser_extension_nth_merge_get(root, pargs, args, 2);

   if(a) 
      free(a);
   if(b)
      free(b);

   return trimn_special_u(parser_extension_indent_remove(content));
}

static UChar *parser_extension_module_iferror(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL, nan[4];

   if(! root) return NULL;
   if(! args) return NULL;

   u_uastrcpy(nan, "nan");
   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp && tmp[0] != '0' ) {
      if(u_strcasecmp(tmp, nan, 0) == 0)
	 content = parser_extension_nth_merge_get(root, pargs, args, 1);
      else {
	 if(args->list && eina_list_count(args->list) > 2)
	    content = parser_extension_nth_merge_get(root, pargs, args, 2);
	 else
	    content = u_strdup(tmp);
      }
   } else {
      content = parser_extension_nth_merge_get(root, pargs, args, 2);
   }

   if(tmp)
      free(tmp);

   return trimn_special_u(parser_extension_indent_remove(content));
}

static UChar *parser_extension_module_ifexpr(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp && tmp[0] != '\0') {
      char *expr = u_strToUTF8_new(tmp);
      if(expr) {
	 double res = 0.0;
	 res = wiki_parser_template_expr_calc(expr);
	 ulog("IFEXPR RES: %g\n", res);
	 if(res == 0.0) {
	    content = parser_extension_nth_merge_get(root, pargs, args, 2);
	 } else {
	    content = parser_extension_nth_merge_get(root, pargs, args, 1);
	 }
	 free(expr);
      }
   } else
      content = parser_extension_nth_merge_get(root, pargs, args, 2);

   if(tmp)
      free(tmp);

   return trimn_special_u(parser_extension_indent_remove(content));
}

static UChar *parser_extension_module_expr(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   char out[512];

   if(! root) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp && tmp[0] != 0) {
      char *expr = u_strToUTF8_new(tmp);
      if(expr) {
	 double res = wiki_parser_template_expr_calc(expr);
	 memset(out, 0, 512);
	 snprintf(out, 511, "%0.15g", res);
	 content = u_strdupC(out);
	 free(expr);
      }
   }

   if(tmp)
      free(tmp);

   return trimn_special_u(parser_extension_indent_remove(content));
}

static UChar *parser_extension_module_ifexist(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   Eina_Bool exists = EINA_FALSE;

   if(! root) return NULL;
   if(! args) return NULL;

   if(root->wd && root->wd->page_exists) {
      tmp = parser_extension_nth_get(root, pargs, args, 0);
      tmp = trim_u(tmp);
      tmp = icu_replace(" {2,}", ICU_STR_SPACE, tmp, 0);
   }
   ulog("IFEXIST: %S\n", tmp);
   if(tmp) {
      Wiki_Title *wt = parser_title_part(root->wd, tmp);
      if(wt) {
	 UChar *name = parser_title_with_namespace(wt);
//	    uprintf("IFEXIST:1: %S\n", name);
	 if(name) {
	    char *title = u_strToUTF8_new(tmp);
	    if(title) {
	       if(root->wd->page_exists(root->wd, title)) {
		  content = parser_extension_nth_merge_get(root, pargs, args, 1);
		  exists = EINA_TRUE;
	       }
	       free(title);
	    }
	    free(name);
	 }
	 if(exists == EINA_FALSE && wt->nsid == WIKI_NS_MEDIA) {
	    if(wt->ns)
	       free(wt->ns);
	    wt->ns = u_strdupC(wiki_parser_namespace_name_get(root->wd, WIKI_NS_FILE));
	    wt->nsid = WIKI_NS_FILE;
	    name = parser_title_with_namespace(wt);
//	    uprintf("IFEXIST:2: %S\n", name);
	    if(name) {
	       char *title = u_strToUTF8_new(tmp);
	       if(title) {
		  if(root->wd->page_exists(root->wd, title)) {
		     content = parser_extension_nth_merge_get(root, pargs, args, 1);
		     exists = EINA_TRUE;
		  }
		  free(title);
	       }
	       free(name);
	    }
	 }
	 parser_title_free(wt);
      }
      free(tmp);
   }

   if(exists == EINA_FALSE)
      content = parser_extension_nth_merge_get(root, pargs, args, 2);

   return trimn_special_u(parser_extension_indent_remove(content));
}

static UChar *parser_extension_module_switch(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL , *tmp = NULL, *cmp = NULL;
   UChar def[12];
   Eina_List *l;
   Wiki_Template_Arg *arg, *def_arg = NULL, *last = NULL;;
   int i = 0, use_first_val = 0;

   if(! root) return NULL;
   if(! args) return NULL;
   if(! args->list) return NULL;

   u_uastrcpy(def, "#default");
   EINA_LIST_FOREACH(args->list, l, arg) {
      if(i == 0) {
	 cmp = trim_u(parser_extension_nth_get(root, pargs, args, 0));
	 i++;
	 continue;
      }

      ulog("FIRST VAL: continue = %d\n", use_first_val);
      if(use_first_val) {
	 if(arg->val && arg->val_size > 0) {
	    content = u_strdup(arg->val);
	    use_first_val = 2;
	    ulog("USE FIRST VAL: %S\n", content);
	    break;
	 } else if(arg->have_equal) {
	    use_first_val = 2;
	    break;
	 }
	 continue;
      }

      tmp = trim_u(parser_extension_nth_key_get(root, pargs, args, i));
      i++;

//      ulog("#SWITCH: cmp = %S, tmp = %S, %d\n", cmp, tmp, u_strcmp(cmp, tmp));
      if(! cmp && ! tmp) {
	 if(arg && arg->val && arg->val_size > 0) {
	    content = u_strdup(arg->val);
	    use_first_val = 2;
	    break;
	 } else if(arg && arg->have_equal) {
	    use_first_val = 2;
	    break;
	 } else {
	    use_first_val = 1;
	    continue;
	 }
      }

      if(tmp && u_strcmp(tmp, def) == 0) {
	 def_arg = arg;
      }

      if(cmp && tmp) {
	 int equal = 0;
	 if((u_strlen(cmp) == 0 && u_strlen(tmp) == 0)
	       || parser_extension_content_is_number(cmp) == EINA_FALSE
	       || parser_extension_content_is_number(tmp) == EINA_FALSE) {
	    if(u_strcmp(cmp, tmp) == 0) {
	       equal = 1;
	    }
	 } else {
	    double a = number_read_u(cmp);
	    double b = number_read_u(tmp);
	    if(! isnan(a) && ! isnan(b) && a == b) {
		  equal = 1;
	    }
	 }
	 if(equal) {
	    if(arg->val && arg->val_size > 0) {
	       content = u_strdup(arg->val);
	       use_first_val = 2;
	       break;
	    } else if(arg->have_equal) {
	       use_first_val = 2;
	       break;
	    } else
	       use_first_val = 1;
	 }
      }

      if(tmp) {
	 free(tmp);
	 tmp = NULL;
      }
   }

   if(use_first_val != 2 && def_arg) {
      if(def_arg->val && ! content)
	 content = u_strdup(def_arg->val);
   }

   if(use_first_val != 2 && ! def_arg && eina_list_count(args->list) > 0) {
      last = eina_list_nth(args->list, eina_list_count(args->list) - 1);
      if(last && last->key && last->have_equal == EINA_FALSE) {
	 content = u_strdup(last->key);
      }
   }

   if(cmp)
      free(cmp);
   if(tmp)
      free(tmp);

   return trimn_special_u(parser_extension_indent_remove(content));
}

static UChar *parser_extension_module_rel2abs(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   char *path = NULL, *part = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 1);
   if(tmp) {
      path = u_strToUTF8_new(tmp);
      free(tmp);
   } else {
      path = strdup(wiki_parser_fullpagename_get(root));
   }

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp && path) {
      part = u_strToUTF8_new(tmp);
      if(part) {
	 char *res = parser_title_rel2abs(root->wd, part, path);
	 if(res) {
	    content = u_strdupC(res);
	    free(res);
	 }
	 free(part);
      }
   }

   if(tmp)
      free(tmp);

   if(path) 
      free(path);

   return content;
}

static UChar *parser_extension_module_lst(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *page = NULL, *begin = NULL, *end = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   page = parser_extension_nth_get(root, pargs, args, 0);
   begin = parser_extension_nth_get(root, pargs, args, 1);
   end = parser_extension_nth_get(root, pargs, args, 2);

   if(page && begin) {
      content = wiki_parser_template_add(root, page, pargs, args, EINA_FALSE, EINA_FALSE);
      if(content) {
	 const UChar *p = NULL, *c = NULL;
	 Eina_Bool r = EINA_FALSE;
	 p = content;
	 /* TODO: Page can have multiple sections with same label like :
	  * <section begin=b/>b<section end=b/><section begin=b/>b<section
	  * end=b/> give bb */
	 while(p && (p = icu_search("<section +begin=", p , 0))) {
	    p = u_strstr(p, ICU_STR_EQUAL_SIGN);
	    c = icu_search(">", p, 0);
	    if(c && u_strFindFirst(p , c - p, begin, u_strlen(begin))) {
     	       size_t size = u_strlen(content) - (c - content);
	       u_memmove(content, c + 1, size);
	       r = EINA_TRUE;
	       p = NULL;
	    } 
	    if(r == EINA_FALSE)
	       p++;
	    if(r == EINA_TRUE && (p = icu_search("<section +end=", content, 0))) {
	       UChar *s = u_strstr(p, ICU_STR_EQUAL_SIGN);
	       UChar *e = (UChar *)p;
	       c = icu_search(">", s, 0);
	       if(c) {
		  c++;
		  if(end) {
		     if(u_strFindFirst(s, c - s, end, u_strlen(end))) {
			*e = '\0';
			r = EINA_TRUE;
		     }
		  } else {
		     if(u_strFindFirst(s, c - s, begin, u_strlen(begin))) {
			*e = '\0';
			r = EINA_TRUE;
		     }
		  }
	       }
	    }
	 }
	 if(r == EINA_FALSE) {
	    if(content)
	       free(content);
  	    content = NULL;
	 }
      }
   }

   if(page)
      free(page);

   if(begin)
      free(begin);

   if(end)
      free(end);

   return content;
}

static UChar *parser_extension_module_lstx(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *page = NULL, *begin = NULL, *end = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   page = parser_extension_nth_get(root, pargs, args, 0);
   begin = parser_extension_nth_get(root, pargs, args, 1);
   end = parser_extension_nth_get(root, pargs, args, 2);

   if(page) {
      content = wiki_parser_template_add(root, page, pargs, args, EINA_FALSE, EINA_FALSE);
      if(content && begin) {
	 UChar *p = NULL;
	 const UChar *c = NULL, *d = NULL;
	 Eina_Bool r = EINA_FALSE;
	 if((p = (UChar *) icu_search("<section +begin=", content, 0))) {
	    c = icu_search("=", p, 0);
	    if(c) {
	       c++;
	       if(u_strFindFirst(c, u_strlen(begin), begin, u_strlen(begin)) == NULL)
		 p = NULL;
	    }
	 } 
	 if((d = icu_search("<section +end=", content, 0))) {
	    c = icu_search("=", p, 0);
	    if(c) {
	       c++;
	       if(u_strFindFirst(c, u_strlen(begin), begin, u_strlen(begin)) == NULL)
	       d = NULL;
	    }
	 }
	 if(d && p) {
	    if(end) {
	       if(d - p < u_strlen(end)) {
		  u_memcpy(p, end, u_strlen(end));
		  p += u_strlen(end);
		  u_memmove(p, d, u_strlen(d) + 1);
		  r = EINA_TRUE;
	       } else {
		  UChar *tmp = calloc((p - content) + u_strlen(d) + u_strlen(end) + 1, sizeof(UChar));
		  if(tmp) {
		     u_memcpy(tmp, content, p - content);
		     u_strcat(tmp, end);
		     u_strcat(tmp, d);
		     free(content);
		     content = tmp;
		     r = EINA_TRUE;
		  }
	       }
	    } else {
	       u_memmove(p, d, u_strlen(d) + 1);
	       r = EINA_TRUE;
	    }
	 }
	 if(0 && r == EINA_FALSE) {
	    if(content)
	       free(content);
  	    content = NULL;
	 }
      }
   } else {
      ulog("no pagename\n");
   }

   if(page)
      free(page);

   if(begin)
      free(begin);

   if(end)
      free(end);

   return content;
}

static UChar *parser_extension_module_len(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   char *val = NULL;
   size_t len = 0;

   if(! root) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp) {
      len = u_strlen(tmp);
      free(tmp);
   }

   asprintf(&val, "%d", (int) len);
   if(val) {
      content = u_strdupC(val);
      free(val);
   } else {
      content = u_strndupC("0", 1);
   }

   return content;
}

static UChar *parser_extension_module_pos(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL, *needle = NULL, *offset = NULL;
   char *val = NULL;
   size_t len = 0, start = 0;

   if(! root) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp) {
      len = u_strlen(tmp);
   }
   needle = parser_extension_nth_get(root, pargs, args, 1);
   if(needle == NULL || u_strlen(needle) == 0) {
      if(needle)
	 free(needle);
      needle = u_strndupC(" ", 1);
   }
   offset = parser_extension_nth_get(root, pargs, args, 2);
   if(offset) {
      start = number_read_u(offset);
      free(offset);
      offset = NULL;
   }

   if(tmp && needle && start < len) {
      offset = u_strstr(tmp + start, needle);
   }

   if(offset)
      asprintf(&val, "%ld", offset - tmp);
   if(val) {
      content = u_strdupC(val);
      free(val);
   } else {
      content = u_strdup(ICU_STR_EMPTY);
   }

   if(needle)
      free(needle);
   if(tmp)
      free(tmp);

   return content;
}

static UChar *parser_extension_module_rpos(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL, *needle = NULL, *offset = NULL;
   char *val = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   needle = parser_extension_nth_get(root, pargs, args, 1);
   if(needle == NULL || u_strlen(needle) == 0) {
      if(needle)
	 free(needle);
      needle = u_strndupC(" ", 1);
   }

   if(tmp && needle) {
      offset = u_strrstr(tmp, needle);
   }

   if(offset)
      asprintf(&val, "%ld", offset - tmp);
   if(val) {
      content = u_strdupC(val);
      free(val);
   } else {
      content = u_strdup(ICU_STR_EMPTY);
   }

   if(needle)
      free(needle);
   if(tmp)
      free(tmp);

   return content;
}

static UChar *parser_extension_module_sub(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   return content;
}

static UChar *parser_extension_module_count(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   return content;
}

static UChar *parser_extension_module_replace(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   return content;
}

static UChar *parser_extension_module_explode(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   return content;
}

static UChar *parser_extension_module_urldecode(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   return content;
}

void wiki_parser_extension_functions_init()
{
   wiki_parser_extension_module_register("#titleparts", parser_extension_module_titleparts);
   wiki_parser_extension_module_register("#if", parser_extension_module_if);
   wiki_parser_extension_module_register("#ifeq", parser_extension_module_ifeq);
   wiki_parser_extension_module_register("#iferror", parser_extension_module_iferror);
   wiki_parser_extension_module_register("#ifexpr", parser_extension_module_ifexpr);
   wiki_parser_extension_module_register("#ifexist", parser_extension_module_ifexist);
   wiki_parser_extension_module_register("#expr", parser_extension_module_expr);
   wiki_parser_extension_module_register("#switch", parser_extension_module_switch);
   wiki_parser_extension_module_register("#rel2abs", parser_extension_module_rel2abs);
   wiki_parser_extension_module_register("#lst", parser_extension_module_lst);
   wiki_parser_extension_module_register("#lstx", parser_extension_module_lstx);
   wiki_parser_extension_module_register("#section", parser_extension_module_lst);
   wiki_parser_extension_module_register("#section-x", parser_extension_module_lstx);

   wiki_parser_extension_module_register("#len", parser_extension_module_len);
   wiki_parser_extension_module_register("#pos", parser_extension_module_pos);
   wiki_parser_extension_module_register("#rpos", parser_extension_module_rpos);
   wiki_parser_extension_module_register("#sub", parser_extension_module_sub);
   wiki_parser_extension_module_register("#count", parser_extension_module_count);
   wiki_parser_extension_module_register("#replace", parser_extension_module_replace);
   wiki_parser_extension_module_register("#explode", parser_extension_module_explode);
   wiki_parser_extension_module_register("#urldecode", parser_extension_module_urldecode);
}
