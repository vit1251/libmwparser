#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sqlite3.h>
#ifdef HAVE_LZMA
#include "lzma_util.h"
#include <lzma.h>
#else
#include <zlib.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "parser.h"
#include "util.h"
#include "wikidump_sqlite.h"
#include "wikidump_server_common.h"
#include "wikidump.h"
#include "parse_sql.h"
#include "pcre_replace.h"


#define WIKIDUMP_SQLITE_DB "WIKIDUMP_SQLITE_DB"
#define WIKIDUMP_FILE_PATH "WIKIDUMP_FILE_PATH"

// PRAGMA page_size = 8192; 
#define QRY_SCHEMA_CREATE " \
   create table wiki_siteinfo (\
	 id int, \
	 sitename text, \
	 localname text, \
	 lang text, \
	 locale text, \
	 base text, \
	 used_case text, \
	 generator text, \
	 file text\
	 ); \
   create table wiki_pages (\
	 siteid int, \
	 id int, \
	 title text, \
	 content blob default NULL \
	 );\
   create table wiki_namespaces (\
	 siteid int, \
	 key int, \
	 label text, \
	 npages int\
	 );\
   create table wiki_categories (\
	 siteid int, \
	 cid int, \
	 pid int \
	 );\
   create table wiki_images (\
	 siteid int, \
	 file text, \
	 size int, \
	 width int, \
	 height int, \
	 mime text\
	 );\
   create table wiki_pages_cat (\
	 siteid int, \
	 id int, \
	 cat text\
	 );\
   create virtual table wiki_fts using fts3 (\
	 siteid int, \
	 title varchar(255)\
	 );\
\
"

#define QRY_INDEX_CREATE " \
   create unique index if not exists wiki_siteinfo_idx on wiki_siteinfo(sitename, lang); \
   create unique index if not exists wiki_pages_title_idx on wiki_pages(siteid, title); \
   create unique index if not exists wiki_pages_id_idx on wiki_pages(siteid, id); \
   create unique index if not exists wiki_images_idx on wiki_images(siteid, file); \
   create index if not exists wiki_pages_cat_idx on wiki_pages_cat(siteid, cat); \
   create index if not exists wiki_namespaces_idx on wiki_namespaces(siteid); \
"

#define QRY_NAMESPACE_LOAD "select key, label, npages \
   from main.wiki_namespaces where siteid = %d"

#define QRY_PAGE_CONTENT_GET "select uncompress(content) \
   from main.wiki_pages where \
   siteid = %d and title = %Q"

#define QRY_PAGE_EXISTS_INTERNAL "select id \
   from main.wiki_pages where siteid = %d and title = %Q"

#define QRY_IMAGE_INFO_GET "select size, width, height, mime \
   from main.wiki_images i, main.wiki_siteinfo s where \
   siteid = %d and i.file = %Q"

#define QRY_PAGE_EXISTS_INTERWIKI "select p.id \
   from main.wiki_pages p, main.wiki_siteinfo s \
   where siteid = s.id and lang = %Q and sitename = %Q and title = %Q"

#define QRY_SITEINFO_GET "select id, localname, locale, base, used_case, generator, file \
   from main.wiki_siteinfo \
   where sitename = %Q and lang = lower(%Q)"

#define QRY_WIKIMEDIA_LIST_GET "select id, sitename, lang, localname,\
   locale, base, used_case from main.wiki_siteinfo \
   order by lang, sitename"

#define QRY_CATEGORY_TITLE_LIST_1_GET "select p.title \
   from main.wiki_pages p \
   where siteid = %d \
   and p.id = %d"

#define QRY_CATEGORY_TITLE_LIST_2_GET "select distinct(id) \
	 from main.wiki_pages_cat where siteid = %d \
	 and cat = %Q limit 200\
	 "

#define QRY_SEARCH_PAGES_GET "select title \
	 from main.wiki_fts where main.wiki_fts.siteid = %d\
	 and main.wiki_fts.title match %Q \
         order by rank(matchinfo(wiki_fts), 0.5, 1.0) DESC \
         limit 200 \
	 "

#define QRY_SITENAME_FROM_BASE "select sitename from main.wiki_siteinfo \
   where base like 'http://%q.%q.%%' "

#define QRY_SITENAME_FROM_BASE_NO_LANG "select sitename from main.wiki_siteinfo \
   where base like 'http://%q.%%' "

#define QRY_STAT_NUMBER_OF_ARTICLES "select npages from main.wiki_namespaces \
   where siteid = %d and key = %d"
   
#define QRY_STAT_NUMBER_OF_PAGES "select sum(npages) from main.wiki_namespaces \
   where siteid = %d"
   
#define QRY_STAT_NUMBER_OF_FILES "select npages from main.wiki_namespaces \
   where siteid = %d and key = %d"
   
/*
#define QRY_STAT_NPAGES_IN_CATEGORY "select count(pid) from main.wiki_categories c, \
   main.wiki_pages p where c.siteid = p.siteid and cid = p.id \
   and p.siteid = %d and p.title = %Q"
*/

#define QRY_STAT_NPAGES_IN_CATEGORY "select count(siteid) \
   from main.wiki_pages_cat \
   where siteid = %d and cat = %Q group by siteid, cat"

#define QRY_DELETE_BY_SITEID "delete from main.%s where siteid = %d"

#define QRY_CATEGORY_LIST_SET "insert into main.wiki_categories \
   (siteid, cid, pid) \
   select p.siteid, p.id, c.id \
      from main.wiki_pages p, main.wiki_pages_cat c \
      where p.siteid = c.siteid and p.title = c.cat and c.siteid = %d"

#define QRY_INSERT_NAMESPACE "insert into main.wiki_namespaces \
   (siteid, key, label, npages) \
   values (%d, %d, %Q, %d)"

#define QRY_INSERT_PAGES "insert into main.wiki_pages \
   (siteid, id, title, content) \
   values(%d, %d, %Q, compress(%Q))"

#define QRY_INSERT_PAGES_CAT "insert into main.wiki_pages_cat \
   (siteid, id, cat) \
   values(%d, %d, %Q)"

#define QRY_INSERT_SITEINFO "insert into main.wiki_siteinfo \
   (id, sitename, localname, lang, locale, base, used_case, generator, file) \
   values \
  (%d, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q)"

#define QRY_INSERT_IMAGE_INFO "insert into main.wiki_images \
   (siteid, file, size, width, height, mime) \
   values(%d, '%q:%q', %d, %d, %d, %Q)"

#define QRY_INSERT_CATEGORYLINKS_INFO "insert into wiki_pages_cat \
   (siteid, id, cat) \
   values (%d, %d, '%q:%q')"

#define QRY_UPDATE_SITEINFO "update main.wiki_siteinfo set \
   locale = %Q, base = %Q, used_case = %Q, generator = %Q, file = %Q \
   where id = %d" 

#define QRY_SITEID_NEXT_GET "select ifnull(max(id) + 1, 1) from main.wiki_siteinfo"

#define QRY_SITEID_GET "select id from main.wiki_siteinfo \
   where lang = %Q and sitename = %Q"

#define PRAGMA_NEW_FORMAT "PRAGMA legacy_file_format = 0"
#define PRAGMA_SYNCHRONOUS "PRAGMA synchronous = 0"

static Eina_Hash *databases = NULL;

