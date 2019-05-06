#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

#include <unicode/utypes.h>
#include <unicode/uclean.h>
#include <unicode/uchar.h>
#include <unicode/putil.h>
#include <unicode/uiter.h>
#include <unicode/ustdio.h>
#include <unicode/uregex.h>
#include <unicode/parseerr.h>

#include <Eina.h>
#include "util.h"

#include "md5.h"

#ifdef HAVE_THREADS
#undef HAVE_THREADS
#endif
#define HAVE_THREADS 1

static Eina_Lock *glock = NULL;
static UFILE *ustdout = NULL;
static UFILE *ustderr = NULL;
static unsigned int icu_initialized = 0;
static Eina_Hash *regex_patterns = NULL;
unsigned int output_log = 0;

const UChar *ICU_STR_EMPTY = NULL;
const UChar *ICU_STR_RETURN = NULL; 
const UChar *ICU_STR_SPACE = NULL; 
const UChar *ICU_STR_SLASH = NULL; 
const UChar *ICU_STR_PIPE = NULL; 
const UChar *ICU_STR_REG_MATCH_1 = NULL; 
const UChar *ICU_STR_REG_MATCH_2 = NULL; 
const UChar *ICU_STR_EQUAL_SIGN = NULL; 
const UChar *ICU_STR_APOSTROPHE = NULL; 
const UChar *ICU_STR_COLON = NULL; 
const UChar *ICU_STR_HASH = NULL; 
const UChar *ICU_STR_ANGL_OPEN = NULL; 
const UChar *ICU_STR_ANGL_CLOSE = NULL; 
const UChar *ICU_STR_DBL_CURL_OPEN = NULL; 
const UChar *ICU_STR_TRI_CURL_OPEN = NULL; 
const UChar *ICU_STR_CURL_OPEN = NULL; 
const UChar *ICU_STR_CURL_CLOSE = NULL; 
const UChar *ICU_STR_CLOSE_TAG = NULL; 
const UChar *ICU_STR_NOWIKI = NULL; 
const UChar *ICU_STR_PRE = NULL; 
const UChar *ICU_STR_MATH = NULL; 
const UChar *ICU_STR_SOURCE = NULL; 

void icu_util_regex_free(void *data);

void icu_util_regex_free(void *data)
{
   URegularExpression *reg = data;
   if(reg) {
      uregex_close(reg);
   }
}

void icu_util_init(void)
{
   if(icu_initialized == 0) {
      UErrorCode err = 0;
      char *debug = NULL;

      u_init(&err);

      ustdout = u_finit(stdout, NULL, NULL);
      ustderr = u_finit(stderr, NULL, NULL);

      regex_patterns = eina_hash_string_superfast_new(icu_util_regex_free);
      glock = calloc(1, sizeof(Eina_Lock));
      if(glock) {
	 if(eina_lock_new(glock) != EINA_TRUE) {
	    printf("Can't init mutex lock\n");
	    exit(1);
	 }
      }

      ICU_STR_EMPTY= u_strdupC("");
      ICU_STR_RETURN = u_strdupC("\n");
      ICU_STR_SPACE = u_strdupC(" ");
      ICU_STR_SLASH = u_strdupC("/");
      ICU_STR_PIPE = u_strdupC("|");
      ICU_STR_REG_MATCH_1 = u_strdupC("$1");
      ICU_STR_REG_MATCH_2 = u_strdupC("$2");
      ICU_STR_EQUAL_SIGN = u_strdupC("=");
      ICU_STR_APOSTROPHE = u_strdupC("'");
      ICU_STR_COLON = u_strdupC(":");
      ICU_STR_HASH = u_strdupC("#");
      ICU_STR_ANGL_OPEN = u_strdupC("<");
      ICU_STR_ANGL_CLOSE = u_strdupC(">");
      ICU_STR_DBL_CURL_OPEN = u_strdupC("{{");
      ICU_STR_TRI_CURL_OPEN = u_strdupC("{{{");
      ICU_STR_CURL_OPEN = u_strdupC("{");
      ICU_STR_CURL_CLOSE = u_strdupC("}");
      ICU_STR_CLOSE_TAG = u_strdupC("</");

      ICU_STR_NOWIKI = u_strdupC("nowiki");
      ICU_STR_PRE = u_strdupC("pre");
      ICU_STR_MATH = u_strdupC("math");
      ICU_STR_SOURCE = u_strdupC("source");

      debug = getenv("WIKI_DEBUG");
      if(debug) {
	 output_log = atoi(debug);
      } else {
	 output_log = 0;
      }
   }
   icu_initialized++;
}

