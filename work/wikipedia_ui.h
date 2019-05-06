#ifndef _WIKIPEDIA_UI_H
#define _WIKIPEDIA_UI_H

#include "parser.h"

char *wiki_parser_html_page_default_get(Wiki_Parser *wp);
char *wiki_parser_xml_page_default_get(Wiki_Parser *wp);
char *wiki_parser_html_page_source_get(Wiki_Parser *wp);
char *wiki_parser_html_page_template_included_get(Wiki_Parser *wp); 
char *wiki_parser_parsed_root_get(Wiki_Parser *wp);

#endif
