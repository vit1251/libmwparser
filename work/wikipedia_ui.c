#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlsave.h>

#include "parser.h"
#include "util.h"
#include "entities.h"
#include "pcre_replace.h"
#include "wikipedia_ui.h"

static xmlNodePtr _wiki_parser_html_page_default_get(Wiki_Parser *wp); 
char *html_xmlnode_to_text(Wiki_Parser *wp, xmlNodePtr node, xmlSaveOption opt);

static void 
make_category_tree_table(Wiki_Parser *wp, Eina_List *list, const char *divtitle, const char *divid)
{
   Eina_List *l = NULL;
   char *title = NULL;

   if(! wp) return;
   if(! wp->wd) return;
   if(! wp->wd->category_pages_get) return;

   if(wp->cat_done) return;

   PARSE_TRACE();

   if(list) {
      int i, j;
      int cnt = eina_list_count(list);
      div_t d = div(cnt, 3);
      int nrow = d.quot;
      xmlNodePtr div = NULL, table = NULL;

      div = xmlNewChild(wp->node, NULL, XML_CONST_CHAR("div"), NULL) ;
      wiki_parser_node_prop_set(div, "id", divid);

      if(divtitle) {
	 xmlNodePtr h2 = xmlNewChild(div, NULL, XML_CONST_CHAR("h2"), NULL) ;
	 wiki_parser_text_utf8_add(h2, divtitle, strlen(divtitle), EINA_TRUE);
      }

      table = xmlNewChild(div, NULL, XML_CONST_CHAR("table"), NULL) ;
      xmlSetProp(table, XML_CONST_CHAR("style"), XML_CONST_CHAR("width: 90%; align: center;"));
      if(remainder(cnt,3)) nrow++;

      ulog("CAT TABLE DEF: %d %d %d\n", eina_list_count(l) , d.quot, remainder(cnt, 3));
      
      for (i = 0; i < nrow; i++) {
         xmlNodePtr tr = xmlNewChild(table, NULL, (xmlChar *)"tr", NULL) ;
         for(j = 0; j < 3; j++) {
	    if(i + (nrow * j) < cnt) {
	       title = eina_list_nth(list, i + (nrow * j));
	    } else {
	       title = NULL;
	    }
	    if(title) {
	       UChar *t = u_strdupC(title);
	       char *url = NULL;
	       int exists = 1;

	       xmlNodePtr n = xmlNewChild(tr, NULL, (xmlChar *)"td", NULL);
               xmlSetProp(n, (xmlChar *)"style", (xmlChar *)"width: 30%;");
	       xmlNodePtr a = xmlNewChild(n, NULL, (xmlChar *)"a", NULL);

	       url = wiki_parser_url_internal_get(wp, WIKI_NS_MAIN, title, wp->wd->output_mode, &exists);
	       xmlSetProp(a, XML_CONST_CHAR("href"), XML_CHAR(url));
               wiki_parser_text_add(a, t, u_strlen(t), EINA_TRUE);
	       if(url)
		  free(url);
	       free(t);
	    } else {
	       UChar *t = u_strdupC("&nbsp;");
	       xmlNodePtr n = xmlNewChild(tr, NULL, XML_CONST_CHAR("td"), NULL);
               wiki_parser_text_add(n, t, u_strlen(t), EINA_TRUE);
	       free(t);
	    }
	 }
      }
   }
}

static void make_category_tree(Wiki_Parser *wp)
{
   Eina_List *list = NULL, *l = NULL;
   char *title = NULL;

   if(! wp) return;
   if(! wp->wd) return;
   if(! wp->wd->category_pages_get) return;

   if(wp->cat_done) return;

   PARSE_TRACE();

   list = wp->wd->category_pages_get(wp->wd, wiki_parser_fullpagename_get(wp));
   if(list) {
      Eina_Bool display = EINA_FALSE;
      const char *cat = wiki_parser_namespace_name_get(wp->wd, WIKI_NS_CATEGORY);
      const char *p = NULL;
      if(cat) {
	 Eina_List *sub = NULL, *pages = NULL;
	 EINA_LIST_FOREACH(list, l, title) {
	    if(( p = strstr(title, cat)) && p == title) {
	       sub = eina_list_append(sub, title);
	    } else
	       pages = eina_list_append(pages, title);
	 }

	 if(sub) {
	    make_category_tree_table(wp, sub, "Sub categories", "mw-subcategories");
	    EINA_LIST_FREE(sub, l);
	    display = EINA_TRUE;
	 }
	 if(pages) {
	    make_category_tree_table(wp, pages, "Pages", "mw-pages");
	    EINA_LIST_FREE(pages, l);
	    display = EINA_TRUE;
	 } 

      }
      if(display == EINA_FALSE)
	 make_category_tree_table(wp, list, "Pages", "mw-pages");

      wp->cat_done = 1;
      EINA_LIST_FREE(list, title) {
	 free(title);
      };
   }
}

static void make_search_tree(Wiki_Parser *wp, const char *search)
{
   Eina_List *list = NULL;
   char *title = NULL;

   if(! wp) return;
   if(! wp->wd) return;
   if(! wp->wd->search_pages_get) return;

   PARSE_TRACE();

   list = wp->wd->search_pages_get(wp->wd, search);
   if(list) {
      make_category_tree_table(wp, list, "Search Result", "mw-pages");

      EINA_LIST_FREE(list, title) {
	 free(title);
      };
   }
}

static xmlNodePtr html_reference_name_get(Wiki_Parser *wp, const char *name)
{
   xmlXPathContextPtr ctx;
   xmlXPathObjectPtr obj;
   xmlNodePtr li = NULL, sup = NULL;
   char search[] = "//sup[@name='%s']";
   char *tmp = NULL, *esc = NULL;

   if(! wp) return li;
   if(! wp->xml) return li;
   if(! name) return li;

   xmlXPathInit();

   ctx = xmlXPathNewContext(wp->xml);
   if(! ctx) return li;

   esc = strdup(name);
   if(esc) {
      esc = pcre_replace("/'/", "&#x27;", esc);
      if(esc) {
	 asprintf(&tmp, search, esc);
	 free(esc);
      }
   }

   if(! tmp)
      asprintf(&tmp, search, name);
   if(tmp) {
      obj = xmlXPathEvalExpression(XML_CHAR(tmp), ctx);
      free(tmp);
   }

   if(obj) {
      xmlNodeSetPtr nodes = obj->nodesetval;
      if(nodes) {
	 int i = 0; 
	 for(i = 0; i < nodes->nodeNr; i++) {
	    if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
	       if(! sup) 
		  sup = nodes->nodeTab[i];
	       else {
		  xmlNodePtr node = nodes->nodeTab[i];
		  xmlUnlinkNode(node);
		  xmlFreeNode(node);
	       }
	    }
	 }
      }
      xmlXPathFreeObject(obj);
   }

   xmlXPathFreeContext(ctx);

   if(sup)
      li = sup->parent;

   return li;
}

