#include <Eina.h>
#include <libxml/xmlreader.h>
#include <bzlib.h>

#include "wiki_define.h"
#include "wikidump.h"
#include "pcre_replace.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

static Wiki_Dump *wikidump_add(void);
static int _wikidump_close(void *ctx);
static int _wikidump_data_read(void *ctx, char *buf, int len);

static Wiki_Dump *wikidump_add(void)
{
   Wiki_Dump *wd = calloc(1, sizeof(Wiki_Dump));
   if(! wd) return NULL;
   wd->pages = NULL;

   return wd;
}

Eina_Bool wikidump_namespaces_del(const Eina_Hash *h, const void *k, void *d, void *fd)
{
   Wiki_Namespace *ns = d;
   int key;

   key = atoi(k);
   if(ns && ((strcmp(k, "0") == 0 && key == 0)
	    || (key && ns->key == key))) {
      if(ns->label) {
	 // eina_stringshare_del(ns->label);
	 free((char *) ns->label);
	 ns->label = NULL;
      }
      free(ns);
      ns = NULL;
   }

   return EINA_TRUE;
}

Eina_Hash *wikidump_namespaces_alias_set(Eina_Hash *namespaces)
{
   Wiki_Namespace *wns = NULL;
   const char *alias[] = {"Media", "Special", "", "Talk", "User", "User talk",
	"Project", "Project talk", "File", "File talk", "MediaWiki", 
	"MediaWiki talk", "Template", "Template talk", "Help", "Help talk",
	"Category", "Category talk", NULL};
   int i = 0, key = -2;
   char ns[10];

   if(! namespaces) return NULL;

   while(alias[i] != NULL) {
      sprintf(ns, "%d", key);
      wns = eina_hash_find(namespaces, ns);
      if(wns && ! eina_hash_find(namespaces, alias[i])) {
	 eina_hash_add(namespaces, alias[i], wns);
      }
      if(key == 6) {
	 const char a[] = "Image";
	 if(wns && ! eina_hash_find(namespaces, a)) {
	    eina_hash_add(namespaces, a, wns);
	 }
      }
      key++;
      i++;
   }

   return namespaces;
}

Wiki_Dump_Page *wikidump_page_parser_get(const char *xml, size_t size)
{
   Wiki_Dump_Page *wp = NULL;
   xmlNodePtr node = NULL;

   if(! xml) return wp;
   if(size <= 0) return wp;

   wp = malloc(sizeof(Wiki_Dump_Page));
   if(! wp) return wp;

   wp->xml = NULL;
   wp->buf = NULL;

   wp->doc = xmlParseMemory(xml, size);
   if(wp->doc) {
      wp->node = xmlDocGetRootElement(wp->doc);
      if(wp->node) {
	 node = wp->node->children;
	 while(node) {
	    if(strcmp((char *) node->name, "revision") == 0) {
	       wp->revision = node;
	       break;
	    }
       	    node = node->next;
	 }
	 //	    xmlElemDump(stdout, wp->node->doc, wp->node);
      }
   } else {
      free(wp);
      wp = NULL;
   }

   return wp;
}

void wikidump_file_prop_set(Wiki_Dump_Page *wdp, int size, int width, int height, const char *mime)
{
   xmlNodePtr node;
   char *title = NULL;
   char buf[255];

   if(! wdp) return;
   if(! wdp->node) return;

   if(wdp->node->children) {
      node = wdp->node->children;
      while(node) {
	 if(! strcmp((char *) node->name, "title")) {
	    break;
	 }
	 node = node->next;
      }
   }

   if(node) {
      if(! xmlHasProp(node, "size")) {
	 sprintf(buf, "%d", size);
	 xmlNewProp(node, "size", buf);
      }
      if(! xmlHasProp(node, "width")) {
	 sprintf(buf, "%d", width);
	 xmlNewProp(node, "width", buf);
      }
      if(! xmlHasProp(node, "height")) {
	 sprintf(buf, "%d", height);
	 xmlNewProp(node, "height", buf);
      }
      if(! xmlHasProp(node, "mime")) {
	 xmlNewProp(node, "mime", mime);
      }
   }
}

char *wikidump_page_buffer_get(Wiki_Dump_Page *wp)
{
   char *content = NULL;
   xmlBufferPtr buf = NULL;

   if(! wp) return NULL;
   if(! wp->node) return NULL;

   buf = xmlBufferCreate();
   if(buf) {
      if(xmlNodeDump(buf, wp->doc, wp->node, 0, 0)) {
	 content = strdup((char *) xmlBufferContent(buf));
      }
      xmlBufferFree(buf);
   }

   return content;
}

