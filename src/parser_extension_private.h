#ifndef _PARSER_EXTENSION_PRIVATE_H
#define _PARSER_EXTENSION_PRIVATE_H

#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h> 

#include <Eina.h>

#include "parser.h"
#include "parser_extension.h"
#include "util.h"

enum {
   WIKI_TIME_UTC,
   WIKI_TIME_LOCAL
};

Eina_Bool parser_extension_content_is_number(const UChar *content);
UChar *parser_extension_indent_remove(UChar *content);
UChar *parser_extension_nth_get(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, int num);
UChar *parser_extension_nth_key_get(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, int num);
UChar *parser_extension_nth_merge_get(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, int num);
UChar *parser_extension_nth_merge_len_get(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, int num, size_t *size);
UChar *parser_extension_time(Wiki_Parser *root, const char *date, const char *format, int timezone, void *data);
UChar *parser_extension_module_null(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data);

double wiki_parser_template_expr_calc(const char *expr);
double number_read(const char *str);
double number_read_u(const UChar *str); 

UChar *url_encode(const UChar *url);

#endif