void icu_util_shutdown(void)
{
   if(icu_initialized > 0) {
      icu_initialized--;
      if(icu_initialized == 0) {
	 if(regex_patterns)
	    eina_hash_free(regex_patterns);
	 regex_patterns = NULL;

	 free((UChar *) ICU_STR_EMPTY);
	 free((UChar *) ICU_STR_RETURN);
	 free((UChar *) ICU_STR_SPACE);
	 free((UChar *) ICU_STR_SLASH);
	 free((UChar *) ICU_STR_PIPE);
	 free((UChar *) ICU_STR_REG_MATCH_1);
	 free((UChar *) ICU_STR_REG_MATCH_2);
	 free((UChar *) ICU_STR_EQUAL_SIGN);
	 free((UChar *) ICU_STR_APOSTROPHE);
	 free((UChar *) ICU_STR_COLON);
	 free((UChar *) ICU_STR_HASH);
	 free((UChar *) ICU_STR_ANGL_OPEN);
	 free((UChar *) ICU_STR_ANGL_CLOSE);
	 free((UChar *) ICU_STR_DBL_CURL_OPEN);
	 free((UChar *) ICU_STR_TRI_CURL_OPEN);
	 free((UChar *) ICU_STR_CURL_OPEN);
	 free((UChar *) ICU_STR_CURL_CLOSE);
	 free((UChar *) ICU_STR_CLOSE_TAG);

	 free((UChar *) ICU_STR_NOWIKI);
	 free((UChar *) ICU_STR_PRE);
	 free((UChar *) ICU_STR_MATH);
	 free((UChar *) ICU_STR_SOURCE);

	 u_fclose(ustdout);
	 u_fclose(ustderr);

	 ustdout = NULL;
	 ustderr = NULL;

	 u_cleanup();

	 if(glock) {
	    eina_lock_free(glock);
	    free(glock);
	 }
      }
   }
}

void uprintf(const char *pattern, ...)
{
   va_list ap;

   if(! icu_initialized) return;
   if(! pattern) return;

   va_start(ap, pattern);
   u_vfprintf(ustdout, pattern, ap);
   va_end(ap);

   u_fflush(ustdout);
}

void ulog(const char *pattern, ...)
{
   va_list ap;

   if(! icu_initialized) return;
   if(! pattern) return;

   if(output_log == 0) return;

   va_start(ap, pattern);
   u_vfprintf(ustderr, pattern, ap);
   va_end(ap);
}

char *u_strToUTF8_new(const UChar *s)
{
   char t[1];
   char *n = NULL;
   int32_t size = 0;
   UErrorCode e = 0;

   if(! s) return NULL;

   u_strToUTF8(t, 0, &size, s, u_strlen(s), &e);
   if(size >= 0) {
      e = 0;
      n = calloc(size + 1, sizeof(char));
      if(n) {
	 int32_t r = 0;
	 u_strToUTF8(n, size, &r, s, u_strlen(s), &e);
      }
   }

   return n;
}