Wiki_Dump_Page *wikidump_page_page_get(Wiki_Dump *wd)
{
   Wiki_Dump_Page *wp = NULL;
   xmlNodePtr node = NULL;
   const char *nodename = NULL;

   if(! wd) return wp;
   if(! wd->xml) return wp;

    xmlTextReaderNext(wd->xml);
    do {
       nodename = (const char *)xmlTextReaderConstName(wd->xml);
      if(nodename && strcmp(nodename, "page") == 0) {
	 wp = calloc(1, sizeof(Wiki_Dump_Page));
	 if(! wp) return NULL;

	 wp->node = xmlTextReaderExpand(wd->xml);
	 wp->xml = NULL;
	 wp->buf = NULL;

      	 if(wp->node) {
	    node = wp->node->children;
	    while(node) {
	       if(strcmp((char *) node->name, "revision") == 0) {
		  wp->revision = node;
		  break;
	       }
	       node = node->next;
	    }
//	    xmlElemDump(stdout, wp->node->doc, wp->node);
	 }

	 break;
      }
    } while(xmlTextReaderRead(wd->xml));
   
   return wp;
}

Wiki_Dump_Page *wikidump_page_free(Wiki_Dump_Page *wp)
{
   if(wp) {
      if(wp->xml)
	 xmlFreeTextReader(wp->xml);
      if(wp->buf)
	 xmlFreeParserInputBuffer(wp->buf);
      if(wp->doc) 
	 xmlFreeDoc(wp->doc);
      free(wp);
   }

   return NULL;
}

char *wikidump_page_title_get(Wiki_Dump_Page *wp)
{
   xmlNodePtr node;
   char *title = NULL;

   if(! wp) return title;
   if(! wp->node) return title;

   if(wp->node->children) {
      node = wp->node->children;
      while(node) {
	 if(! strcmp((char *) node->name, "title")) {
	    title = (char *) xmlNodeGetContent(node);
	    break;
	 }
	 node = node->next;
      }
   }

   return title;
}

Wiki_File *wikidump_page_file_info_get(Wiki_Dump_Page *wp)
{
   xmlNodePtr node;
   Wiki_File *wf = NULL;

   if(! wp) return wf;
   if(! wp->node) return wf;

   if(wp->node->children) {
      node = wp->node->children;
      while(node) {
	 if(strcmp((char *) node->name, "title") == 0) {
	    char *tmp = NULL;
	    wf = calloc(1, sizeof(Wiki_File));
	    if(wf) {
	       if( xmlHasProp(node, "size")) {
		  tmp = (char *) xmlGetProp(node, "size");
		  if(tmp) wf->size = atoi(tmp);
		  free(tmp);
	       }
	       if(xmlHasProp(node, "width")) {
		  tmp = (char *) xmlGetProp(node, "width");
		  if(tmp) wf->width = atoi(tmp);
		  free(tmp);
	       }
	       if(xmlHasProp(node, "height")) {
		  tmp = (char *) xmlGetProp(node, "height");
		  if(tmp) wf->height = atoi(tmp);
		  free(tmp);
	       }
	       if(xmlHasProp(node, "mime")) {
		  wf->mime = (char *) xmlGetProp(node, "mime");
	       }
	    }
	    break;
	 }
	 node = node->next;
      }
   }

   return wf;
}

char *wikidump_page_content_get(Wiki_Dump_Page *wp)
{
   xmlNodePtr node;
   char *text = NULL;

   if(! wp) return text;
   if(! wp->revision) return text;

   node = wp->revision->children;
   while(node) {
      if(! xmlIsBlankNode(node) && ! strcmp((char *) node->name, "text")) {
	 text = (char *) xmlNodeGetContent(node);
//	 if(text)
//	    text = pcre_replace("/&/", "&amp;", text);
	 break;
      }
      node = node->next;
   }

   return text;
}

unsigned int wikidump_page_id_get(Wiki_Dump_Page *wp)
{
   unsigned int id = 0;

   xmlNodePtr node;

   if(! wp) return id;
   if(! wp->node) return id;

   if(wp->node->children) {
      node = wp->node->children;
      while(node) {
	 if(! strcmp((char *) node->name, "id")) {
	    char *tmp = (char *) xmlNodeGetContent(node);
	    if(tmp) {
	       id = atoi(tmp);
	       free(tmp);
	    }
	    break;
	 }
	 node = node->next;
      }
   }

   return id;
}

