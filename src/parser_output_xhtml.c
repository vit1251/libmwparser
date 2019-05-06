#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <sys/time.h>

#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/uregex.h>
#include <unicode/ustdio.h>

#include <libxml/xpath.h>

#include "wiki_define.h"
#include "parser.h"
#include "parser_extension.h"
#include "parser_tag_extensions.h"
#include "util.h"
#include "entities.h"
#include "pcre_replace.h"
#include "icu_eina_hash.h"

#define NODE_TAG_IS(node, tag) (node && node->name && tag && ! strcmp((char *) node->name, tag))
#define NODE_TAG_NOT_IS(node, tag) (node && node->name && tag && strcmp((char *) node->name, tag))
#define TAG_IS(name, tag) ! strcmp(name, tag)

xmlNodePtr 
_wiki_parser_convert_xhtml_part(Wiki_Parser *wp, Wiki_Buffer *wb, int flags);
xmlNodePtr 
_wiki_parser_convert_xhtml(Wiki_Parser *wp, UChar *buf, size_t bufsize, int *error, int flags);

static xmlNodePtr 
wiki_parser_parse_img(Wiki_Parser *root, xmlNodePtr parent, UChar *bufstr, size_t size);
static char *
wiki_parser_html_attributes_get(xmlNodePtr node, char *data, Eina_Bool ignore_content);
static Wiki_Lexer *
wiki_parser_table_elem_attrib_check(Wiki_Parser *wp, Wiki_Buffer *wb, Wiki_Lexer *lexer, const char *type);
static Wiki_Lexer *
wiki_parser_tag_add(Wiki_Parser *wp, Wiki_Buffer *wb, Wiki_Lexer *lexer, int in_pre_format);
xmlNodePtr wiki_parser_parse_link(Wiki_Parser *root, xmlNodePtr parent, UChar *bufstr, size_t size, int type, int image_link, Eina_Bool at_start_line);

void wiki_parser_node_prop_set(xmlNodePtr node , const char *prop, const char *val);

static Wiki_Buffer *
_wiki_buffer_new(UChar *buf, size_t size)
{
   Wiki_Buffer *wb = NULL;

   if(! buf || size <= 0) return wb;
   wb = calloc(1, sizeof(Wiki_Buffer));
   if(wb) {
      int error = 0;
      wb->lexer = wiki_lexer_buf_new(buf, size, &error, EINA_TRUE);
      if(wb->lexer) {
	 wb->buf = buf;
	 wb->size = size;
      } else {
	 free(wb);
	 wb = NULL;
      }
   }

   return wb;
}

static Wiki_Buffer *
_wiki_buffer_free(Wiki_Buffer *wb)
{
   if(wb) {
      wiki_lexer_free(wb->lexer);
      free(wb);
   }

   return NULL;
}

static UChar *
wiki_parser_args_list_merge(Eina_List *, int start);

static UChar *
wiki_parser_args_list_merge(Eina_List *list, int start)
{
   Wiki_Template_Arg *arg = NULL;
   Eina_List *l = NULL;
   UChar *content = NULL;
   size_t size = 0;
   int i = 0;

   if(! list) return NULL;
   if(eina_list_count(list) < start) return NULL;

   EINA_LIST_FOREACH(list, l, arg) {
      UChar *tmp = NULL;
      size_t add = 0;
      if(i < start) {
	 i++;
	 continue;
      }
      if(arg->key)
	 add = arg->key_size;
      if(arg->have_equal)
	 add++;
      if(arg->val) 
	 add += arg->val_size;
      if(content)
	 add++;
      tmp = calloc(size + add + 1, sizeof(UChar));
      if(tmp) {
	 if(content) {
	    u_strcat(tmp, content);
	    u_strcat(tmp, ICU_STR_PIPE);
	    free(content);
	 }
	 if(arg->key)
	    u_strcat(tmp, arg->key);
	 if(arg->have_equal)
	    u_strcat(tmp, ICU_STR_EQUAL_SIGN);
	 if(arg->val)
	    u_strcat(tmp, arg->val);
	 content = tmp;
	 size += add;
      }
   }

   ulog("MERGE: %S\n", content);
   return content;
}

xmlNodePtr 
wiki_parser_node_add(xmlNodePtr parent, const char *tag)
{
   xmlNodePtr node = NULL;

   PARSE_TRACE();

   if(! tag) return NULL;

   if(parent == NULL) {
      node = xmlNewNode(NULL, XML_CONST_CHAR(tag));
      ulog("WANT ADD TAG WITHOUT: parent: %p, tag: %s\n", parent, tag);
   } else {
      xmlNodePtr p = parent, n = NULL;
      ulog("WANT ADD TAG: parent: %s, tag: %s\n", parent->name, tag);
      if(TAG_IS(tag, "li") 
	    && ! (NODE_TAG_IS(p, "ul") || NODE_TAG_IS(p, "ol"))) {
	 while(p && ! (NODE_TAG_IS(p, "ul") || NODE_TAG_IS(p, "ol")))
	    p = p->parent;

	 if(! p || ! p->parent || p->type != XML_ELEMENT_NODE) {
	    p = xmlNewChild(parent, NULL, XML_CONST_CHAR("ul"), NULL);
	 }
      }
      if(TAG_IS(tag, "table") && ! (NODE_TAG_IS(p, "div")
	       || NODE_TAG_IS(p, "td")|| NODE_TAG_IS(p, "th"))) {
	 while(p
	       && ! (NODE_TAG_IS(p, "div") 
		  || NODE_TAG_IS(p, "root") 
		  || NODE_TAG_IS(p, "table") 
		  || NODE_TAG_IS(p, "blockquote") 
		  || NODE_TAG_IS(p, "center") 
		  || NODE_TAG_IS(p, "td")))
	    p = p->parent;
	 if(p && p->type == XML_ELEMENT_NODE && NODE_TAG_IS(p, "table")) {
	    return NULL;
	    n = xmlNewChild(parent, NULL, XML_CONST_CHAR("tr"), NULL);
	    if(n)
	       p = xmlNewChild(n, NULL, XML_CONST_CHAR("td"), NULL);
	 }
      }
      if(TAG_IS(tag, "caption")) {
	 while(p && ! (NODE_TAG_IS(p, "table")))
	    p = p->parent;
      }
      if(TAG_IS(tag, "td") || TAG_IS(tag, "th")) {
	 while(p && ! (NODE_TAG_IS(p, "tr") || (NODE_TAG_IS(p, "table"))))
	    p = p->parent;
//	 if(! p) {
//	    p = xmlNewChild(parent, NULL, XML_CONST_CHAR("table"), NULL);
//	 }
      }
      if(TAG_IS(tag, "tr")) {
	 while(p && ! (NODE_TAG_IS(p, "table")))
	    p = p->parent;
//	 if(! p) {
//	    p = xmlNewChild(parent, NULL, XML_CONST_CHAR("table"), NULL);
//	 }
      }
      if(p && p->type == XML_ELEMENT_NODE && NODE_TAG_IS(p, "table")) {
	 if(! (TAG_IS(tag, "tr") || TAG_IS(tag, "caption")) /* && 
		  (TAG_IS(tag, "th") || TAG_IS(tag, "td")) */) {
	    n = xmlNewChild(p, NULL, XML_CONST_CHAR("tr"), NULL);
	    if(n)
	       p = n;
	 } else if(! (TAG_IS(tag, "tr") || TAG_IS(tag, "caption")))
	    p = p->parent;
      }
      if(p && p->type == XML_ELEMENT_NODE && NODE_TAG_IS(p, "tr")) {
	 if(! (TAG_IS(tag, "th") || TAG_IS(tag, "td") || TAG_IS(tag, "tr"))) {
	    ulog("FORCE TAG TR for %s\n", tag);
	    n = xmlNewChild(p, NULL, XML_CONST_CHAR("td"), NULL);
	    if(n)
	       p = n;
	 }
      }

      if(p && p->type == XML_ELEMENT_NODE) {
	 ulog("SET ADD TAG: parent: %s, tag: %s\n", p->name, tag);
	 if(! p->name) exit(0);
	 node = xmlNewChild(p, NULL, XML_CONST_CHAR(tag), NULL);
      }
   }

   return node;
}

void wiki_parser_node_prop_set(xmlNodePtr node, const char *prop, const char *val)
{
   PARSE_TRACE();

   if(! node) return;
   if(! prop) return;
   if(! val) return;

   if(xmlHasProp(node, XML_CONST_CHAR(prop))) {
      char *old = (char *) xmlGetProp(node, XML_CONST_CHAR(prop));
      if(old) {
	 char *tmp = NULL;
	 asprintf(&tmp, "%s %s", old, val);
	 if(tmp) {
	    xmlSetProp(node, XML_CONST_CHAR(prop), XML_CHAR(tmp));
	    free(tmp);
	 }
	 free(old);
      }
   } else {
      xmlSetProp(node, XML_CONST_CHAR(prop), XML_CHAR(val));
   }
}

void wiki_parser_text_add(xmlNodePtr node, const UChar *data, size_t size, Eina_Bool entity)
{
   PARSE_TRACE();

   if(node && data) {
      char *buf = u_strnToUTF8_new(data, size);
   ulog("add_text: %s\n", buf);
      if(buf) {
	 xmlNodePtr n = NULL;
	 if(entity == EINA_TRUE) {
	    buf = pcre_replace("/&(lang|raw|entry)=/", "&amp;\\1=", buf);
	    n = xmlStringGetNodeList(node->doc, XML_CHAR(buf));
	 } else 
	    n = xmlNewText(XML_CHAR(buf));
	 if(n) 
	    xmlAddChild(node, n);
	 free(buf);
      }
   }
}

void wiki_parser_text_utf8_add(xmlNodePtr node, const char *data, size_t size, Eina_Bool entity)
{
   PARSE_TRACE();

   if(node && data) {
      char *buf = strndup(data, size);
      if(buf) {
	 buf = pcre_replace("/&(lang|raw|entry)=/", "&amp;\\1=", buf);
	 xmlNodePtr n = xmlStringGetNodeList(node->doc, XML_CHAR(buf));
	 if(n) 
	    xmlAddChild(node, n);
	 free(buf);
      }
   }
}

char *wiki_parser_fullpagename_interwiki_get(Wiki_Parser *wp, const char *name, int ns, const char *site, const char *lang)
{
   if(! wp) return NULL;
   if(! wp->wd) return NULL;
   if(! name) return NULL;
   if(! site) return NULL;

   /* TODO */
    
   return NULL;

}

const char *wiki_parser_fullpagename_get(Wiki_Parser *wp)
{
   if(! wp) return NULL;
   if(! wp->wd) return NULL;
   if(! wp->wd->pagename) return NULL;

   PARSE_TRACE();

   if(wp->wd->ns) {
      if(! wp->fullpagename) {
	 const char *ns = wiki_parser_namespace_name_get(wp->wd, wp->wd->ns);
      	 if(ns) {
	    char *fullpagename = calloc(strlen(ns) + strlen(wp->wd->pagename) + 2, sizeof(char));
	    if(fullpagename) {
	       sprintf(fullpagename, "%s:%s", ns, wp->wd->pagename);
	       wp->fullpagename = fullpagename;
	    }
	 }
      }
      if(wp->fullpagename)
         return wp->fullpagename;
   }

   return wp->wd->pagename;
}

static char *
_wiki_parser_html_attributes_uniq_string_restore(Wiki_Parser *wp, char *data, size_t *size)
{
   Wiki_Nowiki_Part *wnp = NULL;
   char *p = NULL, *e = NULL;
   UChar *key = NULL;
   size_t s = 0, skip = 0;;

   if(! wp) return data;
   if(! data) return data;

   if(size)
      s = *size;
   else
      s = strlen(data);

   while((p = strstr(data + skip, "\x7fUNIQ"))) {
      e = strstr(p, "-QINU\x7f");
      if(e)
	 e += 6;
      else
	 e = p + 5;
      key = u_strndupC(p, e - p);
      ulog("KEY: _%S_\n", key);
      if(key && wp->nowiki && (wnp = icu_eina_hash_find(wp->nowiki, key))) {
	 ulog("found: %S\n", wnp->content);
	 if(strcmp(wnp->tagname, "nowiki") == 0 && wnp->content_size > 0) {
	    char *buf = u_strnToUTF8_new(wnp->content, wnp->content_size);
	    if(wnp->content_size > (e - p)) {
	       char *tmp = data;
	       data = realloc(data, s + wnp->content_size);
	       memmove(data + (p - tmp) + wnp->content_size, data + (e - tmp), (s  - (e - tmp)) + 1);
	       memcpy(data + (p - tmp), buf, wnp->content_size);
	       s += (wnp->content_size - (e - p));
	    } else {
	       memcpy(p, buf, wnp->content_size);
	       p += wnp->content_size;
	       memmove(p, e, (s - (e - data)) + 1);
	       s -= (e - p);
	       ulog("replace: %s\n", data);
	    }
	    free(buf);
	 } else if(strcmp(wnp->tagname, "nowiki") == 0) {
	    memmove(p, e, (s - (e - data)) + 1);
	    s -= (e - p);
	 } else
	    skip = (p - data) + 1;
      } else {
	 ulog("Not found: remove\n");
	 memmove(p, e, (s - (e - data)) + 1);
	 s -= (e - p);
      }

      if(key) {
	 free(key);
	 key = NULL;
      }
   }

   if(size)
      *size = s;

   return data;
}

