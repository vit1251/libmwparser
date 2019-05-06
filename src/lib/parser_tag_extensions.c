#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <string.h>
#include <stdio.h>

#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/ustdio.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "parser.h"
#include "parser_tag_extensions.h"
#include "parser_tag_extensions_private.h"

#include "itex2MML.h"
#include "pcre_replace.h"
#include "util.h"
#include "entities.h"

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

static const char *attrs[] = {
   "id", "class", "style", "colspan", "rowspan", "align", "width", "height",
   "title", "alt", "halign", "valign", "begin", "end", "border", "cellpadding",
   "cellspacing", "bgcolor", "lang", "scope", "name", "font", "color",
   "widths", "heights", "perrow", "caption", "padding", "rules", "summary",
   "index", "from", "to", "fromsection", "tosection", "header", "size", 
   "clear", "begin", "end", "dir", "headers", "nowrap",
   NULL
};
static Eina_Hash *html_attr_authorized = NULL;
static Eina_Hash *html_tags = NULL;

static xmlNodePtr wiki_parser_tag_extension_imagemap(const Wiki_Parser_Data *wd, xmlNodePtr tag)
{
   UChar *content = NULL, *link = NULL;
   xmlNodePtr div = NULL;

   if(tag) {
      char *tmp = (char *) xmlNodeGetContent(tag);
      if(tmp) {
	 content = u_strdupC(tmp);
	 free(tmp);
      }
      xmlFreeNode(tag);
   }

   if(content) {
      const UChar *s = NULL, *e = NULL;
      s = icu_search("[Ii]mage:", content, 0);
      if(s) {
	 e = icu_search("\\n", s, 0);
	 if(! e) 
	    e = content + u_strlen(content);
      }
      if(s && e) {
	 UChar *tmp = u_strndup(s, e - s);
	 if(tmp) {
	    link = calloc((e - s) + 5, sizeof(UChar));
	    u_sprintf(link, "[[%S]]", tmp);
	    free(tmp);
	 }
      }
      free(content);
   }

   ulog("IMAGE: %S\n", link);
   if(link) {
      int error = 0;
      Wiki_Parser *wp = NULL;
      div = xmlNewNode(NULL, XML_CONST_CHAR("div"));
      if(div)
	 wp = wiki_parser_buf_new(wd, link, u_strlen(link), &error);
      if(wp) {
	 xmlNodePtr root = wiki_parser_convert_xhtml(wp, &error, 1);
	 while(root->children) {
	    xmlNodePtr node = root->children;
	    xmlUnlinkNode(node);
	    xmlAddChild(div, node);
	 }
	 wiki_parser_free(wp);
      }
      free(link);
   }

   return div;
}

