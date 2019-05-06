#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>

#include <Eina.h>
#include "parser.h"
#include "util.h"
#include "wikidump_fs.h"
// #include "wikidump_server_common.h"
#include "wikidump.h"
#include "parse_sql.h"
#include "pcre_replace.h"


static Eina_Hash *databases = NULL;

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
   //l = list;

   return EINA_TRUE;
}
#ifdef WIKIDUMP_SQLITE_MAIN
int signal_main = 0;

static void signal_handler(int signal)
{
   signal_main = signal;
   printf("Get signal '%d', try to wait programm finish to keep database clean\n");
}

Wiki_FS *wd_fs_free(Wiki_FS *wf)
{
   if(wf) {
      if(wf->wdp)
	 wf->wdp = wikidump_page_free(wf->wdp);
      if(wf->wd)
	 wf->wd = wikidump_del(wf->wd);
      free(wf);
   }

   return NULL;
}

Wiki_FS *
wd_fs_new()
{
   Wiki_FS *wf = NULL;
   wf = calloc(1, sizeof(Wiki_FS));
   return wf;
}

static Eina_Bool wd_fs_siteinfo_update(Wiki_FS *ws, Eina_Bool new)
{
   char *qry = NULL;
   Eina_Bool ret = EINA_FALSE;

   return ret;
}

static Eina_Bool wd_fs_namespaces_update(const Eina_Hash *h, const void *k, void *d, void *fd)
{
   Wiki_FS *ws = fd;
   Wiki_Namespace *ns = d;
   int key = atoi(k);

   if(fd && ns) {
      if((strcmp(k, "0") == 0 && key == 0) || (key && key == ns->key)) {
      }
   }

   return EINA_TRUE;
}

static Eina_Bool wd_fs_data_categorylinks_update(void *fdata, Eina_List *list)
{
   Wiki_FS *ws = fdata;
   const char *nscat = NULL, *tmp = NULL;
   char *cat = NULL, *qry = NULL;

   if(! ws) return EINA_FALSE;
   if(! list) return EINA_FALSE;

   if(signal_main) {
      return EINA_FALSE;
   }

   if(eina_list_count(list) != 4) {
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

	 free(cat);
      }
   }

   if(qry) {
   }

   return EINA_TRUE;
}

static Eina_Bool wd_fs_data_images_update(void *fdata, Eina_List *list)
{
   Wiki_FS *ws = fdata;
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

	 free(file);
	 if(mime)
	    free(mime);
      }
   }

   if(qry) {
   }

   return EINA_TRUE;
}

static Eina_Bool 
mkdir_fname(const char *fname)
{
   Eina_Bool ret = EINA_FALSE;
   char *path = NULL, *p = NULL;
   unsigned int cnt = 0;

   if(! fname) return ret;

   path = strdup(fname);
   if(! path) return ret;

   p = path;
   while((p = index(p, '/'))) {
      *p = '\0';
      mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      *p = '/';
      p++;
   }

   free(path);
   ret = EINA_TRUE;

   return ret;
}

static Eina_Bool 
wd_fs_data_update(Wiki_FS *ws, const char *locale, const char *image_sql, const char *category_sql)
{
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
	    
   catname = wikidump_namespace_name_get(ws->wd->namespaces, WIKI_NS_CATEGORY);
   if(catname) {
      category = calloc(strlen(catname) + 2, sizeof(char));
      if(category)
	 sprintf(category, "%s:", catname);
   }

   if(category && category_sql) {
      printf("Prepare categorylinks info\n");
      read_sql_file(category_sql, "INSERT INTO `categorylinks` VALUES ", wd_fs_data_categorylinks_update, ws);
   }

   if(image_sql) {
      printf("Prepare image info\n");
      read_sql_file(image_sql, "INSERT INTO `image` VALUES ", wd_fs_data_images_update, ws);
   }

   printf("Update pages\n");
   while(signal_main == 0 && (ws->wdp = wikidump_page_page_get(ws->wd)) != NULL) {
      int id = wikidump_page_id_get(ws->wdp);
      char *title = wikidump_page_title_get(ws->wdp);
      char *content = wikidump_page_content_get(ws->wdp);
      int nsid = wikidump_title_namespace_get(ws->wd->namespaces, title);
      char *page = wikidump_page_buffer_get(ws->wdp);

      if(title && (strncmp(title, "User:COIBot", 11) == 0
               || strncmp(title, "User talk:COIBot", 16) == 0
               || strncmp(title, "Category:COIBot", 15) == 0
               )) {
         printf("%s SKIPPED\n", title);
         goto SKIP;
      }

      if(id) {
	 char *fname = NULL;
	 asprintf(&fname, "%s/%s.xml", sitename, title);

	 if(fname) {
	    fname = pcre_replace("/:/", "/", fname);
	    if(mkdir_fname(fname) == EINA_TRUE) {
	       FILE *f = fopen(fname, "w");
	       if(f) {
		  fwrite(page, sizeof(char), strlen(page), f);
		  fclose(f);
	       }
	    }
	    free(fname);
	 }
	 nbpages++;
	 // printf("%d : %s\n", nbpages, title);

	 wikidump_namespace_page_add(ws->wd->namespaces, nsid);
      }

SKIP:
      wikidump_page_free(ws->wdp);

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
	 
	 cnt = 0;
      }
//      if(cnt >= 100) break;
   }

   /*
   if(ws->wd->namespaces) {
      printf("Update namespaces\n");
      eina_hash_foreach(ws->wd->namespaces, wd_fs_namespaces_update, ws);
   }
   */

   if(category) {
      free(category);
   }

   printf("Clean up\n");

   printf("Done\n");

   ws->wd->state = 0;
   xmlFreeNode(siteinfo);

   return EINA_TRUE;
}

int main(int argc, char **argv)
{
   Wiki_FS *ws = NULL;
   struct sigaction *sa = NULL;
   Eina_Bool r;

   eina_init();

   ws = wd_fs_new();
   if(ws) {
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

	 if(wd_fs_data_update(ws, locale, image_sql, category_sql) == EINA_FALSE)
	    printf("Update failed\n");

      	 if(sa)
   	    free(sa);
      }
   }
   wd_fs_free(ws);

   eina_shutdown();

   return EXIT_SUCCESS;
}
#endif