static char *build_query(const char *sitename, char *qry) {
   char *tmp = NULL;
   const char *db = NULL;

   if(! qry) return qry;
   if(! databases) return qry;
   if(! sitename) return qry;

   db = eina_hash_find(databases, sitename);
   if(! db || strcmp(db, "main") == 0) return qry;

   asprintf(&tmp, " %s.", db);
   if(tmp) {
      char *q = strdup(qry);
      if(q) {
	 q = pcre_replace("/ main\\./", tmp, q);
	 sqlite3_free(qry);
	 qry = sqlite3_mprintf("%s", q);
	 free(q);
      }
      free(tmp);
   }

//   printf("QRY 4: %s\n", qry);
   return qry;
}

static Eina_Bool 
_hash_to_list(const Eina_Hash *h, const void *k, void *d, void *l)
{
   char *db = d;
   Eina_List *list = *((Eina_List **)l);

   if(strcmp(db, "main") != 0) {
      Eina_List *l= NULL;
      char *name = NULL;
      Eina_Bool in_list = EINA_FALSE;
      EINA_LIST_FOREACH(list, l, name) {
         if(strcmp(name, db) == 0)
            in_list = EINA_TRUE;
      }
      if(in_list == EINA_FALSE)
         list = eina_list_append(list, db);
   }

   *((void **)l) = list;

   return EINA_TRUE;
}

static char *build_union_query(char *qry) {
   Eina_List *list = NULL, *l = NULL;
   char *tmp = NULL;
   char *db = NULL;


   if(! qry) return qry;
   if(! databases) return qry;

   tmp = strdup(qry);
   if(! tmp) return qry;

   eina_hash_foreach(databases, _hash_to_list, &list); 
   if(list) {
      EINA_LIST_FOREACH(list, l, db) {
         char *t = strdup(qry);
	 char *tdb = NULL;
	 asprintf(&tdb, " %s.", db);
         if(t) {
            t = pcre_replace("/ main\\./", tdb, t);
            if(strstr(tmp, "order by")) {
               tmp = pcre_replace("/(order by .*)$/", " union ", tmp);
            } else {
               tmp = pcre_replace("/$/", " union ", tmp);
            }
            tmp = pcre_replace("/$/", t, tmp);
            if(t)
               free(t);
         }
	 if(tdb)
	    free(tdb);
      }
      eina_list_free(list);
   }

   if(tmp) {
      sqlite3_free(qry);
      qry = sqlite3_mprintf("%s", tmp);
      free(tmp);
   }

//   printf("QRY 4: %s\n", qry);
   return qry;
}

/*
** SQL function to compress content into a blob using libz
*/
static void compressFunc(sqlite3_context *context, int argc, sqlite3_value **argv)
{
  int nIn, nOut;
  unsigned long int nOut2;
  const unsigned char *inBuf = NULL;
  unsigned char *outBuf = NULL;

  if(argc != 1) return;
  nIn = sqlite3_value_bytes(argv[0]);
  inBuf = sqlite3_value_blob(argv[0]);
#ifdef HAVE_LZMA
  nOut = lzma_stream_buffer_bound(nIn);
#else
  nOut = 13 + nIn + (nIn+999)/1000;
#endif
  outBuf = malloc( nOut + 4 );
  outBuf[0] = nIn>>24 & 0xff;
  outBuf[1] = nIn>>16 & 0xff;
  outBuf[2] = nIn>>8 & 0xff;
  outBuf[3] = nIn & 0xff;
  nOut2 = (unsigned long int)nOut;
#ifdef HAVE_LZMA
  lzma_compress(&outBuf[4], &nOut2, inBuf, nIn);
#else
  compress(&outBuf[4], &nOut2, inBuf, nIn);
#endif
//  printf("Compress: in = %ld, out = %ld\n", nIn, nOut2);
  sqlite3_result_blob(context, outBuf, nOut2+4, free);
}

/*
** An SQL function to decompress.
*/
static void uncompressFunc(sqlite3_context *context, int argc, sqlite3_value **argv)
{
  unsigned int nIn, nOut, rc;
  const unsigned char *inBuf = NULL;
  unsigned char *outBuf = NULL;
  unsigned long int nOut2;

  if(argc != 1) return;
  nIn = sqlite3_value_bytes(argv[0]);
  if( nIn <= 4 ){
    return;
  }
  inBuf = sqlite3_value_blob(argv[0]);
  nOut = (inBuf[0]<<24) + (inBuf[1]<<16) + (inBuf[2]<<8) + inBuf[3];
  outBuf = malloc(nOut);
  nOut2 = (unsigned long int)nOut;
#ifdef HAVE_LZMA
  rc = lzma_uncompress(outBuf, &nOut2, &inBuf[4], nIn);
  if(! (rc == LZMA_OK || rc == LZMA_STREAM_END) ){
    free(outBuf);
    outBuf = NULL;
  }
#else
  rc = uncompress(outBuf, &nOut2, &inBuf[4], nIn);
  if( rc != Z_OK ){
    free(outBuf);
    outBuf = NULL;
  }
#endif
  if(outBuf) {
    sqlite3_result_blob(context, outBuf, nOut2, free);
  }
}

/*
** SQLite user defined function to use with matchinfo() to calculate the
** relevancy of an FTS match. The value returned is the relevancy score
** (a real value greater than or equal to zero). A larger value indicates 
** a more relevant document.
**
** The overall relevancy returned is the sum of the relevancies of each 
** column value in the FTS table. The relevancy of a column value is the
** sum of the following for each reportable phrase in the FTS query:
**
**   (<hit count> / <global hit count>) * <column weight>
**
** where <hit count> is the number of instances of the phrase in the
** column value of the current row and <global hit count> is the number
** of instances of the phrase in the same column of all rows in the FTS
** table. The <column weight> is a weighting factor assigned to each
** column by the caller (see below).
**
** The first argument to this function must be the return value of the FTS 
** matchinfo() function. Following this must be one argument for each column 
** of the FTS table containing a numeric weight factor for the corresponding 
** column. Example:
**
**     CREATE VIRTUAL TABLE documents USING fts3(title, content)
**
** The following query returns the docids of documents that match the
full-text
** query <query> sorted from most to least relevant. When calculating
** relevance, query term instances in the 'title' column are given twice the
** weighting of those in the 'content' column.
**
**     SELECT docid FROM documents 
**     WHERE documents MATCH <query> 
**     ORDER BY rank(matchinfo(documents), 1.0, 0.5) DESC
*/
static void rankFunc(sqlite3_context *pCtx, int nVal, sqlite3_value **apVal){
  int *aMatchinfo;                /* Return value of matchinfo() */
  int nCol;                       /* Number of columns in the table */
  int nPhrase;                    /* Number of phrases in the query */
  int iPhrase;                    /* Current phrase */
  double score = 0.0;             /* Value to return */

  /* Check that the number of arguments passed to this function is correct.
  ** If not, jump to wrong_number_args. Set aMatchinfo to point to the array
  ** of unsigned integer values returned by FTS function matchinfo. Set
  ** nPhrase to contain the number of reportable phrases in the users
full-text
  ** query, and nCol to the number of columns in the table.
  */
  if( nVal<1 ) goto wrong_number_args;
  aMatchinfo = (unsigned int *)sqlite3_value_blob(apVal[0]);
  nPhrase = aMatchinfo[0];
  nCol = aMatchinfo[1];
  if( nVal!=(1+nCol) ) goto wrong_number_args;

  /* Iterate through each phrase in the users query. */
  for(iPhrase=0; iPhrase<nPhrase; iPhrase++){
    int iCol;                     /* Current column */

    /* Now iterate through each column in the users query. For each column,
    ** increment the relevancy score by:
    **
    **   (<hit count> / <global hit count>) * <column weight>
    **
    ** aPhraseinfo[] points to the start of the data for phrase iPhrase. So
    ** the hit count and global hit counts for each column are found in 
    ** aPhraseinfo[iCol*3] and aPhraseinfo[iCol*3+1], respectively.
    */
    int *aPhraseinfo = &aMatchinfo[2 + iPhrase*nCol*3];
    for(iCol=0; iCol<nCol; iCol++){
      int nHitCount = aPhraseinfo[3*iCol];
      int nGlobalHitCount = aPhraseinfo[3*iCol+1];
      double weight = sqlite3_value_double(apVal[iCol+1]);
      if( nHitCount>0 ){
        score += ((double)nHitCount / (double)nGlobalHitCount) * weight;
      }
    }
  }

  sqlite3_result_double(pCtx, score);
  return;

  /* Jump here if the wrong number of arguments are passed to this function */
wrong_number_args:
  sqlite3_result_error(pCtx, "wrong number of arguments to function rank()",
-1);
}