static void gallery_thumb_add(const Wiki_Parser_Data *wd, xmlNodePtr parent, const char *name, const char *label, int width)
{
   Wiki_Parser *wp = NULL;
   xmlNodePtr box, thumb, div, p, a, img;
   char *url = NULL, *n = NULL;
   char tmp[PATH_MAX];
   int wthumb = width + 30;
   int wbox = wthumb + 5;
   int padding = width / 3.63; /* TODO: get correct value */
   UChar *buf = NULL;

   ulog("%s: name = %s, label = %s\n", __FUNCTION__, name, label);
   if(! parent) return;
   if(! name) return;
   if(! wd) return;

   if(0 && (n = strstr(name, ":"))) 
      url = wiki_parser_url_image_get(wd, n + 1);
   else
      url = wiki_parser_url_image_get(wd, name);
   if(! url) return;
   
   url = wiki_parser_image_url_thumb_set(url, width);
   if(! url) goto RETURN;

   if(label) {
      buf = u_strdupC(label);
   } else {
      buf = u_strdupC(" ");
   }

   if(buf) {
      int error = 0;
      wp = wiki_parser_buf_new(wd, buf, u_strlen(buf), &error);
      wiki_parser_convert_xhtml(wp, &error, 1);
      free(buf);
   }

   if(! wp) goto RETURN;

   box = xmlNewChild(parent, NULL, XML_CONST_CHAR("div"), NULL);
   if(! box) goto RETURN;

   xmlSetProp(box, XML_CONST_CHAR("class"), XML_CONST_CHAR("gallerybox"));
   snprintf(tmp, PATH_MAX, "width: %dpx;", wbox);
   xmlSetProp(box, XML_CONST_CHAR("style"), XML_CHAR(tmp));

   thumb = xmlNewChild(box, NULL, XML_CONST_CHAR("div"), NULL);
   if(! thumb) goto RETURN;

   xmlSetProp(thumb, XML_CONST_CHAR("class"), XML_CONST_CHAR("thumb"));
   snprintf(tmp, PATH_MAX, "padding: %dpx 0; width: %dpx;", padding, wthumb);
   xmlSetProp(thumb, XML_CONST_CHAR("style"), XML_CHAR(tmp));

   div = xmlNewChild(thumb, NULL, XML_CONST_CHAR("div"), NULL);
   if(! div) goto RETURN;

   snprintf(tmp, PATH_MAX, "margin-left: auto; margin-right: auto; width: %dpx;", width);
   xmlSetProp(div, XML_CONST_CHAR("style"), XML_CHAR(tmp));

   a = xmlNewChild(div, NULL, XML_CONST_CHAR("a"), NULL);
   if(a) {
      char *link = NULL;
      if(wd->page_exists) {
	 int exists = 0;
	 link = wiki_parser_url_internal_get(wp, WIKI_NS_MAIN, name, 0, &exists);
	 if(! exists) {
	    if(link)
	       free(link);
	    char *tmp = strdup(name);
	    if(tmp) {
	       /* TODO: need interface */
	       tmp = pcre_replace("/^([^:]*?):/", "File:", tmp);
	       link = wiki_parser_url_interwiki_get(wp, tmp, "commons", "en", &exists);
	       free(tmp);
	    }
	 }
	 ulog("%s: name: %s, exists = %d, link = %s\n", __FUNCTION__, name, exists, link);
      }

      xmlSetProp(a, XML_CONST_CHAR("href"), XML_CONST_CHAR(link));
      xmlSetProp(a, XML_CONST_CHAR("class"), XML_CONST_CHAR("image"));

      if(link) 
	 free(link);

      img = xmlNewChild(a, NULL, XML_CONST_CHAR("img"), NULL);
      if(! img) goto RETURN;

      xmlSetProp(img, XML_CONST_CHAR("src"), XML_CHAR(url));
      xmlSetProp(img, XML_CONST_CHAR("alt"), XML_CHAR(name));
      snprintf(tmp, PATH_MAX, "%d", width);
      xmlSetProp(img, XML_CONST_CHAR("width"), XML_CHAR(tmp));
   }

   div = xmlNewChild(box, NULL, XML_CONST_CHAR("div"), NULL);
   if(! div) goto RETURN;

   xmlSetProp(div, XML_CONST_CHAR("class"), XML_CONST_CHAR("gallerytext"));

   if(label) {
      p = xmlNewChild(div, NULL, XML_CONST_CHAR("p"), NULL);
      if(p && wp->node) {
	 xmlNodePtr node;
	 while((node = wp->node->children)) {
	    xmlUnlinkNode(node);
	    xmlAddChild(p, node);
	 }
      }
   }

RETURN:
   if(wp)
      wiki_parser_free(wp);

   free(url);
}

