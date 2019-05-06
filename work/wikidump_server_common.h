#ifndef _WIKIDUMP_SERVER_COMMON_H
#define _WIKIDUMP_SERVER_COMMON_H

#include "parser.h"
#include "wiki_static_pages.h"

enum 
{
   CONTENT_TYPE_DEFAULT,
   CONTENT_TYPE_CSS,
   CONTENT_TYPE_JS,
   CONTENT_TYPE_PLAIN,
   CONTENT_TYPE_XML
};

typedef struct _Http_Qry_Arg
{
   char *name;
   char *value;
} Http_Qry_Arg;

char *output_page(Wiki_Parser_Data *wd, const char *pagename, int raw, int redirect);
char *output_content(Wiki_Parser_Data *wd, UChar *content , int raw, int redirect);
void http_send_headers(int content_type);
void *wiki_parser_data_free(Wiki_Parser_Data *wd);
Wiki_Parser_Data *wiki_parser_data_init(Wiki_Sqlite *ws);
#endif
