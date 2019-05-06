#ifndef _WIKI_PARSER_H
#define _WIKI_PARSER_H

#define WIKI_PARSER_VERSION "libmwparser0.0"

#include "lexer.h"

#include <stdlib.h>

#include <unicode/utypes.h>
#include <unicode/ustdio.h>
#include <libxml/tree.h>
#include <Eina.h>

#include "wiki_define.h"
#include "wiki_lang.h"

#define XML_CHAR(str)    (xmlChar *) str
#define XML_CONST_CHAR(str)    (const xmlChar *) str
extern int level_depth;
#define PARSE_TRACE()   ulog("TRACE:%d: %s\n", level_depth, __FUNCTION__);


enum
{
   WIKI_PARSER_ERROR_NONE,
   WIKI_PARSER_ERROR_CANT_OPEN_FILE,
   WIKI_PARSER_ERROR_ALLOCATE_PARSER,
   WIKI_PARSER_ERROR_LEXER,
   WIKI_PARSER_ERROR_TEMPLATE_LOOP,
   WIKI_PARSER_MAX_TEMPLATE_COUNT,
   WIKI_PARSER_MAX_REDIRECT_COUNT,
   WIKI_PARSER_MAX_TEMPLATE_NOT_FOUND
};

enum 
{
   WP_BI_STATE_NONE = 0,
   WP_BI_STATE_I    = 2,
   WP_BI_STATE_B    = 4,
   WP_BI_STATE_BI   = 8,
   WP_BI_STATE_IB   = 16,
   WP_BI_STATE_BOTH = 32
};

typedef struct _Wiki_Buffer
{
   UChar *buf;
   Wiki_Lexer *lexer;
   size_t size;
} Wiki_Buffer;

typedef struct _Wiki_Title
{
   UChar *lang;
   UChar *sitename;
   UChar *ns;
   UChar *title;
   UChar *arg;
   int    nsid;
   Eina_Bool    intext;
   Eina_Bool    lang_first;
} Wiki_Title;

typedef struct _Wiki_Args
{
   Eina_Hash *hash; /* Access template args by hash is faster */
   Eina_List *list; /* but parser function don't use name parameter */
} Wiki_Args;

typedef struct _Wiki_Nowiki_Part
{
   const char *tagname;
   UChar *args;
   UChar *content;
   size_t tagname_size;
   size_t args_size;
   size_t content_size;
} Wiki_Nowiki_Part;

typedef struct _Wiki_Template_Arg 
{
   UChar *key;
   UChar *val;
   size_t key_size;
   size_t val_size;
   Eina_Bool have_equal;
} Wiki_Template_Arg;

typedef struct _Wiki_Template_Loaded
{
   struct timeval tv;
   UChar *title;
   UChar *content;
   UChar *redirect;
   size_t content_size;
   int    usage_count;
   int    redirect_count;
   Eina_Bool is_template;
   Eina_Bool in_error;
} Wiki_Template_Loaded;

typedef struct _Wiki_Parser Wiki_Parser;
struct _Wiki_Parser
{
   Wiki_Lexer       *lexer;
   const Wiki_Parser_Data *wd;
   int               lexer_error;
   const char       *title;
   char             *fullpagename;
   const UChar      *current_template;

   struct timeval    time_start,
		     time_parse,
		     time_xhtml,
		     time_load,
		     time_rval;

   xmlDocPtr         xml;
   xmlNodePtr        node;
   xmlNodePtr        root;
   xmlNodePtr        in_header;

   Eina_List        *langs;
   Eina_List        *categories;

   int               level;
   int               ext_cnt;
   int               templates_num;
   int               templates_use_count;
   int		     templates_not_found;
   int               nowiki_num;
   Eina_Hash        *nowiki;
   Eina_Hash        *templates;
   Eina_Array       *templates_call;

   UChar            *source;
   UChar            *buf;
   size_t            size;

   int               bi_state;
   int               in_table;
   int               cat_done;
   int               external_link;
   int               in_pre_format;
   int               nested;
   Eina_Bool         in_wiki_list;
   Eina_Bool         toc;
   Eina_Bool         is_template;
};