unsigned int wikidump_page_revision_id_get(Wiki_Dump_Page *wp)
{
   unsigned int id = 0;

   xmlNodePtr node;

   if(! wp) return id;
   if(! wp->revision) return id;

   if(wp->revision->children) {
      node = wp->revision->children;
      while(node) {
	 if(! strcmp((char *) node->name, "id")) {
	    char *tmp = (char *) xmlNodeGetContent(node);
	    if(tmp) {
	       id = atoi(tmp);
	       free(tmp);
	    }
	    break;
	 }
	 node = node->next;
      }
   }

   return id;
}

char *wikidump_page_revision_timestamp_get(Wiki_Dump_Page *wp)
{
   char *time = NULL;

   xmlNodePtr node;

   if(! wp) return time;
   if(! wp->revision) return time;

   if(wp->revision->children) {
      node = wp->revision->children;
      while(node) {
	 if(! strcmp((char *) node->name, "timestamp")) {
	    time = (char *) xmlNodeGetContent(node);
	    break;
	 }
	 node = node->next;
      }
   }
   return time;
}

char *wikidump_page_revision_comment_get(Wiki_Dump_Page *wp)
{
   char *comment = NULL;

   xmlNodePtr node;

   if(! wp) return comment;
   if(! wp->revision) return comment;

   if(wp->revision->children) {
      node = wp->revision->children;
      while(node) {
	 if(! strcmp((char *) node->name, "comment")) {
	    comment = (char *) xmlNodeGetContent(node);
	    break;
	 }
	 node = node->next;
      }
   }

   return comment;
}

char *wikidump_page_contributor_username_get(Wiki_Dump_Page *wp)
{
   char *username = NULL;
   xmlNodePtr node;

   if(! wp) return username;
   if(! wp->revision) return username;

   node = wp->revision->children;
   while(node) {
      if(! strcmp((char *) node->name, "contributor")) {
	 xmlNodePtr n = node->children;
	 while(n) {
	    if(! strcmp((char *) n->name, "username")) {
	       username = (char *) xmlNodeGetContent(n);
	       break;
	    }
	    n = n->next;
	 }
	 if(! username) {
	    n = node->children;
	    while(n) {
	       if(! strcmp((char *) n->name, "ip")) {
		  username = (char *) xmlNodeGetContent(n);
		  break;
	       }
	       n = n->next;
	    }
	 }
	 break;
      }
      node = node->next;
   }

   return username;
}

unsigned int wikidump_page_contributor_id_get(Wiki_Dump_Page *wp)
{
   unsigned int id = 0;

   xmlNodePtr node;

   if(! wp) return id;
   if(! wp->revision) return id;

   node = wp->revision->children;
   while(node) {
      if(! strcmp((char *) node->name, "contributor")) {
	 xmlNodePtr n = node->children;
	 while(n) {
	    if(! strcmp((char *) n->name, "id")) {
	       char *tmp = (char *) xmlNodeGetContent(n);
	       if(tmp) {
		  id = atoi(tmp);
		  free(tmp);
	       }
	    }
	    n = n->next;
	 }
	 break;
      }
      node = node->next;
   }

   return id;
}

void wikidump_namespace_page_add(Eina_Hash *namespaces, int nsid)
{
   Wiki_Namespace *ns = NULL;
   char id[sizeof(int) + 1];

   if(! namespaces) return;

   snprintf(id, sizeof(id), "%d", nsid);
   ns = eina_hash_find(namespaces, id);
   if(ns)
      ns->npages++;
   
}

const char *wikidump_namespace_name_get(Eina_Hash *namespaces, int nsid)
{
   const char *nsstr = NULL;
   Wiki_Namespace *ns = NULL;
   char id[sizeof(int) + 1];

   if(! namespaces) return nsstr;

   snprintf(id, sizeof(id), "%d", nsid);
   ns = eina_hash_find(namespaces, id);
   if(ns)
      nsstr = ns->label;
   
   return nsstr;
}

int wikidump_title_namespace_get(Eina_Hash *namespaces, const char *title)
{
   int nsid = 0;
   char *p = NULL;

   if(! namespaces) return nsid;
   if(! title) return nsid;

   if((p = strstr(title, ":"))) {
      char *nsstr = strndup(title, p - title);
      if(nsstr) {
	 Wiki_Namespace *ns = eina_hash_find(namespaces, nsstr);
	 if(ns) {
	    nsid = ns->key;
	 }
	 free(nsstr);
      }
   }
  
  if(! nsid) {
      Wiki_Namespace *ns = eina_hash_find(namespaces, 
	    wikidump_namespace_name_get(namespaces, WIKI_NS_MAIN));
      if(ns) {
	 nsid = ns->key;
      }
   }

   return nsid;
}

#ifdef WIKIDUMP_SQLITE_MAIN
static int _wikidump_close(void *ctx)
{
   return 1;
}

