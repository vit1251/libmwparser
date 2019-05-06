#ifndef _WIKIDUMP_H
#define _WIKIDUMP_H

#include <bzlib.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <libxml/xmlreader.h>

typedef struct _Wiki_Dump_Page
{
   xmlParserInputBufferPtr buf;
   xmlTextReaderPtr        xml;
   xmlDocPtr               doc;
   xmlNodePtr              node;
   xmlNodePtr              revision;
   size_t                  page_read;
   off_t                 current_block;
} Wiki_Dump_Page;

typedef struct _Wiki_Dump_Idx
{
   off_t      offset_block;
   unsigned int page_start;
} Wiki_Dump_Idx;

typedef struct _Wiki_Dump
{
   BZFILE *bzfile;

   xmlParserInputBufferPtr  xbuf;
   xmlTextReaderPtr         xml;

   char buf[4097];
   int len;
   char *p;
   
   Eina_Hash *namespaces;
   Eina_List *pages;

   size_t read;
   int reading;
   int state;

   const char *locale;
   const char  *filename;
   const char  *sitename;
   const char  *localname;
   const char  *lang;
   const char  *base;
   const char  *generator;
   const char  *case_used;
   int    id;
   int    old_id;
} Wiki_Dump;

#ifdef WIKIDUMP_SQLITE_MAIN
Wiki_Dump *wikidump_open_filename_new(const char *filename);
Wiki_Dump *wikidump_del(Wiki_Dump *wd);
Wiki_Dump_Page *wikidump_page_page_get(Wiki_Dump *wd);
#endif

xmlNodePtr wikidump_siteinfo_read(Wiki_Dump *wd);

Wiki_Dump_Page *wikidump_page_parser_get(const char *xml, size_t size);
char *wikidump_page_buffer_get(Wiki_Dump_Page *wp);

void wikidump_file_prop_set(Wiki_Dump_Page *wdp, int size, int width, int height, const char *mime);

Wiki_Dump_Page *wikidump_page_free(Wiki_Dump_Page *wp);
Eina_Bool wikidump_namespaces_print(const Eina_Hash *h, const void *k, void *d, void *fd);

unsigned int wikidump_page_id_get(Wiki_Dump_Page *wp);
char *wikidump_page_title_get(Wiki_Dump_Page *wp);
char *wikidump_page_content_get(Wiki_Dump_Page *wp);
unsigned int wikidump_page_revision_id_get(Wiki_Dump_Page *wp);
char *wikidump_page_revision_timestamp_get(Wiki_Dump_Page *wp);
char *wikidump_page_revision_comment_get(Wiki_Dump_Page *wp);
char *wikidump_page_contributor_username_get(Wiki_Dump_Page *wp);
unsigned int wikidump_page_contributor_id_get(Wiki_Dump_Page *wp);
Wiki_File *wikidump_page_file_info_get(Wiki_Dump_Page *wp);


int wikidump_title_namespace_get(Eina_Hash *namespaces, const char *title);
const char *wikidump_namespace_name_get(Eina_Hash *namespaces, int nsid);
void wikidump_namespace_page_add(Eina_Hash *namespaces, int nsid);
Eina_Hash *wikidump_namespaces_alias_set(Eina_Hash *namespaces);
Eina_Bool wikidump_namespaces_del(const Eina_Hash *h, const void *k, void *d, void *fd);

#endif