int wiki_parser_init(void);
void wiki_parser_shutdown(void);

void wiki_parser_template_load_callback_set(int (*template_load)(void *, const char*, UChar **, size_t *), void *data);

Wiki_Parser *wiki_parser_buf_new(const Wiki_Parser_Data *wd, const UChar *buf, size_t size, int *error);
UChar *wiki_parser_parse_part(Wiki_Parser *root, UChar *buf, size_t size, Wiki_Args *args, int *error);

Wiki_Parser *wiki_parser_buf_full_new(const Wiki_Parser_Data *wd, const UChar *buf, size_t size, Wiki_Args *args, Eina_Bool is_include, int *error);

Wiki_Parser *wiki_parser_free(Wiki_Parser *wp);

UChar *
wiki_parser_template_replace(Wiki_Parser *root, Wiki_Args *pargs, UChar *buf, size_t buf_size, size_t *new_size);
Eina_List *wiki_parser_template_args_list_get(UChar *buf, int noexpr);
Eina_List *wiki_parser_template_args_list_get2(UChar *buf, Eina_Bool expr);
Wiki_Args *wiki_parser_wiki_args_get(Wiki_Parser *root, Wiki_Args *pargs, UChar *buf, Eina_Bool is_expr);
Wiki_Args *wiki_parser_wiki_args_free(Wiki_Args *wa);

UChar *wiki_parser_template_noinclude_remove(UChar *content);
UChar *wiki_parser_template_add(Wiki_Parser *root, UChar *tname, Wiki_Args *pargs, Wiki_Args *args, Eina_Bool redirect, Eina_Bool parse);
Wiki_Template_Loaded* wiki_parser_template_load(Wiki_Parser *root, UChar *tname, Eina_Bool redirect);
UChar *wiki_parser_uniq_string_get(Wiki_Parser *wp, const char *tagname);

Wiki_Parser *wiki_parser_add(const Wiki_Parser_Data *wd);
xmlNodePtr wiki_parser_node_add(xmlNodePtr parent, const char *tag);

void wiki_parser_extension_module_register(const char *extname, UChar *(func)(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data));


UChar *wiki_parser_intl_get(UChar *msgid, size_t *size);
const Wiki_Lang *wiki_parser_language_infos_get(const char *code);

const char *wiki_parser_namespace_name_get(const Wiki_Parser_Data *wd, int nsid);

const char *wiki_parser_fullpagename_get(Wiki_Parser *wp);

char *wiki_parser_fullpagename_interwiki_get(Wiki_Parser *wp, const char *name, int ns, const char *site, const char *lang);

char *wiki_parser_url_image_get(const Wiki_Parser_Data *wd, const char *file); 

char *wiki_parser_image_url_thumb_set(char *url, int size); 

char *wiki_parser_url_internal_get(Wiki_Parser *wp, int ns, const char *pagename, int raw, int *exists);

char *wiki_parser_url_origin_get(Wiki_Parser *wp, int ns, const char *pagename);

char *wiki_parser_url_interwiki_get(Wiki_Parser *wp, const char *pagename, const char *site, const char *lang, int *exists);


xmlNodePtr wiki_parser_convert_xhtml(Wiki_Parser *parser, int *error, int flags);
UChar *wiki_parser_title_case_set(const Wiki_Parser_Data *wd, UChar *str);
char *wiki_parser_utitle_case_set(const Wiki_Parser_Data *wd, char *str);

Wiki_Title *parser_title_part(const Wiki_Parser_Data *wd, const UChar *pagename);
Wiki_Title *parser_title_free(Wiki_Title *wt);
UChar *parser_title_with_namespace(Wiki_Title *wt);

char *parser_title_rel2abs(const Wiki_Parser_Data *wd, const char *title, const char *path);
char *wiki_parser_anchor_encode(const char *url);

void wiki_parser_text_add(xmlNodePtr node, const UChar *data, size_t size, Eina_Bool entity);
void wiki_parser_text_utf8_add(xmlNodePtr node, const char *data, size_t size, Eina_Bool entity);

Wiki_File *wiki_parser_file_free(Wiki_File *wf);

#endif