static xmlNodePtr wiki_parser_tag_extension_gallery(const Wiki_Parser_Data *wd, xmlNodePtr tag)
{
   Wiki_Parser *wp = NULL;
   xmlNodePtr table = NULL, tr, td;
   char *content = NULL, *p = NULL, *s = NULL, *e = NULL, *name = NULL, *label = NULL;
   int perrow = 4, width = 120;
   int i = 0;

   if(! tag) {
      return NULL;
   }

   wp = wiki_parser_add(wd);
   if(! wp) goto FAILED;

   content = (char *) xmlNodeGetContent(tag);
   if(! content) goto FAILED;

   table = xmlNewNode(NULL, XML_CONST_CHAR("table"));
   if(!table) goto FAILED;
   
   xmlSetProp(table, XML_CONST_CHAR("class"), XML_CONST_CHAR("gallery"));
   xmlSetProp(table, XML_CONST_CHAR("cellpadding"), XML_CONST_CHAR("0"));
   xmlSetProp(table, XML_CONST_CHAR("cellspacing"), XML_CONST_CHAR("0"));

   if(xmlHasProp(tag, XML_CONST_CHAR("perrow"))) {
      char *text = (char *) xmlGetProp(tag, XML_CONST_CHAR("perrow"));
      if(text) {
	 int n = atoi(text);
	 if(n > 0 && n <= 20) 
	    perrow = n;
	 free(text);
      }
   }

   if(xmlHasProp(tag, XML_CONST_CHAR("widths"))) {
      char *text = (char *) xmlGetProp(tag, XML_CONST_CHAR("widths"));
      if(text) {
	 text = pcre_replace("/^([0-9]*)/", "\\1", text);
	 int n = atoi(text);
	 if(n > 0 && n < 201) 
	    width = n;
	 free(text);
      }
   }

   if(xmlHasProp(tag, XML_CONST_CHAR("heights")) 
	 && xmlHasProp(tag, XML_CONST_CHAR("widths"))) {
      char *text = (char *) xmlGetProp(tag, XML_CONST_CHAR("heights"));
      if(text) {
	 text = pcre_replace("/^([0-9]*)/", "\\1", text);
	 int n = atoi(text);
	 if(n > 0 && n < 201) 
	    width = n;
	 free(text);
      }
   }

   if(xmlHasProp(tag, XML_CONST_CHAR("caption"))) {
      char *text = (char *) xmlGetProp(tag, XML_CONST_CHAR("caption"));
      if(text) {
	 xmlNewChild(table, NULL, XML_CONST_CHAR("caption"), XML_CHAR(text));
	 free(text);
      }
   }

   tr = xmlNewChild(table, NULL, XML_CONST_CHAR("tr"), NULL);
   if(! tr) goto FAILED;
   
   p = s = content;
   e = content + strlen(content) + 1;
   while(p < e) {
      if(*p == '|') {
	 char *l = p + 1;
	 name = strndup(s, p - s);
	 while(p < e) {
	    if(*p == '\n' || *p == '\0') {
	       label = strndup(l, p - l);
	       s = p + 1;
	       break;
	    }
	    p++;
	 }
      } else if(*p == '\n') {
	 name = strndup(s, p - s);
	 s = p + 1;
      }

      if(name) {
	 UChar *tmp = u_strdupC(name);
	 if(tmp) {
	    Wiki_Title *wt = parser_title_part(wd, tmp);
	    free(tmp);
	    if(wt) {
	       tmp = parser_title_with_namespace(wt);
	       if(tmp) {
		  free(name);
		  name = u_strToUTF8_new(tmp);
		  free(tmp);
	       }
	       parser_title_free(wt);
	    }
	 }
	 /*
	 const char *nsname = wiki_parser_namespace_name_get(wd, WIKI_NS_FILE);
	 if(strncmp(name, "File:", 5) == 0 || strncmp(name, "Image:", 6) == 0
	       || strncmp(name, nsname, strlen(nsname) == 0)) {
	    name = pcre_replace("/^( |\n|\r|\t)*?/", "", name);
	    name = pcre_replace("/[[:space:]]+$/", "", name);
	 } else {
	    free(name); name = NULL;
	    if(label) {
	       free(label);
	       label = NULL;
	    }
	 }
	 if(name && nsname && strlen(name) > 0) {
	    char *ns = calloc(strlen(nsname) + 2, sizeof(char));
	    if(ns) {
	       sprintf(ns, "%s:", nsname);
	       if(strstr(name, ":"))
		  name = pcre_replace("/^([^:]*?):/", ns, name);
	       else
		  name = pcre_replace("/^/", ns, name);
	       free(ns);
	    }
	 }
	 */
      }

      if(name && strlen(name) > 0) {
	 td = xmlNewChild(tr, NULL, XML_CONST_CHAR("td"), NULL);
      	 if(td) {
	    gallery_thumb_add(wd, td, name, label, width);
	    i++;
	 }

	 free(name);
	 if(label)
	    free(label);
	 name = label = NULL;

	 if(i == perrow) {
	    i = 0;
	    tr = xmlNewChild(table, NULL, XML_CONST_CHAR("tr"), NULL);
	    if(! tr) goto FAILED;
	 }
      } else if(name) {
	 free(name);
	 if(label)
	    free(label);
	 name = label = NULL;
      }
      p++;
   }

FAILED:   
   if(content)
      free(content);

   if(wp)
      wiki_parser_free(wp);

   xmlFreeNode(tag);
   return table;
}

