#ifndef _WIKIDUMP_SQLITE_H
#define _WIKIDUMP_SQLITE_H

#include "wiki_define.h"
#include "wikidump.h"

typedef struct _Wiki_FS
{
   Wiki_Dump      *wd;
   Wiki_Dump_Page *wdp;

   int             siteid;
   const char     *sitename;
   const char     *localname;
   const char     *lang;
   const char     *locale;
   const char     *base;
   const char     *case_used;
   const char     *file;
} Wiki_FS;

Wiki_FS *wd_fs_new(void);
Wiki_FS *wd_fs_free(Wiki_FS *ws);

// Eina_Bool wd_fs_database_open(Wiki_FS *ws, int mode);
// Eina_Bool wd_fs_database_close(Wiki_FS *ws);

/*
Eina_Hash *wd_fs_namespace_load(Wiki_Parser_Data *wpd);
Wiki_Dump_Page *wd_fs_page_load(Wiki_Parser_Data *wpd, const char *title);
*/

/* Parser callback */
/*
char *wd_fs_template_content_get(const Wiki_Parser_Data *wd, const char *title, size_t *size);

Eina_Bool wd_fs_page_exists(const Wiki_Parser_Data *wd, const char *title);
Eina_Bool wd_fs_page_exists_interwiki(const Wiki_Parser_Data *wd, const char *title, const char *sitename, const char *lang);

Eina_List *wd_fs_category_pages_list_get(const Wiki_Parser_Data *wd, const char *title);
Eina_List *wd_fs_search_pages_list_get(const Wiki_Parser_Data *wd, const char *search);
int wd_fs_page_namespace_id_get(const Wiki_Parser_Data *wd, const char *title);
unsigned int wd_fs_stats_get(const Wiki_Parser_Data *wd, Wiki_Stat stat);
unsigned int wd_fs_stats_npages_in_category(const Wiki_Parser_Data *wd, const char *title);
char *wd_fs_sitename_from_base(const Wiki_Parser_Data *wd, const char *base, const char *lang);
Eina_List *wd_fs_wikimedia_list_get(const Wiki_Parser_Data *wd);
const char *wd_fs_base_origin_get(const Wiki_Parser_Data *wd);

unsigned int wd_fs_page_revision_id_get(const Wiki_Parser_Data *wd);
char *wd_fs_page_revision_timestamp_get(const Wiki_Parser_Data *wd);
char *wd_fs_page_revision_comment_get(const Wiki_Parser_Data *wd);
char *wd_fs_page_contributor_username_get(const Wiki_Parser_Data *wd);
unsigned int wd_fs_page_contributor_id_get(const Wiki_Parser_Data *wd);
Wiki_File *wd_fs_image_info_get(const Wiki_Parser_Data *wd, const char *title);
*/

#endif