static int title_list_sort(const void *data1, const void *data2)
{
   const char *a = data1;
   const char *b = data2;

   if(a && b) {
      return strcmp(a, b);
   } else if(a) {
      return -1;
   } else if(b) {
      return 1;
   }

   return 0;
}

unsigned int wd_sqlite_page_revision_id_get(const Wiki_Parser_Data *wd)
{
   Wiki_Dump_Page *wdp = NULL;

   if(! wd) return 0;
   if(! wd->data_page) return 0;

   wdp = wd->data_page;

   return wikidump_page_revision_id_get(wdp);
}

char *wd_sqlite_page_revision_timestamp_get(const Wiki_Parser_Data *wd)
{
   Wiki_Dump_Page *wdp = NULL;

   if(! wd) return NULL;
   if(! wd->data_page) return NULL;

   wdp = wd->data_page;

   return wikidump_page_revision_timestamp_get(wdp);
}

char *wd_sqlite_page_revision_comment_get(const Wiki_Parser_Data *wd)
{
   Wiki_Dump_Page *wdp = NULL;

   if(! wd) return NULL;
   if(! wd->data_page) return NULL;

   wdp = wd->data_page;

   return wikidump_page_revision_comment_get(wdp);
}

char *wd_sqlite_page_contributor_username_get(const Wiki_Parser_Data *wd)
{
   Wiki_Dump_Page *wdp = NULL;

   if(! wd) return NULL;
   if(! wd->data_page) return NULL;

   wdp = wd->data_page;

   return wikidump_page_contributor_username_get(wdp);
}

unsigned int wd_sqlite_page_contributor_id_get(const Wiki_Parser_Data *wd)
{
   Wiki_Dump_Page *wdp = NULL;

   if(! wd) return 0;
   if(! wd->data_page) return 0;

   wdp = wd->data_page;

   return wikidump_page_contributor_id_get(wdp);
}

Wiki_File *wd_sqlite_image_info_get(const Wiki_Parser_Data *wd, const char *title)
{
   Wiki_Sqlite *ws = NULL;
   Wiki_File *wf = NULL;
   char *qry = NULL;

   if(! wd) return wf;
   if(! wd->data) return wf;
   if(! title) return wf;

   ws = wd->data;
   qry = sqlite3_mprintf(QRY_IMAGE_INFO_GET, wd->siteid, title);
   qry = build_query(wd->sitename, qry);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, strlen(qry) + 1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 r = sqlite3_step(stmt);
	 if(r == SQLITE_ROW) {
	    wf = calloc(1, sizeof(Wiki_File));
	    if(wf) {
	       const char *mime = NULL;
	       wf->size = sqlite3_column_int(stmt, 0);
	       wf->width = sqlite3_column_int(stmt, 1);
	       wf->height = sqlite3_column_int(stmt, 2);
	       mime = sqlite3_column_text(stmt, 3);
	       if(mime) {
		  wf->mime = strdup(mime);
	       }
	    }
	 }
      }
      sqlite3_finalize(stmt);
      sqlite3_free(qry);
   }

   return wf;
}

char *wd_sqlite_template_content_get(const Wiki_Parser_Data *wd, const char *title, size_t *size)
{
   Wiki_Sqlite *ws = NULL;
   char *qry = NULL, *content = NULL;

   *size = 0;

   if(! wd) return content;
   if(! wd->data) return content;
   if(! title) return content;

   ws = wd->data;
   qry = sqlite3_mprintf(QRY_PAGE_CONTENT_GET, wd->siteid, title);
   qry = build_query(wd->sitename, qry);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, strlen(qry) + 1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 r = sqlite3_step(stmt);
	 if(r == SQLITE_ROW) {
	    const char *tmp = (const char *)sqlite3_column_text(stmt, 0);
	    if(tmp) {
	       Wiki_Dump_Page *wdp = wikidump_page_parser_get(tmp, strlen(tmp));
	       if(wdp) {
		  content = wikidump_page_content_get(wdp);;
		  *size = strlen(content);
		  wikidump_page_free(wdp);
	       } else {
		  ulog("%s: failed to get page\n%s\n", __FUNCTION__, qry);
	       }
	    } else {
	       *size = 0;
	    }
	 } else {
	    ulog("%s: No result\n%s\n", __FUNCTION__, qry);
	 }
      } else
	 fprintf(stderr, "QRY FAILED: %s : %s : %s\n", __FUNCTION__, qry,
	       sqlite3_errmsg(ws->db));
      sqlite3_finalize(stmt);
      sqlite3_free(qry);
   }

   return content;
}

Eina_List *wd_sqlite_search_pages_list_get(const Wiki_Parser_Data *wd, const char *search)
{
   Wiki_Sqlite *ws = NULL; 
   Eina_List *l = NULL;
   char *qry = NULL;

   if(! wd) return l;
   if(! wd->data) return l;
   if(! search) return l;

   ws = wd->data;

   qry = sqlite3_mprintf(QRY_SEARCH_PAGES_GET, wd->siteid, search);
   qry = build_query(wd->sitename, qry);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, -1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 while(sqlite3_step(stmt) == SQLITE_ROW) {
            const char *t = (const char *) sqlite3_column_text(stmt, 0);
            if(t) {
               l = eina_list_append(l, strdup(t));
            }
         }
      } else {
	 fprintf(stderr, "QRY FAILED: %s : %s : %s\n", __FUNCTION__, qry,
	       sqlite3_errmsg(ws->db));
      }
      sqlite3_finalize(stmt);
      sqlite3_free(qry);
   }

   return l;
}