static void html_reference_list_build(Wiki_Parser *wp)
{
   xmlXPathContextPtr ctx;
   xmlXPathObjectPtr obj;
   xmlNodePtr ref = NULL, div = NULL, ol = NULL;

   if(! wp) return;
   if(! wp->xml) return;

   xmlXPathInit();

   ctx = xmlXPathNewContext(wp->xml);
   if(! ctx) return;

   obj = xmlXPathEvalExpression(XML_CONST_CHAR("//references"), ctx);
   if(obj) {
      xmlNodeSetPtr nodes = obj->nodesetval;
      if(nodes) {
	 int i = 0; 
	 for(i = nodes->nodeNr - 1; i >= 0; i--) {
	    if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
	       if(! ref) 
		  ref = nodes->nodeTab[i];
	       else {
		  xmlNodePtr node = nodes->nodeTab[i];
		  xmlUnlinkNode(node);
		  xmlFreeNode(node);
	       }
	    }
	 }
      }
      xmlXPathFreeObject(obj);
      obj = NULL;
   }

   if(ref) {
      div = xmlNewNode(NULL, XML_CONST_CHAR("div"));
      if(div) {
	 xmlSetProp(div, XML_CONST_CHAR("style"), XML_CONST_CHAR("font-size: 85%"));
	 ol = xmlNewChild(div, NULL, XML_CONST_CHAR("ol"), NULL);
	 if(ol)
	    xmlSetProp(ol, XML_CONST_CHAR("class"), XML_CONST_CHAR("references"));
	 xmlReplaceNode(ref, div);
	 xmlFreeNode(ref);
      }
   }

   obj = xmlXPathEvalExpression(XML_CONST_CHAR("//ref"), ctx);
   if(obj) {
      xmlNodeSetPtr nodes = obj->nodesetval;
      if(nodes && ol) {
	 int i = 0, cnt = 0; 
	 for(i = 0; i < nodes->nodeNr; i++) {
	    if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
	       xmlNodePtr node = nodes->nodeTab[i];
	       xmlNodePtr sup = NULL, li = NULL;
	       char *name = NULL;
	       char *note = NULL;
	       Eina_Bool is_multi = EINA_FALSE;

	       if(xmlHasProp(node, XML_CONST_CHAR("name"))) {
		  name = (char *) xmlGetProp(node, XML_CONST_CHAR("name"));
		  ulog("REFERENCE: %s\n", name);
		  if(name) {
		     name = pcre_replace("/( |\\/)/", "", name);
		     name = strtolower(name);
		  }
		  li = html_reference_name_get(wp, name);
	       }

	       if(li) 
		  is_multi = EINA_TRUE;
	       else {
     		  li = xmlNewChild(ol, NULL, XML_CONST_CHAR("li"), NULL);
		  cnt++;
	       }

	       if(li) {
		  xmlNodePtr a = NULL;
		  if(is_multi == EINA_TRUE && li->children) {
		     sup = li->children;
		     while(sup && strcmp((char *) sup->name, "sup") != 0)
			sup = sup->next;
		  } else {
		     sup = xmlNewChild(li, NULL, XML_CONST_CHAR("sup"), NULL);
		     wiki_parser_node_prop_set(sup, "name", name);
		  }

		  if(sup)
		     a = xmlNewChild(sup, NULL, XML_CONST_CHAR("a"), NULL);

		  if(a) {
		     char *anchor = NULL;
		     if(name)
			asprintf(&anchor, "#cite_ref-%s-%d", name, i);
		     else
			asprintf(&anchor, "#cite_ref-%d", i);
		     if(anchor) {
			wiki_parser_node_prop_set(a, "href", anchor);
			free(anchor);
		     }
		     if(is_multi == EINA_TRUE) {
			xmlNodePtr n = sup->children;
			char *la = strdup("a, ");
			if(! sup->prev) {
			   xmlNodePtr back = xmlNewNode(NULL, XML_CONST_CHAR("span"));
			   if(back) {
			      wiki_parser_text_utf8_add(back, "↑ ", strlen("↑ "), EINA_TRUE);
			      xmlAddPrevSibling(sup, back);
			   }
			}
			if(la) {
			   char c = 'a';
			   while(n && c <= 'z') {
			      if(! n->next) {
				 la[1] = ' ';
				 la[2] = '\0';
			      }
			      xmlNodeSetContent(n, XML_CHAR(la));
			      c++;
			      la[0] = c;
			      n = n->next;
			   }
			   free(la);
			}
		     } else {
			wiki_parser_text_utf8_add(a, "↑ ", strlen("↑ "), EINA_TRUE);
		     }
		  }
		  
		  if(! xmlHasProp(li, XML_CONST_CHAR("id"))) {
		     asprintf(&note, "cite_note-%d", cnt);
		     if(note) {
			wiki_parser_node_prop_set(li, "id", note);
		     }
		  } else {
	     	     note = (char *) xmlGetProp(li, XML_CONST_CHAR("id"));
	     	     xmlSetProp(li, XML_CONST_CHAR("id"), XML_CHAR(note));
		  }


		  if(node->children) {
		     xmlNodePtr n = node->children;
		     while(n) {
			xmlNodePtr next = n->next;
			xmlUnlinkNode(n);
			xmlAddChild(li, n);
			n = next;
		     }
		  } else {
		     xmlChar *content = xmlNodeGetContent(node);
		     xmlNodeAddContent(li, content);
		     free(content);
		  }
	       }

	       sup = xmlNewNode(NULL, XML_CONST_CHAR("sup"));
	       if(sup) {
		  char *id = NULL;
		  if(name)
		     asprintf(&id, "cite_ref-%s-%d", name, i);
		  else
		     asprintf(&id, "cite_ref-%d", i);
		  if(id) {
		     wiki_parser_node_prop_set(sup, "id", id);
		     free(id);
		     id = NULL;
		  }
		  wiki_parser_node_prop_set(sup, "class", "reference");
		  xmlNodePtr a = xmlNewChild(sup, NULL, XML_CONST_CHAR("a"), NULL);
		  if(a) {
		     char *label = NULL;
		     int pos = 0;
		     xmlNodePtr p = li;
		     while(p) {
			pos--;
			p = p->prev;
		     }
   		     asprintf(&label, "[%d]", abs(pos));
		     if(label) {
			wiki_parser_text_utf8_add(a, label, strlen(label), EINA_TRUE);
			free(label);
		     }
		     if(note) {
			asprintf(&id, "#%s", note);
			if(id) {
			   wiki_parser_node_prop_set(a, "href", id);
			   free(id);
			}
		     }
		  }
	       }

	       if(name) 
		  free(name);

	       if(note)
		  free(note);

	       xmlReplaceNode(node, sup);
	       xmlFreeNode(node);
	    }
	 }
      }
      xmlXPathFreeObject(obj);
   } else {
      ulog("REF: failed to eval xpath expr\n");
   }

   xmlXPathFreeContext(ctx);

}

