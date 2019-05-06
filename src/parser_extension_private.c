#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/ustdio.h>

#include <Eina.h>

#include "parser.h"
#include "parser_extension_private.h"

#define WD_EXTENSION_COUNT_MAX {\
  root->ext_cnt++; \
   if(root->ext_cnt > 5000000) { \
      printf("EXTENSION_COUNT_MAX_LIMIT: %s : count = %d\n", __FUNCTION__, root->ext_cnt); \
      return NULL; \
   } \
}

Eina_Bool parser_extension_content_is_number(const UChar *content)
{
   const UChar *p = content;
   Eina_Bool ret = EINA_TRUE, have_number = EINA_FALSE;
   Eina_Bool is_hex = EINA_FALSE;
   int i = 0;
   double n = 0.0;

   if(! content) return EINA_FALSE;
   n = number_read_u(content);
   if(isnan(n)) ret = EINA_FALSE;
   ulog("ISNUMBER: %S => %d (%g)\n", content, ret, n);
   return ret;

   while(*p && ret == EINA_TRUE) {
      switch(*p) {
	 case '0':
	    if(i == 0 && *(p + 1) && (*(p + 1)== 'x' || *(p + 1) == 'X')) {
	       is_hex = EINA_TRUE;
	       p++;
	       break;
	    }
	 case '1':
	 case '2':
	 case '3':
	 case '4':
	 case '5':
	 case '6':
	 case '7':
	 case '8':
	 case '9':
	    have_number = EINA_TRUE;
	 case '.':
	 case '+':
	 case '-':
	    break;
	 default:
	    if(is_hex == EINA_TRUE) { 
	       if(! (*p == 'A' || *p == 'B' || *p == 'C' || *p == 'D' || *p == 'E' || *p == 'F'
		     || *p == 'a' || *p == 'b' || *p == 'c' || *p == 'd' || *p == 'e' || *p == 'f'))
		  is_hex = EINA_FALSE;
	    } else
	       ret = EINA_FALSE;
	    break;
      }
      p++;
   }

   if(is_hex == EINA_FALSE && have_number == EINA_FALSE)
      ret = EINA_FALSE;

   return ret;
}

UChar *parser_extension_indent_remove(UChar *content)
{
   UChar *p = content;
   UChar search[5];
   size_t size = 0;
   if(! content) return content;

   u_sprintf(search, "\n ");
   size = u_strlen(content) + 1;
   p = content;

   while((p = u_strFindFirst(p, (size - (p - content)), search, 2))) {
      if(*p == '\n') {
	 UChar *s = p;
	 s++;
	 int n = 0;
	 while(*s && u_isWhitespace(*s)) {
	    s++;
	    n++;
         }
	 if(n > 0) {
	    if(! *s) s--;
	    u_memmove(p + 1, s, (size - (s - content)));
	    size -= n;
	 }
      }
      p++;
   }

   return content;
}

UChar *parser_extension_module_null(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return NULL;
}

UChar *parser_extension_nth_key_get(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, int num)
{
   UChar             *content = NULL;
   Wiki_Template_Arg *arg;
   int                error = 0;

   WD_EXTENSION_COUNT_MAX;

   if(! root) return NULL;
   if(! args) return NULL;
   if(! args->list) return NULL;

   if(eina_list_count(args->list) > num) {
      arg = eina_list_nth(args->list, num);
      if(arg && arg->key) {
	 content = u_strndup(arg->key, arg->key_size);
	 if(content && u_strstr(content, ICU_STR_DBL_CURL_OPEN)) {
	    content = wiki_parser_parse_part(root, content, u_strlen(content), pargs, &error);
	 }
      }
   }

   return content;
}

UChar *parser_extension_nth_merge_get(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, int num)
{
   UChar             *content = NULL;
   size_t size = 0;

   content = parser_extension_nth_merge_len_get(root, pargs, args, num, &size);

   return content;
}

UChar *parser_extension_nth_merge_len_get(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, int num, size_t *size)
{
   UChar             *content = NULL;
   Wiki_Template_Arg *arg;
   size_t s = 0;

   WD_EXTENSION_COUNT_MAX;

   if(! root) return NULL;
   if(! args) return NULL;
   if(! args->list) return NULL;

   if(eina_list_count(args->list) > num) {
      arg = eina_list_nth(args->list, num);
      if(arg) {
	 if(arg->key)
	    s = arg->key_size;
	 if(arg->have_equal)
	    s++;
	 if(arg->val) {
	    if(arg->have_equal && arg->val[0] == '=')
	       s += 1;
	    s += arg->val_size;
	 }
	 if(s > 0)
	    content = calloc(s + 1, sizeof(UChar));
	 if(content) {
	    if(arg->key)
	       u_strcat(content, arg->key);
	    if(arg->have_equal && arg->val && arg->val[0] == '=')
	       u_strcat(content, ICU_STR_RETURN);
	    if(arg->have_equal)
	       u_strcat(content, ICU_STR_EQUAL_SIGN);
	    if(arg->val)
	       u_strcat(content, arg->val);
	 }
      }
   }

   *size = (content) ? s : 0;

   return content;
}

UChar *parser_extension_nth_get(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, int num)
{
   UChar       *content = NULL, *tmp = NULL;
   size_t       size = 0;

   WD_EXTENSION_COUNT_MAX;

   tmp = parser_extension_nth_merge_len_get(root, pargs, args, num, &size);
   if(tmp) {
      if(u_strstr(tmp, ICU_STR_DBL_CURL_OPEN)) {
	 int error = 0;
	 tmp = wiki_parser_parse_part(root, tmp, size, pargs, &error);
      }
      content = tmp;
   }

   return content;
}

UChar *parser_extension_time(Wiki_Parser *root, const char *date, const char *format, int timezone, void *data)
{
   struct tm result;
   time_t now;
   UChar *content = NULL;
   char tmp[1024];

   if(! root) return content;
   if(! format) return content;

   now = time(NULL);
   if(timezone == WIKI_TIME_UTC) {
      gmtime_r(&now, &result);
   } else {
      localtime_r(&now, &result);
   }

   if(strftime(tmp, sizeof(tmp), format, &result)) {
      content = u_strdupC(tmp);
   }

   return content;
}

double number_read(const char *str) {
   return wiki_parser_template_expr_calc(str);
}

double number_read_u(const UChar *str) 
{
   double ret = 0.0;
   char *tmp = NULL;

   if(!str)
      return nan((const char *) str);

   tmp = u_strToUTF8_new(str);
   if(tmp) {
      ret = number_read(tmp);
      free(tmp);
   } else {
      ret = nan((const char *) str);
   }

   return ret;
}