Eina_List *wd_sqlite_category_pages_list_get(const Wiki_Parser_Data *wd, const char *title)
{
   Wiki_Sqlite *ws = NULL; 
   Eina_List *l = NULL;
   char *qry = NULL;

   if(! wd) return l;
   if(! wd->data) return l;
   if(! title) return l;

   ws = wd->data;

   qry = sqlite3_mprintf(QRY_CATEGORY_TITLE_LIST_2_GET, wd->siteid, title);
   qry = build_query(wd->sitename, qry);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, -1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 while(sqlite3_step(stmt) == SQLITE_ROW) {
	    char *q = sqlite3_mprintf(QRY_CATEGORY_TITLE_LIST_1_GET, wd->siteid,
		  sqlite3_column_int(stmt, 0));
            q = build_query(wd->sitename, q);
	    if(q) {
	       sqlite3_stmt *s = NULL;
	       r = sqlite3_prepare_v2(ws->db, q, -1, &s, NULL);
	       if(r == SQLITE_OK) {
		  if(sqlite3_step(s) == SQLITE_ROW) {
		     const char *t = (const char *) sqlite3_column_text(s, 0);
		     if(t) {
			l = eina_list_append(l, strdup(t));
		     }
		  }
	       }
	       sqlite3_finalize(s);
	       sqlite3_free(q);
	    }
	 }
      } else {
	 fprintf(stderr, "QRY FAILED: %s : %s : %s\n", __FUNCTION__, qry,
	       sqlite3_errmsg(ws->db));
      }
      sqlite3_finalize(stmt);
      sqlite3_free(qry);
   }

   if(l)
      l = eina_list_sort(l, eina_list_count(l), title_list_sort);

   return l;
}

int wd_sqlite_page_namespace_id_get(const Wiki_Parser_Data *wd, const char *title)
{
   if(! wd) return 0;
   if(! wd->namespaces) return 0;
   if(! title) return 0;

   return wikidump_title_namespace_get(wd->namespaces, title);
}

const char *wd_sqlite_base_origin_get(const Wiki_Parser_Data *wd)
{
   Wiki_Sqlite *ws = NULL;
      
   if(! wd) return NULL;
   if(! wd->data) return NULL;

   ws = wd->data;

   return wd->base;
}

Eina_List *wd_sqlite_wikimedia_list_get(const Wiki_Parser_Data *wd)
{
   Wiki_Sqlite *ws = NULL;
   Eina_List *l = NULL;
   char *qry = NULL;

   if(! wd) return l;
   if(! wd->data) return l;

   ws = wd->data;

   qry = sqlite3_mprintf(QRY_WIKIMEDIA_LIST_GET);
   qry = build_union_query(qry);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, -1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 while(sqlite3_step(stmt) == SQLITE_ROW) {
	    Wiki_Media *w = malloc(sizeof(Wiki_Media));
	    if(w) {
	       w->siteid = sqlite3_column_int(stmt, 0);
	       w->sitename = eina_stringshare_add((const char *)sqlite3_column_text(stmt, 1));
	       w->lang = eina_stringshare_add((const char *)sqlite3_column_text(stmt, 2));
	       w->localname = eina_stringshare_add((const char *)sqlite3_column_text(stmt, 3));
	       w->locale = eina_stringshare_add((const char *)sqlite3_column_text(stmt, 4));
	       w->base = eina_stringshare_add((const char *)sqlite3_column_text(stmt, 5));
	       w->case_used = eina_stringshare_add((const char *)sqlite3_column_text(stmt, 6));
	       l = eina_list_append(l, w);
	    }
	 }
      } else {
	 fprintf(stderr, "QRY FAILED: %s : %s : %s\n", __FUNCTION__, qry,
	       sqlite3_errmsg(ws->db));
      }
      sqlite3_finalize(stmt);
      sqlite3_free(qry);
   }

   return l;
}

unsigned int wd_sqlite_stats_get(const Wiki_Parser_Data *wd, Wiki_Stat stat)
{
   Wiki_Sqlite *ws;
   char *qry = NULL;
   int res = 0;

   if(! wd) return 0;
   if(! wd->data) return 0;

   ws = wd->data;

   switch(stat) {
      case WIKI_STAT_NUMBER_OF_ARCTICLES:
	 qry = sqlite3_mprintf(QRY_STAT_NUMBER_OF_ARTICLES, wd->siteid, WIKI_NS_MAIN);
	 break;
      case WIKI_STAT_NUMBER_OF_PAGES:
	 qry = sqlite3_mprintf(QRY_STAT_NUMBER_OF_PAGES, wd->siteid);
	 break;
      case WIKI_STAT_NUMBER_OF_FILES:
	 qry = sqlite3_mprintf(QRY_STAT_NUMBER_OF_FILES, wd->siteid, WIKI_NS_FILE);
	 break;
      case WIKI_STAT_NUMBER_OF_EDITS:
      case WIKI_STAT_NUMBER_OF_VIEWS:
      case WIKI_STAT_NUMBER_OF_USERS:
      case WIKI_STAT_NUMBER_OF_ADMINS:
      case WIKI_STAT_NUMBER_OF_ACTIVE_USERS:
      default:
	 /* None*/
	 break;
   }

   qry = build_query(wd->sitename, qry);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, -1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 while(sqlite3_step(stmt) == SQLITE_ROW) {
	    res += sqlite3_column_int(stmt, 0);
	 }
      }
      sqlite3_finalize(stmt);
      sqlite3_free(qry);
   }

   return res;
}

unsigned int wd_sqlite_stats_npages_in_category(const Wiki_Parser_Data *wd, const char *title)
{
   Wiki_Sqlite *ws = NULL;
   unsigned int ret = 0;
   char *qry = NULL;

   if(! wd) return ret;
   if(! wd->data) return ret;
   if(! title) return ret;

   ws = wd->data;
   
   qry = sqlite3_mprintf(QRY_STAT_NPAGES_IN_CATEGORY, wd->siteid, title);
   qry = build_query(wd->sitename, qry);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, -1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 if(sqlite3_step(stmt) == SQLITE_ROW) {
	    ret = sqlite3_column_int(stmt, 0);
	 }
      }
      sqlite3_finalize(stmt);
      sqlite3_free(qry);
   }

   return ret;
}

Eina_Bool wd_sqlite_page_exists(const Wiki_Parser_Data *wd, const char *title)
{
   Wiki_Sqlite *ws = NULL;
   Eina_Bool ret = EINA_FALSE;
   char *qry = NULL;

   if(! wd) return ret;

   ws = wd->data;
   if(! ws) return ret;
   if(! ws->db) return ret;
   if(! title) return ret;

   qry = sqlite3_mprintf(QRY_PAGE_EXISTS_INTERNAL, wd->siteid, title);
   qry = build_query(wd->sitename, qry);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, strlen(qry) + 1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 r = sqlite3_step(stmt);
	 if(r == SQLITE_ROW)
	    ret = EINA_TRUE;
      } else
	 fprintf(stderr, "QRY FAILED: %s : %s : %s\n", __FUNCTION__, qry,
	       sqlite3_errmsg(ws->db));
      sqlite3_finalize(stmt);
      sqlite3_free(qry);
   }

   return ret;
}

Eina_Bool wd_sqlite_page_exists_interwiki(const Wiki_Parser_Data *wd, const char *title, const char *sitename, const char *lang)
{
   Wiki_Sqlite *ws = NULL;
   Eina_Bool ret = EINA_FALSE;
   char *qry = NULL;

   if(! wd) return ret;

   ws = wd->data;
   if(! ws) return ret;
   if(! ws->db) return ret;
   if(! title) return ret;
   if(! sitename) return ret;
   if(! lang) return ret;

   qry = sqlite3_mprintf(QRY_PAGE_EXISTS_INTERWIKI, lang, sitename, title);
   qry = build_query(sitename, qry);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, strlen(qry) + 1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 r = sqlite3_step(stmt);
	 if(r == SQLITE_ROW)
	    ret = EINA_TRUE;
      } else
	 fprintf(stderr, "QRY FAILED: %s : %s : %s\n", __FUNCTION__, qry,
	       sqlite3_errmsg(ws->db));
      sqlite3_finalize(stmt);
      sqlite3_free(qry);
   }

   return ret;
}