static int _wikidump_data_read(void *ctx, char *buf, int len) 
{
   Wiki_Dump *wd = ctx;
   int r = 0;

   if(wd) {
      if(wd->bzfile) {
	 r = BZ2_bzread(wd->bzfile, buf, len);
      }
   }

   return r;
}

xmlNodePtr wikidump_siteinfo_read(Wiki_Dump *wd)
{
   xmlNodePtr site, node, namespace = NULL;
   xmlNodePtr siteinfo = NULL;
   int        res = 0, max_read = 5;;


   if(! wd) return 0;

   if(! wd->xbuf) {
      wd->xbuf = xmlParserInputBufferCreateIO(_wikidump_data_read, 
	    _wikidump_close, wd, XML_CHAR_ENCODING_UTF8);

      wd->xml = xmlNewTextReader(wd->xbuf, NULL);
   }

   if(! wd->xml) return 0;

   xmlTextReaderRead(wd->xml);
   do {
       if(! strcmp((char *)xmlTextReaderConstName(wd->xml), "mediawiki")) {
	  wd->lang = eina_stringshare_add((char *) xmlTextReaderConstXmlLang(wd->xml));
	  if(!wd->lang)
	     return 0;
       } else if(! strcmp((char *)xmlTextReaderConstName(wd->xml), "siteinfo")) {
	  site = xmlTextReaderExpand(wd->xml);
	  if(site) {
	     siteinfo = xmlCopyNode(site, 1);
	     res = 1;
             node = site->children;
	     while(node) {
		if(node->children && strcmp((char *)node->name, "namespaces") == 0) {
		   namespace = node->children;
		} else if(node->type != XML_TEXT_NODE) {
		   char *tmp = (char *) xmlNodeGetContent(node);
		   if(strcmp((char *) node->name, "sitename") == 0) {
		      wd->localname = eina_stringshare_add(tmp);
		   } else if(strcmp((char *) node->name, "generator") == 0) {
		      wd->generator = eina_stringshare_add(tmp);
		   } else if(strcmp((char *) node->name, "base") == 0) {
		      wd->base = eina_stringshare_add(tmp);
		   } else if(strcmp((char *) node->name, "case") == 0) {
		      wd->case_used = eina_stringshare_add(tmp);
		   }
		   if(tmp)
		      free(tmp);
		}
		node = node->next;
	     }
	     break;
	  } else {
	     break;
	  }
       }
       max_read--;
   } while(xmlTextReaderRead(wd->xml) && max_read);

   if(res && namespace) {
      node = namespace;
      wd->namespaces = eina_hash_string_superfast_new(NULL);
      while(node && wd->namespaces) {
	 if(node->type != XML_TEXT_NODE && xmlHasProp(node, (xmlChar *) "key")) {
	    Wiki_Namespace *ns = malloc(sizeof(Wiki_Namespace));
	    if(ns) {
	       char *key = (char *) xmlGetProp(node, (xmlChar *) "key");
	       char *label = (char *) xmlNodeGetContent(node);
	       if(key) 
		  ns->key = atoi(key);
	       if(label) {
		  ns->label = label;
	       }
	       ns->npages = 0;

	       if(key)
		  eina_hash_add(wd->namespaces, key, ns);
	       if(ns->label)
		  eina_hash_add(wd->namespaces, ns->label, ns);
	       free(key);
	    }
	 }
	 node = node->next;
      }
   }

   if(wd->base) {
      char *sitename = strdup(wd->base);
      if(sitename) {
	 sitename = pcre_replace("/^http:\\/\\/([^\\/]*)\\/.*/", "$1", sitename);
	 printf("sitename: %s\n", sitename);
	 if(sitename) {
	    wd->sitename = eina_stringshare_add(sitename);
	    free(sitename);
	 }
      }
   }

   return siteinfo;
}

Wiki_Dump *wikidump_del(Wiki_Dump *wd)
{
   if(wd) {
      if(wd->bzfile)
	 BZ2_bzclose(wd->bzfile);
      if(wd->filename)
	 eina_stringshare_del(wd->filename);
      if(wd->sitename)
	 eina_stringshare_del(wd->sitename);
      if(wd->localname)
	 eina_stringshare_del(wd->localname);
      if(wd->base) 
	 eina_stringshare_del(wd->base);
      if(wd->generator)
	 eina_stringshare_del(wd->generator);
      if(wd->case_used)
	 eina_stringshare_del(wd->case_used);

      if(wd->xml)
	 xmlFreeTextReader(wd->xml);
      if(wd->xbuf)
	 xmlFreeParserInputBuffer(wd->xbuf);
      if(wd->namespaces) {
	 eina_hash_foreach(wd->namespaces, wikidump_namespaces_del, NULL);
	 eina_hash_free(wd->namespaces);
      }
      free(wd);
   }

   return NULL;
}

