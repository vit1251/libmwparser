#ifndef _PARSER_TAG_EXTENSIONS_H
#define _PARSER_TAG_EXTENSIONS_H

#include <Eina.h>
#include <libxml/tree.h>

#include "wiki_define.h"

typedef enum _Wiki_Parser_Tag_Conten_Type {
   WIKI_PARSER_TAG_CONTENT_PARSE_ALL,
   WIKI_PARSER_TAG_CONTENT_PARSE_TEMPLATE,
   WIKI_PARSER_TAG_CONTENT_PARSE_NONE
} Wiki_Parser_Tag_Content_Type;

typedef struct _Wiki_Parser_Tag_Extension
{
   const char *name;
   xmlNodePtr (*parse) (const Wiki_Parser_Data *wd, xmlNodePtr tag); 
   Wiki_Parser_Tag_Content_Type type;  
   Eina_Bool   block;
} Wiki_Parser_Tag_Extension;

Eina_Hash *wiki_parser_tag_extension_register(Eina_Hash *tags, 
      const char *tagname, 
      xmlNodePtr (*parse) (const Wiki_Parser_Data *wd, const xmlNodePtr tag),
	 Wiki_Parser_Tag_Content_Type type, Eina_Bool block);

const Eina_Hash *wiki_parser_tag_extension_init(void);
const Eina_Hash *wiki_parser_html_attributes(void);
const Eina_Hash *wiki_parser_tag_extension_free(void);


#endif