char *u_strnToUTF8_new(const UChar *s, size_t size)
{
   char t[1];
   char *n = NULL;
   int32_t size_dest = 0;
   UErrorCode e = 0;

   if(! s) return NULL;

   u_strToUTF8(t, 0, &size_dest, s, size, &e);
   if(size_dest >= 0) {
      e = 0;
      n = calloc(size_dest + 1, sizeof(char));
      if(n) {
	 int32_t r = 0;
	 u_strToUTF8(n, size_dest, &r, s, size, &e);
      }
   }

   return n;
}

UChar *u_strdup(const UChar *s)
{
   UChar *n = NULL;
   size_t size = 0;

   if(! s) return NULL;

   size = u_strlen(s);
   n = calloc(size + 1, sizeof(UChar));
   if(n) {
      u_memcpy(n, s, size);
   }

   return n;
}

UChar *u_strdupC(const char *s)
{
   UChar t[1];
   UChar *n = NULL;
   int32_t size = 0;
   UErrorCode e = 0;

   if(! s) return NULL;

   u_strFromUTF8(t, 0, &size, s, strlen(s), &e);
   if(size >= 0) {
      e = 0;
      n = calloc(size + 1, sizeof(UChar));
      if(n) {
	 int32_t r = 0;
	 u_strFromUTF8(n, size, &r, s, strlen(s), &e);
      }
   } 

   return n;
}

UChar *u_strndup(const UChar *s, size_t size)
{
   UChar *n = NULL;
   if(! s) return NULL;

   if(size < 0) return NULL;

   n = calloc(size + 1, sizeof(UChar));
   if(n) {
      u_memcpy(n , s, size);
   }

   return n;
}

UChar *u_strndupC(const char *s, size_t size)
{
   UChar t[1];
   UChar *n = NULL;
   int32_t size_dest = 0;
   UErrorCode e = 0;

   if(! s) return NULL;

   if(size <= 0) return NULL;

   u_strFromUTF8(t, 0, &size_dest, s, size, &e);
   if(size_dest >= 0) {
      e = 0;
      n = calloc(size_dest + 1, sizeof(UChar));
      if(n) {
	 int32_t r = 0;
	 u_strFromUTF8(n, size_dest, &r, s, size, &e);
      }
   }

   return n;
}

UChar *trim_u(UChar *str)
{
   size_t size = 0;
   if(! str) return NULL;

   size = u_strlen(str);
   return trimn_u(str, size, NULL);
}

UChar *trimn_u(UChar *str, size_t size, size_t *nsize)
{
   UChar *p = str;
   size_t s = size;

   if(! str) return NULL;

   p = str + s - 1;
   while(p >= str && *p && u_isWhitespace(*p)) {
      *p = '\0';
      p--;
      s--;
   }

   p = str;
   while(*p && u_isWhitespace(*p)) {
      p++;
      s--;
   }

   if(p > str && s > 0) {
      u_memmove(str, p, s + 1);
   }

   if(str && s == 0) {
      free(str);
      str = NULL;
   }

   if(nsize) {
      *nsize = s;
   }

   return str;
}

UChar *trimn_special_u(UChar *str)
{
   UChar *p = str;
   size_t s = 0;

   if(! str) return NULL;
   s = u_strlen(str);

   p = str + s - 1;
   while(p >= str && *p && u_isWhitespace(*p)) {
      *p = '\0';
      p--;
      s--;
   }

   p = str;
   while(*p && u_isWhitespace(*p)) {
      p++;
      s--;
   }

   if(p > str && (p - str) == 1 && 
	 ((*p == '{' && *(p + 1) == '|')
	  || *p == '='
	  || *p == ':'
	  || *p == ';'
	  || *p == '*'
	  || *p == '#'))
      p = str;

   if(p > str && s > 0) {
      u_memmove(str, p, s + 1);
   }

   if(str && s == 0) {
      free(str);
      str = NULL;
   }

   return str;
}