static void html_toc_build(Wiki_Parser *wp)
{
   xmlXPathContextPtr ctx;
   xmlXPathObjectPtr obj;
   xmlNodePtr toc = NULL;
   const char *pagename = NULL;
   UChar *content = NULL;

   if(! wp) return;
   if(! wp->xml) return;

   xmlXPathInit();

   ctx = xmlXPathNewContext(wp->xml);
   if(! ctx) return;

   obj = xmlXPathEvalExpression(XML_CONST_CHAR("//tableofcontent"), ctx);
   if(obj) {
      xmlNodeSetPtr nodes = obj->nodesetval;
      if(nodes) {
	 int i = 0; 
	 for(i = 0; i < nodes->nodeNr; i++) {
	    if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
	       if(! toc) 
		  toc = nodes->nodeTab[i];
	       else {
		  xmlNodePtr node = nodes->nodeTab[i];
		  xmlUnlinkNode(node);
		  xmlFreeNode(node);
	       }
	    }
	 }
      }
      xmlXPathFreeObject(obj);
   }

   if(toc) {
      const char table[] = "{| id=\"toc\" class=\"toc\"\n|\n<div id=\"toctitle\"><h2>Contents</h2></div>\n";
      content = calloc(strlen(table) + 10, sizeof(UChar));
      u_sprintf(content, "%s", table);
   }

   pagename = wiki_parser_fullpagename_get(wp);
   obj = xmlXPathEvalExpression(XML_CONST_CHAR("//h1 | //h2 | //h3 | //h4 | //h5 | //h6"), ctx);
   if(obj) {
      xmlNodeSetPtr nodes = obj->nodesetval;
      if(nodes) {
	 int i = 0; 
	 if(nodes->nodeNr < 4) {
	    if(toc && content) {
	       free(content);
	       content = NULL;
	       xmlUnlinkNode(toc);
	       xmlFreeNode(toc);
	       toc = NULL;
	    }
	 }
	 for(i = 0; i < nodes->nodeNr; i++) {
	    if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
	       xmlNodePtr node = nodes->nodeTab[i];
	       int h = atoi((char *)node->name + 1);
	       char *title = (char *) xmlNodeGetContent(node);
	       title = pcre_replace("/\n/", " ", title);
	       if(title && strlen(title) > 255)
		  title[255] = '\0';
	       char *enc = wiki_parser_anchor_encode(title);

	       if(enc) {
		  wiki_parser_node_prop_set(node, "id", enc);
	       }

	       if(enc && toc && h > 1) {
		  UChar *tmp = calloc(h + strlen(enc) + strlen(title) + 30, sizeof(UChar));
		  if(tmp) {
		     int d = h - 1;
		     while(d > 0) {
			tmp[d - 1] = '#';
			d--;
		     }
		     u_sprintf(tmp + (h - 1), " [[#%s|<nowiki>%s</nowiki>]]\n", enc, title);
		     // uprintf("BUILD TITLE: h = %d, %S\n", h, tmp);
		     if(content == NULL) {
			content = u_strdup(tmp);
		     } else {
			UChar *buf= calloc(u_strlen(content) + u_strlen(tmp) + 10, sizeof(UChar));
			if(buf) {
			   u_sprintf(buf, "%S%S", content, tmp);
			   free(content);
			   content = buf;
			}
		     }
		     free(tmp);
		  }
	       }

	       if(title)
		  free(title);

	       if(enc)
		  free(enc);
	    }
	 }
      }
      xmlXPathFreeObject(obj);
   } else {
      ulog("TOC: failed to eval xpath expr\n");
   }

   if(content) {
      Wiki_Parser *w = NULL;
      int error = 0;
      content = icu_replace_utf8("$", "\n|}\n", content, 0);
//      w = wiki_parser_buf_new(wp->wd, content, u_strlen(content), &error);
      w = wiki_parser_buf_new(wp->wd, NULL, 0, &error);
      if(w) {
	 w->buf = u_strdup(content);
	 w->size = u_strlen(content);
	 xmlNodePtr root = wiki_parser_convert_xhtml(w, &error, 0);
	 if(root) {
	    xmlNodePtr n = root->children;
	    if(n) {
	       xmlUnlinkNode(n);
	       xmlReplaceNode(toc, n);
	       xmlFreeNode(toc);
	    }
	 }
	 wiki_parser_free(w);
      }
      free(content);
   }

   xmlXPathFreeContext(ctx);
}

static void
pagenames_show(Wiki_Parser *wp, xmlNodePtr parent)
{
   UChar *content = NULL;

   if(! wp) return;
   if(! parent) return;

   content = u_strdupC("namespace: {{NAMESPACE}}, pagename: {{PAGENAME}}, basepagename: {{BASEPAGENAME}}, fullpagename: {{FULLPAGENAME}}\n");
   if(content) {
      Wiki_Parser *w = NULL;
      int error = 0;

      w = wiki_parser_buf_new(wp->wd, content, u_strlen(content), &error);
      if(w) {
	 xmlNodePtr root = wiki_parser_convert_xhtml(w, &error, 0);
	 if(root) {
	    xmlNodePtr n = root->children;
	    while(n) {
	       xmlUnlinkNode(n);
	       xmlAddChild(parent, n);
	       n = root->children;
	    }
	 }
	 wiki_parser_free(w);
      }
      free(content);
   }
}

static xmlNodePtr html_portlet_list_add(Wiki_Parser *wp, const char *id, const char *title, xmlNodePtr parent)
{
   xmlNodePtr portlet, div, h5, ul;

   if(! wp) return NULL;
   if(! wp->wd) return NULL;
   if(! wp->node) return NULL;
   if(! id) return NULL;
   
   portlet = xmlNewChild(parent, NULL, XML_CONST_CHAR("div"), NULL);
   if(! portlet) return NULL;

   xmlSetProp(portlet, XML_CONST_CHAR("class"), XML_CONST_CHAR("portlet"));
   xmlSetProp(portlet, XML_CONST_CHAR("id"), XML_CONST_CHAR(id));

   if(title) {
      h5 = xmlNewChild(portlet, NULL, XML_CONST_CHAR("h5"), NULL);
      if(h5) {
	 xmlSetProp(h5, XML_CONST_CHAR("lang"), XML_CONST_CHAR(wp->wd->lang));
	 xmlSetProp(h5, XML_CONST_CHAR("xml:lang"), XML_CONST_CHAR(wp->wd->lang));
	 wiki_parser_text_utf8_add(h5, title, strlen(title), EINA_TRUE);
      }
   }

   div = xmlNewChild(portlet, NULL, XML_CONST_CHAR("div"), NULL);
   if(!div) return NULL;
   xmlSetProp(div, XML_CONST_CHAR("class"), XML_CONST_CHAR("pBody"));

   ul = xmlNewChild(div, NULL, XML_CONST_CHAR("ul"), NULL);

   return ul;
}

static xmlNodePtr html_head_default_get(Wiki_Parser *wp, xmlNodePtr content)
{
   xmlNodePtr html = NULL, head = NULL, link = NULL, title = NULL;
   const Wiki_Lang *wl = NULL;
   int i;

   if(! wp) return html;
   if(! wp->wd) return html;
   
   html = xmlNewNode(NULL, XML_CONST_CHAR("html"));
   if(! html) return html;

   head = xmlNewChild(html, NULL, XML_CONST_CHAR("head"), NULL);
   if(! head) return html;

   title = xmlNewChild(head, NULL, XML_CONST_CHAR("title"), NULL);
   if(title) {
      wiki_parser_text_utf8_add(title, wiki_parser_fullpagename_get(wp), 
	    strlen(wiki_parser_fullpagename_get(wp)), EINA_TRUE);
   }

   title = xmlNewChild(head, NULL, XML_CONST_CHAR("meta"), NULL);
   if(title) {
      xmlSetProp(title, XML_CONST_CHAR("http-equiv"), XML_CONST_CHAR("Content-Type"));
      xmlSetProp(title, XML_CONST_CHAR("content"), XML_CONST_CHAR("text/html; charset=utf-8"));
   }
   for(i = 0; i < 4; i++) {
      link = xmlNewChild(head, NULL, XML_CONST_CHAR("link"), NULL);
      if(link) {
	 xmlSetProp(link, XML_CONST_CHAR("rel"), XML_CONST_CHAR("stylesheet"));
	 xmlSetProp(link, XML_CONST_CHAR("type"), XML_CONST_CHAR("text/css"));
	 xmlSetProp(link, XML_CONST_CHAR("media"), XML_CONST_CHAR("all"));
	 if(i == 0) 
	    xmlSetProp(link, XML_CONST_CHAR("href"), XML_CONST_CHAR("/static/shared.css"));
	 else if(i == 1) 
	    xmlSetProp(link, XML_CONST_CHAR("href"), XML_CONST_CHAR("/static/main.css"));
	 else if(i == 2) {
	    char *url = NULL;
	    int exists = 0;
	    url = wiki_parser_url_internal_get(wp, WIKI_NS_MEDIAWIKI, "Common.css", 1, &exists);
	    xmlSetProp(link, XML_CONST_CHAR("href"), XML_CONST_CHAR(url));
	    if(url) 
	       free(url);
	 } else if(i == 3) {
	    char *url = NULL;
	    int exists = 0;
	    url = wiki_parser_url_internal_get(wp, WIKI_NS_MEDIAWIKI, "Monobook.css", 1, &exists);
	    xmlSetProp(link, XML_CONST_CHAR("href"), XML_CONST_CHAR(url));
	    if(url) 
	       free(url);
	 }

      }
   }

   xmlSetProp(html, XML_CONST_CHAR("xmlns"), XML_CONST_CHAR("http://www.w3.org/1999/xhtml"));
   xmlSetProp(html, XML_CONST_CHAR("xml:lang"), XML_CONST_CHAR(wp->wd->lang));
   xmlSetProp(html, XML_CONST_CHAR("lang"), XML_CONST_CHAR(wp->wd->lang));

   wl = wiki_parser_language_infos_get(wp->wd->lang);
   if(wl)
      xmlSetProp(html, XML_CONST_CHAR("dir"), XML_CONST_CHAR(wl->dir));

   if(content)
      xmlAddChild(html, content);

   return html;
}