static xmlNodePtr wiki_parser_tag_extension_pages(const Wiki_Parser_Data *wd, xmlNodePtr tag)
{
   Wiki_Parser *wp = NULL;
   xmlNodePtr div = NULL;
   char *index = NULL, *content = NULL;
   char *fromsection = NULL, *tosection = NULL;
   int from = 0, to = 0, head = 0, max = 1000;

   div = xmlNewNode(NULL, XML_CONST_CHAR("div"));
   if(! div) return NULL;

   if(tag) {
      if(xmlHasProp(tag, XML_CONST_CHAR("index")))
	 index = (char *) xmlGetProp(tag, XML_CONST_CHAR("index"));

      if(xmlHasProp(tag, XML_CONST_CHAR("from"))) {
	 char *tmp = (char *) xmlGetProp(tag, XML_CONST_CHAR("from"));
	 if(tmp) {
	    from = atoi(tmp);
	    free(tmp);
	 }
      }

      if(xmlHasProp(tag, XML_CONST_CHAR("fromsection"))) {
	 fromsection = (char *) xmlGetProp(tag, XML_CONST_CHAR("fromsection"));
      }

      if(xmlHasProp(tag, XML_CONST_CHAR("to"))) {
	 char *tmp = (char *) xmlGetProp(tag, XML_CONST_CHAR("to"));
	 if(tmp) {
	    to = atoi(tmp);
	    free(tmp);
	 }
      }

      if(xmlHasProp(tag, XML_CONST_CHAR("tosection"))) {
	 tosection = (char *) xmlGetProp(tag, XML_CONST_CHAR("tosection"));
      }

      if(xmlHasProp(tag, XML_CONST_CHAR("header"))) {
	 char *tmp = (char *) xmlGetProp(tag, XML_CONST_CHAR("header"));
	 if(tmp) {
	    head = atoi(tmp);
	    free(tmp);
	 }
      }

      xmlFreeNode(tag);
   }

   if(to - from > max) to = from + max;

   if(index) {
      content = strdup("");
      while(from <= to) {
	 char *title = NULL;
	 if(fromsection) {
	    asprintf(&title, "<span style=\"display: none;\"><span class=\"pagenum\" id=\"%d\" title=\"%s\">[[Page:%s/%d|<nowiki>[%d]</nowiki>]]</span></span>\n{{#section: Page:%s/%d|%s}}\n", from, index, index, from, from, index, from, fromsection);
	    free(fromsection);
	    fromsection = NULL;
	 } else if(tosection && from == to) {
	    asprintf(&title, "<span style=\"display: none;\"><span class=\"pagenum\" id=\"%d\" title=\"%s\">[[Page:%s/%d|<nowiki>[%d]</nowiki>]]</span></span>\n{{#section: Page:%s/%d|%s}}\n", from, index, index, from, from, index, from, tosection);
	    
	 } else {
	    asprintf(&title, "<span style=\"display: none;\"><span class=\"pagenum\" id=\"%d\" title=\"%s\">[[Page:%s/%d|<nowiki>[%d]</nowiki>]]</span></span>\n{{Page:%s/%d}}\n", from, index, index, from, from, index, from);
	 }
	 if(title) {
	    content = pcre_replace("/$/", title, content);
	    free(title);
	    title = NULL;
	 } else 
	    from = to;

	 from++;
      }
      free(index);
   }

   if(content) {
      UChar *ucontent = u_strdupC(content);
      if(ucontent) {
	 int error = 0;
	 wp = wiki_parser_buf_new(wd, ucontent, u_strlen(ucontent), &error);
	 if(wp) {
	    xmlNodePtr root = wiki_parser_convert_xhtml(wp, &error, 1);
            if(root->children) {
               while(root->children) {
                  xmlNodePtr node = root->children;
                  xmlUnlinkNode(node);
                  xmlAddChild(div, node);
               }
            } 
	    wiki_parser_free(wp);
	 }
	 free(ucontent);
      }
      free(content);
   }

   if(fromsection)
      free(fromsection);
   if(tosection)
      free(tosection);

   return div;
}