char *wd_sqlite_sitename_from_base(const Wiki_Parser_Data *wd, const char *base, const char *lang)
{
   Wiki_Sqlite *ws = NULL;
   char *ret = NULL;
   char *qry = NULL;
   const char *tmp = base;
   int cnt = 0;

   if(! wd) return ret;
   if(! wd->data) return ret;
   if(! base) return ret;

   while((tmp = index(tmp, '.'))) {
      cnt++;
      tmp++;
   }

   if(strcmp(base, "wikimedia") == 0) {
      asprintf(&qry, "%s.wikipedia.org", lang);
   } else if(lang && cnt == 1 /* && strncmp(lang, base, 2) != 0 */) {
      asprintf(&qry, "%s.%s", lang, base);
   } else if(lang && cnt == 0) {
      asprintf(&qry, "%s.%s.org", lang, base);
   } else {
      qry = strdup(base);
   }
   return qry;
}

Wiki_Dump_Page *wd_sqlite_page_load(const Wiki_Parser_Data *wpd, const char *title)
{
   Wiki_Dump_Page *wdp = NULL;
   Wiki_Sqlite *ws = NULL;
   char *qry = NULL;

   if(! wpd) return NULL;
   if(! title) return NULL;

   ws = wpd->data;
   if(! ws) return NULL;

   qry = sqlite3_mprintf(QRY_PAGE_CONTENT_GET, wpd->siteid, title);
   qry = build_query(wpd->sitename, qry);
   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, strlen(qry) + 1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 r = sqlite3_step(stmt);
	 if(r == SQLITE_ROW) {
	    const char *tmp = (const char *)sqlite3_column_text(stmt, 0);
	    if(tmp) {
	       wdp = wikidump_page_parser_get(tmp, strlen(tmp));
	    }
	 } 
      } else
	 fprintf(stderr, "QRY FAILED: %s : %s : %s\n", __FUNCTION__, qry,
	       sqlite3_errmsg(ws->db));
      sqlite3_finalize(stmt);
      sqlite3_free(qry);
   }

   return wdp;
}

Eina_Hash *wd_sqlite_namespace_load(Wiki_Parser_Data *wpd)
{
   Wiki_Sqlite *ws = NULL;
   Eina_Hash *namespaces = NULL;

   if(! wpd) return NULL;

   ws = wpd->data;
   if(! ws) return NULL;
   if(! ws->db) return NULL;

   namespaces = eina_hash_string_superfast_new(NULL);

   if(namespaces) {
      sqlite3 *db = ws->db;
      char *qry = NULL;

      qry = sqlite3_mprintf(QRY_NAMESPACE_LOAD, wpd->siteid);
      qry = build_query(wpd->sitename, qry);
      if(qry) {
	 sqlite3_stmt *stmt = NULL;
	 int r = 0;
         r = sqlite3_prepare_v2(db , qry, strlen(qry) + 1, &stmt, NULL);
	 if(r == SQLITE_OK) {
	    while((r = sqlite3_step(stmt)) == SQLITE_ROW) {
	       Wiki_Namespace *ns = malloc(sizeof(Wiki_Namespace));
	       if(ns) {
		  char k[sizeof(int) + 1];
		  const char *label = NULL;

		  ns->key = sqlite3_column_int(stmt, 0);
		  ns->npages = sqlite3_column_int(stmt, 2);
		  label = (const char *) sqlite3_column_text(stmt, 1);

		  snprintf(k, sizeof(k), "%d", ns->key);
		  eina_hash_add(namespaces, k, ns);
		  if(label) {
		     size_t size = sqlite3_column_bytes(stmt, 1);
		     // ns->label = eina_stringshare_add_length(label, size);
		     ns->label = strdup(label);
		     eina_hash_add(namespaces, ns->label, ns);
		  }
		  ulog("NAMESPACE: key: %d, label: %s, npages: %d\n",
			ns->key, ns->label, ns->npages);
	       }
	    }
	    sqlite3_finalize(stmt);
	 }
	 sqlite3_free(qry);
      }
   }

   wikidump_namespaces_alias_set(namespaces);

   return namespaces;
}

Eina_Bool wd_sqlite_database_close(Wiki_Sqlite *ws)
{
   sqlite3_stmt *stmt = NULL;
   int ret = EINA_FALSE;

   if(! ws) return ret;
   if(! ws->db) return ret;

   while(0 && (stmt = sqlite3_next_stmt(ws->db, NULL)) != NULL) {
      printf("Finalize: %p\n", stmt);
      sqlite3_finalize(stmt);
   }

   sqlite3_close(ws->db);
   ws->db = NULL;

   ret = EINA_TRUE;

   return ret;
}

static void _databases_hash_free(void *data)
{
   char *dbname = data;
   /* TODO: Sort uniq before free dbname */
   if(0 && dbname)
      free(dbname);
}

Eina_Bool wd_sqlite_database_open(Wiki_Sqlite *ws, int mode)
{
   sqlite3 *db = NULL;
   char *dbfile = NULL;
   char **split = NULL;
   unsigned int cnt =0;
   Eina_Bool ret = EINA_FALSE;

   if(! databases)
      databases = eina_hash_string_superfast_new(_databases_hash_free);

   if(getenv(WIKIDUMP_SQLITE_DB))
      split = eina_str_split_full(getenv(WIKIDUMP_SQLITE_DB), ":", -1, &cnt);
   printf("must open: %d\n", cnt);
   if(split) {
      dbfile = split[0];
      if(dbfile) {
         int dboption = 0;
         if(mode == WIKIDUMP_DBMODE_INSERT) {
            dboption = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
         } else {
            dboption = SQLITE_OPEN_READONLY;
         }

         if(sqlite3_open_v2(dbfile, &db, dboption, NULL) == SQLITE_OK) {
            ws->db = db;
            ret = EINA_TRUE;
         } else {
            fprintf(stderr, "Can't open db '%s'\n", dbfile);
         }
      } else {
         fprintf(stderr, "ERROR: variable envirronment WIKIDUMP_SQLITE_DB not set\n");
      }

      if(ws->db && cnt > 1) {
         int i;
         char *qry = NULL;
         qry = sqlite3_mprintf("select sitename from wiki_siteinfo");
         if(qry) {
            sqlite3_stmt *stmt = NULL;
            int r = 0;
            r = sqlite3_prepare_v2(db , qry, strlen(qry) + 1, &stmt, NULL);
            if(r == SQLITE_OK) {
               while((r = sqlite3_step(stmt)) == SQLITE_ROW) {
                  const char *sitename = (const char *) sqlite3_column_text(stmt, 0);
                  if(databases)
                     eina_hash_add(databases, eina_stringshare_add(sitename), strdup("main"));
               }
            }
            sqlite3_finalize(stmt);
            sqlite3_free(qry);
         }
         for(i = 1; i < cnt; i++) {
            char *dbname = NULL;
            asprintf(&dbname, "db%d", i);
            if(dbname) {
               int r = -1;
               qry = sqlite3_mprintf("attach %Q as %s", split[i], dbname);
               if(qry) {
                  r = sqlite3_exec(db, qry, NULL, NULL, NULL);
                  sqlite3_free(qry);
               }
               if(r == SQLITE_OK) {
                  qry = sqlite3_mprintf("select sitename from %s.wiki_siteinfo", dbname);
                  if(qry) {
                     sqlite3_stmt *stmt = NULL;
                     int r = 0;
                     r = sqlite3_prepare_v2(db , qry, strlen(qry) + 1, &stmt, NULL);
                     if(r == SQLITE_OK) {
                        while((r = sqlite3_step(stmt)) == SQLITE_ROW) {
                           const char *sitename = (const char *) sqlite3_column_text(stmt, 0);
                           if(databases && ! eina_hash_find(databases, sitename)) {
                              eina_hash_add(databases, eina_stringshare_add(sitename), dbname);
                           }
                        }
                     }
                     sqlite3_finalize(stmt);
                     sqlite3_free(qry);
                  }
               } else {
                  free(dbname);
               }
            }
         }
      }

      if(split[0])
         free(split[0]);
      free(split);
   }

   return ret;
}

