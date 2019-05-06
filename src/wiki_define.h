#ifndef _WIKI_DEFINE_H
#define _WIKI_DEFINE_H

#include <Eina.h>
#include <sys/time.h>

#define WIKI_THUMB_IMG_SIZE 180
#define WIKI_THUMB_BORDER_SIZE WIKI_THUMB_IMG_SIZE + 2
#define WIKI_PATH_SPLIT_MAX 25

enum
{
   WIKI_NS_MEDIA = -2,
   WIKI_NS_SPECIAL,
   WIKI_NS_MAIN,
   WIKI_NS_TALK,
   WIKI_NS_USER,
   WIKI_NS_USER_TALK,
   WIKI_NS_PROJECT,
   WIKI_NS_PROJECT_TALK,
   WIKI_NS_FILE,
   WIKI_NS_FILE_TALK,
   WIKI_NS_MEDIAWIKI,
   WIKI_NS_MEDIAWIKI_TALK,
   WIKI_NS_TEMPLATE,
   WIKI_NS_TEMPLATE_TALK,
   WIKI_NS_HELP,
   WIKI_NS_HELP_TALK,
   WIKI_NS_CATEGORY,
   WIKI_NS_CATEGORY_TALK
};

typedef enum _Wiki_Case
{
   WIKI_CASE_UNKNOW,
   WIKI_SENSITIVE,
   WIKI_FIRST_LETTER
} Wiki_Case;

typedef enum _Wiki_Ouput_Mode
{
   WIKI_OUTPUT_DEFAULT,
   WIKI_OUTPUT_RAW,
   WIKI_OUTPUT_SOURCE,
   WIKI_OUTPUT_SOURCE_TEMPLATE,
   WIKI_OUTPUT_PAGE_DUMP,
   WIKI_OUTPUT_XML,
   WIKI_OUTPUT_PARSED_ROOT,
   WIKI_OUTPUT_MODE_INVALID
} Wiki_Output_Mode;

typedef enum _Wiki_Stat
{
   WIKI_STAT_UNDEFINED,
   WIKI_STAT_NUMBER_OF_ARCTICLES,
   WIKI_STAT_NUMBER_OF_PAGES,
   WIKI_STAT_NUMBER_OF_FILES,
   WIKI_STAT_NUMBER_OF_EDITS,
   WIKI_STAT_NUMBER_OF_VIEWS,
   WIKI_STAT_NUMBER_OF_USERS,
   WIKI_STAT_NUMBER_OF_ADMINS,
   WIKI_STAT_NUMBER_OF_ACTIVE_USERS
} Wiki_Stat;

typedef struct _Wiki_Namespace 
{
   int   key;
   int   npages;
   const char *label;
} Wiki_Namespace;

typedef struct _Wiki_File
{
   const char  *mime;
   unsigned int size;
   unsigned int width;
   unsigned int height;
} Wiki_File;

typedef struct _Wiki_Ext
{
   int    cnt;
   struct timeval te;
   struct timeval ts;
   struct timeval tv;
} Wiki_Ext;

typedef struct _Wiki_Media 
{
   int siteid;
   const char *sitename;
   const char *lang;
   const char *localname;
   const char *locale;
   const char *base;
   const char *case_used;
   const void *data;
} Wiki_Media;

typedef struct _Wiki_Parser_Data Wiki_Parser_Data;
struct _Wiki_Parser_Data
{
   int   siteid;
   int   pageid;
   int   ns;
   Wiki_Case  wcase;
   int   port;
   const char *proto;
   const char *sitename;
   const char *localname;
   const char *pagename;
   const char *lang;
   const char *locale;
   const char *base;

   const char *servername;
   const char *scriptpath;
   const char *image_url;

   const char *search;

   Eina_Hash  *interwiki;
   Eina_Hash  *namespaces;
   Eina_List  *wikilist;
   const Wiki_Parser_Data *commons;

   Wiki_Output_Mode output_mode;
   Wiki_Output_Mode default_output_mode;

   void       *data;
   void       *data_page;

   Eina_Bool have_search;
   Eina_Bool no_page_exists;

   char         *(*namespace_get) (const Wiki_Parser_Data *, int); 
   char         *(*template_content_get) (const Wiki_Parser_Data *, const char *title, size_t *size);
   char         *(*image_url_get) (const Wiki_Parser_Data *, const char *title);
   Eina_Bool     (*page_exists) (const Wiki_Parser_Data *, const char *title);
   Eina_Bool     (*page_exists_interwiki) (const Wiki_Parser_Data *, const char *title, const char *sitename, const char* lang);
   int           (*page_nsid_get) (const Wiki_Parser_Data *, const char *title);
   Eina_List    *(*category_pages_get) (const Wiki_Parser_Data *, const char *);
   Eina_List    *(*search_pages_get) (const Wiki_Parser_Data *, const char *);
   int           (*pages_in_category_num_get) (const Wiki_Parser_Data *, const char *);
   char         *(*sitename_from_base) (const Wiki_Parser_Data *, const char *, const char *);
   unsigned int  (*stats_get) (const Wiki_Parser_Data *, Wiki_Stat);
   unsigned int  (*stats_npages_in_category) (const Wiki_Parser_Data *, const char *);

   unsigned int  (*page_revision_id_get) (const Wiki_Parser_Data *);
   char         *(*page_revision_timestamp_get) (const Wiki_Parser_Data *);
   char         *(*page_revision_comment_get) (const Wiki_Parser_Data *);
   char         *(*page_contributor_username_get) (const Wiki_Parser_Data *);
   unsigned int  (*page_contributor_id_get) (const Wiki_Parser_Data *);
   const char   *(*base_origin) (const Wiki_Parser_Data *);
   Wiki_File    *(*page_file_info_get) (const Wiki_Parser_Data *, const char * title);
};


#endif