static xmlNodePtr wiki_parser_tag_extension_content_only(const Wiki_Parser_Data *wd, xmlNodePtr tag)
{
   Wiki_Parser *wp = NULL;
   xmlNodePtr div = NULL;
   char *content = NULL;

   div = xmlNewNode(NULL, XML_CONST_CHAR("div"));

   if(div && tag) {
      content = (char *) xmlNodeGetContent(tag);
      if(content) {
	 UChar *utmp = u_strdupC(content);
	 if(utmp) {
	    int error = 0;
	    wp = wiki_parser_buf_new(wd, utmp, u_strlen(utmp), &error);
	    wiki_parser_convert_xhtml(wp, &error, 1);
	    free(utmp);
	 }
	 if(wp) {
	    if(wp->node) {
	       xmlNodePtr node;
	       while((node = wp->node->children)) {
		  xmlUnlinkNode(node);
		  xmlAddChild(div, node);
	       }
	    }
	    wiki_parser_free(wp);
	 }
	 free(content);
      }
      xmlFreeNode(tag);
   }

   return div;
}

static xmlNodePtr wiki_parser_tag_extension_poem(const Wiki_Parser_Data *wd, xmlNodePtr tag)
{
   Wiki_Parser *wp = NULL;
   xmlNodePtr div = NULL;
   char *content = NULL;

   div = xmlNewNode(NULL, XML_CONST_CHAR("div"));

   if(div && tag) {
      xmlSetProp(div, XML_CONST_CHAR("class"), XML_CONST_CHAR("poem"));
      content = (char *) xmlNodeGetContent(tag);
      content = pcre_replace("/\n/", "<br />\n", content);
      while((strstr(content, "\n  "))) { 
   	 content = pcre_replace("/^ ( *)([^ ])/m", "\\1&nbsp;\\2", content);
      }
      content = pcre_replace("/\n /", "\n&nbsp;", content);
      if(content) {
	 UChar *utmp = u_strdupC(content);
	 if(utmp) {
	    int error = 0;
	    wp = wiki_parser_buf_new(wd, utmp, u_strlen(utmp), &error);
	    wiki_parser_convert_xhtml(wp, &error, 1);
	    free(utmp);
	 }
	 if(wp) {
	    if(wp->node) {
	       xmlNodePtr node;
	       while((node = wp->node->children)) {
		  xmlUnlinkNode(node);
		  xmlAddChild(div, node);
	       }
	    }
	    wiki_parser_free(wp);
	 }
	 free(content);
      }
      xmlFreeNode(tag);
   }

   return div;
}

static xmlNodePtr wiki_parser_tag_extension_math(const Wiki_Parser_Data *wd, xmlNodePtr tag)
{
   xmlNodePtr node = NULL;
   if(! wd) return NULL;

   ulog("%s\n", __FUNCTION__);
   if(tag) {
      char *math = (char*) xmlNodeGetContent(tag);
      if(math) {
      	 math = pcre_replace("/([ \n]+)$/", " ", math);
	 if(math) {
	    char *tmp = NULL;
	    asprintf(&tmp, "<math>%s</math>", math);
	    if(tmp) {
	       free(math);
	       math = tmp;
	    }
	 }
	 // printf("TRY MATH: %s\n", math);
	 char *ml = itex2MML_parse(math, strlen(math));
	 char *p = NULL;
	 size_t skip = 0;
	 if(! ml /* || (ml && (strlen(ml) < 10 || strstr(ml, "<merror")))*/) {
	    ulog("MATHML ERROR: %s\n", math);
	    node = xmlNewNode(NULL, XML_CONST_CHAR("span"));
	    if(node) {
	       xmlNewProp(node, XML_CONST_CHAR("class"), XML_CONST_CHAR("matherror"));
	       xmlNewProp(node, XML_CONST_CHAR("style"),
		     XML_CONST_CHAR("background-color: red"));
	       xmlNodePtr t = xmlNewText(XML_CHAR(math));
	       if(t)
		  xmlAddChild(node, t);
	    }
	 }

      	 while(! node && ml && (p = index(ml + skip, '&'))) {
	    char *e = NULL;
	    if(*(p + 1) != '#') 
	       e = index(p, ';');
	    if(e) {
	       char *ent = strndup(p, (e + 1) - p);
	       if(ent) {
		  const char *hex = html_entity_to_hex(ent);
		  if(hex) {
		     char *rep = NULL;
		     asprintf(&rep, "/%s/", ent);
		     if(rep) {
			ml = pcre_replace(rep, hex, ml);
			skip = 0;
			p = NULL;
			free(rep);
		     }
		  }
		  free(ent);
	       }
	    }
	    if(p)
	       skip = (p - ml) + 1;
	 }

	 if(! node && ml) {
	    xmlDocPtr xml = xmlReadMemory(ml, strlen(ml), NULL, "", 0);
	    if(xml) {
	       xmlNodePtr root = xmlDocGetRootElement(xml);
	       node = xmlCopyNode(root, 1);
	       xmlFreeDoc(xml);
	    } else
	       ulog("\nML: %s\n", ml);
	 }

	 if(ml)
	    free(ml);

	 free(math);
      }
      xmlFreeNode(tag);
   }

   return node;
}