UChar *u_strtolower(UChar *s)
{
   UChar *n = NULL;
   int32_t size = 0;
   UErrorCode error = 0;

   if(! s) return n;

   size = u_strToLower(n, 0, s, -1, "en_US.UTF-8", &error);
   if(size > 0) {
      error = 0;
      n = calloc(size + 1, sizeof(UChar));
      if(n) {
	 u_strToLower(n, size, s, -1, "en_US.UTF-8", &error);
      };
   }

   free(s);
   return n;
}

char *strtoupper(char *str) 
{
   char *p = str;

   if(! str) return str;

   while(*p) {
      if(islower(*p))
	 *p = toupper(*p);
      p++;
   }

   return str;
}

char *strtolower(char *str) 
{
   char *p = str;

   if(! str) return str;

   while(*p) {
      if(isupper(*p))
	 *p = tolower(*p);
      p++;
   }

   return str;
}

void uregex_cache_free(void)
{
   Eina_Hash *tmp = NULL;

#ifdef HAVE_THREADS
   if(eina_lock_take(glock) != EINA_LOCK_SUCCEED)
      return;
#endif

   if(regex_patterns) 
      tmp = regex_patterns;

   regex_patterns = eina_hash_string_superfast_new(icu_util_regex_free);

   if(tmp) 
      eina_hash_free(tmp);
   tmp = NULL;

#ifdef HAVE_THREADS
   eina_lock_release(glock);
#endif
}

UChar *icu_replace_utf8(const char *match, const char *replace, UChar *text, uint32_t flags)
{
   UChar *tmp = NULL;

   tmp = u_strdupC(replace);
   if(tmp) {
      text = icu_replace(match, tmp, text, flags);
      free(tmp);
   }

   return text;
}

UChar *icu_replace(const char *match, const UChar *replace, UChar *text, uint32_t flags)
{
   UChar *res = NULL;
   if(text) {
      res = icu_replace_new(match, replace, text, flags);
      free(text);
   }

   return res;
}


URegularExpression *__icu_regex_open(const char *match, uint32_t flags);
URegularExpression *__icu_regex_open(const char *match, uint32_t flags)
{
   URegularExpression *reg = NULL;
   UParseError *pe = NULL;
   UErrorCode ec = U_ZERO_ERROR;
   char *key = NULL;

   if(! regex_patterns) return NULL;
   if(! match) return NULL;

   if(flags == 0) 
      reg = eina_hash_find(regex_patterns, match);
   else {
      asprintf(&key, "%s_%d", match, flags);
      if(key) {
         reg = eina_hash_find(regex_patterns, key);
      }
   }
   if(reg == NULL) {
     // pe = calloc(1, sizeof(UParseError));
      ec = U_ZERO_ERROR;
      reg = uregex_openC(match, flags, pe, &ec);
      if(! reg || ec != U_ZERO_ERROR) {
	 if(pe) {
	    ulog("[%d] Regex Error in : \"%s\" at line : %d, offset : %d\n",
		  __LINE__, match, pe->line, pe->offset);
	 } else {
	    ulog("[%d] Regex Error in : \"%s\"\n", __LINE__, match);
	 }
	 if(reg) {
	    uregex_close(reg);
            reg = NULL;
         }
      }
      if(reg && flags == 0)
         eina_hash_add(regex_patterns, match, reg);
      else if(reg && key)
         eina_hash_add(regex_patterns, key, reg);
   } else {
      // uregex_reset(reg, 0, &ec);
      if(ec != U_ZERO_ERROR) {
         printf("Failed (%d) to fetch (%d) reg: %s\n", ec, flags, match);
         if(flags == 0)
            eina_hash_del(regex_patterns, match, reg);
         else if(key)
            eina_hash_del(regex_patterns, key, reg);
         uregex_close(reg);
         reg = __icu_regex_open(match, flags);
      }
   }

   if(key)
      free(key);

   if(reg == NULL)
      printf("Failed (%d) to fetch reg: %s\n", ec, match);

   return reg;
}