static xmlNodePtr html_body_default_get(Wiki_Parser *wp, xmlNodePtr content)
{
   xmlNodePtr body = NULL, glob, div, node, bcontent;
   const Wiki_Lang *wl = NULL;
   char *classnames = NULL;
   char *pagename = NULL;
   char *origin = NULL;

   if(! wp) return body;
   if(! wp->wd) return body;
   
   body = xmlNewNode(NULL, XML_CONST_CHAR("body"));
   if(! body) return body;

   pagename = (char *) wiki_parser_fullpagename_get(wp);
   if(pagename) {
      pagename = strdup(wiki_parser_fullpagename_get(wp));
      pagename = pcre_replace("/( |\\/)/", "_", pagename);
      pagename = pcre_replace("/:/", "-", pagename);
   }

   wl = wiki_parser_language_infos_get(wp->wd->lang);

   asprintf(&classnames, 
	 "mediawiki %s ns-%d ns-subject page-%s skin-monobook", 
	 ((wl) ? wl->dir : ""), wp->wd->ns, pagename);
   xmlSetProp(body, XML_CONST_CHAR("class"), XML_CONST_CHAR(classnames));

   if(classnames)
      free(classnames);
   if(pagename)
      free(pagename);

   glob = xmlNewChild(body, NULL, XML_CONST_CHAR("div"), NULL);
   if(! glob) return body;
   xmlSetProp(glob, XML_CONST_CHAR("id"), XML_CONST_CHAR("globalWrapper"));

   node = xmlNewChild(glob, NULL, XML_CONST_CHAR("div"), NULL);
   if(! node) return body;
   xmlSetProp(node, XML_CONST_CHAR("id"), XML_CONST_CHAR("column-content"));

   div = xmlNewChild(node, NULL, XML_CONST_CHAR("div"), NULL);
   if(! div) return body;
   xmlSetProp(div , XML_CONST_CHAR("id"), XML_CONST_CHAR("content"));

   node = xmlNewChild(div, NULL, XML_CONST_CHAR("h1"), NULL);
   if(node) {
      xmlSetProp(node, XML_CONST_CHAR("class"), XML_CONST_CHAR("firstHeading"));
      xmlSetProp(node, XML_CONST_CHAR("id"), XML_CONST_CHAR("firstHeading"));
      wiki_parser_text_utf8_add(node, wiki_parser_fullpagename_get(wp),
	    strlen(wiki_parser_fullpagename_get(wp)), EINA_TRUE);
   }
   
   bcontent = xmlNewChild(div, NULL, XML_CONST_CHAR("div"), NULL);
   if(bcontent) {
      xmlSetProp(bcontent , XML_CONST_CHAR("id"), XML_CONST_CHAR("bodyContent"));
      
      if(content) {
	 if(strcmp(content->name, (const xmlChar *) "root") == 0) {
	    xmlNodePtr n = NULL;
	    while((n = content->children)) {
	       xmlUnlinkNode(n);
	       xmlAddChild(bcontent, n);
	    }
	 } else
	    xmlAddChild(bcontent, content);
      }
   }

   div = xmlNewChild(glob, NULL, XML_CONST_CHAR("div"), NULL);
   if(! div) return body;
   xmlSetProp(div, XML_CONST_CHAR("id"), XML_CONST_CHAR("column-one"));

   if(1 || wp->wd->have_search) {
      node = html_portlet_list_add(wp, "p-search", "Search", div);
      if(node) {
         xmlNodePtr f = NULL, i = NULL, g = NULL;
         f = xmlNewChild(node, NULL, XML_CONST_CHAR("form"), NULL);
         if(f) {
            int exists = 0;
            char *url = wiki_parser_url_internal_get(wp, WIKI_NS_MAIN, "", wp->wd->default_output_mode, &exists);
            if(url) {
               xmlSetProp(f, XML_CONST_CHAR("action"), XML_CHAR(url));
               free(url);
            }
            xmlSetProp(f, XML_CONST_CHAR("method"), XML_CONST_CHAR("get"));
            i = xmlNewChild(f, NULL, XML_CONST_CHAR("input"), NULL);
            if(i) {
               xmlSetProp(i, XML_CONST_CHAR("type"), XML_CONST_CHAR("hidden"));
               xmlSetProp(i, XML_CONST_CHAR("name"), XML_CONST_CHAR("wikimedia"));
               xmlSetProp(i, XML_CONST_CHAR("value"), XML_CONST_CHAR(wp->wd->sitename));
            }
            i = xmlNewChild(f, NULL, XML_CONST_CHAR("input"), NULL);
            if(i) {
               xmlSetProp(i, XML_CONST_CHAR("type"), XML_CONST_CHAR("hidden"));
               xmlSetProp(i, XML_CONST_CHAR("name"), XML_CONST_CHAR("lang"));
               xmlSetProp(i, XML_CONST_CHAR("value"), XML_CONST_CHAR(wp->wd->lang));
            }
            if(i) {
	       char *raw = NULL;
	       asprintf(&raw, "%d", wp->wd->output_mode);
               xmlSetProp(i, XML_CONST_CHAR("type"), XML_CONST_CHAR("hidden"));
               xmlSetProp(i, XML_CONST_CHAR("name"), XML_CONST_CHAR("raw"));
	       if(raw) {
		  xmlSetProp(i, XML_CONST_CHAR("value"), XML_CHAR(raw));
		  free(raw);
	       }
            }
            i = xmlNewChild(f, NULL, XML_CONST_CHAR("input"), NULL);
            if(i) {
               xmlSetProp(i, XML_CONST_CHAR("type"), XML_CONST_CHAR("text"));
               xmlSetProp(i, XML_CONST_CHAR("name"), XML_CONST_CHAR("search"));
               xmlSetProp(i, XML_CONST_CHAR("size"), XML_CONST_CHAR("15"));
               xmlSetProp(i, XML_CONST_CHAR("value"), XML_CONST_CHAR(""));
            }
            // g = xmlNewChild(f, NULL, XML_CONST_CHAR("input"), NULL);
            if(g) {
               xmlSetProp(g, XML_CONST_CHAR("type"), XML_CONST_CHAR("submit"));
               xmlSetProp(g, XML_CONST_CHAR("name"), XML_CONST_CHAR("go"));
               xmlSetProp(g, XML_CONST_CHAR("value"), XML_CONST_CHAR("go"));
            }
         }
      }
   }

   if(wp->wd->wikilist) {
      node = html_portlet_list_add(wp, "p-wmedia", "Wikimedias", div);
      if(node) {
	 Eina_List *l;
	 xmlNodePtr n, a;
	 Wiki_Media *w;
	 EINA_LIST_FOREACH(wp->wd->wikilist, l, w) {
	    n = xmlNewChild(node, NULL, XML_CONST_CHAR("li"), NULL);
	    if(n) {
	       a = xmlNewChild(n, NULL, XML_CONST_CHAR("a"), NULL);
	       if(a) {
		  xmlNodePtr b = NULL;
		  int exists = 0;
		  char *url = wiki_parser_url_interwiki_get(wp, "", w->sitename, w->lang, &exists);
		  if(url) {
		     xmlSetProp(a, XML_CONST_CHAR("href"), XML_CHAR(url));
		     free(url);
		  }
//		  ulog("%s, %s, %s, %s\n", w->sitename, wp->wd->sitename,
//			w->lang, wp->wd->lang);
		  if(wp->wd->lang && 
			strcmp(w->sitename, wp->wd->sitename) == 0 &&
			strcmp(w->lang, wp->wd->lang) == 0 ) {
		     b = wiki_parser_node_add(a, "b");
		     if(b) a = b;
		  }
		  wiki_parser_text_utf8_add(a, w->sitename, strlen(w->sitename), EINA_TRUE);
	       }
	       xmlSetProp(n, XML_CONST_CHAR("class"), XML_CONST_CHAR("interwiki"));
	    }
	 }

      }
   }

   if(wp->langs && eina_list_count(wp->langs) > 0) {
      node = html_portlet_list_add(wp, "p-lang", "Langues", div);
      if(node) {
	 Eina_List *l, *ln = NULL;
	 xmlNodePtr n, a;
	 EINA_LIST_FOREACH_SAFE(wp->langs, l, ln, a) {
	    n = xmlNewChild(node, NULL, XML_CONST_CHAR("li"), NULL);
	    if(n) {
	       xmlSetProp(n, XML_CONST_CHAR("class"), XML_CONST_CHAR("interwiki"));
	       xmlAddChild(n, a);
	    } else
	       xmlFreeNode(a);
	    wp->langs = eina_list_remove(wp->langs, a);
	 }
      }
   }

   node = html_portlet_list_add(wp, "p-cactions", "Affichages", div);
   if(node) {
      xmlNodePtr li, a;

      li = xmlNewChild(node, NULL, XML_CONST_CHAR("li"), NULL);
      if(li) {
	 a = xmlNewChild(li, NULL, XML_CONST_CHAR("a"), NULL);
	 if(a) {
	    char *url = NULL;
	    int exists = 0;

	    url = wiki_parser_url_internal_get(wp, (wp->wd->ns % 2 == 1) ? wp->wd->ns - 1 : wp->wd->ns, wp->wd->pagename, wp->wd->default_output_mode, &exists);
	    wiki_parser_text_utf8_add(a, "Page", 4, EINA_TRUE);
	    xmlSetProp(a, XML_CONST_CHAR("href"), XML_CONST_CHAR(url));
	    if(! exists)
	       xmlSetProp(a, XML_CONST_CHAR("class"), XML_CONST_CHAR("new"));
	    else if(wp->wd->output_mode == WIKI_OUTPUT_XML && wp->wd->ns % 2 == 0) 
	       xmlSetProp(li, XML_CONST_CHAR("class"), XML_CONST_CHAR("selected"));
	    if(url)
	       free(url);
	 }
      }


      li = xmlNewChild(node, NULL, XML_CONST_CHAR("li"), NULL);
      if(li) {
	 a = xmlNewChild(li, NULL, XML_CONST_CHAR("a"), NULL);
	 if(a) {
	    char *url = NULL;
	    int exists = 0;

	    url = wiki_parser_url_internal_get(wp, (wp->wd->ns % 2 == 1) ? wp->wd->ns : wp->wd->ns + 1, wp->wd->pagename, wp->wd->default_output_mode, &exists);
	    wiki_parser_text_utf8_add(a, "Discussion", 10, EINA_TRUE);
	    xmlSetProp(a, XML_CONST_CHAR("href"), XML_CONST_CHAR(url));
	    if(! exists) {
	       xmlSetProp(a, XML_CONST_CHAR("class"), XML_CONST_CHAR("new"));
	       xmlSetProp(a, XML_CONST_CHAR("style"), XML_CONST_CHAR("color: red !important;"));
	    } else if(wp->wd->output_mode == WIKI_OUTPUT_XML && wp->wd->ns % 2 == 1) {
	       xmlSetProp(li, XML_CONST_CHAR("class"), XML_CONST_CHAR("selected"));
	    }
	    if(url)
	       free(url);
	 }
      }

      li = xmlNewChild(node, NULL, XML_CONST_CHAR("li"), NULL);
      if(li) {
	 a = xmlNewChild(li, NULL, XML_CONST_CHAR("a"), NULL);
	 if(a) {
	    char *url = NULL;
	    int exists = 0;
	    url = wiki_parser_url_internal_get(wp, wp->wd->ns, wp->wd->pagename, 2, &exists);
	    xmlSetProp(a, XML_CONST_CHAR("href"), XML_CONST_CHAR(url));
	    wiki_parser_text_utf8_add(a, "view source", 11, EINA_TRUE);
	    if(wp->wd->output_mode == WIKI_OUTPUT_SOURCE) {
	       xmlSetProp(li, XML_CONST_CHAR("class"), XML_CONST_CHAR("selected"));
	    }
	    if(url)
	       free(url);
	 }
      }

      li = xmlNewChild(node, NULL, XML_CONST_CHAR("li"), NULL);
      if(li) {
	 a = xmlNewChild(li, NULL, XML_CONST_CHAR("a"), NULL);
	 if(a) {
	    char *url = NULL;
	    int exists = 0;
	    url = wiki_parser_url_internal_get(wp, wp->wd->ns, wp->wd->pagename, 3, &exists);
	    xmlSetProp(a, XML_CONST_CHAR("href"), XML_CONST_CHAR(url));
	    wiki_parser_text_utf8_add(a, "view source with include", 24, EINA_TRUE);
	    if(wp->wd->output_mode == WIKI_OUTPUT_SOURCE_TEMPLATE) {
	       xmlSetProp(li, XML_CONST_CHAR("class"), XML_CONST_CHAR("selected"));
	    }
	    if(url)
	       free(url);
	 }
      }

      li = xmlNewChild(node, NULL, XML_CONST_CHAR("li"), NULL);
      if(li) {
	 a = xmlNewChild(li, NULL, XML_CONST_CHAR("a"), NULL);
	 if(a) {
	    origin = wiki_parser_url_origin_get(wp, wp->wd->ns, wp->wd->pagename);
	    xmlSetProp(a, XML_CONST_CHAR("href"), XML_CONST_CHAR(origin));
	    wiki_parser_text_utf8_add(a, "Origin", 6, EINA_TRUE);
	 }
      }
   }

   node = xmlNewChild(glob, NULL, XML_CONST_CHAR("div"), NULL);
   if(node) {
      xmlSetProp(node , XML_CONST_CHAR("class"), XML_CONST_CHAR("visualClear"));
   }

   node = xmlNewChild(glob, NULL, XML_CONST_CHAR("div"), NULL);
   gettimeofday(&wp->time_xhtml, NULL);
   if(node) {
      struct timeval tv;
      char tmp[512];
      xmlSetProp(node , XML_CONST_CHAR("id"), XML_CONST_CHAR("footer"));

      div = xmlNewChild(node, NULL, XML_CONST_CHAR("p"), NULL);
      if(div) {
	 const char text[] = "This local page was last modified on [%s?oldid=%d %s] by [[%s:%s|%s]]";
	 char *time = NULL, *user = NULL, *tmp = NULL;
	 const char *ns = wiki_parser_namespace_name_get(wp->wd, WIKI_NS_USER);
	 unsigned int revid = 0;

	 xmlSetProp(div , XML_CONST_CHAR("style"), XML_CONST_CHAR("text-align: center;"));
	 if(wp->wd->page_revision_timestamp_get)
	    time = wp->wd->page_revision_timestamp_get(wp->wd);
	 if(wp->wd->page_contributor_username_get)
	    user = wp->wd->page_contributor_username_get(wp->wd);
	 if(wp->wd->page_revision_id_get)
	    revid = wp->wd->page_revision_id_get(wp->wd);

	 origin = pcre_replace("/ /", "_", origin);

	 asprintf(&tmp, text, origin, revid, time, ns, user, user);
	 if(tmp) {
	    UChar *buf = u_strdupC(tmp);
	    if(buf) {
	       int error = 0;
	       Wiki_Parser *w = wiki_parser_buf_new(wp->wd, buf, u_strlen(buf), &error);
	       if(w) {
		  xmlNodePtr r = wiki_parser_convert_xhtml(w, &error, 1);
		  if(r) {
		     while(r->children) {
			xmlNodePtr n = r->children;
			xmlUnlinkNode(n);
			xmlAddChild(div, n);
		     }
		  }
		  wiki_parser_free(w);
	       }
	       free(buf);
	    }

	    free(tmp);
	 }
	 if(time) free(time);
	 if(user) free(user);
      }

      div = xmlNewChild(node, NULL, XML_CONST_CHAR("div"), NULL);
      if(div) {
      	 timersub(&wp->time_parse, &wp->time_start, &tv);
	 snprintf(tmp, sizeof(tmp), "Parsing: %d.%06d, ", (int)tv.tv_sec, (int)tv.tv_usec);
	 wiki_parser_text_utf8_add(div, tmp, strlen(tmp), EINA_TRUE);
	 timersub(&wp->time_xhtml, &wp->time_parse, &tv);
	 snprintf(tmp, sizeof(tmp), "Rendering: %d.%06d, ", (int)tv.tv_sec, (int)tv.tv_usec);
	 wiki_parser_text_utf8_add(div, tmp, strlen(tmp), EINA_TRUE);
	 timersub(&wp->time_xhtml, &wp->time_start, &tv);
	 snprintf(tmp, sizeof(tmp), "Total: %d.%06d", (int)tv.tv_sec, (int)tv.tv_usec);
	 wiki_parser_text_utf8_add(div, tmp, strlen(tmp), EINA_TRUE);
	 snprintf(tmp, sizeof(tmp), ", Loading: %d.%06d", (int)wp->time_load.tv_sec, (int)wp->time_load.tv_usec);
	 wiki_parser_text_utf8_add(div, tmp, strlen(tmp), EINA_TRUE);
//	 pagenames_show(wp, div);
      }
   }

   if(origin)
      free(origin);

   return body;
}