static xmlNodePtr wiki_parser_tag_extension_section(const Wiki_Parser_Data *wd, xmlNodePtr tag)
{
   xmlNodePtr msg = NULL;

   if(tag) {
      // printf("%s\n", xmlNodeGetContent(tag));
      xmlFreeNode(tag);
   }

   return msg;
}

static xmlNodePtr wiki_parser_tag_extension_source(const Wiki_Parser_Data *wd, xmlNodePtr tag)
{
   xmlNodePtr pre = NULL;

   if(tag) {
      pre = xmlNewNode(NULL, XML_CONST_CHAR("pre"));
      if(pre) {
	 if(xmlHasProp(tag, XML_CONST_CHAR("lang"))) {
	    xmlChar *lang = xmlGetProp(tag, XML_CONST_CHAR("lang"));
	    if(lang) {
	       xmlSetProp(pre, XML_CONST_CHAR("lang"), lang);
	       free(lang);
	    }
	 }

	 ulog("SOURCE: %s, %s\n", tag->name, tag->content);
	 xmlChar *content = xmlNodeGetContent(tag);
	 xmlNodeAddContent(pre, content);
	 free(content);
      }
      xmlFreeNode(tag);
   }

   return pre;
}

static xmlNodePtr wiki_parser_tag_extension_nowiki(const Wiki_Parser_Data *wd, xmlNodePtr tag)
{
   xmlNodePtr span = NULL;

   if(tag && tag->children) {
      span = xmlNewNode(NULL, XML_CONST_CHAR("span"));
      if(span) {
	 xmlSetProp(span, XML_CONST_CHAR("class"), XML_CONST_CHAR("nowiki"));

	 ulog("SOURCE: %s, %p\n", tag->name, tag->children);
	 xmlChar *content = xmlNodeGetContent(tag);
	 xmlNodeAddContent(span, content);
	 free(content);
      }
   }

   if(tag)
      xmlFreeNode(tag);

   return span;
}

static xmlNodePtr wiki_parser_tag_extension_remove(const Wiki_Parser_Data *wd, xmlNodePtr tag)
{
   xmlNodePtr msg = NULL;
   
   msg = xmlNewNode(NULL, XML_CONST_CHAR("span"));
   if(msg) {
      xmlSetProp(msg, XML_CONST_CHAR("style"), XML_CONST_CHAR("background-color: #FF0000; color: #000000"));
   }

   if(tag) {
      ulog("TAGREMOVE: %s\n", tag->name);
      char tmpmsg[] = "__tag <%s> removed__";
      char *text = calloc(strlen((char *)tag->name) + sizeof(tmpmsg) + 1, sizeof(char));
      if(text) {
	 sprintf(text, tmpmsg, (char *)tag->name);
	 xmlNodePtr txt = xmlNewText(XML_CHAR(text));
	 if(msg)
	    xmlAddChild(msg, txt);
	 free(text);
      }
      xmlFreeNode(tag);
   } else {
      ulog("TAGREMOVE: tag is NULL ????\n");
      xmlNodePtr txt = xmlNewText(XML_CONST_CHAR("__tag removed__"));
      if(msg)
	 xmlAddChild(msg, txt);
   }

   return msg;
}

static void wiki_parser_tag_extension_unregister(void *data)
{
   Wiki_Parser_Tag_Extension *tag = data;

   if(tag) {
      if(0 && tag->name)
	 eina_stringshare_del(tag->name);
      free(tag);
   }
}