Wiki_Dump *wikidump_open_filename_new(const char *filename)
{
   Wiki_Dump *wd = NULL;
   BZFILE *bzfile = NULL;
   char *f = NULL;

   wd = wikidump_add();
   if(! wd) return NULL;

   if(! (bzfile = BZ2_bzopen(filename, "r"))) {
      fprintf(stderr, "Can't open '%s'\n", filename);
      wikidump_del(wd);
      return NULL;
   }

   wd->bzfile = bzfile;
   f = strdup(filename);
   if(f) {
      wd->filename = eina_stringshare_add(basename(f));
      free(f);
   }

   return wd;
}
#endif

Eina_Bool wikidump_namespaces_print(const Eina_Hash *h, const void *k, void *d, void *fd)
{
   Wiki_Namespace *ns = d;
   static int cnt = 0;

   if(! ns) return EINA_FALSE;

   cnt += ns->npages;

   printf("key: %s, id: %d, label: %s, npages: %d, total: %d\n", (char *)k, 
	 ns->key, ns->label, ns->npages, cnt / 2);

   return EINA_TRUE;
}


#ifdef WIKI_DUMP_MAIN
static void get_pages(Wiki_Dump *wd, const char *filename)
{
   const char *cat = NULL;
   char *category = NULL;

   if(! wd) return;

   wd->state = 1;

   wikidump_siteinfo_read(wd);
   
   cat = wikidump_namespace_name_get(wd->namespaces, WIKI_NS_CATEGORY);
   if(cat) {
      category = calloc(strlen(cat) + 2, sizeof(char));
      sprintf(category, "%s:", cat);
   }

   int i = 1000;
   while(i) {
      Wiki_Dump_Page *wp = wikidump_page_page_get(wd);
      if(wp) {
	 int id = wikidump_page_id_get(wp);
	 int rev_id = wikidump_page_revision_id_get(wp);
	 char *title = wikidump_page_title_get(wp);
	 char *modif = wikidump_page_revision_timestamp_get(wp);
	 int cid = wikidump_page_contributor_id_get(wp);
	 char *comment = wikidump_page_revision_comment_get(wp);
	 char *content = wikidump_page_content_get(wp);
	 char *buf = NULL;
	 int nsid = wikidump_title_namespace_get(wd->namespaces, title);

	 if(comment == NULL) {
	    comment = strdup("");
	 }

	 if(category && content) {
	    char *p;
	    char *tmp = content;
	    size_t len = strlen(content);
	    while((p = strstr(tmp, category))) {
	       if(p[-2] == '[' && p [-1] == '[') {
		  char *e = strstr(p, "]]");
		  if(e && e <= content + len) {
		     char *cat = strndup(p, e - p);
		     if(cat) {
			char *pipe = strstr(cat, "|");
			if(pipe)
			   *pipe = '\0';
//			fprintf(pageincat, "%d\t%d\t%s\n", wd->id, id, cat);
			free(cat);
		     }
		  }
	       }
	       tmp = p + 1;
	       if(tmp >= content + len)
		  break;
	    }
	 }

	 wikidump_namespace_page_add(wd->namespaces, nsid);
	 asprintf(&buf, "INFO: %d\t%d\t%d\t%d\t%s\t%s\t%d\n", 
	       wd->id, id, rev_id, cid, modif, title, nsid);
	 if(buf) {
	    printf("%s", buf);
	    free(buf);
	 } else {
	    break;
	 }

	 wikidump_page_free(wp);

	 if(title)
	    free(title);
	 if(modif)
	    free(modif);
	 if(comment)
	    free(comment);
	 if(content)
	    free(content);
      } else {
	 break;
      }
   }

   if(wd->namespaces)
      eina_hash_foreach(wd->namespaces, wikidump_namespaces_print, NULL);
}

int main(int argc, char **argv) {
   Wiki_Dump *wd = NULL;
   const char filename[] = "../frwikiversity-20090815-pages-meta-current.xml.bz2";
//   const char filename[] = "../frwiki-20090828-pages-meta-current.xml.bz2";
   const char *file;

   eina_init();

   if(argc > 1) {
      file = argv[1];
   } else {
      file = filename;
   }

   wd = wikidump_open_filename_new(file);
   if(wd) {
      get_pages(wd, file);
      wikidump_del(wd);
   }

   eina_shutdown();

   return EXIT_SUCCESS;
}
#endif