static void html_catlink_add(Wiki_Parser *wp, xmlNodePtr parent)
{
   xmlNodePtr node, n;

   if(! wp) return;
   if(! parent) return;

   if(wp->categories && eina_list_count(wp->categories) > 0) {
      node = xmlNewChild(parent, NULL, XML_CONST_CHAR("div"), NULL);
      if(node) {
	 const char *ns = wiki_parser_namespace_name_get(wp->wd, WIKI_NS_CATEGORY);
	 Eina_List *l = NULL, *ln = NULL;
         Eina_Hash *uniq = NULL;
	 int i = 0;

	 xmlSetProp(node, XML_CONST_CHAR("class"), XML_CONST_CHAR("catlinks"));
	 xmlSetProp(node, XML_CONST_CHAR("id"), XML_CONST_CHAR("catlinks"));

	 if(ns) {
	    wiki_parser_text_utf8_add(node, ns, strlen(ns), EINA_TRUE);
	    wiki_parser_text_utf8_add(node, ": " , strlen(": "), EINA_TRUE);
	 }

         uniq = eina_hash_string_superfast_new(NULL);

	 EINA_LIST_FOREACH_SAFE(wp->categories, l, ln, n) {
            if(uniq) {
               char *c = (char *) xmlNodeGetContent(n);
               if(c) {
                  if(eina_hash_find(uniq, c)) {
                     free(c);
		     xmlFreeNode(n);
		     wp->categories = eina_list_remove(wp->categories, n);
                     continue;
                  }
                  eina_hash_add(uniq, c, "");
                  free(c);
               }
            }
	    if(i > 0)
	       wiki_parser_text_utf8_add(node, " | ", 3, EINA_TRUE);
	    xmlAddChild(node, n);
	    i++;
	    wp->categories = eina_list_remove(wp->categories, n);
	 }

         if(uniq)
            eina_hash_free(uniq);
	 wiki_parser_text_utf8_add(node, "&nbsp;", 6, EINA_TRUE);
      }
   }

   node = xmlNewChild(parent, NULL, XML_CONST_CHAR("div"), NULL);
   if(node) {
      xmlSetProp(node, XML_CONST_CHAR("class"), XML_CONST_CHAR("visualClear"));
   }
}