Wiki_Sqlite *wd_sqlite_free(Wiki_Sqlite *ws)
{
   if(ws) {
#ifdef WIKIDUMP_SQLITE_MAIN
      if(ws->wd)
	 ws->wd = wikidump_del(ws->wd);
#endif
      if(ws->db)
	 wd_sqlite_database_close(ws);
      if(ws->sitename)
	 eina_stringshare_del(ws->sitename);
      if(ws->lang)
	 eina_stringshare_del(ws->lang);
      if(ws->locale)
	 eina_stringshare_del(ws->locale);
      if(ws->base)
	 eina_stringshare_del(ws->base);
      if(ws->case_used)
	 eina_stringshare_del(ws->case_used);
      if(ws->file)
	 eina_stringshare_del(ws->file);

      if(databases)
         eina_hash_free(databases);
      databases = NULL;
      memset(ws, 0, sizeof(Wiki_Sqlite));
      free(ws);
   }
   return NULL;
}

Wiki_Sqlite *wd_sqlite_new(void)
{
   Wiki_Sqlite *ws = NULL;

   ws = calloc(1, sizeof(Wiki_Sqlite));

   return ws;
}

Wiki_Sqlite *wd_sqlite_open_new(void)
{
   Wiki_Sqlite *ws = NULL;
   Eina_Bool r = EINA_FALSE;

   ws = wd_sqlite_new();;
   if(! ws) return ws;

   r = wd_sqlite_database_open(ws, WIKIDUMP_DBMODE_READ);
   if(r == EINA_TRUE) {
      sqlite3_create_function(ws->db, "compress", 1, SQLITE_UTF8, 0, compressFunc, 0, 0);
      sqlite3_create_function(ws->db, "uncompress", 1, SQLITE_UTF8, 0, uncompressFunc, 0, 0);
      sqlite3_create_function(ws->db, "rank", 3, SQLITE_UTF8, 0, rankFunc, 0, 0);
   } else {
      ws = wd_sqlite_free(ws);
   }

   return ws;
}

Wiki_Sqlite *wd_sqlite_sitename_lang_new(const char *sitename, const char *lang)
{
   Wiki_Sqlite *ws = NULL;
   Eina_Bool r = EINA_FALSE;

   if(! sitename) return ws;
   if(! lang) return ws;

   ws = wd_sqlite_new();;
   if(! ws) return ws;

   r = wd_sqlite_database_open(ws, WIKIDUMP_DBMODE_READ);
   if(r == EINA_TRUE) {
      char *qry = sqlite3_mprintf(QRY_SITEINFO_GET, sitename, lang);
      sqlite3_create_function(ws->db, "compress", 1, SQLITE_UTF8, 0, compressFunc, 0, 0);
      sqlite3_create_function(ws->db, "uncompress", 1, SQLITE_UTF8, 0, uncompressFunc, 0, 0);
      sqlite3_create_function(ws->db, "rank", 3, SQLITE_UTF8, 0, rankFunc, 0, 0);
      r = EINA_FALSE;
      if(qry) {
	 sqlite3_stmt *stmt = NULL;
	 int res = sqlite3_prepare_v2(ws->db, qry, -1, &stmt, NULL);
	 if(res == SQLITE_OK) {
	    res = sqlite3_step(stmt);
	    if(res == SQLITE_ROW) {
	       ws->siteid = sqlite3_column_int(stmt, 0);
	       ws->localname = eina_stringshare_add((const char *)sqlite3_column_text(stmt, 1));
	       ws->locale = eina_stringshare_add((const char *)sqlite3_column_text(stmt, 2));
	       ws->base = eina_stringshare_add((const char *)sqlite3_column_text(stmt, 3));
	       ws->case_used = eina_stringshare_add((const char *)sqlite3_column_text(stmt, 4));
	       ws->file = eina_stringshare_add((const char *)sqlite3_column_text(stmt, 6));
	       ws->lang = eina_stringshare_add(lang);
	       ws->sitename = eina_stringshare_add(sitename);
	       r = EINA_TRUE;
	    }
	 }
	 sqlite3_finalize(stmt);
	 sqlite3_free(qry);
      }
   }

   if(r == EINA_FALSE) {
      ws = wd_sqlite_free(ws);
   }

   return ws;
}

#ifdef WIKIDUMP_SQLITE_MAIN
int signal_main = 0;

static void signal_handler(int signal)
{
   signal_main = signal;
   printf("Get signal '%d', try to wait programm finish to keep database clean\n");
}

static void wd_sqlite_table_delete(Wiki_Sqlite *ws, const char *tablename)
{
   char *qry = NULL;

   if(! ws) return;
   if(! ws->db) return;

   qry = sqlite3_mprintf(QRY_DELETE_BY_SITEID, tablename, ws->siteid);
   if(qry) {
      sqlite3_exec(ws->db, qry, NULL, NULL, NULL);
      sqlite3_free(qry);
   }
}

static void wd_sqlite_index_create(Wiki_Sqlite *ws)
{
   int r = 0;
   sqlite3_stmt *creat = NULL;
   const char *tail = NULL;
   const char *qry_create = QRY_INDEX_CREATE;

   if(! ws) return;
   if(! ws->db) return;

   while(strlen(qry_create) > 0 && (r = sqlite3_prepare_v2(ws->db, qry_create, strlen(qry_create), &creat, &tail)) == SQLITE_OK) {
      sqlite3_step(creat);
      sqlite3_finalize(creat);

      qry_create = tail;
   }
}

static int wd_sqlite_siteid_get(Wiki_Sqlite *ws, const char *sitename, Eina_Bool *new)
{
   int siteid = 0, r = 0, schema = 1;
   char *qry = NULL;
   sqlite3_stmt *stmt = NULL;

   if(! ws) return siteid;
   if(! ws->db) return siteid;
   if(! ws->wd) return siteid;

   qry = sqlite3_mprintf(QRY_SITEID_GET, ws->wd->lang, sitename);
RETRY:
   if(qry) {
      r = sqlite3_prepare_v2(ws->db, qry, strlen(qry) + 1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 r = sqlite3_step(stmt);
	 if(r == SQLITE_DONE) {
	    sqlite3_finalize(stmt);
	    r = sqlite3_prepare_v2(ws->db, QRY_SITEID_NEXT_GET, sizeof(QRY_SITEID_NEXT_GET), &stmt, NULL);
	    r = sqlite3_step(stmt);
            *new = EINA_TRUE;
	 }
	 if(r == SQLITE_ROW) {
	    siteid = sqlite3_column_int(stmt, 0);
	 }
	 sqlite3_finalize(stmt);
      } else if(schema) {
	 sqlite3_stmt *creat = NULL;
	 const char *tail = NULL;
	 const char *qry_create = QRY_SCHEMA_CREATE;

	 schema = 0;

	 r = sqlite3_finalize(stmt);
	 while((r = sqlite3_prepare_v2(ws->db, qry_create, strlen(qry_create), &creat, &tail)) == SQLITE_OK) {
	    sqlite3_step(creat);
	    sqlite3_finalize(creat);

      	    qry_create = tail;
	    if(strlen(qry_create) == 0) {
	       goto RETRY;
	    }
	 }
	 // fprintf(stderr, "Failed to prepare create statement: %d, %s, %s\n", r, sqlite3_errmsg(ws->db), qry_create);
	 
      } else {
	 fprintf(stderr, "Failed to get siteid: %d, %s\n", r, sqlite3_errmsg(ws->db));

      }
      sqlite3_free(qry);
   }

   ws->siteid = siteid;
   return siteid;
}

