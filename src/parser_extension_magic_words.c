#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/ustdio.h>

#include "util.h"
#include "parser_extension.h"
#include "parser_extension_private.h"

static UChar *parser_extension_module_fullpagename(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data);

static UChar *url_encode_content(UChar *content, const UChar *ename)
{
   UChar *tmp = NULL;

   if(content && ename && icu_search("EE$", ename, 0)) {
      tmp = url_encode(content);
      free(content);
      content = tmp;
   }

   return content;
}

static UChar *parser_extension_module_protectionlevel(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return u_strdupC("autoconfirmed");
}

static UChar *parser_extension_module_current_version(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return u_strdupC(WIKI_PARSER_VERSION);
}

static UChar *parser_extension_module_pagesize(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   /* TODO: need implement */
   return u_strdupC("0");
}

static UChar *parser_extension_module_sitename(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! root->wd) return NULL;

   content = u_strdupC(root->wd->sitename);

   return content;
}

static UChar *parser_extension_module_servername(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! root->wd) return NULL;

   content = u_strdupC(root->wd->servername);

   return content;
}

static UChar *parser_extension_module_server(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;

   if(! root) return NULL;
   if(! root->wd) return NULL;

   tmp = u_strdupC(root->wd->servername);
   if(tmp) {
      const char http[] = "http://";
      content = calloc(sizeof(http) + u_strlen(tmp), sizeof(UChar));
      if(content)
	 u_sprintf(content, "%s%S", http, tmp);
      free(tmp);
   }

   return content;
}

static UChar *parser_extension_module_contentlang(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! root->wd) return NULL;

   content = u_strdupC(root->wd->lang);

   return content;
}

static UChar *parser_extension_module_namespace(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   const UChar *ename = data;

   if(! root) return NULL;
   if(! root->wd) return NULL;

   if(args && args->list && eina_list_count(args->list) > 0) {
      tmp = parser_extension_nth_get(root, pargs, args, 0);
      if(tmp) {
	 Wiki_Title *wt = parser_title_part(root->wd, tmp);
	 if(wt) {
	    if(wt->ns)
	       content = u_strdup(wt->ns);
	    parser_title_free(wt);
	 }

	 free(tmp);
      }
   } else {
      content = u_strdupC(wiki_parser_namespace_name_get(root->wd, root->wd->ns));
   }

   content = url_encode_content(content, ename);

   return content;
}

static UChar *parser_extension_module_talkspace(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   const UChar *ename = data;
   int skip = 0;

   if(! root) return NULL;
   if(! root->wd) return NULL;

   if(args && args->list && eina_list_count(args->list) > 0) {
      tmp = parser_extension_nth_get(root, pargs, args, 0);
      if(tmp) {
	 if(tmp[0] == ':')
	    skip = 1;

	 UChar *p = u_strstr(tmp + skip, ICU_STR_COLON);
	 if(p) {
	    content = u_strndup(tmp + skip, p - (tmp + skip));
	 }

	 free(tmp);
      }
   } else {
      int nsid = (root->wd->ns % 2 == 1) ? root->wd->ns : root->wd->ns + 1;
      content = u_strdupC(wiki_parser_namespace_name_get(root->wd, nsid));
   }

   content = url_encode_content(content, ename);

   return content;
}

static UChar *parser_extension_module_pagename(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   Wiki_Title *wt = NULL;
   const UChar *ename = data;

   if(! root) return NULL;
   if(! root->wd) return NULL;

   if(args && args->list && eina_list_count(args->list) > 0) {
      tmp = parser_extension_nth_get(root, pargs, args, 0);
      if(tmp) {
	 wt = parser_title_part(root->wd, tmp);
	 free(tmp);
      }
   } else {
      tmp = u_strdupC(root->wd->pagename);
      if(tmp) {
	 wt = parser_title_part(root->wd, tmp);
	 free(tmp);
      }
   }

   if(wt && wt->title) {
      content = u_strdup(wt->title);
   }

   if(wt) {
      parser_title_free(wt);
   }

   content = url_encode_content(content, ename);

   return content;
}

static UChar *parser_extension_module_basepagename(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   const UChar *ename = data;

   if(! root) return NULL;
   if(! root->wd) return NULL;

   if(args && eina_list_count(args->list) > 0)
      tmp = parser_extension_nth_get(root, pargs, args, 0);
   else
      tmp = parser_extension_module_fullpagename(root, pargs, args, data);
   if(tmp) {
      Wiki_Title *wt = parser_title_part(root->wd, tmp);
      if(wt) {
	 UChar *p = NULL;
	 /* see
	  * wikimedia=meta.wikimedia.org&lang=en&entry=Help:Link&raw=5#Subpage_feature */
	 if(wt->nsid == WIKI_NS_SPECIAL || wt->nsid == WIKI_NS_USER || wt->nsid == WIKI_NS_PROJECT
	       || wt->nsid % 2 == 1 
	       || wt->nsid > WIKI_NS_CATEGORY
	       || (wt->nsid == 0 && strcmp(root->wd->sitename, "meta.wikimedia.org") == 0)) {
	    p = u_strFindLast(wt->title, -1, ICU_STR_SLASH, -1);
	    if(p) 
	       content = u_strndup(wt->title, p - wt->title);
	 }
	 if(!p && ! content)
	    content = u_strdup(wt->title);
	 parser_title_free(wt);
      }
      free(tmp);
   }
   ulog("BASEPAGENAME: %S\n", content);

   content = url_encode_content(content, ename);

   return content;
}