static char *_html_cleanup(Wiki_Parser *wp, char *content)
{
   if(! content) return NULL;
   if(! wp) return content;
   if(! wp->wd) return content;

   if(wp->wd->default_output_mode != WIKI_OUTPUT_DEFAULT)
      content = pcre_replace("/^<html/", "<?xml version=\"1.0\" ?>\n\
<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1 plus MathML 2.0//EN\"\n\
	\"http://www.w3.org/Math/DTD/mathml2/xhtml-math11-f.dtd\" [\n\
	<!ENTITY mathml \"http://www.w3.org/1998/Math/MathML\">\n\
]>\n\
<html ", content);
   else
      content = pcre_replace("/^<html/", "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<html", content);
   content = pcre_replace("/<pre[^>]*>[ \\n\\r\\t]*</pre>/", "", content);
   content = pcre_replace("/<pre[^\\/>]*\\/>/", "", content);
   /*
   content = pcre_replace("/&#(27|xA0);/", "", content);
   content = pcre_replace("/<(h1|h2|h3|h4|h5|h6)([^>]*)\\/>/", "<$1$2></$1>", content);
   */

   return content;
}

static char *html_cleanup(Wiki_Parser *wp, char *buf)
{
   UChar *content = NULL;

   return _html_cleanup(wp, buf);

   if(! buf) return NULL;
   if(! wp) return buf;
   if(! wp->wd) return buf;

   content = u_strdupC(buf);
   free(buf);
   buf = NULL;
   if(! content) return NULL;

   if(wp->wd->default_output_mode != WIKI_OUTPUT_DEFAULT)
      content = icu_replace_utf8("^<html", "<?xml version=\"1.0\" ?>\n\
<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1 plus MathML 2.0//EN\"\n\
	\"http://www.w3.org/Math/DTD/mathml2/xhtml-math11-f.dtd\" [\n\
	<!ENTITY mathml \"http://www.w3.org/1998/Math/MathML\">\n\
]>\n\
<html ", content, 0);
   else
      content = icu_replace_utf8("^<html", "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<html", content, 0);
   content = icu_replace("&#(27|xA0);", ICU_STR_EMPTY, content, 0);
   content = icu_replace("<pre[^>]*>[ \\n\\r\\t]*</pre>", ICU_STR_EMPTY, content, 0);
   content = icu_replace("<pre[^/>]*/>", ICU_STR_EMPTY, content, 0);
   content = icu_replace_utf8("<(h1|h2|h3|h4|h5|h6)([^>]*)/>", "<$1$2></$1>", content, 0);
//   content = icu_replace_utf8("<(a|script|div|span)([^>]*)/>", "<$1$2></$1>", content, 0);
//   content = icu_replace_utf8("<(table|td|tr|ref)([^>]*)/>", "<$1$2></$1>", content, 0);
//   content = icu_replace("<(abbr|small|strong|em|p|b|i|s|u|tt|center|pre|tr|sup|sub)[^>]*/>", ICU_STR_EMPTY, content, 0);
//   content = icu_replace("<pre>[\\n\\r\\t ]+</pre>", ICU_STR_EMPTY, content, 0);
//   content = icu_replace_utf8("(</[^>]*>)<", "$1\n<", content, 0);
//   content = icu_replace_utf8("><", ">\n<", content, 0);

   if(content) {
      buf = u_strToUTF8_new(content);
      free(content);
   }

   return buf;
}