static Eina_Bool wd_sqlite_siteinfo_update(Wiki_Sqlite *ws, Eina_Bool new)
{
   char *qry = NULL;
   Eina_Bool ret = EINA_FALSE;

   if(! ws) return EINA_FALSE;
   if(! ws->db) return EINA_FALSE;
   if(! ws->wd) return EINA_FALSE;

   if(new) {
      qry = sqlite3_mprintf(QRY_INSERT_SITEINFO, ws->siteid,
	    ws->wd->sitename, ws->wd->localname, ws->wd->lang, ws->wd->locale,
	    ws->wd->base, ws->wd->case_used, ws->wd->generator,
	    ws->wd->filename);
   } else {
      qry = sqlite3_mprintf(QRY_UPDATE_SITEINFO, 
	    ws->wd->locale,
	    ws->wd->base, ws->wd->case_used, ws->wd->generator,
	    ws->wd->filename, ws->siteid);
   }

   if(qry) {
      sqlite3_stmt *stmt = NULL;
      int r = sqlite3_prepare_v2(ws->db, qry, strlen(qry) + 1, &stmt, NULL);
      if(r == SQLITE_OK) {
	 sqlite3_step(stmt);
	 ret = EINA_TRUE;
      } else
	 fprintf(stderr, "Failed to prepare statement: %d, %s\n", r, sqlite3_errmsg(ws->db));

      sqlite3_finalize(stmt);
      sqlite3_free(qry);
   }

   return ret;
}

static Eina_Bool wd_sqlite_namespaces_update(const Eina_Hash *h, const void *k, void *d, void *fd)
{
   Wiki_Sqlite *ws = fd;
   Wiki_Namespace *ns = d;
   int key = atoi(k);

   if(fd && ns) {
      if((strcmp(k, "0") == 0 && key == 0) || (key && key == ns->key)) {
	 char *qry = sqlite3_mprintf(QRY_INSERT_NAMESPACE, ws->siteid,
	       ns->key, ns->label, ns->npages);
	 if(qry) {
	    if(sqlite3_exec(ws->db, qry, NULL, NULL, NULL) != SQLITE_OK)
	       fprintf(stderr, "SQL ERROR: %s\n",  sqlite3_errmsg(ws->db));
	    sqlite3_free(qry);
	 }
      }
   }

   return EINA_TRUE;
}

static Eina_Bool wd_sqlite_data_categorylinks_update(void *fdata, Eina_List *list)
{
   Wiki_Sqlite *ws = fdata;
   const char *nscat = NULL, *tmp = NULL;
   char *cat = NULL, *qry = NULL;

   if(! ws) return EINA_FALSE;
   if(! list) return EINA_FALSE;

   if(signal_main) {
      return EINA_FALSE;
   }

   if(eina_list_count(list) < 4) {
      printf("Categorylinks callback fail\n");
      return EINA_FALSE;
   }

   nscat = wikidump_namespace_name_get(ws->wd->namespaces, WIKI_NS_CATEGORY);

   if(nscat) {
      int id = atoi((char *) eina_list_nth(list, 0));
      cat = strdup(eina_list_nth(list, 1));
      if(id && cat) {
	 cat = pcre_replace("/_/", " ", cat);
	 cat = pcre_replace("/\\\\/", "", cat);
	 cat = pcre_replace("/^'/", "", cat);
	 cat = pcre_replace("/'$/", "", cat);

	 qry = sqlite3_mprintf(QRY_INSERT_CATEGORYLINKS_INFO,
	       ws->siteid, id, nscat, cat);
	 free(cat);
      }
   }

   if(qry) {
      if(sqlite3_exec(ws->db, qry, NULL, NULL, NULL) != SQLITE_OK)
	 fprintf(stderr, "SQL ERROR: %s\n",  sqlite3_errmsg(ws->db));
      sqlite3_free(qry);
   }

   return EINA_TRUE;
}

static Eina_Bool wd_sqlite_data_images_update(void *fdata, Eina_List *list)
{
   Wiki_Sqlite *ws = fdata;
   const char *nsfile = NULL, *tmp = NULL;
   char *file = NULL, *qry = NULL;

   if(! ws) return EINA_FALSE;
   if(! list) return EINA_FALSE;

   if(signal_main) {
      return EINA_FALSE;
   }

   if(eina_list_count(list) != 14) {
      printf("Image callback fail\n");
      return EINA_FALSE;
   }

   nsfile = wikidump_namespace_name_get(ws->wd->namespaces, WIKI_NS_FILE);

   tmp = eina_list_nth(list, 0);
   if(tmp && tmp[0] == '\'') {
      file = strdup(tmp + 1);
      if(file) {
	 char *mime = NULL;
	 int s = atoi((char *) eina_list_nth(list, 1));
	 int w = atoi((char *) eina_list_nth(list, 2));
	 int h = atoi((char *) eina_list_nth(list, 3));

	 file[strlen(file) - 1] = '\0';
	 file = pcre_replace("/_/", " ", file);
	 file = pcre_replace("/\\\\/", "", file);

	 asprintf(&mime, "%s/%s",
	       (char *) eina_list_nth(list, 7),
	       (char *) eina_list_nth(list, 8));
	 if(mime) 
	    mime = pcre_replace("/'/", "", mime);

	 qry = sqlite3_mprintf(QRY_INSERT_IMAGE_INFO,
	       ws->siteid, nsfile, file,
	       s, w, h, mime);
	 free(file);
	 if(mime)
	    free(mime);
      }
   }

   if(qry) {
      if(sqlite3_exec(ws->db, qry, NULL, NULL, NULL) != SQLITE_OK)
	 fprintf(stderr, "SQL ERROR: %s\n",  sqlite3_errmsg(ws->db));
      sqlite3_free(qry);
   }

   return EINA_TRUE;
}

