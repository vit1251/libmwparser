#ifndef _WIKIDUMP_SQLITE_H
#define _WIKIDUMP_SQLITE_H

#include <sqlite3.h>

#include "wiki_define.h"
#include "wikidump.h"

enum 
{
   WIKIDUMP_DBMODE_READ,
   WIKIDUMP_DBMODE_INSERT
};

typedef struct _Wiki_Sqlite
{
   sqlite3        *db;
   Wiki_Dump      *wd;

   int             siteid;
   const char     *sitename;
   const char     *localname;
   const char     *lang;
   const char     *locale;
   const char     *base;
   const char     *case_used;
   const char     *file;
} Wiki_Sqlite;

Wiki_Sqlite *wd_sqlite_new(void);
Wiki_Sqlite *wd_sqlite_open_new(void);
Wiki_Sqlite *wd_sqlite_sitename_lang_new(const char *sitename, const char *lang);
Wiki_Sqlite *wd_sqlite_free(Wiki_Sqlite *ws);

// Eina_Bool wd_sqlite_database_open(Wiki_Sqlite *ws, int mode);
// Eina_Bool wd_sqlite_database_close(Wiki_Sqlite *ws);

Eina_Hash *wd_sqlite_namespace_load(Wiki_Parser_Data *wpd);
Wiki_Dump_Page *wd_sqlite_page_load(const Wiki_Parser_Data *wpd, const char *title);

/* Parser callback */
char *wd_sqlite_template_content_get(const Wiki_Parser_Data *wd, const char *title, size_t *size);

Eina_Bool wd_sqlite_page_exists(const Wiki_Parser_Data *wd, const char *title);
Eina_Bool wd_sqlite_page_exists_interwiki(const Wiki_Parser_Data *wd, const char *title, const char *sitename, const char *lang);

Eina_List *wd_sqlite_category_pages_list_get(const Wiki_Parser_Data *wd, const char *title);
Eina_List *wd_sqlite_search_pages_list_get(const Wiki_Parser_Data *wd, const char *search);
int wd_sqlite_page_namespace_id_get(const Wiki_Parser_Data *wd, const char *title);
unsigned int wd_sqlite_stats_get(const Wiki_Parser_Data *wd, Wiki_Stat stat);
unsigned int wd_sqlite_stats_npages_in_category(const Wiki_Parser_Data *wd, const char *title);
char *wd_sqlite_sitename_from_base(const Wiki_Parser_Data *wd, const char *base, const char *lang);
Eina_List *wd_sqlite_wikimedia_list_get(const Wiki_Parser_Data *wd);
const char *wd_sqlite_base_origin_get(const Wiki_Parser_Data *wd);

unsigned int wd_sqlite_page_revision_id_get(const Wiki_Parser_Data *wd);
char *wd_sqlite_page_revision_timestamp_get(const Wiki_Parser_Data *wd);
char *wd_sqlite_page_revision_comment_get(const Wiki_Parser_Data *wd);
char *wd_sqlite_page_contributor_username_get(const Wiki_Parser_Data *wd);
unsigned int wd_sqlite_page_contributor_id_get(const Wiki_Parser_Data *wd);
Wiki_File *wd_sqlite_image_info_get(const Wiki_Parser_Data *wd, const char *title);


#endif