static UChar *parser_extension_module_subpagename(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   const UChar *ename = data;

   if(! root) return NULL;
   if(! root->wd) return NULL;

   tmp = parser_extension_module_pagename(root, pargs, args, data);
   if(tmp) {
      if(root->wd->ns != 0) {
	 UChar *p = u_strFindLast(tmp, -1, ICU_STR_SLASH, -1);
	 if(p) {
	    content = u_strdup(p + 1);
	 }
      }
      free(tmp);
   }

   content = url_encode_content(content, ename);

   return content;
}

static UChar *parser_extension_module_fullpagename(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   const UChar *ename = data;

   if(! root) return NULL;
   if(! root->wd) return NULL;

   if(args && args->list && eina_list_count(args->list) > 0) {
      tmp = parser_extension_nth_get(root, pargs, args, 0);
      if(tmp && tmp[0] == ':') {
	 content = u_strdup(tmp + 1);
	 free(tmp);
      } else
	 content = tmp;
   } else {
      content = u_strdupC(wiki_parser_fullpagename_get(root));
   }

   content = url_encode_content(content, ename);

   return content;
}

static UChar *parser_extension_module_talkpagename(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp= NULL, *pagename = NULL;
   const char *ns = NULL;
   const UChar *ename = data;
   int nsid = 0;

   if(! root) return NULL;
   if(! root->wd) return NULL;

   if(args && args->list && eina_list_count(args->list) > 0) {
      tmp = parser_extension_nth_get(root, pargs, args, 0);
      if(tmp) {
	 char *pname = u_strToUTF8_new(tmp);
	 if(pname) {
	    nsid = root->wd->page_nsid_get(root->wd, pname);
	    free(pname);
	 }
	 if(nsid % 2 == 0) {
	    ns = wiki_parser_namespace_name_get(root->wd, nsid + 1);
	 } else {
	    ns = wiki_parser_namespace_name_get(root->wd, nsid);
	 }
	 free(tmp);
      }
   } else {
      int nsid = (root->wd->ns % 2 == 1) ? root->wd->ns : root->wd->ns + 1;
      ns = wiki_parser_namespace_name_get(root->wd, nsid);
   }

   if(ns) {
      pagename = parser_extension_module_pagename(root, pargs, args, data);
      if(pagename) {
	 content = calloc(u_strlen(pagename) + strlen(ns) + 2, sizeof(UChar));
	 if(content) {
	    u_sprintf(content, "%s:%S", ns, pagename);
	 }
	 free(pagename);
      }
   }

   content = url_encode_content(content, ename);

   return content;
}

static UChar *parser_extension_module_revision_id(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;
   int rev_id = 0;

   if(! root) return NULL;
   if(! root->wd) return NULL;
   if(! root->wd->page_revision_id_get) return NULL;

   rev_id = root->wd->page_revision_id_get(root->wd);
   if(rev_id) {
      content = calloc(INT_STR_SIZE + 1, sizeof(UChar));
      u_sprintf(content, "%d", rev_id);
   }

   return content;
}

static UChar *parser_extension_module_revision_user(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;
   char *user = NULL;

   if(! root) return NULL;
   if(! root->wd) return NULL;
   if(! root->wd->page_contributor_username_get) return NULL;

   user = root->wd->page_contributor_username_get(root->wd);
   if(user) {
      content = u_strdupC(user);
      free(user);
   }

   return content;
}

static UChar *parser_extension_module_revision_time(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;
   char *name = data;
   char *timestamp = NULL;

   if(! root) return NULL;
   if(! root->wd) return NULL;
   if(! root->wd->page_revision_timestamp_get) return NULL;

   timestamp= root->wd->page_revision_timestamp_get(root->wd);
   if(timestamp) {
      content = u_strdupC(timestamp);
      if(name) {
	 if(strstr(name, "TIMESTAMP")) {
	    content = icu_replace("[-:TZ]", ICU_STR_EMPTY, content, 0);
	 } else if(strstr(name, "DAY2")) {
	    content = icu_replace("[0-9]{4}-[0-9]{2}-([0-9]{2})T.*", ICU_STR_REG_MATCH_1, content, 0);
	 } else if(strstr(name, "DAY")) {
	    content = icu_replace("[0-9]{4}-[0-9]{2}-([0-9]{2})T.*", ICU_STR_REG_MATCH_1, content, 0);
	    content = icu_replace("^0?", ICU_STR_EMPTY, content, 0);
	 } else if(strstr(name, "MONTH")) {
	    content = icu_replace("[0-9]{4}-([0-9]{2})-([0-9]{2})T.*", ICU_STR_REG_MATCH_1, content, 0);
	 } else if(strstr(name, "YEAR")) {
	    content = icu_replace("([0-9]{4})-.*", ICU_STR_REG_MATCH_1, content, 0);
	 }
      }
      free(timestamp);
   }

   return content;
}