static Eina_Bool wd_sqlite_data_update(Wiki_Sqlite *ws, const char *locale, const char *image_sql, const char *category_sql)
{
   Wiki_Dump_Page *wdp = NULL;
   Eina_Bool new = EINA_FALSE;
   const char *catname = NULL;
   char *category = NULL;
   const char *sitename = NULL;
   unsigned int cnt = 0, nbpages = 0;
   xmlNodePtr siteinfo = NULL;

   if(! ws) return EINA_FALSE;
   if(! ws->wd) return EINA_FALSE;
   
   ws->wd->state = 1;

   siteinfo = wikidump_siteinfo_read(ws->wd);
   if(! siteinfo) return EINA_FALSE;


   sitename = ws->wd->sitename;
   if(! sitename)
      return EINA_FALSE;

   if(wd_sqlite_siteid_get(ws, sitename, &new) == 0) {
      return EINA_FALSE;
   }

   if(! locale) {
      const char *loc = getenv("LANGUAGE");
      if(! loc)
	 loc = getenv("LANG");
      if(loc) {
	 fprintf(stderr, "Warning: using %s locale for %s with lang = %s\n",
	       loc, ws->wd->sitename, ws->wd->lang);
	 ws->wd->locale = loc;
      } else {
	 fprintf(stderr, "Warning: locale not defined for %s with lang = %s\n",
	       ws->wd->sitename, ws->wd->lang);
      }
   } else {
      ws->wd->locale = locale;
   }
	    
   if(wd_sqlite_siteinfo_update(ws, new) == EINA_FALSE)
      return EINA_FALSE;

   sqlite3_exec(ws->db, PRAGMA_NEW_FORMAT, NULL, NULL, NULL);
   sqlite3_exec(ws->db, PRAGMA_SYNCHRONOUS, NULL, NULL, NULL);

   /* TODO: rollback on interrupt */
   if(sqlite3_exec(ws->db, "begin exclusive transaction", NULL, NULL, NULL)
	 != SQLITE_OK)
      return EINA_FALSE;

   printf("Remove old data\n");
   wd_sqlite_table_delete(ws, "wiki_images");
   wd_sqlite_table_delete(ws, "wiki_pages");
   wd_sqlite_table_delete(ws, "wiki_namespaces");
   wd_sqlite_table_delete(ws, "wiki_categories");
   wd_sqlite_table_delete(ws, "wiki_fts");

   catname = wikidump_namespace_name_get(ws->wd->namespaces, WIKI_NS_CATEGORY);
   if(catname) {
      category = calloc(strlen(catname) + 2, sizeof(char));
      if(category)
	 sprintf(category, "%s:", catname);
   }

   if(category && category_sql) {
      printf("Prepare categorylinks info\n");
      read_sql_file(category_sql, "INSERT INTO `categorylinks` VALUES ", wd_sqlite_data_categorylinks_update, ws);
   }

   if(image_sql) {
      printf("Prepare image info\n");
      read_sql_file(image_sql, "INSERT INTO `image` VALUES ", wd_sqlite_data_images_update, ws);
   }

   printf("Update pages\n");
   while(signal_main == 0 && (wdp = wikidump_page_page_get(ws->wd)) != NULL) {
      int id = wikidump_page_id_get(wdp);
      char *title = wikidump_page_title_get(wdp);
      char *content = wikidump_page_content_get(wdp);
      int nsid = wikidump_title_namespace_get(ws->wd->namespaces, title);
      char *page = wikidump_page_buffer_get(wdp);

      if(title && (strncmp(title, "User:COIBot", 11) == 0
               || strncmp(title, "User talk:COIBot", 16) == 0
               || strncmp(title, "Category:COIBot", 15) == 0
               )) {
//         printf("%s SKIPPED\n", title);
         goto SKIP;
      }

      if(id) {
         char *qry = NULL;
         qry = sqlite3_mprintf(QRY_INSERT_PAGES, ws->siteid, id, title, page);
	 if(qry) {
	    if(sqlite3_exec(ws->db, qry, NULL, NULL, NULL) != SQLITE_OK) {
	       fprintf(stderr, "SQL ERROR: %s\n",  sqlite3_errmsg(ws->db));
	       /* Continue on error like duplicate title */
	       // break;
	    } 
	    sqlite3_free(qry);
	    qry = NULL;
	 }

	 nbpages++;
	 // printf("%d : %s\n", nbpages, title);

	 wikidump_namespace_page_add(ws->wd->namespaces, nsid);
      }

SKIP:
      wikidump_page_free(wdp);

      if(title) 
	 free(title);
      if(content)
	 free(content);
      if(page)
	 free(page);

      cnt++;
      if(cnt >= 10000) {
	 static int ncommit = 0;
	 ncommit++;
	 printf("Commit part: %d, %d\n", ncommit, ncommit * cnt);
	 sqlite3_exec(ws->db, "commit transaction", NULL, NULL, NULL);
	 sqlite3_exec(ws->db, "begin transaction", NULL, NULL, NULL);
	 
	 cnt = 0;
      }
//      if(cnt >= 100) break;
   }

   if(ws->wd->namespaces) {
      printf("Update namespaces\n");
      eina_hash_foreach(ws->wd->namespaces, wd_sqlite_namespaces_update, ws);
   }

   sqlite3_exec(ws->db, "commit transaction", NULL, NULL, NULL);
   if(! signal_main) {
      printf("Update indices\n");
      wd_sqlite_index_create(ws);
   }
   if(! signal_main) {
      char *qry = sqlite3_mprintf("insert into wiki_fts select siteid, title from wiki_pages where siteid = %d", ws->siteid);
      printf("Update fts\n");
      if(qry) {
         sqlite3_exec(ws->db, "begin transaction", NULL, NULL, NULL);
         sqlite3_exec(ws->db, qry, NULL, NULL, NULL);
         sqlite3_exec(ws->db, "commit transaction", NULL, NULL, NULL);
         sqlite3_free(qry);
      }
   }

   if(category) {
      free(category);
   }

   printf("Clean up\n");

//   sqlite3_exec(ws->db, "vacuum", NULL, NULL, NULL);

   printf("Done\n");

   ws->wd->state = 0;
   xmlFreeNode(siteinfo);

   return EINA_TRUE;
}

int main(int argc, char **argv)
{
   Wiki_Sqlite *ws = NULL;
   struct sigaction *sa = NULL;
   Eina_Bool r;

   eina_init();

   ws = wd_sqlite_new();
   if(ws) {
      r = wd_sqlite_database_open(ws, WIKIDUMP_DBMODE_INSERT);
      if(r == EINA_TRUE && argc >= 2) {
           sqlite3_create_function(ws->db, "compress", 1, SQLITE_UTF8, 0, compressFunc, 0, 0);
           sqlite3_create_function(ws->db, "uncompress", 1, SQLITE_UTF8, 0, uncompressFunc, 0, 0);
           sqlite3_create_function(ws->db, "rank", 3, SQLITE_UTF8, 0, rankFunc, 0, 0);


         ws->wd = wikidump_open_filename_new(argv[1]);
	 if(ws->wd) {
	    const char *locale = NULL;
	    const char *image_sql = NULL;
	    const char *category_sql = NULL;

	    if(argc > 2) 
	       locale = argv[2];
	    if(argc > 3)
	       image_sql = argv[3];
	    if(argc > 4)
	       category_sql = argv[4];

            sa = calloc(1, sizeof(struct sigaction));
            if(sa) {
               const struct sigaction *s = sa;
               sa->sa_handler = signal_handler;
               sigaction(SIGQUIT, s, NULL);
               sigaction(SIGTERM, s, NULL);
               sigaction(SIGINT, s, NULL);
               sigaction(SIGHUP, s, NULL);
            }
	    
	    if(wd_sqlite_data_update(ws, locale, image_sql, category_sql) == EINA_FALSE)
	       printf("Update failed\n");

            if(sa)
               free(sa);
	 }
      }
      wd_sqlite_free(ws);
   }

   eina_shutdown();

   return EXIT_SUCCESS;
}
#endif