UChar *icu_replace_new(const char *match, const UChar *replace, const UChar *text, uint32_t flags)
{
   UChar *res = NULL;
   URegularExpression *reg = NULL;
   UErrorCode ec = 0;
   size_t size = 0, size_org = 0;

   if(! icu_initialized) return NULL;
   if(! match || ! replace || !text) return NULL;

#ifdef HAVE_THREADS
   if(eina_lock_take(glock) != EINA_LOCK_SUCCEED)
      return NULL;
#endif

   reg = __icu_regex_open(match, flags);
   if(! reg) return NULL;

   size_org = u_strlen(text);
   ec = U_ZERO_ERROR;
   uregex_setText(reg, text, size_org, &ec);
   if(ec != U_ZERO_ERROR) {
      ulog("[%d] Regex Error: set text failed : %d\n", __LINE__, ec);
      return NULL;
   }

   size = size_org;
   res = calloc(size + 1, sizeof(UChar));
   if(! res) {
      ulog("malloc error in %s at %d\n", __FILE__, __LINE__);
      return NULL;
   }

   ec = U_ZERO_ERROR;
   size = uregex_replaceAll(reg, replace, u_strlen(replace), res, size_org, &ec);
   while(ec == U_BUFFER_OVERFLOW_ERROR) {
      free(res);
      res = calloc(size + 1, sizeof(UChar));
      if(! res) {
	 ulog("malloc error in %s at %d\n", __FILE__, __LINE__);
	 return NULL;
      }
      ec = U_ZERO_ERROR;
      size_org = size;
      size = uregex_replaceAll(reg, replace, u_strlen(replace), res, size_org, &ec);
   }

#ifdef HAVE_THREADS
   eina_lock_release(glock);
#endif

   return res;
}

const UChar *icu_search(const char *match, const UChar *text, uint32_t flags)
{
   const UChar *res = NULL;
   size_t size = 0;

   if(! icu_initialized) return NULL;
   if(!match || !text) return NULL;

   size = u_strlen(text);

   res = icu_search_size(match, text, size, flags);

   return res;
}

const UChar *icu_search_size(const char *match, const UChar *text, size_t size, uint32_t flags)
{
   const UChar *res = NULL;
   URegularExpression *reg = NULL;
   UErrorCode ec = 0;

   if(! icu_initialized) return NULL;
   if(! match || ! text || size <= 0) return NULL;
   if(! regex_patterns) return NULL;

#ifdef HAVE_THREADS
   if(eina_lock_take(glock) != EINA_LOCK_SUCCEED)
      return NULL;
#endif

   reg = __icu_regex_open(match, flags);
   if(! reg) return NULL;

   uregex_setText(reg, text, size, &ec);
   if(ec != U_ZERO_ERROR) {
      ulog("[%d] Regex Error: set text failed : %d, size = %d\n", 
	    __LINE__, ec, size);
      ulog("TEXT: %s, %S\n", match, text);
      return NULL;
   }

   ec = U_ZERO_ERROR;
   if(uregex_find(reg, -1, &ec)) {
      int pos = uregex_start(reg, 0, &ec);
      if(pos > -1)
	 res = text + pos;
   }

#ifdef HAVE_THREADS
   eina_lock_release(glock);
#endif

   return res;
}

char *get_md5(const char *str, size_t size)
{
   char *buf = NULL;
   unsigned int md_len = 0, i, p;
   unsigned char md_val[65];

   if(! str || size < 1) return buf;

   md5_buffer(str, size, md_val);
   md_len = 16;

   if(md_len)
      buf = calloc((md_len * 2) + 1, sizeof(char));

   if(! buf) return buf;

   p = 0;
   for(i = 0; i < md_len; i++) {
      sprintf(buf + p, "%02x", md_val[i]);
      p += 2;
   }

   return buf;
}