void wiki_parser_extension_magic_words_init()
{
   wiki_parser_extension_module_register("SITENAME", parser_extension_module_sitename);
   wiki_parser_extension_module_register("SERVER", parser_extension_module_server);
   wiki_parser_extension_module_register("SERVERNAME", parser_extension_module_servername);
   wiki_parser_extension_module_register("CONTENTLANGUAGE", parser_extension_module_contentlang);
   wiki_parser_extension_module_register("CONTENTLANG", parser_extension_module_contentlang);

   wiki_parser_extension_module_register("DISPLAYTITLE", parser_extension_module_null);
   wiki_parser_extension_module_register("DEFAULTSORTKEY", parser_extension_module_null);
   wiki_parser_extension_module_register("DEFAULTSORT", parser_extension_module_null);
   wiki_parser_extension_module_register("DEFAULTCATEGORYSORT", parser_extension_module_null);

   wiki_parser_extension_module_register("NAMESPACE", parser_extension_module_namespace);
   wiki_parser_extension_module_register("SUBJECTSPACE", parser_extension_module_namespace);
   wiki_parser_extension_module_register("ARTICLESPACE", parser_extension_module_namespace);
   wiki_parser_extension_module_register("TALKSPACE", parser_extension_module_talkspace);

   wiki_parser_extension_module_register("PAGENAME", parser_extension_module_pagename);
   wiki_parser_extension_module_register("BASEPAGENAME", parser_extension_module_basepagename);
   wiki_parser_extension_module_register("SUBPAGENAME", parser_extension_module_subpagename);
   wiki_parser_extension_module_register("SUBJECTPAGENAME", parser_extension_module_fullpagename);
   wiki_parser_extension_module_register("ARTICLEPAGENAME", parser_extension_module_fullpagename);
   wiki_parser_extension_module_register("FULLPAGENAME", parser_extension_module_fullpagename);
   wiki_parser_extension_module_register("TALKPAGENAME", parser_extension_module_talkpagename);

   wiki_parser_extension_module_register("NAMESPACEE", parser_extension_module_namespace);
   wiki_parser_extension_module_register("SUBJECTSPACEE", parser_extension_module_namespace);
   wiki_parser_extension_module_register("ARTICLESPACEE", parser_extension_module_namespace);
   wiki_parser_extension_module_register("TALKSPACEE", parser_extension_module_talkspace);

   wiki_parser_extension_module_register("PAGENAMEE", parser_extension_module_pagename);
   wiki_parser_extension_module_register("BASEPAGENAMEE", parser_extension_module_basepagename);
   wiki_parser_extension_module_register("SUBPAGENAMEE", parser_extension_module_subpagename);
   wiki_parser_extension_module_register("SUBJECTPAGENAMEE", parser_extension_module_fullpagename);
   wiki_parser_extension_module_register("ARTICLEPAGENAMEE", parser_extension_module_fullpagename);
   wiki_parser_extension_module_register("FULLPAGENAMEE", parser_extension_module_fullpagename);
   wiki_parser_extension_module_register("TALKPAGENAMEE", parser_extension_module_talkpagename);

   wiki_parser_extension_module_register("REVISIONID", parser_extension_module_revision_id);
   wiki_parser_extension_module_register("REVISIONUSER", parser_extension_module_revision_user);
   wiki_parser_extension_module_register("REVISIONDAY", parser_extension_module_revision_time);
   wiki_parser_extension_module_register("REVISIONDAY2", parser_extension_module_revision_time);
   wiki_parser_extension_module_register("REVISIONMONTH", parser_extension_module_revision_time);
   wiki_parser_extension_module_register("REVISIONYEAR", parser_extension_module_revision_time);
   wiki_parser_extension_module_register("REVISIONTIMESTAMP", parser_extension_module_revision_time);

   wiki_parser_extension_module_register("CURRENTVERSION", parser_extension_module_current_version);
   wiki_parser_extension_module_register("PAGESIZE", parser_extension_module_pagesize);

   wiki_parser_extension_module_register("DIRMARK", parser_extension_module_null);
   wiki_parser_extension_module_register("DIRECTIONMARK", parser_extension_module_null);
   wiki_parser_extension_module_register("SCRIPTPATH", parser_extension_module_null);
   wiki_parser_extension_module_register("PROTECTIONLEVEL", parser_extension_module_protectionlevel);
}