static char *
wiki_parser_html_attributes_get(xmlNodePtr node, char *data, Eina_Bool ignore_content)
{
   char *p = data;
   char *s = p;
   Eina_Bool have_attr = 0;
   size_t size = 0, skip = 0;

   PARSE_TRACE();

   if(! node) return data;
   if(! data) return data;
   size = strlen(data);

   ulog("ATTRIBUTES STR: %s\n", data);
   while(skip < size && (p = strchr(data + skip, '='))) {
      char *p2 = p;
      p2--;
      while(p2 > data && *p2 == ' ')
        p2--;
      if(p2 < p) p2++;
      s = p2;
      while(s > data && *(s - 1) != ' ')
	 s--;
      if(s && s < p2) {
	 char *key = strtolower(strndup(s, p2 - s));
	 char *e = NULL;
	 ulog("ATTRIBUTES KEY: '%s'\n", key);
	 if(key) {
	    const Eina_Hash *html_attr_authorized = wiki_parser_html_attributes();
	    if(eina_hash_find(html_attr_authorized, key)) {
	       char *val = NULL;
	       p++;
	       while(*p == ' ' && p < data + size)
		  p++;
	       if(*p == '"') {
		  p++;
		  e = strchr(p, '"'); 
		  if(! e) p = data + size;
	       } else {
		  e = strchr(p, ' ');
		  if(! e) e = data + size;
	       }
	       if(e && e > p) {
		  val = strndup(p, e - p);
		  if(val) {
		     char *c = strstr(val, ">");
		     if(c)
			*c = '\0';
		     // val = pcre_replace("/display[ ]?:[ ]?none/", "", val);
		     ulog("ATTRIBUTES KEY OK: tag = %s, key = %s, val: %s\n", node->name, key, val);
		     wiki_parser_node_prop_set(node , key, val);
		     free(val);
		  }
		  e++;
		  memset(s, 32,  e - s);
		  have_attr = 1;
	       } else {
		  memset(s, 32,  p - s);
	       }
	    } 
	    p++;
	    free(key);
	 }
      } else
	 p++;
      skip = p - data;
   }
   if(strstr(data, "nowrap")) {
      wiki_parser_node_prop_set(node , "nowrap", "nowrap");
      data = pcre_replace("/nowrap/", "", data);
   }

   /*
   if(! have_attr && ! ignore_content) {
      wiki_parser_text_utf8_add(node, data, strlen(data), EINA_TRUE);
   } else */ 
   if(have_attr) {
      free(data);
      data = NULL;
   }

   return data;
}

static Wiki_Lexer *
wiki_parser_tag_add(Wiki_Parser *wp, Wiki_Buffer *wb, Wiki_Lexer *lexer, int in_pre_format)
{
   Wiki_Lexer *l = NULL;
   UChar *data = NULL;
   xmlNodePtr node = NULL;
   int count = in_pre_format, nowiki = 0;
   const char *tag = NULL;
   const Eina_Hash *tags = wiki_lexer_tags_get();

   PARSE_TRACE();

   if(! wp) return lexer;
   if(! wb) return lexer;
   if(! wp->node) return lexer;
   if(! lexer) return lexer;

   l = lexer;
   data = wb->buf;

   while(l) {
      node = NULL;
      if(nowiki == 0 && l->tag_ext && l->tag_ext->name
	    && (l->token == WIKI_TOKEN_TAG_CLOSE
	    || l->token == WIKI_TOKEN_NOWIKI_CLOSE
	    || l->token == WIKI_TOKEN_PRE_CLOSE
	    /* || l->token == WIKI_TOKEN_SOURCE_CLOSE */)) {
	 const Wiki_Parser_Tag_Extension *te = l->tag_ext, *t = NULL;
	 tag = l->tag_ext->name;

	 if(te->type != WIKI_PARSER_TAG_CONTENT_PARSE_ALL) {
	    if(count > in_pre_format)
	       count--;
	 }
	 if(count == 0) {
	    node = wp->node;
	    while(node && node->type == XML_ELEMENT_NODE && ! NODE_TAG_IS(node, tag)) {
	       if(! node->name) exit(0);
	       t = eina_hash_find(tags, node->name);
	       if(t && t->block && ! te->block) {
		  wp->node = node;
		  node = NULL;
	       } else if((TAG_IS(tag, "td") && NODE_TAG_IS(node, "tr"))
		     || (TAG_IS(tag, "tr") && NODE_TAG_IS(node, "table"))) {
		  wp->node = node;
		  node = NULL;
	       } else if(TAG_IS(tag, "div") && NODE_TAG_IS(node, "table")) {
		  node = NULL;
	       } else {
		  node = node->parent;
	       }
	    }
	    if(node && NODE_TAG_IS(node, tag) && node->parent) {
     	       wp->node = node->parent;
	       ulog("GET PARENT %s for </%s>\n", wp->node->name, tag);
	    }
	 } else {
	    wiki_parser_text_add(wp->node, data + l->offset, l->size, EINA_FALSE);
	 }
      } else if(nowiki == 0 && l->tag_ext && l->tag_ext->name
	    && (l->token == WIKI_TOKEN_TAG_OPEN
	    || l->token == WIKI_TOKEN_TAG_OPEN_CLOSE
	    || l->token == WIKI_TOKEN_PRE_OPEN
	    || l->token == WIKI_TOKEN_NOWIKI_OPEN
	    /* || l->token == WIKI_TOKEN_SOURCE_OPEN */)) {
	 const Wiki_Parser_Tag_Extension *te = l->tag_ext;
	 UChar *e = NULL;
	 tag = l->tag_ext->name;
	 int addcount = 0;
	 e = data + l->offset;
	 while(e < data + l->offset + l->size 
	       && ! (u_isWhitespace(*e) || *e == '>')) 
	    e++;
	 if(te->type != WIKI_PARSER_TAG_CONTENT_PARSE_ALL
     	       && l->token != WIKI_TOKEN_TAG_OPEN_CLOSE) {
	    if(count > in_pre_format)
	       count++;
	    else
	       addcount = 1;
	 }
	 if(count == 0 && wp->node) {
      	    node = wiki_parser_node_add(wp->node, tag);
	    if(node && ! node->parent) {
	       xmlFreeNode(node);
	       node = NULL;
	    }
	    if(node) {
	       if(l->token != WIKI_TOKEN_TAG_OPEN_CLOSE) {
		  wp->node = node;
	       }
	       if(addcount)
		  count++;
	       char *buf =  u_strnToUTF8_new(e, l->size - (e - (data + l->offset)));
	       ulog("RTAG_OPEN:%d: %s, %s, %p, %p, %s\n", nowiki, tag, wiki_lexer_token_name_get(l->token), wp->node, node, buf);
	       if(buf) {
		  Eina_Bool ic = EINA_TRUE;
		  buf = pcre_replace("/\n/", " ", buf);
		  buf = pcre_replace("/<\\/?[ ]*nowiki[ ]*\\/?>/", "", buf);
		  // buf = pcre_replace("/&#123;&#123;&#123;.*&#125;&#125;&#125;/", " ", buf);
		  buf = _wiki_parser_html_attributes_uniq_string_restore(wp, buf, NULL);
		  buf = wiki_parser_html_attributes_get(node, buf, ic);
		  if(buf)
		     free(buf);
	       }
	       node = NULL;
	    } else {
	       // wiki_parser_text_add(wp->node, data + l->offset, l->size, EINA_FALSE);
	    }
	 } else {
	    wiki_parser_text_add(wp->node, data + l->offset, l->size, EINA_FALSE);
	 }
      } else {
	 wiki_parser_text_add(wp->node, data + l->offset, l->size, EINA_FALSE);
	 if(0) {
	    UChar *tmp = u_strndup(data + l->offset, l->size);
	    if(tmp) {
	       ulog("PARSE_TAG UNKNOW: %S\n", tmp);
	       free(tmp);
	    }
	 }
      }

      if(count == in_pre_format && nowiki == 0)
	 break;

      if(l)
	 l = l->next;
   }

   return l;
}