char *html_xmlnode_to_text(Wiki_Parser *wp, xmlNodePtr node, xmlSaveOption opt)
{
   char *content = NULL;

   if(! wp) return content;
   if(! wp->xml) return content;
   if(! node) return content;

   xmlBufferPtr buf = xmlBufferCreate();

   if(buf) {
      xmlNodeDump(buf, wp->xml, node, opt, 0);
      content = strdup((char *) xmlBufferContent(buf));
      xmlBufferFree(buf);
   }
   
   return content;
}

static int template_loaded_sort_cb(const void *data1, const void *data2)
{
   const Wiki_Template_Loaded *wtl1 = data1, *wtl2 = data2;

   if(wtl1 && wtl2) {
      if(wtl1->title && wtl2->title) {
	 return u_strcmp(wtl1->title, wtl2->title);
      }
   }

   return 0;
}

static Eina_Bool template_loaded_hash_to_list_sorted(const Eina_Hash *h, const void *k, void *d, void *fd)
{
   Eina_List *l = *((Eina_List **)fd);
//   Eina_List *l = fd;

   if(l)
      l = eina_list_sorted_insert(l, template_loaded_sort_cb, d);
   else
      l = eina_list_append(l, d);

   ulog("LISTSORT: %d\n", eina_list_count(l));
   *((Eina_List **)fd) = l;
   return EINA_TRUE;
}

// static Eina_Bool html_template_list_loaded(const Eina_Hash *h, const void *k, void *d, void *fd)
static Eina_Bool html_template_list_loaded(Wiki_Parser *wp, Wiki_Template_Loaded *wtl)
{
//   const char *key = k;
   char *url = NULL, *title = NULL;
   int exists = 0;
//   Wiki_Template_Loaded *wtl = d;
//   Wiki_Parser *wp = fd;

   if(! wp) return 0;
   if(! wp->wd) return 0;
   if(! wp->node) return 0;
   if(! wtl) return 0;

   title = u_strToUTF8_new(wtl->title);
   if(! title) return 0;
   const char *key = title;

   wp->templates_num++;
   wp->templates_use_count += wtl->usage_count;

   if(index(key, ':')) {
      url = wiki_parser_url_internal_get(wp, WIKI_NS_MAIN, title, 2, &exists);
   } else if(key[0] == '/') {
      url = wiki_parser_url_internal_get(wp, WIKI_NS_MAIN, title, 2, &exists);
   } else {
      url = wiki_parser_url_internal_get(wp, WIKI_NS_MAIN, title, 2, &exists);
   }

   if(url) {
      xmlNodePtr tr, td, a;
      
      tr = xmlNewChild(wp->node, NULL, XML_CONST_CHAR("tr"), NULL);
      if(tr) 
	 td = xmlNewChild(tr, NULL, XML_CONST_CHAR("td"), NULL);
      if(td) {
	 a = xmlNewChild(td, NULL, XML_CONST_CHAR("a"), NULL);
	 if(a) {
	    xmlSetProp(a, XML_CONST_CHAR("href"), XML_CONST_CHAR(url));
	    xmlSetProp(a, XML_CONST_CHAR("title"), XML_CONST_CHAR(key));
	    if(! exists)
	       xmlSetProp(a, XML_CONST_CHAR("class"), XML_CONST_CHAR("new"));
	    wiki_parser_text_utf8_add(a, title, strlen(title), EINA_TRUE);
	 }
      }

      td = xmlNewChild(tr, NULL, XML_CONST_CHAR("td"), NULL);
      if(td) {
	 char t[50];
	 snprintf(t, sizeof(t), "%d", wtl->usage_count);
	 wiki_parser_text_utf8_add(td, t, strlen(t), EINA_TRUE);
      }

      td = xmlNewChild(tr, NULL, XML_CONST_CHAR("td"), NULL);
      if(td) {
	 char t[50];
	 snprintf(t, sizeof(t), "%d.%06d", (int) wtl->tv.tv_sec, (int) wtl->tv.tv_usec);
	 if(!wtl->redirect && (int) wtl->tv.tv_sec > 0) {
	    xmlSetProp(td, XML_CONST_CHAR("style"), XML_CONST_CHAR("color: red;"));
	 }
	 wiki_parser_text_utf8_add(td, t, strlen(t), EINA_TRUE);
      }

      free(url);
   }

   free(title);

   return 1;
}

static void html_template_list_add(Wiki_Parser *wp, xmlNodePtr parent)
{
   if(! wp) return;
   if(! wp->wd) return;
   if(! parent) return;

   if(wp->templates) {
      Eina_List *list = NULL, *l = NULL;
      xmlNodePtr h2, table, tr, th;
      char template[] = "Template";
      char duree[] = "Durée";
      char usage[] = "Usage";
      char empty[] = "&nbsp;";

      h2 = xmlNewChild(parent, NULL, XML_CONST_CHAR("h2"), NULL);
      if(! h2) return;
      wiki_parser_text_utf8_add(h2, template, sizeof(template), EINA_TRUE);

      table = xmlNewChild(parent, NULL, XML_CONST_CHAR("table"), NULL);
      if(! table) return;

      tr = xmlNewChild(table, NULL, XML_CONST_CHAR("tr"), NULL);
      if(! tr) return;

      th = xmlNewChild(tr, NULL, XML_CONST_CHAR("th"), NULL);
      if(th) {
	 wiki_parser_text_utf8_add(th, empty, sizeof(empty), EINA_TRUE);
      }

      th = xmlNewChild(tr, NULL, XML_CONST_CHAR("th"), NULL);
      if(th) {
	 wiki_parser_text_utf8_add(th, usage, sizeof(usage), EINA_TRUE);
      }

      th = xmlNewChild(tr, NULL, XML_CONST_CHAR("th"), NULL);
      if(th) {
	 wiki_parser_text_utf8_add(th, duree, sizeof(duree), EINA_TRUE);
      }

      wp->node = table;
//      eina_hash_foreach(wp->templates, html_template_list_loaded, wp);
      eina_hash_foreach(wp->templates, template_loaded_hash_to_list_sorted, &list);
      if(list) {
	 Wiki_Template_Loaded *wtl = NULL;
	 EINA_LIST_FOREACH(list, l, wtl) {
	    html_template_list_loaded(wp, wtl);
	 }
	 EINA_LIST_FREE(list, wtl);
      } 

      h2 = xmlNewChild(parent, NULL, XML_CONST_CHAR("p"), NULL);
      if(h2) {
	 char *t = NULL;
	 asprintf(&t, "%d templates used %d times and %d not found",
   	       wp->templates_num, wp->templates_use_count, wp->templates_not_found);
	 if(t) {
	    wiki_parser_text_utf8_add(h2, t, strlen(t), EINA_TRUE);
	    free(t);
	 }
      }
   }
}

static xmlNodePtr _wiki_parser_html_page_default_get(Wiki_Parser *wp) 
{
   int error = 0;
   xmlNodePtr html = NULL, body = NULL, node = NULL;

   if(! wp) return html;

   node = wiki_parser_convert_xhtml(wp, &error, 0);
   if(node) {
      xmlNodePtr n;
      
      if(wp->wd->ns == WIKI_NS_CATEGORY)
	 make_category_tree(wp);
      else if(wp->wd->search) {
         make_search_tree(wp, wp->wd->search);
      }

      html_reference_list_build(wp);

      html_toc_build(wp);

      html_catlink_add(wp, node);

      xmlUnlinkNode(node);
      body = html_body_default_get(wp, node);
      xmlDocSetRootElement(wp->xml, body); 
      xmlFreeNode(node);
      wp->root = NULL;

      xmlUnlinkNode(body);
      html = html_head_default_get(wp, body);

      if(html) {
	 wp->root = html;
	 wp->node = html;
	 xmlDocSetRootElement(wp->xml, html); 
      }
   }

   return html;
}