char *get_hash_path(const char *name, int split)
{
   unsigned int md_len = 0, i, j, s, p;
   char *ret = NULL, *buf = NULL;
   unsigned char md_val[65];

   if(! name) return buf;

   md5_buffer(name, strlen(name), md_val);
   md_len = 16;

   s = split;
   if(s > 8) s = 8;

   if(md_len)
      buf = calloc((md_len * 2) + 1, sizeof(char));
   if(! buf) return ret;

   p = 0;
   for(i = 0; i < md_len; i++) {
      sprintf(buf + p, "%02x", md_val[i]);
      p += 2;
   }

   p = 0;
   ret = calloc(powl(2, s) + 1, sizeof(char));
   if(ret) {
      for(i = 0; i < s; i++) {
	 if(i > 0) {
	    ret[p] = '/';
	    p++;
	 }
	 for(j = 0; j < i + 1; j++) {
	    ret[p] = buf[j];
	    p++;
	 }
      }
   }

   free(buf);

   return ret;
}

UChar *url_encode(const UChar *url)
{
   UChar *content = NULL;
   if(! url) return NULL;

   content = url_encode_size(url, u_strlen(url));

   return content;
}

UChar *url_encode_size(const UChar *url, size_t size)
{
   UChar *content = NULL;
   char *tmp = NULL;

   if(! url || size < 0) return NULL;
   
   tmp = u_strnToUTF8_new(url, size);
   if(tmp) {
      char *enc = url_encodeC(tmp);
      if(enc) {
	 content = u_strdupC(enc);
	 free(enc);
      }
      free(tmp);
   }
   return content;
}

char *url_encodeC(const char *url)
{
   return url_encode_sizeC(url, (url) ? strlen(url) : 0);
}

static int is_hexdigit(unsigned char c) 
{
   if((c >= '0' && c <= '9') 
	 || (c >= 'A' && c <= 'Z')
	 || (c >= 'a' && c <= 'z'))
      return 1;
   else
      return 0;
}

char *url_encode_sizeC(const char *url, size_t size)
{
   char *content = NULL;
   char *tmp = NULL;
   unsigned int i = 0, pos = 0;
   size_t s = size;

   if(! url || size < 0) return NULL;

   tmp = url_desencode_sizeC(url, size);
   if(! tmp) return NULL;

   s = strlen(tmp);
   content = calloc((s * 4) + 1, sizeof(char));
   if(! content) return NULL;

   while(i < s) {
      unsigned char c = tmp[i];
      if(tmp[i] == ' ') {
	 content[pos] = '_';
	 pos++;
      } else if(tmp[i] == '.' 
	    || tmp[i] == ':'
	    || tmp[i] == '-'
	    || tmp[i] == '_') {
	 content[pos] = tmp[i];
	 pos++;
      } else if(! is_hexdigit(c)) {
	 snprintf(content + pos , 4, "%%%02X", c);
	 pos += 3;
      } else {
	 content[pos] = tmp[i];
	 pos++;
      }
      i++;
   }

   free(tmp);
   
   return content;
}

char *url_desencodeC(const char *url)
{
   return url_desencode_sizeC(url, (url) ? strlen(url) : 0);
}

char *url_desencode_sizeC(const char *url, size_t size)
{
   char *content = NULL;
   unsigned int i = 0, pos = 0;

   if(! url || size < 0) return NULL;

   content = calloc(size + 1, sizeof(char));
   if(! content) return NULL;

   while(url[i] && i < size) {
      if(url[i] == '_') {
	 content[pos] = ' ';
	 pos++;
      } else if(i + 2 < size && url[i] == '%' 
	    && is_hexdigit(url[i + 1]) && is_hexdigit(url[i + 2])) {
	 char tmp[3];
	 tmp[0] = url[i + 1];
	 tmp[1] = url[i + 2];
	 tmp[2] = '\0';
	 content[pos] = strtoul(tmp, NULL, 16);
	 pos++;
	 i += 2;
      } else {
	 content[pos] = url[i];
	 pos++;
      }

      i++;
   }

   return content;
}