static xmlNodePtr 
wiki_parser_parse_img(Wiki_Parser *root, xmlNodePtr parent, UChar *bufstr, size_t size)
{
   Eina_List *el = NULL;
   Wiki_Template_Arg *arg;
   Wiki_File *wf = NULL;
   Wiki_Title *wt = NULL;
   UChar *data = NULL;
   UChar *name = NULL, *link = NULL;
   char *url = NULL, *title = NULL, *alt = NULL, *valign = NULL;
   char *page = NULL, *caption = NULL; 
   int border = 0, frameless = 0, align = 0, width = 0, height = 0;
   int in_commons = 0;
   xmlNodePtr img = NULL;

   if(! root) return NULL;
   if(! root->wd) return NULL;
   if(! parent) return NULL;
   if(! bufstr) return NULL;
   if(size < 0) return NULL;

   PARSE_TRACE();

   data = u_strndup(bufstr, size);
   if(! data) return NULL;

   data = trim_u(icu_replace("\\n", ICU_STR_EMPTY, data, 0));
   if(! data) return NULL;

   wiki_parser_title_case_set(root->wd, data);

      data = icu_replace_utf8("^", "fake image|", data, 0);
   el = wiki_parser_template_args_list_get2(data, EINA_FALSE);
   if(! el) {
      free(data);
      return NULL;
   }

   arg = eina_list_nth(el, 0);
   if(arg && arg->key)
      name = trim_u(u_strdup(arg->key));
   if(name) {
      wt = parser_title_part(root->wd, name);
      if(wt) {
	 UChar *tmp = parser_title_with_namespace(wt);
	 if(tmp) {
	    title = u_strToUTF8_new(tmp);
	    free(tmp);
	 }
	 url = wiki_parser_url_image_get(root->wd, title);

	 if(root->wd->page_file_info_get) {
	    wf = root->wd->page_file_info_get(root->wd, title);
	    if(! wf) {
	       UChar *tmp = parser_title_with_namespace(wt);
	       if(tmp) {
		  tmp = icu_replace_utf8(wiki_parser_namespace_name_get(root->wd, WIKI_NS_FILE), "File", tmp, 0);
		  if(tmp) {
		     char *tname = u_strToUTF8_new(tmp);
		     if(tname) {
			wf = root->wd->page_file_info_get(root->wd, tname);
			free(tname);
			in_commons = 1;
		     }
		     free(tmp);
		  }
	       }
	    }
	 } 

	 if(title && url && eina_list_count(el) > 1) {
	    Eina_List *l;
	    int resize = 0;
	    int cnt = 0;
	    EINA_LIST_FOREACH(el, l, arg) {
	       if(cnt == 0) {
		  cnt++;
		  continue;
	       }
	       if(arg && arg->key) {
		  arg->key = trimn_u(arg->key, arg->key_size, &arg->key_size);
		  arg->key = wiki_parser_intl_get(arg->key, &arg->key_size);
	       }
	       ulog("IMG CHECK ARG: %S\n", arg->key);
	       if(arg && arg->key) {
		  if(! resize && icu_search("^[0-9 ]+px", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     char *px = u_strToUTF8_new(arg->key);
		     if(px) {
			px = pcre_replace("/ /", "", px);
			width = atoi(px);
			free(px);
		     }
		  } else if(! resize && icu_search("^[0-9 ]+x[0-9 ]+px", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     char *px = u_strToUTF8_new(arg->key);
		     if(px) {
			char *w = NULL, *h = NULL;
			px = pcre_replace("/( |px)+/i", "", px);
			w = strdup(px);
			if(w) {
			   w = pcre_replace("/([0-9]+)x([0-9]+)/", "\\1", w);
			   width = atoi(w);
			   free(w);
			}
			h = strdup(px);
			if(h) {
			   h = pcre_replace("/(.*)x(.*)/", "\\2", h);
			   height = atoi(h);
			   free(h);
			}
			if(0 && height < width) {
			   width = height;
			   height = 0;
			}
			free(px);
		     }
		  } else if(! resize && icu_search("^x[0-9 ]+px", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     char *px = u_strToUTF8_new(arg->key);
		     if(px) {
			px = pcre_replace("/( |p|x)+/i", "", px);
			height = atoi(px);
			ulog("XPX: %d, %d, %s\n", height, width, px);
			free(px);
		     }
		  } else if(! valign && icu_search("^(baseline|sub|super|middle|bottom|text-top|text-bottom)$", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     char style[] = "vertical-align: ";
		     char *tmp = u_strToUTF8_new(arg->key);
		     if(tmp) {
			valign = calloc(strlen(style) + strlen(tmp) + 1, sizeof(char));
			if(valign) {
			   sprintf(valign, "%s%s", style, tmp);
			}
			free(tmp);
		     }
		  } else if(icu_search("^(border|frame|thumb|thumbnail)$", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     if(icu_search("^(border)$", arg->key, UREGEX_CASE_INSENSITIVE)) 
			border = 1;
		     else if(icu_search("^(frame)$", arg->key, UREGEX_CASE_INSENSITIVE)) 
			border = 2;
		     else if(icu_search("^(thumb|thumbnail)$", arg->key, UREGEX_CASE_INSENSITIVE)) 
			border = 3;
		  } else if(icu_search("^(frameless)$", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     frameless = 1;
		  } else if(icu_search("^(left)$", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     ulog("align left\n");
		     align = 1;
		  } else if(icu_search("^(center|centre)$", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     align = 2;
		  } else if(icu_search("^(right|rigth)$", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     align = 3;
		  } else if(icu_search("^(none)$", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     align = 4;
		  } else if(! link  && icu_search("^(link)$", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     link = (arg->val) ? u_strdup(arg->val) : u_strdupC("");
		  } else if(! alt  && icu_search("^(alt)$", arg->key, UREGEX_CASE_INSENSITIVE) && arg->val) {
		     alt = u_strToUTF8_new(arg->val);
		  } else if(! alt  && icu_search("^(page)$", arg->key, UREGEX_CASE_INSENSITIVE) && arg->val) {
		  } else if(icu_search("^(upright)", arg->key, UREGEX_CASE_INSENSITIVE)) {
		     // TODO: scale image size to number

		  } else if(! caption) {
		     if(arg->val) {
			UChar *tmp = calloc(u_strlen(arg->key) + u_strlen(arg->val) + 2, sizeof(UChar));
			if(tmp) {
			   u_sprintf(tmp, "%S=%S", arg->key, arg->val);
			   caption = u_strToUTF8_new(tmp);
			   free(tmp);
			}
		     } else {
			caption = u_strToUTF8_new(arg->key);
		     }
		  }
	       }
	    }
	 }
      }
      free(name);
   } 

   if(width > 2000)
      width = 0;
   if(height > 2000)
      height = 0;

   EINA_LIST_FREE(el, arg) {
      if(arg) {
	 if(arg->key) free(arg->key);
	 if(arg->val) free(arg->val);
	 free(arg);
      }
   }

   if(data)
      free(data);

   if(url && parent) {
      xmlNodePtr parent_img = parent;
      if(border == 3 || border == 2) {
	 xmlNodePtr n = xmlNewChild(parent_img, NULL, XML_CONST_CHAR("div"), NULL);
	 if(n) {
	    const char *calign = NULL;
	    xmlNodePtr p = xmlNewChild(n, NULL, XML_CONST_CHAR("div"), NULL);

	    if(align == 1)
	       calign = "thumb tleft";
	    else if(align == 2)
	       calign = "thumb tcenter";
	    else if(align == 4)
	       calign = "thumb tnone";
	    else 
	       calign = "thumb tright";

	    wiki_parser_node_prop_set(n, "class", calign);
	    if(p) {
	       if(border == 3) {
		  char wst[] = "width: %dpx;";
		  char *style = calloc(INT_STR_SIZE + strlen(wst) + 1, sizeof(char));
		  if(style) {
		     if(width)
			sprintf(style, wst, width  + 2);
		     else
			sprintf(style, wst, WIKI_THUMB_BORDER_SIZE);
		     wiki_parser_node_prop_set(p, "style", style);
		     free(style);
		  }
	       }

	       if(! width && border == 3) 
		  width = WIKI_THUMB_IMG_SIZE;
	       height = 0;

	       wiki_parser_node_prop_set(p, "class", "thumbinner");
	       xmlNodePtr a = NULL;
	       if(p) {
		  Eina_Bool have_page = EINA_FALSE;
		  UChar *tmp = NULL;
		  if(1) {
		     char *w = NULL;
		     asprintf(&w, "width: %dpx", ((width) ? width : WIKI_THUMB_BORDER_SIZE) + 2);
		     if(w) {
			wiki_parser_node_prop_set(p, "style", w);
			free(w);
		     }
		  }
		  tmp = calloc(strlen(title) + strlen(wiki_parser_namespace_name_get(root->wd, WIKI_NS_FILE)) + 15, sizeof(UChar));
		  if(tmp) {
		     u_sprintf(tmp, "%s:%s", wiki_parser_namespace_name_get(root->wd, WIKI_NS_FILE), title);
		     if(root->wd->page_exists) {
			char *ctmp = u_strToUTF8_new(tmp);
			if(ctmp) { 
			   if(root->wd->page_exists(root->wd, ctmp) == EINA_TRUE) {
			      a = wiki_parser_parse_link(root, p, tmp, u_strlen(tmp), 2, 1, EINA_FALSE);
			      have_page = EINA_TRUE;
			   }
			   free(ctmp);
			}
		     }
		     if(have_page == EINA_FALSE && wt && wt->title) {
			u_sprintf(tmp, "en:C:File:%S", wt->title);
			a = wiki_parser_parse_link(root, p, tmp, u_strlen(tmp), 2, 1, EINA_FALSE);
		     }
		     free(tmp);
		  }
		  if(a)
		     parent_img = a;
		  else
		     parent_img = p;
	       }

	    
	       xmlNodePtr c = xmlNewChild(p, NULL, XML_CONST_CHAR("div"), NULL);
	       if(c) {
		  wiki_parser_node_prop_set(c, "class", "thumbcaption");
		  xmlNodePtr m = xmlNewChild(c, NULL, XML_CONST_CHAR("div"), NULL);
		  if(m) {
		     wiki_parser_node_prop_set(m, "class", "magnify");
		  }
		  if(caption) {
		     UChar *ucaption = u_strdupC(caption);
		     if(ucaption) {
			int error = 0;
#if 0
			Wiki_Buffer *wb = _wiki_buffer_new(ucaption, u_strlen(ucaption));
			printf("IMG CAPTION\n");
			if(wb) {
			   root->node = c;
			   _wiki_parser_convert_xhtml_part(root, wb, 0);
			   _wiki_buffer_free(wb);
			}
#else
			Wiki_Parser *wp = wiki_parser_buf_new(root->wd, ucaption, u_strlen(ucaption), &error);
			if(wp)
			   wiki_parser_convert_xhtml(wp, &error, 0);
		     	if(wp->node) {
	     		   xmlNodePtr node;
     			   while((node = wp->node->children)) {
			      xmlUnlinkNode(node);
			      xmlAddChild(c, node);
			   }
			}
			if(wp)
			   wiki_parser_free(wp);
#endif
			free(ucaption);
		     } else
			wiki_parser_text_utf8_add(c, caption, strlen(caption), EINA_TRUE);
		     free(caption);
		     caption = NULL;
		  }
	       }
	    }
	 }
      } 
     

      if(align) {
	 xmlNodePtr n = xmlNewChild(parent_img, NULL, XML_CONST_CHAR("div"), NULL);
	 if(align == 2) {
	    wiki_parser_node_prop_set(n, "class", "center");
	    n = xmlNewChild(n, NULL, XML_CONST_CHAR("div"), NULL);

	 }
	 if(align == 1) {
	    wiki_parser_node_prop_set(n, "class", "floatleft");
	 } else if(align == 3) {
	    wiki_parser_node_prop_set(n, "class", "floatright");
	 } else {
	    wiki_parser_node_prop_set(n, "class", "floatnone");
	 }
	 parent_img = n;

      }

      if(frameless) {
	 if(! width) {
	    width = WIKI_THUMB_IMG_SIZE;
	    height = 0;
	 }
	 if(! link) {
	    if(0 && in_commons) {
	       link = calloc(strlen(title) + 12, sizeof(UChar));
	       if(link) {
		  u_sprintf(link, "en:C:File:%s", title);
	       }
	    } else if (0) {
	       link = calloc(strlen(title) + strlen(wiki_parser_namespace_name_get(root->wd, WIKI_NS_FILE)) + 2, sizeof(UChar));
	       if(link) {
		  u_sprintf(link, "%s:%s", wiki_parser_namespace_name_get(root->wd, WIKI_NS_FILE), title);
	       }
	    } else
	       link = u_strdupC(title);
	    ulog("LINK: %S\n", link);
	 }
      }

      if(link) {
	 xmlNodePtr n = wiki_parser_parse_link(root, parent_img, link, u_strlen(link), 2, 1, EINA_FALSE);
	 if(n) {
	    wiki_parser_node_prop_set(n, "class", "image");
	    parent_img = n;
	 }
      }

      Eina_Bool nothumb = EINA_FALSE;
      if(wf) {
	 if(width > wf->width)
	    width = 0;
	 else if (width == wf->width)
	    nothumb = EINA_TRUE;
	 if(height > wf->height)
	    height = 0;
	 else if(height == wf->height)
	    nothumb = EINA_TRUE;
	 if(height == 0 && width == 0 && wf->width > 220)
	    width = 220;
      } else if(0 && ! width && ! height)
	 width = 220;

      char *p = rindex(url, '.');
      if(p && (strcasestr(p, "ogg") || strcasestr(p, "ogv"))) {
	 img = xmlNewNode(NULL, XML_CONST_CHAR("video"));
      } else if(! width && ! height && p && strcasestr(p, "svg")) {
	 // Embed or oject since we don't the size to get png result ?
	 img = xmlNewNode(NULL, XML_CONST_CHAR("embed"));
	 wiki_parser_node_prop_set(img, "type", "image/svg+xml");
	 wiki_parser_node_prop_set(img, "style", "display: inline;");
      } else
	 img = xmlNewNode(NULL, XML_CONST_CHAR("img"));

      if(parent != parent_img && parent_img->children) {
	 xmlAddPrevSibling(parent_img->children, img);
      } else {
	 xmlAddChild(parent_img, img);
      }

      if(width && nothumb == EINA_FALSE) {
	 char str[INT_STR_SIZE];
	 snprintf(str, sizeof(str), "%d", width);
	 wiki_parser_node_prop_set(img, "width", str);
	 url = wiki_parser_image_url_thumb_set(url, width);
      } 
      if(height && nothumb == EINA_FALSE) {
	 char str[INT_STR_SIZE];
	 snprintf(str, sizeof(str), "%d", height);
	 wiki_parser_node_prop_set(img, "height", str);
/*	 if(! width)
	    width = height;
	 url = wiki_parser_image_url_thumb_set(url, width); */
      }

      wiki_parser_node_prop_set(img, "src", url);
      free(url);

      if(alt)
	 wiki_parser_node_prop_set(img, "alt", alt);
      else if(caption && border < 2)
	 wiki_parser_node_prop_set(img, "alt", caption);
      else
	 wiki_parser_node_prop_set(img, "alt", "");

      if(valign) {
	 wiki_parser_node_prop_set(img, "style", valign);
      }
   }

   if(wt) parser_title_free(wt);
   if(wf) wiki_parser_file_free(wf);
   if(title) free(title);
   if(alt) free(alt);
   if(caption) free(caption);
   if(page) free(page);
   if(link) free(link);
   if(valign) free(valign);

   return img;
}

xmlNodePtr wiki_parser_parse_link(Wiki_Parser *root, xmlNodePtr parent, UChar *bufstr, size_t size, int type, int image_link, Eina_Bool at_start_line)
{
   Eina_List *el = NULL, **list = NULL;
   Wiki_Parser *wp = NULL;
   Wiki_Lang *wl = NULL;
   int page_exists = 0;
   UChar *data = NULL;
   char *url = NULL;
   xmlNodePtr node = NULL;
   
   PARSE_TRACE();

   if(! root) return node;
   if(! parent) return node;
   if(! bufstr) return node;

   data = trim_u(icu_replace("\\n", ICU_STR_EMPTY, u_strndup(bufstr, size), 0));
   if(! data) return node;

   wp = wiki_parser_add(root->wd);
   if(! wp) return node;

   if((type == 1 || type == 2) 
         && icu_search("^([a-z]+://|mailto:|\\?wikimedia=)", data, 0)) {
      UChar *a =NULL, *p = NULL;

      p = u_strstr(data, ICU_STR_SPACE);
      a = u_strstr(data, ICU_STR_ANGL_OPEN);

      ulog("DATAFULLURL: %S, %S, %S, %d\n", data, p, a, (a < p));
      
      node = xmlNewChild(parent, NULL, XML_CONST_CHAR("a"), NULL);
      if(! node) {
	 wiki_parser_free(wp);
	 return node;
      }

      if(a && p) {
	 if(a < p)
	    p = a;
      } else if(a)
	 p = a;
      ulog("DATAFULLURL: %S, %S, %S, %d\n", data, p, a, (a < p));
      if(p) {
	 url = u_strnToUTF8_new(data, p - data);
         // url = pcre_replace("/&/", "&amp;", url);
	 wp->buf = u_strdup(p);
	 wp->size = u_strlen(wp->buf);
//	 wp->lexer = wiki_lexer_get(NULL, wp->buf, wp->size, 0, &wp->lexer_error, EINA_TRUE);
	 if(type == 1)
	    wiki_parser_node_prop_set(node, "class", "external text");
      } else {
         // data = icu_replace_utf8("&", "&amp;", data, 0);
	 url = u_strToUTF8_new(data);
	 if(type == 1) {
	    wiki_parser_node_prop_set(node, "class", "external autonumber");
	    root->external_link++;
	 }
      }
      if(type == 1)
	 wiki_parser_node_prop_set(node, "rel", "nofollow");
   } else if(type == 2) {
      Wiki_Template_Arg *arg = NULL;
      Wiki_Title *wt = NULL;
      data = icu_replace_utf8("^", "fake link|", data, 0);
      el = wiki_parser_template_args_list_get2(data, EINA_FALSE);
      if(el) {
	 UChar *name = NULL;
	 arg = eina_list_nth(el, 0);
	 if(arg && arg->key)
	    name = trim_u(icu_replace("[Mm]ediawikiwiki:", ICU_STR_EMPTY, u_strdup(arg->key), 0));

	 if(name) {
	    wt = parser_title_part(root->wd, name);
	    if(wt) {
	       UChar *title = NULL;
	       char *site_c = u_strToUTF8_new(wt->sitename);

//	       uprintf("Sould add lang : %S, %d = 1, %d = 0, %d = 0, %d = 1\n", wt->lang, wt->lang_first, wt->intext, image_link, at_start_line);
	       if(wt->lang && wt->lang_first == EINA_TRUE
		     && wt->intext == EINA_FALSE && ! image_link
		     && (! site_c || at_start_line == EINA_TRUE)) {
		  char *lang_c = u_strToUTF8_new(wt->lang);
		  if(lang_c) {
		     const Wiki_Lang *wl = wiki_parser_language_infos_get(lang_c);
		     if(wl) {
			wp->buf = u_strdupC(wl->local);
			wp->size = u_strlen(wp->buf);
//			wp->lexer = wiki_lexer_get(NULL, wp->buf, wp->size, 0, &wp->lexer_error, EINA_TRUE);
		     }
		     free(lang_c);
		  }
		  list = &root->langs;
	       } else if(wt->ns && ! wt->sitename 
		     && wt->intext == EINA_FALSE && ! image_link) {
		  char *ns = u_strToUTF8_new(wt->ns);
		  if(ns) {
		     char *c = strstr(ns, ":");
		     if(c)
			*c = '\0';
		     const Wiki_Namespace *wns = eina_hash_find(root->wd->namespaces, ns);
		     if(wns) {
			if(wns->key == WIKI_NS_CATEGORY && wt->title) {
			   list = &root->categories;
			   wp->buf = u_strdup(wt->title);
			   wp->size = u_strlen(wp->buf);
//			   wp->lexer = wiki_lexer_get(NULL, wp->buf, wp->size, 0, &wp->lexer_error, EINA_TRUE);
			   
			} else if(wns->key == WIKI_NS_FILE && ! image_link) {
			   node = wiki_parser_parse_img(root, parent, bufstr, size);
			   wt = parser_title_free(wt);
			   wp = wiki_parser_free(wp);
			}
		     }
		     free(ns);
		  }
	       }

	       if(wt) {
		  title = parser_title_with_namespace(wt);
		  if(! title) {
		     title = u_strdupC(wiki_parser_fullpagename_get(wp));
		  }

		  if(wt->sitename || wt->lang) {
		     char *lang_c = u_strToUTF8_new(wt->lang);
		     char *title_c = u_strToUTF8_new(title);

		     if(! lang_c && root->wd->lang)
			lang_c = strdup(root->wd->lang);

		     if(! site_c) {
			if(strstr(root->wd->sitename, "wikimedia.org") 
			      || strstr(root->wd->sitename, "mediawiki.org")) {
			   site_c = strdup("wikipedia.org");
			} else {
			   site_c = strdup(root->wd->base); 
			   site_c = pcre_replace("/http:\\/\\/.*\\.(.*)\\..*/", "\\1", site_c);
			}
		     }

		     if(site_c && strcmp(site_c, wp->wd->sitename) == 0) {
			url = wiki_parser_url_internal_get(wp, WIKI_NS_MAIN, title_c, wp->wd->default_output_mode, &page_exists);
		     } else {
			url = wiki_parser_url_interwiki_get(wp, title_c, site_c, lang_c, &page_exists);
		     }

		     if(lang_c) free(lang_c);
		     if(site_c) free(site_c);
		     if(title_c) free(title_c);
		  } else {
		     char *title_c = u_strToUTF8_new(title);
		     url = wiki_parser_url_internal_get(wp, WIKI_NS_MAIN, title_c, wp->wd->default_output_mode, &page_exists);
		     if(title_c)
			free(title_c);

		  }

		  if(list) {
		     node = xmlNewNode(NULL, XML_CONST_CHAR("a"));
		     *list = eina_list_append(*list, node);
		  } else {
		     node = xmlNewChild(parent, NULL, XML_CONST_CHAR("a"), NULL);
		  }

		  if(node && ! page_exists) 
		     wiki_parser_node_prop_set(node, "class", "new");

		  if(title) free(title);
	       }
	    }

	    free(name);
	 }

	 if(! list && wp)
	    wp->lexer = wiki_lexer_free(wp->lexer);

	 if(wp && ! image_link && ! list) {
	    UChar *p = NULL;
	    if(eina_list_count(el) > 1)
	       p = wiki_parser_args_list_merge(el, 1);
	    else
	       p = wiki_parser_args_list_merge(el, 0);
	    if(p) {
     	       wp->buf = p;
	       wp->size = u_strlen(wp->buf);
//	       wp->lexer = wiki_lexer_get(NULL, wp->buf, wp->size, 0, &wp->lexer_error, EINA_TRUE);
	    }
	 }

	 EINA_LIST_FREE(el, arg) {
	    if(arg) {
	       if(arg->key) free(arg->key);
	       if(arg->val) free(arg->val);
	       free(arg);
	    }
	 }

	 wt = parser_title_free(wt);
      }
   }

   if(node && url) {
      ulog("PARSE LINK URL: %s\n", url);
      wiki_parser_node_prop_set(node, "href", url);
      free(url);
   }

   if(wp && node) {
      wp->nowiki = root->nowiki;
      if(wp->buf) {
	 int error = 0;
	 xmlNodePtr r = _wiki_parser_convert_xhtml(wp, wp->buf, wp->size, &error, 1);
	 if(r && r->children) {
	    while(r->children) {
	       xmlNodePtr n = r->children;
	       xmlUnlinkNode(n);
	       xmlAddChild(node, n);
	    }
	 }
      } else if(node && type == 2 && ! image_link) {
	 if(0 && icu_search("Special:", data, 0))
	    wiki_parser_text_utf8_add(node, "Special Page", strlen("Special Page"), EINA_TRUE);
	 else if(wl)
	    wiki_parser_text_utf8_add(node, wl->local, strlen(wl->local), EINA_TRUE);
	 else
	    wiki_parser_text_add(node, data, u_strlen(data), EINA_TRUE);
      } else if(node && type == 1) {
	 char num[INT_STR_SIZE];
	 snprintf(num, sizeof(num), "%d", root->external_link);
	 wiki_parser_text_utf8_add(node, num, strlen(num), EINA_TRUE);
      }
   }

   if(wp) {
      wp->nowiki = NULL;
      wiki_parser_free(wp);
   }

   if(data)
      free(data);

   return node;
}

static Wiki_Lexer *
wiki_parser_table_elem_attrib_check(Wiki_Parser *wp, Wiki_Buffer *wb, Wiki_Lexer *lexer, const char *type)
{
   Wiki_Lexer *l = NULL, *n = NULL;

   PARSE_TRACE();

   if(! wp) return lexer;
   if(! wb) return lexer;
   if(! wb->buf) return lexer;
   if(! lexer) return lexer;

   l = lexer;
   if(wp->node && l->next 
	 && (l->next->token != WIKI_TOKEN_NEWLINE
	    || l->next->token != WIKI_TOKEN_NEWPAR)) {
      n = l->next;
      if(n ) {
	 if(0 && (lexer->token == WIKI_TOKEN_TAB_OPEN
		  || lexer->token == WIKI_TOKEN_TAB_NEWLINE)) {
	    while(n && ! (n->token == WIKI_TOKEN_NEWLINE
		     || n->token == WIKI_TOKEN_NEWPAR))
	       n = n->next;
	 } else {
	    while(n && 
		  ((n->token == WIKI_TOKEN_TEXT
		   || n->token == WIKI_TOKEN_NEWLINE
		   || n->token == WIKI_TOKEN_NEWPAR
		   || n->token == WIKI_TOKEN_UNIQ_STRING
		   || n->token == WIKI_TOKEN_ENTITY
		   || n->token == WIKI_TOKEN_WHITESPACE
		   || n->token == WIKI_TOKEN_COLON
		   || n->token == WIKI_TOKEN_APOSTROPHE
		   || n->token == WIKI_TOKEN_QUOTE
		   || n->token == WIKI_TOKEN_HASH
		   || n->token == WIKI_TOKEN_DASH
		   || n->token == WIKI_TOKEN_ASTERISK
		   || n->token == WIKI_TOKEN_SEMICOLON
		   || n->token == WIKI_TOKEN_NOWIKI_OPEN
		   || n->token == WIKI_TOKEN_NOWIKI_CLOSE
		   || n->token == WIKI_TOKEN_NOWIKI_OPEN_CLOSE
		   || n->token == WIKI_TOKEN_CURL_CLOSE
		   || n->token == WIKI_TOKEN_EQUAL_SIGN)
		  )) {
	       if(n == l->next && (n->token == WIKI_TOKEN_NEWLINE 
			|| n->token == WIKI_TOKEN_NEWPAR))
		  break;
	       if(n->prev && n->prev->token == WIKI_TOKEN_NEWLINE && 
		     ! (n->token == WIKI_TOKEN_TEXT 
			|| n->token == WIKI_TOKEN_HASH
			|| n->token == WIKI_TOKEN_WHITESPACE)) {
		  n = n->prev;
		  break;
	       }
	       n = n->next;
	    }
	 }
	 /* Eat line for TAB_OPEN  or TAB_NEWLINE */
	 if(n && (lexer->token == WIKI_TOKEN_TAB_OPEN || lexer->token == WIKI_TOKEN_TAB_NEWLINE)
	       && n->prev && 
	       ! (n->prev->token == WIKI_TOKEN_NEWLINE || n->prev->token == WIKI_TOKEN_NEWPAR)) {
//	    ulog("EAT LINE: %s, prev = %s\n", wiki_lexer_token_name_get(n->token), wiki_lexer_token_name_get(n->prev->token));
	    while(n && ! (n->token == WIKI_TOKEN_NEWLINE || n->token == WIKI_TOKEN_NEWPAR))
	       n = n->next;
	 }
	 if(n /* && n->prev != l->next */) {
	    Wiki_Token t = l->token;
	    l = l->next;
	    char *buf = u_strnToUTF8_new(wb->buf + l->offset, n->offset - (l->offset ));
//	    buf = pcre_replace("/<[\\/]?nowiki>/", "", buf);
	    ulog("FIRST COL %s: %s\n", type, buf);
	    if(buf) {
	       int ignore = 0;
	       UChar *tmp = NULL;
	       if(t == WIKI_TOKEN_TAB_OPEN || t == WIKI_TOKEN_TAB_NEWLINE)
		  ignore = 1;
	       buf = pcre_replace("/<\\/?[ ]*nowiki[ ]*\\/?>/", "", buf);
	       buf = _wiki_parser_html_attributes_uniq_string_restore(wp, buf, NULL);
	       buf = wiki_parser_html_attributes_get(wp->node, buf, ignore);
	       if(buf && ! ignore) 
		  tmp = u_strdupC(buf);
	       if(tmp) {
#if 1
		  Wiki_Buffer *wb = _wiki_buffer_new(tmp, u_strlen(tmp));
		  if(wb) {
		     _wiki_parser_convert_xhtml_part(wp, wb, 1);
		     _wiki_buffer_free(wb);
		  }
#else
		  int error = 0;
		  Wiki_Parser *w = wiki_parser_buf_new(wp->wd, tmp, u_strlen(tmp), &error);
		  if(w) {
		     xmlNodePtr root = _wiki_parser_convert_xhtml(w, w->buf, w->size, &error, 1);
		     if(root) {
			xmlNodePtr node = NULL;
			while(root->children) {
			   node = root->children;
			   xmlUnlinkNode(node);
			   xmlAddChild(wp->node, node);
			}
		     }
		     wiki_parser_free(w);
		  }
#endif
		  free(tmp);
	       }
	       if(buf)
		  free(buf);
	    }
	    if(0 && n->token == WIKI_TOKEN_PIPE && n->next->token != WIKI_TOKEN_PIPE)
	       l = n;
	    else
	       l = n->prev;
//	 } else {
//	    uprintf("n->prev = l->next: %s, %s, %s\n", wiki_lexer_token_name_get(n->prev->token), wiki_lexer_token_name_get(l->next->token), wiki_lexer_token_name_get(n->token));
	 }
      }
   }

   return l;
}

typedef struct _Wiki_Html_List
{
   Wiki_Token token;
   const char *tag;
   const char *container;
} Wiki_Html_List;

static Wiki_Lexer *wiki_parser_parse_list(Wiki_Parser *wp, Wiki_Buffer *wb, Wiki_Lexer *lexer)
{
   Wiki_Lexer *l = lexer;
   Wiki_Html_List *hl = NULL, *h = NULL;
   Eina_List *lhl = NULL, *lh = NULL;

   PARSE_TRACE();

   if(! wp) return lexer;
   if(! wb) return lexer;
   if(! wp->node) return lexer;
   if(! lexer) return lexer;

   if(l->prev && ! (l->prev->token == WIKI_TOKEN_NEWLINE 
	    || l->prev->token == WIKI_TOKEN_NEWPAR)) {
      wiki_parser_text_add(wp->node, wb->buf + l->offset, l->size, EINA_TRUE);
      return l;
   }

   while(l && (l->token == WIKI_TOKEN_ASTERISK
	    || l->token == WIKI_TOKEN_HASH
	    || l->token == WIKI_TOKEN_COLON
	    || l->token == WIKI_TOKEN_SEMICOLON)) {
      hl = malloc(sizeof(Wiki_Html_List));
      if(! hl)
	 break;

      if(l->token == WIKI_TOKEN_ASTERISK) {
	 hl->token = WIKI_TOKEN_ASTERISK;
	 hl->tag = "li";
	 hl->container = "ul";
      } else if(l->token == WIKI_TOKEN_HASH) {
	 hl->token = WIKI_TOKEN_HASH;
	 hl->tag = "li";
	 hl->container = "ol";
      } else if(l->token == WIKI_TOKEN_COLON) {
	 hl->token = WIKI_TOKEN_COLON;
	 hl->tag = "dd";
	 hl->container = "dl";
      } else if(l->token == WIKI_TOKEN_SEMICOLON) {
	 hl->token = WIKI_TOKEN_SEMICOLON;
	 hl->tag = "dt";
	 hl->container = "dl";
      }
      lhl = eina_list_append(lhl, hl);
      h = hl;
      l = l->next;
   }
   l = l->prev;

   if(lhl) {
      xmlNodePtr node = wp->node;
      while(node && node->parent  
	 && ! (NODE_TAG_IS(node, "root")
 	       || NODE_TAG_IS(node, "td")
 	       /* || NODE_TAG_IS(node, "th")
 	       || NODE_TAG_IS(node, "tr") */
 	       || NODE_TAG_IS(node, "table")
 	       || NODE_TAG_IS(node, "div"))) {
	 node = node->parent;
      }
      ulog("HLIST:%d: parent: %s\n", eina_list_count(lhl), node->name);
      if(node) {
	 int i = eina_list_count(lhl);
	 EINA_LIST_FOREACH(lhl, lh, hl) {
	    i--;
	    xmlNodePtr last = node->last;
	    if(last && last->type != XML_ELEMENT_NODE)
	       last = NULL; // last->prev;
            if(! last) {
	       ulog("HLIST: tag %s, %p\n", hl->tag, last);
	       xmlNodePtr n = wiki_parser_node_add(node, hl->container);
	       ulog("HLIST not last:%d: add container %s to %s\n", i, hl->container, node->name);
	       if(n)
		  node = n;
	    } else if(last) {
	       ulog("HLIST: tag %s, last = %s, type = %d \n", hl->tag, last->name, last->type);
	       if(NODE_TAG_IS(last, hl->container)) {
		  ulog("HLIST:%d: use container %s to %s\n", i, last->name, node->name);
		  node = last;
	       } else {
		  xmlNodePtr n = wiki_parser_node_add(node, hl->container);
		  ulog("HLIST last:%d: add container %s to %s\n", i, hl->container, node->name);
		  if(n)
		     node = n;
	       }
	    }
	    /* Force indent */
	    if(h != hl && NODE_TAG_IS(node, "dl") && hl->token == WIKI_TOKEN_COLON) {
     	       xmlNodePtr n = wiki_parser_node_add(node, "dd");
	       if(n)
		  node = n;
	    }
	    wp->node = node;
	 }
      }
      if(h && node) {
	 xmlNodePtr n = wiki_parser_node_add(node, h->tag);
	 if(n)
	    wp->node = n;
      }

      EINA_LIST_FREE(lhl, hl) {
	 free(hl);
      }
   }

   return l;
}

static xmlNodePtr
_wiki_parser_xmldoc_init(Wiki_Parser *wp)
{
   PARSE_TRACE();

   if(! wp) return NULL;

   if(! wp->root && ! wp->xml) {
      if(! wp->xml)
	 wp->xml = xmlNewDoc(XML_CONST_CHAR("1.0"));
      if(wp->xml) {
	 wp->root = xmlNewDocNode(wp->xml, NULL, XML_CONST_CHAR("root"), NULL);
	 if(wp->root)
	    xmlDocSetRootElement(wp->xml, wp->root);
      }
   }

   if(wp->root && ! wp->node) {
      wp->node = wp->root;
   }

   if(! wp->root) {
      printf("No root node\n"); exit(0);
   }
   if(! wp->root->name) {
      printf("No root name\n"); exit(0);
   }
   return wp->node;
}

static void
_wiki_parser_apostrophe_check(Wiki_Parser *wp, Wiki_Buffer *wb, Wiki_Lexer *lexer, int flags)
{
   Wiki_Lexer *l = NULL, *n = NULL;
   int italic = 0, bold = 0;

   PARSE_TRACE();

   if(! wp) return;
   if(! wb) return;
   if(! wb->lexer) return;

   l = lexer;

   if(l->prev == NULL 
	 || l->token == WIKI_TOKEN_NEWLINE || l->token == WIKI_TOKEN_NEWPAR) {
      int num = 0;
      Wiki_Lexer *firstspace = NULL, *firstsingleletter = NULL, *firstmultiletter = NULL;
      wp->bi_state = 0;

      n = (l->prev) ? l->next : l;
      while(n && ! (n->token == WIKI_TOKEN_NEWLINE || n->token == WIKI_TOKEN_NEWPAR)) {
	 if(n->token == WIKI_TOKEN_APOSTROPHE) {
	    Wiki_Lexer *a = n;
	    num = 0;
	    while(a && a->token == WIKI_TOKEN_APOSTROPHE) {
	       num++;
	       a = a->next;
	    }
	    if(num == 4) num = 3;
	    if(num > 5) num = 5;
	    if(num > 1) {
	       if(num == 2) italic++;
	       else if(num == 3) bold++;
	       else if(num == 5) {
		  italic++; bold++;
	       }
	       if(num == 3) {
		  UChar x1 = (n->offset > 0) ? *(wb->buf + (n->offset - 1)) : '\0';
		  UChar x2 = (n->offset > 1) ? *(wb->buf + (n->offset - 2)) : '\0';
		  if(x1 == ' ') {
		     if(firstspace == NULL) firstspace = n;
		  } else if(x2 == ' ') {
		     if(firstsingleletter == NULL) firstsingleletter = n;
		  } else {
		     if(firstmultiletter == NULL) firstmultiletter = n;
		  }
	       }
	       n = (a) ? a->prev : a;
	    }
	 }
	 if(n)
	    n = n->next;
      }
      ulog("APOSTROPHE num: %d, bold: %d, italic: %d, state: %d\n", num, bold, italic, wp->bi_state);
      if(((italic % 2) == 1 && (bold % 2) == 1) || (italic > 1 && bold == 1))  {
	 if(firstsingleletter) {
	    firstsingleletter->token = WIKI_TOKEN_TEXT;
	 } else if(firstmultiletter) {
	    firstmultiletter->token = WIKI_TOKEN_TEXT;
	 } else if(firstspace) {
	    firstspace->token = WIKI_TOKEN_TEXT;
	 }
      }
   }
}

static xmlNodePtr
_wiki_parser_node_in_paragraph_check(Wiki_Parser *wp, Wiki_Lexer *lexer, int flags)
{
   Wiki_Lexer *l = lexer;
   xmlNodePtr node = NULL, n = NULL;

   PARSE_TRACE();

   if(! wp) return NULL;
   node = wp->node;
   if(node != wp->root) return node;
   if(! lexer) return node;

   /* Start new paragraph if current node is root and element should be
    * inline */
   switch (l->token) {
      case WIKI_TOKEN_TEXT:
      case WIKI_TOKEN_AMP:
      case WIKI_TOKEN_APOSTROPHE:
      case WIKI_TOKEN_CURL_OPEN:
      case WIKI_TOKEN_CURL_CLOSE:
      case WIKI_TOKEN_SQRE_OPEN:
      case WIKI_TOKEN_SQRE_CLOSE:
      case WIKI_TOKEN_QUOTE:
      case WIKI_TOKEN_ENTITY:
      case WIKI_TOKEN_TAG_OPEN:
      case WIKI_TOKEN_TAG_OPEN_CLOSE:
	 if(! flags && node == wp->root 
	       && (l->tag_ext == NULL || l->tag_ext->block == EINA_FALSE)) {
	    n = wiki_parser_node_add(node, "p");
	    if(n) {
	       wiki_parser_node_prop_set(n, "class", "forcep");
	       node = n;
	    }
	 }
	 if(0 && l->tag_ext) {
	    uprintf("tag: %s => %d\n", l->tag_ext->name, l->tag_ext->block);
	 }
      default:
	 break;
   }

   return node;
}

static Wiki_Lexer *
_wiki_parser_newline_parse(Wiki_Parser *wp, Wiki_Buffer *wb, Wiki_Lexer *lexer, int flags)
{
   Wiki_Lexer *l = lexer, *n = NULL;
   xmlNodePtr node;

   PARSE_TRACE();

   if(! wp) return lexer;
   if(! wb) return lexer;
   if(! lexer) return lexer;

   node = wp->node;
   if(! node) return lexer;

   if(wp->in_header) {
      node = wp->in_header;
      if(node && node->parent)
	 wp->node = node->parent;
      else
	 wp->node = wp->root;
   } else if(wp->in_wiki_list == EINA_TRUE) {
      ulog("STOP WIKILIST: %d: %s\n", wp->in_wiki_list, node->name);
      if(NODE_TAG_IS(node, "li")
	    || NODE_TAG_IS(node, "i")
	    || NODE_TAG_IS(node, "b")
	    || NODE_TAG_IS(node, "dd")
	    || NODE_TAG_IS(node, "dt")
	    || NODE_TAG_IS(node, "td")) {
	 while(wp->in_wiki_list == EINA_TRUE && node) {
	    if(NODE_TAG_IS(node, "root")
		  || NODE_TAG_IS(node, "td")
		  || NODE_TAG_IS(node, "div")) {
	       wp->node = node;
	       wp->in_wiki_list = EINA_FALSE;
	    } else
	       node = node->parent;
	 }
	 wp->in_wiki_list = EINA_FALSE;
      }
      if(l->token == WIKI_TOKEN_NEWPAR)
	 wiki_parser_text_utf8_add(wp->node, "\n", 1, EINA_TRUE);
   } else if(wp->in_pre_format && l->next
	 && l->next->token == WIKI_TOKEN_WHITESPACE) {
      if(l->token == WIKI_TOKEN_NEWLINE)
	 wiki_parser_text_utf8_add(wp->node, "\n", 1, EINA_TRUE);
      else
	 wiki_parser_text_utf8_add(wp->node, "\n\n", 2, EINA_TRUE); 
   } else if(wp->in_pre_format) {
      node = wp->node;
      while(node && ! NODE_TAG_IS(node, "pre")) {
	 node = node->parent;
      }
      if(node && node->parent) {
	 Eina_Bool empty = EINA_TRUE;
	 xmlChar *c = xmlNodeGetContent(node);
	 if(c) {
	    xmlChar *cc = c;
	    while(*cc && empty == EINA_TRUE) {
	       if(!(*cc == ' ' || *cc == '\n'))
		  empty = EINA_FALSE;
	       cc++;
	    }
	    free(c);
	 }
	 wp->node = node->parent;
	 if(empty == EINA_TRUE) {
	    xmlUnlinkNode(node);
	    xmlFreeNode(node);
	 }
      } else
	 wp->node = wp->root;

      wp->in_pre_format--;
   }
   
   if(l->next) {
      n = l->next;
      node = wp->node;

      if(l->token == WIKI_TOKEN_NEWPAR) {
	 while(node && ! (
		  NODE_TAG_IS(node, "td")
		  || NODE_TAG_IS(node, "th")
		  || NODE_TAG_IS(node, "tr")
		  || NODE_TAG_IS(node, "table")
		  || NODE_TAG_IS(node, "div")
		  || NODE_TAG_IS(node, "blockquote")
		  || NODE_TAG_IS(node, "root"))) {
	    node = node->parent;
	 }
	 if(node)
	    wp->node = node;
	 else 
	    wp->node = wp->root;
      }

      node = wp->node;

      /* It's pre if 
       *   - first char of line is whitespace
       *   - there are no close tag of block tag (maybe only current)
       *     on the same line 
       *   - force no pre in li
       */
      if(n->token == WIKI_TOKEN_WHITESPACE) {
	 const Eina_Hash *tags = wiki_lexer_tags_get();
	 const Wiki_Parser_Tag_Extension *tag = NULL;
	 if(tags)
	    tag = eina_hash_find(tags, wp->node->name);

	 /* TODO: Need to check if whitespace should be remove after parsing
	  * temlate which enable false <pre> */
	 ulog("SHOULD ENABLE PRE\n");
	 node = wp->node;
	 if(! (NODE_TAG_IS(node, "root") 
		  || NODE_TAG_IS(node, "p")
		  || (( NODE_TAG_IS(node, "div")
		  || NODE_TAG_IS(node, "td")) && l->token == WIKI_TOKEN_NEWPAR ))) {
	    ulog("NOT ENABLE PRE because: %s or %s\n", node->name,
		  wiki_lexer_token_name_get(n->token));
	    n = NULL;
	 }
	 if(n) {
	    ulog("ENABLE PRE\n");
	    node = wiki_parser_node_add(wp->node, "pre");
	    if(node) {
	       wiki_parser_node_prop_set(node, "class", "wikiPreFormat");
	       wp->in_pre_format++;
	       wp->node = node;
	    }
	 }
	 n = l->next;
      }

      node = wp->node;
      if(! wp->in_pre_format 
	    && l->token == WIKI_TOKEN_NEWPAR 
	    && n->block == EINA_FALSE 
	    && ! NODE_TAG_IS(node, "tr")
	    && ! NODE_TAG_IS(node, "caption")
	    && ! NODE_TAG_IS(node, "table")
	    && n->token < WIKI_TOKEN_NEWPAR
	    && ! (n->token == WIKI_TOKEN_PIPE
	       || n->token == WIKI_TOKEN_EXCL)) {
	 node = wiki_parser_node_add(wp->node, "p");
	 if(node) {
	    wp->node = node;
	 }
      }	
      if(! wp->in_pre_format && n->token && !(
	       n->token == WIKI_TOKEN_COLON
	       || n->token == WIKI_TOKEN_SEMICOLON
	       || n->token == WIKI_TOKEN_HASH
	       || n->token == WIKI_TOKEN_ASTERISK))
	 wiki_parser_text_utf8_add(wp->node, "\n", 1, EINA_TRUE);

   } else {
      wiki_parser_text_utf8_add(wp->node, "\n", 1, EINA_TRUE);
   }

   return l;
}

static Wiki_Lexer *
_wiki_parser_sqre_parse(Wiki_Parser *wp, Wiki_Buffer *wb, Wiki_Lexer *lexer, int flags)
{
   Wiki_Lexer *l = lexer, *s = lexer, *n = lexer;
   xmlNodePtr node = NULL;
   int sqre_cnt = 0, type = 0;
   Eina_Bool at_start_line = EINA_FALSE;

   PARSE_TRACE();

   if(! wp) return lexer;
   if(! wb) return lexer;
   if(! lexer) return lexer;

   if(! flags && (! s->prev || s->prev->token == WIKI_TOKEN_NEWLINE 
	    || s->prev->token == WIKI_TOKEN_NEWPAR))
      at_start_line = EINA_TRUE;
   while(s && s->token == WIKI_TOKEN_SQRE_OPEN) {
      sqre_cnt++;
      type++;
      s = s->next;
   }
   n = s;
	       
   while(n && sqre_cnt > 0) {
      if(n->token == WIKI_TOKEN_SQRE_OPEN) {
	 if(type == 1) 
	    break; 
	 else 
	    sqre_cnt++;
      }
      if(n->token == WIKI_TOKEN_SQRE_CLOSE) sqre_cnt--;
      n = n->next;
   }
   if(n && sqre_cnt == 0) {
      sqre_cnt = type;
      while(n && sqre_cnt && 
	    n->prev && n->prev->token == WIKI_TOKEN_SQRE_CLOSE) {
	 n = n->prev;
	 sqre_cnt--;
      }
   } else {
      /* Unclose Tag */
      n = s;
      while(n && ! (n->token == WIKI_TOKEN_NEWLINE 
	       || n->token == WIKI_TOKEN_WHITESPACE
	       || n->token == WIKI_TOKEN_NEWPAR))
	 n = n->next;
   }
   node = NULL;
   if(n) {
      UChar *buf = u_strndup(wb->buf + s->offset, n->offset - s->offset);
      if(buf) {
	 node = wiki_parser_parse_link(wp, wp->node, buf, u_strlen(buf), type, 0, at_start_line);
	 free(buf);
      }
      if(n->token == WIKI_TOKEN_NEWLINE 
	    || n->token == WIKI_TOKEN_NEWPAR) {
	 n = n->prev;
      }
   }

   if(node) {
      l = n;
      while(l && l->next && l->next->token == WIKI_TOKEN_SQRE_CLOSE)
	 l = l->next;
   } else
      wiki_parser_text_add(wp->node, wb->buf + l->offset, l->size, EINA_TRUE);

   return l;
}

static Wiki_Lexer *
_wiki_parser_apostrophe_parse(Wiki_Parser *wp, Wiki_Buffer *wb, Wiki_Lexer *lexer, int flags)
{
   Wiki_Lexer *l = lexer, *n = NULL;
   xmlNodePtr node = NULL;
   int num = 0, k = 1;

   PARSE_TRACE();

   if(! wp) return lexer;
   if(! wb) return lexer;
   if(! lexer) return lexer;

   n = l;
   while(n && n->token == WIKI_TOKEN_APOSTROPHE) {
      num++;
      n = n->next;
   }
   ulog("APOSTROPH COUNT: %d : %d %d\n", num, l->offset, (n) ? n->offset : -1);
   if(num == 4) {
      k = 1;
      num--;
   } else if(num > 5) {
      k = num - 5;
      num = 5;
   } else if(num > 1)
      k = 0;

   ulog("APOSTROPH COUNT: %d\n", num);
   if(k)
      wiki_parser_text_add(wp->node, wb->buf + l->offset, l->size * k, EINA_TRUE);
   if(num > 1) {
      l = (n) ? n->prev : NULL;

      if(num == 2) {
	 if(wp->bi_state == WP_BI_STATE_I) {
	    node = wp->node;
	    while(node && ! NODE_TAG_IS(node, "i"))
	       node = node->parent;
	    if(node && node->parent)
	       wp->node = node->parent;
	    wp->bi_state = WP_BI_STATE_NONE;
	 } else if(wp->bi_state == WP_BI_STATE_BI) {
	    node = wp->node;
	    while(node && ! NODE_TAG_IS(node, "i"))
	       node = node->parent;
	    if(node && node->parent)
	       wp->node = node->parent;
	    wp->bi_state = WP_BI_STATE_B;
	 } else if(wp->bi_state == WP_BI_STATE_IB
	       || wp->bi_state == WP_BI_STATE_BOTH) {
	    node = wp->node;
	    while(node && ! NODE_TAG_IS(node, "b"))
	       node = node->parent;
	    if(node && node->parent && NODE_TAG_IS(node->parent, "i"))
	       node = node->parent;
	    if(node && node->parent)
	       wp->node = node->parent;
	    node = wiki_parser_node_add(wp->node, "b");
	    if(node)
	       wp->node = node; 
	    wp->bi_state = WP_BI_STATE_B;
	 } else {
	    node = wiki_parser_node_add(wp->node, "i");
	    if(node)
	       wp->node = node; 
	    wp->bi_state = (wp->bi_state == WP_BI_STATE_B) ?
	       WP_BI_STATE_BI : WP_BI_STATE_I;
	 }
      } else if(num == 3) {
	 if(wp->bi_state == WP_BI_STATE_B) {
	    node = wp->node;
	    while(node && ! NODE_TAG_IS(node, "b"))
	       node = node->parent;
	    if(node && node->parent)
	       wp->node = node->parent;
	    wp->bi_state = WP_BI_STATE_NONE;
	 } else if(wp->bi_state == WP_BI_STATE_IB
	       || wp->bi_state == WP_BI_STATE_BOTH) {
	    node = wp->node;
	    while(node && ! NODE_TAG_IS(node, "b"))
	       node = node->parent;
	    if(node && node->parent)
	       wp->node = node->parent;
	    wp->bi_state = WP_BI_STATE_I;
	 } else if(wp->bi_state == WP_BI_STATE_BI) {
	    node = wp->node;
	    while(node && ! NODE_TAG_IS(node, "i"))
	       node = node->parent;
	    if(node && node->parent && NODE_TAG_IS(node->parent, "b"))
	       node = node->parent;
	    if(node && node->parent)
	       wp->node = node->parent;
	    node = wiki_parser_node_add(wp->node, "i");
	    if(node)
	       wp->node = node; 
	    wp->bi_state = WP_BI_STATE_I;
	 } else {
	    node = wiki_parser_node_add(wp->node, "b");
	    if(node)
	       wp->node = node; 
	    wp->bi_state = (wp->bi_state == WP_BI_STATE_I) ?
	       WP_BI_STATE_IB : WP_BI_STATE_B;
	 }
      } else if(num == 5) {
	 if(wp->bi_state == WP_BI_STATE_B) {
	    node = wp->node;
	    while(node && ! NODE_TAG_IS(node, "b"))
	       node = node->parent;
	    if(node && node->parent)
	       wp->node = node->parent;
	    node = wiki_parser_node_add(wp->node, "i");
	    if(node)
	       wp->node = node; 
	    wp->bi_state = WP_BI_STATE_I;
	 } else if(wp->bi_state == WP_BI_STATE_I) {
	    node = wp->node;
	    while(node && ! NODE_TAG_IS(node, "i"))
	       node = node->parent;
	    if(node && node->parent)
	       wp->node = node->parent;
	    node = wiki_parser_node_add(wp->node, "b");
	    if(node)
	       wp->node = node; 
	    wp->bi_state = WP_BI_STATE_B;
	 } else if(wp->bi_state == WP_BI_STATE_BI) {
	    node = wp->node;
	    while(node && ! NODE_TAG_IS(node, "i"))
	       node = node->parent;
	    if(node && node->parent && NODE_TAG_IS(node->parent, "b"))
	       node = node->parent;
	    if(node && node->parent)
    	       wp->node = node->parent;
	    wp->bi_state = WP_BI_STATE_NONE;
	 } else if(wp->bi_state == WP_BI_STATE_IB
	       || wp->bi_state == WP_BI_STATE_BOTH) {
	    node = wp->node;
	    while(node && ! NODE_TAG_IS(node, "b"))
	       node = node->parent;
	    if(node && node->parent && NODE_TAG_IS(node->parent, "i"))
	       node = node->parent;
	    if(node && node->parent)
	       wp->node = node->parent;
	    wp->bi_state = WP_BI_STATE_NONE;
	 } else {
	    node = wiki_parser_node_add(wp->node, "i");
	    if(node) {
	       wp->node = node;
	       node = wiki_parser_node_add(wp->node, "b");
	       if(node)
		  wp->node = node;
	    }
	    wp->bi_state = WP_BI_STATE_BOTH;
	 }
      }
   }

   return l;
}

static Wiki_Lexer *
_wiki_parser_equal_sign_parse(Wiki_Parser *wp, Wiki_Buffer *wb, Wiki_Lexer *lexer, int flags)
{
   Wiki_Lexer *l = lexer, *n = NULL;
   xmlNodePtr node = NULL;

   PARSE_TRACE();

   if(! wp) return lexer;
   if(! wb) return lexer;
   if(! lexer) return lexer;

   if(! l->prev 
	 || l->prev->token == WIKI_TOKEN_NEWLINE 
	 || l->prev->token == WIKI_TOKEN_NEWPAR) {
      int num = 0, c = 0;
      while(l && l->token == WIKI_TOKEN_EQUAL_SIGN) {
	 num++;
	 l = l->next;
      }
      if(num > 6) num = 6;
      if(l) {
	 n = l;
	 while(n && !(
		  n->token == WIKI_TOKEN_NEWLINE ||  
		  n->token == WIKI_TOKEN_NEWPAR ||  
		  n->token == WIKI_TOKEN_EOF )) {
	    if(n->token == WIKI_TOKEN_EQUAL_SIGN) 
	       c++;
	    else if(!(n->token == WIKI_TOKEN_WHITESPACE/* || n->token == WIKI_TOKEN_TEXT*/))
	       c = 0;
	    n = n->next;
	 } 
	 if(c > 6) c = 6;
	 l = l->prev;
      }
      if(num > 0 && c == num) {
	 char buf[3] = "h1"; sprintf(buf, "h%d", num);
	 node = wp->node;
	 while(node && ! (
		  NODE_TAG_IS(node, "td")
		  || NODE_TAG_IS(node, "th")
		  || NODE_TAG_IS(node, "div")
		  || NODE_TAG_IS(node, "root"))) {
	    node = node->parent;
	 }

	 if(node)
	    wp->node = node;

	 if(wp->node && wp->toc == EINA_TRUE) {
	    wiki_parser_node_add(wp->node, "tableofcontent");
	    wp->toc = EINA_FALSE;
	 }
	 node = wiki_parser_node_add(wp->node, buf);
	 wp->node = node;
	 wp->in_header = node;
      } else
	 wiki_parser_text_add(wp->node, ICU_STR_EQUAL_SIGN, 1, EINA_TRUE);
   } else if (wp->in_header) {
      int num = 0;
      n = l;
      wp->in_header = NULL;
      while(n && n->token == WIKI_TOKEN_EQUAL_SIGN) {
	 num++;
	 n = n->next;
      }
      while(n && (n->token == WIKI_TOKEN_WHITESPACE || n->token == WIKI_TOKEN_TEXT)) {
	 n = n->next;
      }
      if(num && (!n || (n && (n->token == WIKI_TOKEN_NEWLINE 
	       || n->token == WIKI_TOKEN_NEWPAR
	       || n->token == WIKI_TOKEN_EOF)))) {
	 if(num > 6) num = 6;
	 char buf[3]; 
	 sprintf(buf, "h%d", num);
	 node = wp->node;
	 if(! node)
	    node = wp->root;
	 while(node && ! NODE_TAG_IS(node, buf)) {
	    node = node->parent;
	 }
	 if(node && node->parent) {
	    wp->node = node->parent;
	    l = (n) ? n->prev : n;
	 } else {
	    wiki_parser_text_add(wp->node, ICU_STR_EQUAL_SIGN, 1, EINA_TRUE);
	 }
      } else {
	 wiki_parser_text_add(wp->node, ICU_STR_EQUAL_SIGN, 1, EINA_TRUE);
      }
   } else {
      wiki_parser_text_add(wp->node, wb->buf + l->offset, l->size, EINA_TRUE);
   }

   return l;
}

xmlNodePtr 
_wiki_parser_convert_xhtml(Wiki_Parser *wp, UChar *buf, size_t bufsize, int *error, int flags)
{
   Wiki_Buffer *wb = NULL;
   xmlNodePtr  node = NULL;

   PARSE_TRACE();

   if(! wp) return NULL;

   node = _wiki_parser_xmldoc_init(wp);
   if(! node) return NULL;

   if(! buf || bufsize <= 0) return NULL;

   wb = _wiki_buffer_new(buf, bufsize);
   if(! wb) return NULL;

   _wiki_parser_convert_xhtml_part(wp, wb, flags);

   _wiki_buffer_free(wb);

   return wp->root;
}

xmlNodePtr 
_wiki_parser_convert_xhtml_part(Wiki_Parser *wp, Wiki_Buffer *wb, int flags)
{
   Wiki_Lexer *l, *n;
   xmlNodePtr  node = NULL;

   PARSE_TRACE();

   if(! wp) return NULL;
   if(! wb) return NULL;

   wiki_lexer_dump_with_buf(wb->lexer, wb->buf, wb->size);
   l = wb->lexer;
   while(l && l->offset + l->size <= wb->size && wp->root) {
      if(! wp->node) {
	 wp->node = wp->root;
      }

      /* Don't make pre if in subpart */
      if(flags && l->token == WIKI_TOKEN_WHITESPACE
	    && (l == wb->lexer
	       || l->prev->token == WIKI_TOKEN_NEWLINE
	       || l->prev->token == WIKI_TOKEN_NEWPAR)) {
	 n = l;
	 while(n && n->token == WIKI_TOKEN_WHITESPACE) {
	    n->token = WIKI_TOKEN_NEWLINE;
	    n = n->next;
	 }
      }

      ulog("PARSER NODE: %s, token: %s\n", wp->node->name,
	    wiki_lexer_token_name_get(l->token));
      if(! wp->node->name) exit(0);

      _wiki_parser_apostrophe_check(wp, wb, l, flags);
      node = _wiki_parser_node_in_paragraph_check(wp, l, flags);
      if(node) 
	 wp->node = node;
      switch (l->token) {
	 case WIKI_TOKEN_SQRE_OPEN:
	    if(l->next) {
	       l = _wiki_parser_sqre_parse(wp, wb, l, flags);
	    } else {
	       wiki_parser_text_add(wp->node, wb->buf + l->offset, l->size, EINA_TRUE);
	    }
	    break;
	 case WIKI_TOKEN_EXCL:
	 case WIKI_TOKEN_PIPE:
	    {
	       int add_col = 0;
	       if(l->prev && (l->prev->token == WIKI_TOKEN_NEWLINE
			|| l->prev->token == WIKI_TOKEN_NEWPAR)) {
		  if(l->next && l->next->token == l->token)
		     l = l->next;
		  add_col = 1;
	       } else if(l->next && l->next->token == l->token) {
		  n = l;
		  while(n && ! ( n->token == WIKI_TOKEN_NEWLINE
			   || n->token == WIKI_TOKEN_NEWPAR))
		     n = n->prev;
		  if(n && n->next && 
			((n->next->token == WIKI_TOKEN_PIPE && l->token == WIKI_TOKEN_PIPE)
			 || (n->next->token == WIKI_TOKEN_EXCL))) {
		     l = l->next;
		     add_col = 1;
		     if(l->token == WIKI_TOKEN_PIPE && n->next->token == WIKI_TOKEN_EXCL)
			l->token = WIKI_TOKEN_EXCL;
		  }
	       } else {
		  n = l;
		  while(n && ! ( n->token == WIKI_TOKEN_NEWLINE
			   || n->token == WIKI_TOKEN_NEWPAR)) {
		     n = n->prev;
		  }
		  if(n && n->next && n->next->next &&
			n->next->token == n->next->next->token &&
			((n->next->token == WIKI_TOKEN_PIPE && l->token == WIKI_TOKEN_PIPE) || 
			 n->next->token == WIKI_TOKEN_EXCL ||
			 n->next->token == WIKI_TOKEN_TAB_CAPTION)) {
		     add_col = 2;
		  } else if(n && n->next  &&
			((n->next->token == WIKI_TOKEN_PIPE && l->token == WIKI_TOKEN_PIPE) || 
			 n->next->token == WIKI_TOKEN_EXCL ||
			 n->next->token == WIKI_TOKEN_TAB_CAPTION)) {
		     add_col = 3;
		  } 
	       }
	    
	       if(add_col == 1) {
		  if(l->token == WIKI_TOKEN_PIPE) {
		     if((node = wiki_parser_node_add(wp->node, "td"))) {
			wp->node = node;
			l = wiki_parser_table_elem_attrib_check(wp, wb, l, "TD");
		     }
		  } else {
		     if((node = wiki_parser_node_add(wp->node, "th"))) {
			wp->node = node;
			l = wiki_parser_table_elem_attrib_check(wp, wb, l, "TH");
		     }
		  }
	       } else if(add_col == 2) {
		  l = wiki_parser_table_elem_attrib_check(wp, wb, l, (const char *) wp->node->name);
	       } else if(add_col == 0) {
		  wiki_parser_text_add(wp->node, wb->buf + l->offset, l->size, EINA_TRUE);
	       } 
	    } 
 	    break;
	 case WIKI_TOKEN_TAB_OPEN:
	    node = wiki_parser_node_add(wp->node, "table");
	    if(node) {
	       wp->node = node;
	    }
	    l = wiki_parser_table_elem_attrib_check(wp, wb, l, "TABLE");

	    break;
	 case WIKI_TOKEN_TAB_CAPTION:
	    node = wiki_parser_node_add(wp->node, "caption");
	    if(node) {
	       wp->node = node;
	       l = wiki_parser_table_elem_attrib_check(wp, wb, l, "CAPTION");
	    }

	    break;
	 case WIKI_TOKEN_TAB_NEWLINE:
	    node = wiki_parser_node_add(wp->node, "tr");
	    if(node) {
     	       wp->node = node;
	    }
	    l = wiki_parser_table_elem_attrib_check(wp, wb, l, "TR");

	    break;
	 case WIKI_TOKEN_TAB_CLOSE:
	    node = wp->node;

	    while(node && ! NODE_TAG_IS(node, "table")) {
	       node = node->parent;
	    }

	    if(node && node->parent && NODE_TAG_IS(node, "table")) {
	       if(node->parent->name)
		  wp->node = node->parent;
	    } else if(wp->node->parent && wp->node->parent->type == XML_ELEMENT_NODE) {
	       wp->node = wp->node->parent;
  	    }

	    break;
	 case WIKI_TOKEN_NEWLINE:
	 case WIKI_TOKEN_NEWPAR:
	    l = _wiki_parser_newline_parse(wp, wb, l, flags);
	    break;
	 case WIKI_TOKEN_ASTERISK:
	 case WIKI_TOKEN_HASH:
	 case WIKI_TOKEN_SEMICOLON:
	 case WIKI_TOKEN_COLON:
	    if(flags == 0 && (! l->prev || (l->prev && (l->prev->token == WIKI_TOKEN_NEWLINE
		  || l->prev->token == WIKI_TOKEN_NEWPAR)))) {
	       l = wiki_parser_parse_list(wp, wb, l);
	       ulog("ACTIVE IN_WIKI_LIST\n");
     	       wp->in_wiki_list = EINA_TRUE;
	    } else {
	       if(l->token == WIKI_TOKEN_COLON && wp->node
		     && NODE_TAG_IS(wp->node, "dt")) {
		  if(wp->node->parent)
		     node = wiki_parser_node_add(wp->node->parent, "dd");
		  if(node)
		     wp->node = node;
	       } else  {
		  wiki_parser_text_add(wp->node, wb->buf + l->offset, l->size, EINA_TRUE);
	       }
	    }
	    break;
	 case WIKI_TOKEN_EQUAL_SIGN:
	    l = _wiki_parser_equal_sign_parse(wp, wb, l, flags);
	    break;
	 case WIKI_TOKEN_DASH:
	    node = wiki_parser_node_add(wp->node, "hr");
	    if(node) {
	       xmlSetProp(node, XML_CONST_CHAR("size"), XML_CONST_CHAR("1"));
	    }
	    break;
	 case WIKI_TOKEN_APOSTROPHE:
	    if(l->next) {
	       l = _wiki_parser_apostrophe_parse(wp, wb , l, flags);
	    } else
	       wiki_parser_text_add(wp->node, wb->buf + l->offset, l->size, EINA_TRUE);
	    break;
	 case WIKI_TOKEN_TAG_OPEN:
	 case WIKI_TOKEN_TAG_OPEN_CLOSE:
	 case WIKI_TOKEN_TAG_CLOSE:
	 case WIKI_TOKEN_PRE_OPEN:
	 case WIKI_TOKEN_PRE_CLOSE:
	 case WIKI_TOKEN_NOWIKI_OPEN:
	 case WIKI_TOKEN_NOWIKI_CLOSE:
	    l = wiki_parser_tag_add(wp, wb, l, 0);
	    break;
	 case WIKI_TOKEN_NOWIKI_OPEN_CLOSE:
	    break;
	 case WIKI_TOKEN_TAG_UNKNOW_OPEN:
	 case WIKI_TOKEN_TAG_UNKNOW_OPEN_CLOSE:
	 case WIKI_TOKEN_TAG_UNKNOW_CLOSE:
	    node = wiki_parser_node_add(wp->node, "span");
	    if(node) {
	       xmlSetProp(node , XML_CONST_CHAR("style"), XML_CONST_CHAR("color: red"));
	       wiki_parser_text_add(node, wb->buf + l->offset, l->size, EINA_TRUE);
	    }
	    break;
	 case WIKI_TOKEN_UNIQ_STRING:
	    if(wp->nowiki) {
	       Wiki_Nowiki_Part *wnp = NULL;
	       UChar *key = u_strndup(wb->buf + l->offset, l->size);
	       if(key) {
		  wnp = icu_eina_hash_find(wp->nowiki, key);
		  free(key);
	       }
	       if(wnp && (wnp->args || wnp->content)) {
		  node = wiki_parser_node_add(wp->node, wnp->tagname);
		  if(node) {
		     if(wnp->args) {
			char *buf = u_strnToUTF8_new(wnp->args, wnp->args_size);
      			buf = wiki_parser_html_attributes_get(node, buf, EINA_TRUE);
			if(buf)
			   free(buf);
		     }
		     if(wnp->content) {
			wiki_parser_text_add(node, wnp->content, wnp->content_size, EINA_FALSE);
		     }
		  }
	       }
	    } else
	       wiki_parser_text_add(node, wb->buf + l->offset, l->size, EINA_TRUE);
	    break;
	 case WIKI_TOKEN_ENTITY:
	    if(1) {
	       UChar *buf = u_strndup(wb->buf + l->offset, l->size);
	       if(buf) {
		  Eina_Bool is_entity = EINA_FALSE;
		  if(buf[1] == '#') 
		    is_entity = EINA_TRUE;
		  else { 
		     char *tmp = u_strToUTF8_new(buf);
		     if(tmp) {
			if(html_entity_to_hex(tmp) != NULL) 
			   is_entity = EINA_TRUE;
			free(tmp);
		     }
		  }

		  wiki_parser_text_add(wp->node, buf, u_strlen(buf), is_entity);

		  free(buf);
	       }
	    }
	    break;
	 case WIKI_TOKEN_AMP:
	    wiki_parser_text_utf8_add(wp->node, "&amp;", 5, EINA_TRUE);
	    break;
	 case WIKI_TOKEN_WHITESPACE:
	    if(wp->in_pre_format)
	       wiki_parser_text_add(wp->node, wb->buf + l->offset, l->size, EINA_TRUE);
	    else
	       wiki_parser_text_utf8_add(wp->node, " ", 1, EINA_TRUE);
	    break;
	 case WIKI_TOKEN_CURL_OPEN:
	 case WIKI_TOKEN_CURL_CLOSE:
	 case WIKI_TOKEN_SQRE_CLOSE:
	 case WIKI_TOKEN_TEXT:
	 default:
	    wiki_parser_text_add(wp->node, wb->buf + l->offset, l->size, EINA_TRUE);
	    break;
      }
      if(l)
	 l = l->next;
   }

   return wp->node;
}

char *wiki_parser_image_url_thumb_set(char *url, int size) 
{
   static const char reinit[] = "/thumb\\1\\2/%dpx-\\2";
   char *re = NULL;
   if(! url) return NULL;

   re = calloc(sizeof(reinit) + INT_STR_SIZE + 1, sizeof(char));
   if(re) {
      sprintf(re, reinit, size);
      url = pcre_replace("/(\\/.\\/..\\/)(.*)/", re, url);
      free(re);
   }
   return url;
}

char *wiki_parser_url_image_get(const Wiki_Parser_Data *wd, const char *file) 
{
   char *url = NULL;
   char *hash = NULL;
   char *base = NULL;
   char *filename = NULL;

   if(! wd) return url;
   if(! file) return url;

   if(wd->image_url_get) {
      url = wd->image_url_get(wd, file);
   }

   return url;

   filename = strdup(file);
   if(filename) {
      filename = pcre_replace("/^ +/", "", filename);
      filename = pcre_replace("/ /", "_", filename);
      hash = get_hash_path(filename, 2);
      free(filename);
   }

   filename = url_desencodeC(file);
   if(filename) {
      if(wd->page_exists && ! strstr(wd->base, "commons")) {
	 char *tmp = calloc(strlen(filename) 
	       + strlen(wiki_parser_namespace_name_get(wd, WIKI_NS_FILE)) + 2, 
	       sizeof(char));
	 if(tmp) {
	    sprintf(tmp, "%s:%s", 
		  wiki_parser_namespace_name_get(wd, WIKI_NS_FILE), filename);
	    ulog("BASE: %s\n", tmp);
	    if(wd->page_exists(wd, tmp)) {
	       free(tmp);
	       tmp = strdup(wd->sitename);
	       if(tmp) {
		  tmp = pcre_replace("/.*\\.([^.]*)\\..*/", "\\1", tmp);
		  asprintf(&base, "%s/%s", tmp, wd->lang);
	       }
	    }
	    if(tmp)
	       free(tmp);
	 }
      }
      free(filename);
   }

   if(! base)
      base = strdup("wikipedia/commons");
   ulog("BASE: %s\n", base);

   filename = url_encodeC(file);
   if(! filename) return url;

   url = calloc(strlen(wd->image_url) + strlen(base) 
	 + strlen(hash) + strlen(filename) + 5, sizeof(char));
   if(! url) return url;

   strcpy(url, wd->image_url);
   strcat(url, "/");
   if(base) {
      strcat(url, base);
      strcat(url, "/");
   }
   if(hash) {
      strcat(url, hash);
      strcat(url, "/");
      free(hash);
   }
   strcat(url, filename);
   free(filename);
   
   url = pcre_replace("/( |\\+)/", "_", url);
   url = pcre_replace("/%20/", "_", url);
   url = pcre_replace("/%2E/", ".", url);

   if(base) free(base);

   return url;
}

char *wiki_parser_url_interwiki_get(Wiki_Parser *wp, const char *pagename, const char *site, const char *lang, int *exists)
{
   char *url = NULL, *title = NULL, *arg = NULL, *p = NULL;
   char *sitename = NULL;
   size_t size = 0;
   int skip = 0;
   const char empty[] = "Main Page";

   if(! wp) return NULL;
   if(! wp->wd) return NULL;
   if(! wp->wd->servername) return NULL;

   if(wp->wd->no_page_exists == EINA_TRUE)
      *exists = 1;
   else
      *exists = 0;

   if(pagename != NULL && strcmp(pagename, "") != 0) {
      if(pagename[0] == ':') skip = 1;
      p = strstr(pagename, "#");

      title = strdup(pagename + skip);
   } else {
      title = strdup(empty);
   }

   if(! title) return NULL;

   title = pcre_replace("/#.*/", "", title);
   if(! title) return NULL;

   title = pcre_replace("/(&nbsp;|_)/", " ", title);
   if(! title) return NULL;

   size = strlen(title);
   if(size > 0 && title[size - 1] == '/') {
      title[size - 1] = '\0';
      size--;
   }

   if(p) {
      arg = wiki_parser_anchor_encode(p + 1);
   }

   if(wp->wd->sitename_from_base) 
      sitename = wp->wd->sitename_from_base(wp->wd, site, lang);

   if(! sitename && site)
      sitename = strdup(site);
   else if(! sitename)
      sitename = strdup(wp->wd->sitename);

   if(strcmp(title, "") != 0 && *exists != 1
	 && wp->wd->page_exists_interwiki) {
      char *pname = url_desencodeC(title);
      if(pname) {
	 if(wp->wd->page_exists_interwiki(wp->wd, pname, sitename, lang))
	    *exists = 1;
	 else {
	    char *tmp = NULL;
	    asprintf(&tmp, "%s:%s", sitename, title);
	    if(tmp) {
	       if(wp->wd->page_exists_interwiki(wp->wd, tmp, sitename, lang)) {
		  *exists = 1;
		  free(title);
		  title = tmp;
	       } else
		  free(tmp);
	    }
	 }
	 free(pname);
      }
   }

   if(sitename) {
      char *tmp = url_encodeC(sitename);
      if(tmp) {
	 free(sitename);
	 sitename = tmp;
      }
   }

   if(arg) {
      asprintf(&url, "?wikimedia=%s&lang=%s&entry=%s&raw=%d#%s",
	    sitename, lang, title, wp->wd->output_mode, arg);
      free(arg);
   } else {
      asprintf(&url, "?wikimedia=%s&lang=%s&entry=%s&raw=%d",
	    sitename, lang, title, wp->wd->output_mode);
   }

   free(title);
   if(sitename)
      free(sitename);

   return url;
}

char *wiki_parser_url_origin_get(Wiki_Parser *wp, int ns, const char *pagename)
{
   char *url = NULL, *title = NULL, *arg = NULL, *base = NULL, *p = NULL;
   int skip = 0, size = 0;

   if(! wp) return NULL;
   if(! wp->wd) return NULL;
   if(! wp->wd->base_origin) return NULL;
   if(! pagename) return NULL;

   if(pagename[0] == ':') skip = 1;

   title = strdup(pagename + skip);
   if(! title) return NULL;

   title = pcre_replace("/#.*/", "", title);
   if(! title) return NULL;

   title = pcre_replace("/(&nbsp;|_)/", " ", title);
   if(! title) return NULL;

   char *tmp = url_desencodeC(title);
   if(tmp) {
      free(title);
      title = tmp;
   }

   size = strlen(title);
   if(size > 0 && title[size - 1] == '/') {
      title[size - 1] = '\0';
      size--;
   }

   if(( p = strstr(pagename, "#"))) {
      arg = wiki_parser_anchor_encode(p + 1);
   }

   if(wp->wd->base_origin(wp->wd))
      base = strdup(wp->wd->base_origin(wp->wd));

   if(base) {
      char *p = rindex(base, '/');
      if(p) 
	 *p = '\0';
   }
   if(ns != WIKI_NS_MAIN) {
      title = wiki_parser_utitle_case_set(wp->wd, title);
      const char *nsname = wiki_parser_namespace_name_get(wp->wd, ns);
	 asprintf(&url, "%s/%s:%s",
	       base, nsname, title);
   } else {
      title = wiki_parser_utitle_case_set(wp->wd, title);
      if(arg) {
	 asprintf(&url, "%s/%s#%s",
	       base, title, arg);
      } else {
	 asprintf(&url, "%s/%s",
	       base, title);
      }
   }

   if(title)
      free(title);
   if(arg)
      free(arg);
   if(base)
      free(base);

   return url;
}

char *wiki_parser_url_internal_get(Wiki_Parser *wp, int ns, const char *pagename, int raw, int *exists)
{
   char *url = NULL, *title = NULL, *arg = NULL, *p = NULL;
   char *sitename = NULL;
   int skip = 0, size = 0;
   int output_mode = raw;

   if(! wp) return NULL;
   if(! wp->wd) return NULL;
   if(! wp->wd->servername) return NULL;
   if(! pagename) return NULL;

   if(pagename[0] == ':') skip = 1;
   if(wp->wd->no_page_exists == EINA_TRUE)
      *exists = 1;
   else
      *exists = 0;

   title = strdup(pagename + skip);
   if(! title) return NULL;

   title = pcre_replace("/\\/+/", "/", title);
   title = pcre_replace("/#.*/", "", title);
   if(! title) return NULL;

   title = pcre_replace("/(&nbsp;|_)/", " ", title);
   if(! title) return NULL;

   title = pcre_replace("/( +)/", " ", title);
   if(! title) return NULL;

   char *tmp = url_desencodeC(title);
   if(tmp) {
      free(title);
      title = tmp;
   }

   size = strlen(title);
   if(size > 0 && title[size - 1] == '/') {
      title[size - 1] = '\0';
      size--;
   }
   
   sitename = url_encodeC(wp->wd->sitename);

   if(strstr(title, "..") || title[0] == '/') {
      const char *path = wiki_parser_fullpagename_get(wp);
      char *tmp = parser_title_rel2abs(wp->wd, title, path);
      if(tmp) {
	 free(title);
	 title = tmp;
      }
   }

   if((p = strstr(pagename, "#"))) {
      arg = wiki_parser_anchor_encode(p + 1);
   }

   if(raw == WIKI_OUTPUT_DEFAULT 
         && wp->wd->output_mode > WIKI_OUTPUT_SOURCE_TEMPLATE
         && raw != wp->wd->output_mode)
      output_mode = wp->wd->output_mode;

   if(strlen(title) == 0 && arg) {
      *exists = 1;
      asprintf(&url, "#%s", arg);
      free(title);
      title = NULL;
   } else if(ns != WIKI_NS_MAIN || ns != wp->wd->ns) {
      const char *nsname = wiki_parser_namespace_name_get(wp->wd, ns);
      p = index(title, ':');
      if(nsname && strlen(nsname) > 0 && p) {
	 char *tmp = strdup(p + 1);
	 if(tmp) {
	    free(title);
	    title = tmp;
	 }
      }
      title = wiki_parser_utitle_case_set(wp->wd, title);
      if(nsname) {
	 char *pname = NULL;
	 if(strlen(nsname) > 0) {
	    asprintf(&pname, "%s:%s", nsname, title);
	    if(pname) {
	       free(title);
	       title = pname;
	    }
	 }
      }
   } else {
      title = wiki_parser_utitle_case_set(wp->wd, title);
   }

   if(wp->wd->no_page_exists == EINA_FALSE && *exists != 1
	 && wp->wd->page_exists && wp->wd->page_exists(wp->wd, title))
      *exists = 1;

   if(title) {
      char *encode = url_encodeC(title);
      if(encode) {
	 free(title);
	 title = encode;
      }
   }
      
   if(title && arg) {
      asprintf(&url, "?wikimedia=%s&lang=%s&entry=%s&raw=%d#%s",
	    sitename, wp->wd->lang, title, output_mode, arg);
   } else if(title) {
      asprintf(&url, "?wikimedia=%s&lang=%s&entry=%s&raw=%d",
	    sitename, wp->wd->lang, title, output_mode);
   }

   if(title)
      free(title);
   if(sitename)
      free(sitename);
   if(arg)
      free(arg);

   return url;
}

static Eina_Bool wiki_parser_html_tag_extension_parse(const Eina_Hash *h, const void *key, void *data, void *fdata)
{
   Wiki_Parser *wp = fdata;
   Wiki_Parser_Tag_Extension *tag = data;
   xmlXPathContextPtr ctx = NULL;
   xmlXPathObjectPtr obj = NULL;
   Eina_List *list = NULL, *l = NULL;
   char *search = NULL;
   const char *k = key;

   if(! tag || tag->parse == NULL) return EINA_TRUE;
   if(! wp || ! wp->wd) return EINA_TRUE;
   if(! wp->xml) return EINA_TRUE;

   xmlXPathInit();

   ctx = xmlXPathNewContext(wp->xml);
   if(! ctx) return EINA_TRUE;

   ulog("TAG EXT: %s\n", k);
   search = calloc(strlen(k) + 3, sizeof(char));
   if(search) {
      sprintf(search, "//%s", k);
      obj = xmlXPathEvalExpression(XML_CHAR(search), ctx);
      if(! obj) {
	 ulog("Tag extension: failed to eval xpath expr: %s\n", search);
      }
      free(search);
   }

   if(obj) {
      xmlNodeSetPtr nodes = obj->nodesetval;
      if(nodes) {
	 int i = 0; 
	 for(i = 0; i < nodes->nodeNr; i++) {
	    if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
	       list = eina_list_append(list, nodes->nodeTab[i]);
	    }
	 }
      }
      xmlXPathFreeObject(obj);
   }
   xmlXPathFreeContext(ctx);

   if(list) {
      xmlNodePtr node;
      EINA_LIST_FOREACH(list, l, node) {
	 xmlNodePtr parent = NULL, prev = NULL, next = NULL, res = NULL;
	 if(node->prev)
	    prev = node->prev;
	 else if(node->next)
  	    next = node->next;

   	 parent = node->parent;
	 while(parent) {
	    if(parent->name && strcmp((char *)parent->name, "pre") == 0) {
	       node = NULL;
	       break;
	    }
	    parent = parent->parent;
	 }

   	 if(node) {
	    parent = node->parent;

  	    xmlUnlinkNode(node);
	    res = tag->parse(wp->wd, node);

  	    if(res) {
	       if(prev) 
		  xmlAddNextSibling(prev, res);
	       else if(next)
		  xmlAddPrevSibling(next, res);
	       else if(parent)
		  xmlAddChild(parent, res);
	       else
		  xmlFreeNode(res);
	    }
	 }
      }
      EINA_LIST_FREE(list, node);
   }

   return EINA_TRUE;
}

xmlNodePtr 
wiki_parser_convert_xhtml(Wiki_Parser *wp, int *error, int flags)
{
   xmlNodePtr root = NULL;
   const Eina_Hash *tags = wiki_lexer_tags_get();

//   return _wiki_parser_xmldoc_init(wp);
   if(! wp) return NULL;

   root = _wiki_parser_convert_xhtml(wp, wp->buf, wp->size, error, flags);
   if(root && tags && flags == 0) {
      eina_hash_foreach(tags, wiki_parser_html_tag_extension_parse, wp);
   }

   return root;
}