char *wiki_parser_html_page_default_get(Wiki_Parser *wp) 
{
   xmlNodePtr html = NULL;
   char *content = NULL;

   html = _wiki_parser_html_page_default_get(wp);
   content = html_xmlnode_to_text(wp, html, XML_SAVE_AS_HTML);

   if(content) {
      content = html_cleanup(wp, content);
   }

   return content;
}

char *wiki_parser_xml_page_default_get(Wiki_Parser *wp) 
{
   xmlNodePtr html = NULL;
   char *content = NULL;

   html = _wiki_parser_html_page_default_get(wp);
   content = html_xmlnode_to_text(wp, html, XML_SAVE_AS_XML);

   if(content) {
      content = html_cleanup(wp, content);
   }

   return content;
}

char *wiki_parser_html_page_source_get(Wiki_Parser *wp) 
{
   char *content = NULL;
   int error = 0;
   xmlNodePtr html = NULL, body = NULL, node = NULL;

   if(! wp) return content;

   node = wiki_parser_convert_xhtml(wp, &error, 0);
   if(node) {
      xmlNodePtr n, text, table, tr, td;

      xmlUnlinkNode(node);
      xmlFreeNode(node);

      n = xmlNewNode(NULL, XML_CONST_CHAR("div"));
      if(!n) return content;

      text = xmlNewChild(n, NULL, XML_CONST_CHAR("textarea"), NULL);
      if(!text) return content;

      table = xmlNewChild(n, NULL, XML_CONST_CHAR("table"), NULL);
      tr = xmlNewChild(table, NULL, XML_CONST_CHAR("tr"), NULL);
      td = xmlNewChild(tr, NULL, XML_CONST_CHAR("td"), NULL);
      html_template_list_add(wp, td);

#if 0
      td = xmlNewChild(tr, NULL, XML_CONST_CHAR("td"), NULL);
      html_extensions_list_add(wp, td);
#endif

      body = html_body_default_get(wp, n);
      html = html_head_default_get(wp, body);

      if(text) {
	 xmlSetProp(text, XML_CONST_CHAR("rows"), XML_CONST_CHAR("30"));
	 xmlSetProp(text, XML_CONST_CHAR("cols"), XML_CONST_CHAR("auto"));
	 xmlSetProp(text, XML_CONST_CHAR("class"), XML_CONST_CHAR("wikitable"));
	 wp->source = icu_replace_utf8("&", "&amp;", wp->source, 0);
	 wiki_parser_text_add(text, wp->source, u_strlen(wp->source), EINA_TRUE);
      }

      xmlDocSetRootElement(wp->xml, html);
      wp->root = html;
      content = html_xmlnode_to_text(wp, html, XML_SAVE_XHTML);
      content = html_cleanup(wp, content);
   }

   return content;
}

static Eina_Bool
_nowiki_content_restore(const Eina_Hash *h, const void *k, void *d, void *fd)
{
   Wiki_Parser *wp = fd;
   Wiki_Nowiki_Part *wnp = d;
   const UChar *key = k;
   UChar *p = NULL, *c = NULL, *tag = NULL;
   size_t s = 0, len = 0;

   if(! wp) return EINA_FALSE;
   if(! wp->buf) return EINA_FALSE;
   if(! wnp) return EINA_FALSE;
   if(! key) return EINA_FALSE;

   tag = u_strndupC(wnp->tagname, wnp->tagname_size);
   if(! tag) return EINA_FALSE;

   s = wnp->tagname_size + 3;
   if(wnp->args)
      s += wnp->args_size + 1;
   if(wnp->content) {
      s += wnp->content_size;
      s += wnp->tagname_size + 2;
   }

   c = calloc(s + 1, sizeof(UChar));
   if(c) {
      u_strncat(c, ICU_STR_ANGL_OPEN, 1);
      u_strncat(c, tag, wnp->tagname_size);
      if(wnp->args) {
	 u_strncat(c, ICU_STR_SPACE, 1);
	 u_strncat(c, wnp->args, wnp->args_size);
      }
      if(! wnp->content)
	 u_strncat(c, ICU_STR_SLASH, 1);
      u_strncat(c, ICU_STR_ANGL_CLOSE, 1);
      if(wnp->content) {
	 u_strncat(c, wnp->content, wnp->content_size);
	 u_strncat(c, ICU_STR_ANGL_OPEN, 1);
	 u_strncat(c, ICU_STR_SLASH, 1);
	 u_strncat(c, tag, wnp->tagname_size);
	 u_strncat(c, ICU_STR_ANGL_CLOSE, 1);
      }

//      uprintf("size: %d == %d => %S\n", s, u_strlen(c), c);

      len = u_strlen(key);
      while((p = u_strstr(wp->buf, key))) {
	 if(s > len) {
	    UChar *tmp = calloc(wp->size + (s - len) + 1, sizeof(UChar));
	    if(tmp) {
	       u_memcpy(tmp, wp->buf, p - wp->buf);
	       u_memcpy(tmp + (p - wp->buf), c, s);
	       u_memcpy(tmp + ((p - wp->buf) + s), p + len, wp->size - ((p + len) - wp->buf) + 1);
	       free(wp->buf);
	       wp->buf = tmp;
	       wp->size += (s - len);
	    } else {
	       u_memcpy(p, ICU_STR_SPACE, 1);
	    }
	 } else {
	    u_memcpy(p, c, s);
	    if(s < len) {
	       u_memmove(p, p + len, (wp->size - ((p + len) - wp->buf)) + 1);
	       wp->size -= (len - s);
	    }
	 }
      }
      free(c);
   }

   free(tag);

   return EINA_TRUE;
}

char *wiki_parser_html_page_template_included_get(Wiki_Parser *wp) 
{
   char *content = NULL;
   int error = 0;
   xmlNodePtr html = NULL, body = NULL, node = NULL;

   if(! wp) return content;

   node = wiki_parser_convert_xhtml(wp, &error, 0);
   if(node) {
      xmlNodePtr n, text;

      xmlUnlinkNode(node);
      xmlFreeNode(node);

      n = xmlNewNode(NULL, XML_CONST_CHAR("div"));
      if(!n) return content;

      text = xmlNewChild(n, NULL, XML_CONST_CHAR("textarea"), NULL);
      if(!text) return content;

      html_template_list_add(wp, n);

      body = html_body_default_get(wp, n);
      html = html_head_default_get(wp, body);

      if(text) {
	 xmlSetProp(text, XML_CONST_CHAR("rows"), XML_CONST_CHAR("30"));
	 if(wp->nowiki)
	    eina_hash_foreach(wp->nowiki, _nowiki_content_restore, wp);
	 wp->buf = icu_replace_utf8("&", "&amp;", wp->buf, 0);
	 wiki_parser_text_add(text, wp->buf, u_strlen(wp->buf), EINA_TRUE);
      }

      xmlDocSetRootElement(wp->xml, html);
      wp->root = html;
      content = html_xmlnode_to_text(wp, html, XML_SAVE_XHTML);
      content = html_cleanup(wp, content);
   }

   return content;
}

char *
wiki_parser_parsed_root_get(Wiki_Parser *wp)
{
   char *content = NULL;
   int error = 0;
   xmlNodePtr node = NULL;

   if(! wp) return content;

   node = wiki_parser_convert_xhtml(wp, &error, 0);
   content = html_xmlnode_to_text(wp, node, XML_SAVE_XHTML);

   return content;
}
