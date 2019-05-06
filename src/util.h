#ifndef _ICU_UTIL_H
#define _ICU_UTIL_H

#include <stdarg.h>
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/putil.h>
#include <unicode/uiter.h>

#define INT_STR_SIZE 256

extern const UChar *ICU_STR_EMPTY; 
extern const UChar *ICU_STR_RETURN; 
extern const UChar *ICU_STR_SPACE; 
extern const UChar *ICU_STR_SLASH; 
extern const UChar *ICU_STR_PIPE; 
extern const UChar *ICU_STR_REG_MATCH_1; 
extern const UChar *ICU_STR_REG_MATCH_2; 
extern const UChar *ICU_STR_EQUAL_SIGN; 
extern const UChar *ICU_STR_APOSTROPHE; 
extern const UChar *ICU_STR_COLON; 
extern const UChar *ICU_STR_HASH; 
extern const UChar *ICU_STR_ANGL_OPEN; 
extern const UChar *ICU_STR_ANGL_CLOSE; 
extern const UChar *ICU_STR_DBL_CURL_OPEN; 
extern const UChar *ICU_STR_TRI_CURL_OPEN; 
extern const UChar *ICU_STR_CURL_OPEN; 
extern const UChar *ICU_STR_CURL_CLOSE; 
extern const UChar *ICU_STR_NOWIKI; 
extern const UChar *ICU_STR_PRE; 
extern const UChar *ICU_STR_MATH; 
extern const UChar *ICU_STR_SOURCE; 
extern const UChar *ICU_STR_CLOSE_TAG; 

extern unsigned int output_log;

void icu_util_init(void);
void icu_util_shutdown(void);

void uprintf(const char *pattern, ...);
void ulog(const char *pattern, ...);

char *u_strToUTF8_new(const UChar *s);
char *u_strnToUTF8_new(const UChar *s, size_t size);

UChar *trim_u(UChar *str);
UChar *trimn_u(UChar *str, size_t size, size_t *nsize);
UChar *trimn_special_u(UChar *str);
UChar *u_strdup(const UChar *s);
UChar *u_strdupC(const char *s);
UChar *u_strndup(const UChar *s, size_t size);
UChar *u_strndupC(const char *s, size_t size);
UChar *u_strtolower(UChar *s);

UChar *icu_replace_new(const char *match, const UChar *replace, const UChar *text, uint32_t flags);
UChar *icu_replace(const char *match, const UChar *replace, UChar *text, uint32_t flags);
UChar *icu_replace_utf8(const char *match, const char *replace, UChar *text, uint32_t flags);

const UChar *icu_search(const char *match, const UChar *text, uint32_t flags);
const UChar *icu_search_size(const char *match, const UChar *text, size_t size, uint32_t flags);
void uregex_cache_free(void);

char *strtoupper(char*);
char *strtolower(char *);

char *get_hash_path(const char *name, int split);
char *get_md5(const char *str, size_t size);

UChar *url_encode(const UChar *url);
UChar *url_encode_size(const UChar *url, size_t size);
char *url_encodeC(const char *url);
char *url_encode_sizeC(const char *url, size_t size);
char *url_desencode_sizeC(const char *url, size_t size);
char *url_desencodeC(const char *url);

#endif