Eina_Hash *wiki_parser_tag_extension_register(Eina_Hash *tags, 
      const char *tagname, 
      xmlNodePtr (*parse) (const Wiki_Parser_Data *wd, xmlNodePtr tag),
      Wiki_Parser_Tag_Content_Type type, Eina_Bool block)
{
   Wiki_Parser_Tag_Extension *tag = NULL;

   if(! tagname) return tags;

   if(! tags) {
      tags = eina_hash_string_superfast_new(wiki_parser_tag_extension_unregister);
      if(! tags) return NULL;
   }

   tag = malloc(sizeof(Wiki_Parser_Tag_Extension));
   tag->name = tagname; // eina_stringshare_add(tagname);
   tag->parse = parse;
   tag->type = type;
   tag->block = block;

   if(tag) {
      eina_hash_add(tags, tagname, tag);
   }

   return tags;
}

const Eina_Hash *wiki_parser_html_attributes(void)
{
   
   return html_attr_authorized;
}

const Eina_Hash *wiki_parser_tag_extension_free(void)
{
   if(html_tags) {
      eina_hash_free(html_tags);
      html_tags = NULL;
   }

   if(html_attr_authorized) {
      eina_hash_free(html_attr_authorized);
      html_attr_authorized = NULL;
   }

   return NULL;
}

const Eina_Hash *wiki_parser_tag_extension_init(void)
{
   Eina_Hash *tags = NULL;
   int i = 0;

   if(html_tags)
      return html_tags;

   if(! html_attr_authorized) {
      html_attr_authorized = eina_hash_string_superfast_new(NULL);
      while(attrs[i] != NULL) {
	 eina_hash_add(html_attr_authorized, attrs[i], attrs);
	 i++;
      }
   }

   tags = wiki_parser_tag_extension_register(tags, "kbd", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "b", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "i", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "del", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "ins", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "u", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "font", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "big", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "small", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "sub", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "sup", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "h1", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "h2", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "h3", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "h4", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "h5", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "h6", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "cite", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "code", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "em", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "s", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "strike", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "strong", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "tt", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "var", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "div", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "center", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "blockquote", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "ol", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "ul", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "dl", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "li", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "dt", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "dd", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "table", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "caption", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "tr", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "th", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "td", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "ref", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "abbr", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "dfn", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "samp", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "acronym", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "ruby", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "rt", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "rb", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "rp", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "p", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "span", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "br", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_FALSE);
   tags = wiki_parser_tag_extension_register(tags, "hr", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);

   tags = wiki_parser_tag_extension_register(tags, "pre", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_NONE, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "nowiki", wiki_parser_tag_extension_nowiki, WIKI_PARSER_TAG_CONTENT_PARSE_NONE, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "math", wiki_parser_tag_extension_math, WIKI_PARSER_TAG_CONTENT_PARSE_NONE, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "source", wiki_parser_tag_extension_source, WIKI_PARSER_TAG_CONTENT_PARSE_NONE, EINA_TRUE);

   tags = wiki_parser_tag_extension_register(tags, "references", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "tableofcontent", NULL, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "poem", wiki_parser_tag_extension_poem, WIKI_PARSER_TAG_CONTENT_PARSE_NONE, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "pages", wiki_parser_tag_extension_pages, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "section", wiki_parser_tag_extension_section, WIKI_PARSER_TAG_CONTENT_PARSE_ALL, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "gallery", wiki_parser_tag_extension_gallery, WIKI_PARSER_TAG_CONTENT_PARSE_NONE, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "imagemap", wiki_parser_tag_extension_imagemap, WIKI_PARSER_TAG_CONTENT_PARSE_NONE, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "timeline", wiki_parser_tag_extension_remove, WIKI_PARSER_TAG_CONTENT_PARSE_NONE, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "dynamicpagelist", wiki_parser_tag_extension_remove, WIKI_PARSER_TAG_CONTENT_PARSE_NONE, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "inputbox", wiki_parser_tag_extension_remove, WIKI_PARSER_TAG_CONTENT_PARSE_NONE, EINA_TRUE);
   tags = wiki_parser_tag_extension_register(tags, "categorytree", wiki_parser_tag_extension_remove, WIKI_PARSER_TAG_CONTENT_PARSE_NONE, EINA_TRUE);

   html_tags = tags;
   return tags;
}
