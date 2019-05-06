#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/utrans.h>
#include <unicode/ustdio.h>

#include "parser_extension.h"
#include "parser_extension_private.h"

#include "pcre_replace.h"

static UChar *parser_extension_module_stats_not_implemented(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;

   if(! root) return NULL;
   if(! args) return NULL;

//   content = parser_extension_nth_get(root, pargs, args, 0);
   content = u_strdupC("0");

   return content;
}

static UChar *parser_extension_module_pages_in_category(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL, *tmp = NULL;
   unsigned int res = 0;

   if(! root) return NULL;
   if(! root->wd) return NULL;
   if(! root->wd->stats_npages_in_category) return NULL;

   tmp = parser_extension_nth_get(root, pargs, args, 0);
   if(tmp) {
      char *title = u_strToUTF8_new(tmp);
      if(title) {
	 const char *ns = wiki_parser_namespace_name_get(root->wd, WIKI_NS_CATEGORY);
	 char *p = NULL;
	 if((p = strstr(title, ns)) == NULL || p != title) {
	    title = pcre_replace("/^/", ":", title);
	    title = pcre_replace("/^/", ns, title);
	 }
	 res = root->wd->stats_npages_in_category(root->wd, title);
	 free(title);
      }
      free(tmp);
   }

   content = calloc(INT_STR_SIZE, sizeof(UChar));
   if(content) {
      u_sprintf(content, "%d", res);
   }

   return content;
}

static UChar *parser_extension_module_stats_number(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   UChar *content = NULL;
   UChar *name = data;
   Wiki_Stat stat = WIKI_STAT_UNDEFINED;
   char *tmp = NULL;
   int res = 0;

   if(! root) return NULL;
   if(! root->wd) return NULL;
   if(! root->wd->stats_get) return NULL;

   if(icu_search("^NUMBEROFARTICLES", name, 0))
      stat = WIKI_STAT_NUMBER_OF_ARCTICLES;
   else if(icu_search("^NUMBEROFPAGES", name, 0))
      stat = WIKI_STAT_NUMBER_OF_PAGES;
   else if(icu_search("^NUMBEROFFILES", name, 0))
      stat = WIKI_STAT_NUMBER_OF_FILES;
   else if(icu_search("^NUMBEROFEDITS", name, 0))
      stat = WIKI_STAT_NUMBER_OF_EDITS;
   else if(icu_search("^NUMBEROFVIEWS", name, 0))
      stat = WIKI_STAT_NUMBER_OF_VIEWS;
   else if(icu_search("^NUMBEROFUSERS", name, 0))
      stat = WIKI_STAT_NUMBER_OF_USERS;
   else if(icu_search("^NUMBEROFADMINS", name, 0))
      stat = WIKI_STAT_NUMBER_OF_ADMINS;
   else if(icu_search("^NUMBEROFACTIVEUSERS", name, 0))
      stat = WIKI_STAT_NUMBER_OF_ACTIVE_USERS;

   if(stat != WIKI_STAT_UNDEFINED) 
      res = root->wd->stats_get(root->wd, stat);

   asprintf(&tmp, "%d", res);
   if(tmp) {
      content = u_strdupC(tmp);
      free(tmp);
   }

   return content;
}

void wiki_parser_extension_stats_init()
{
   wiki_parser_extension_module_register("PAGESINCATEGORY", parser_extension_module_pages_in_category);
   wiki_parser_extension_module_register("PAGESINCAT", parser_extension_module_pages_in_category);
   wiki_parser_extension_module_register("NUMBERINGROUP", parser_extension_module_stats_not_implemented);
   wiki_parser_extension_module_register("NUMINGROUP", parser_extension_module_stats_not_implemented);
   wiki_parser_extension_module_register("PAGESINNS", parser_extension_module_stats_not_implemented);
   wiki_parser_extension_module_register("PAGESINNAMESPACE", parser_extension_module_stats_not_implemented);
   wiki_parser_extension_module_register("NUMBEROFARTICLES", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFARTICLES:R", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFPAGES", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFPAGES:R", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFFILES", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFFILES:R", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFEDITS", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFEDITS:R", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFVIEWS", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFVIEWS:R", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFUSERS", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFUSERS:R", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFADMINS", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFADMINS:R", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFACTIVEUSERS", parser_extension_module_stats_number);
   wiki_parser_extension_module_register("NUMBEROFACTIVEUSERS:R", parser_extension_module_stats_number);
//   wiki_parser_extension_module_register("", parser_extension_module_stats_not_implemented);
}
