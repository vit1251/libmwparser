#include "parser_extension.h"
#include "parser_extension_private.h"

#include <stdlib.h>
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/utrans.h>
#include <unicode/ustdio.h>

static UChar *parser_extension_module_time_year(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%Y", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_month(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%m", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_monthname(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%B", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_monthnamegen(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%B", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_monthabbrev(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%b", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_day(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%-d", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_day2(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%d", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_dow(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%w", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_dayname(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%A", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_time(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%R", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_hour(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%H", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_week(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%W", WIKI_TIME_UTC, data);
}

static UChar *parser_extension_module_time_timestamp(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%Y%m%d%H%M%S", WIKI_TIME_UTC, data);
}

/* LOCALTIME */
static UChar *parser_extension_module_time_local_year(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%Y", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_month(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%m", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_monthname(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%B", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_monthnamegen(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%B", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_monthabbrev(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%b", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_day(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%d", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_day2(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%-d", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_dow(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%w", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_dayname(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%A", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_time(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%R", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_hour(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%H", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_week(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%W", WIKI_TIME_LOCAL, data);
}

static UChar *parser_extension_module_time_local_timestamp(Wiki_Parser *root, Wiki_Args *pargs, Wiki_Args *args, void *data)
{
   return parser_extension_time(root, NULL, "%Y%m%d%H%M%S", WIKI_TIME_LOCAL, data);
}

void wiki_parser_extension_time_init()
{
   wiki_parser_extension_module_register("CURRENTYEAR", parser_extension_module_time_year);
   wiki_parser_extension_module_register("CURRENTMONTH", parser_extension_module_time_month);
   wiki_parser_extension_module_register("CURRENTMONTH1", parser_extension_module_time_month);
   wiki_parser_extension_module_register("CURRENTMONTH2", parser_extension_module_time_month);
   wiki_parser_extension_module_register("CURRENTMONTHNAME", parser_extension_module_time_monthname);
   wiki_parser_extension_module_register("CURRENTMONTHNAMEGEN", parser_extension_module_time_monthnamegen);
   wiki_parser_extension_module_register("CURRENTMONTHABBREV", parser_extension_module_time_monthabbrev);
   wiki_parser_extension_module_register("CURRENTDAY", parser_extension_module_time_day);
   wiki_parser_extension_module_register("CURRENTDAY2", parser_extension_module_time_day2);
   wiki_parser_extension_module_register("CURRENTDOW", parser_extension_module_time_dow);
   wiki_parser_extension_module_register("CURRENTDAYNAME", parser_extension_module_time_dayname);
   wiki_parser_extension_module_register("CURRENTTIME", parser_extension_module_time_time);
   wiki_parser_extension_module_register("CURRENTHOUR", parser_extension_module_time_hour);
   wiki_parser_extension_module_register("CURRENTWEEK", parser_extension_module_time_week);
   wiki_parser_extension_module_register("CURRENTTIMESTAMP", parser_extension_module_time_timestamp);

   wiki_parser_extension_module_register("LOCALYEAR", parser_extension_module_time_local_year);
   wiki_parser_extension_module_register("LOCALMONTH", parser_extension_module_time_local_month);
   wiki_parser_extension_module_register("LOCALMONTH1", parser_extension_module_time_local_month);
   wiki_parser_extension_module_register("LOCALMONTH2", parser_extension_module_time_local_month);
   wiki_parser_extension_module_register("LOCALMONTHNAME", parser_extension_module_time_local_monthname);
   wiki_parser_extension_module_register("LOCALMONTHNAMEGEN", parser_extension_module_time_local_monthnamegen);
   wiki_parser_extension_module_register("LOCALMONTHABBREV", parser_extension_module_time_local_monthabbrev);
   wiki_parser_extension_module_register("LOCALDAY", parser_extension_module_time_local_day);
   wiki_parser_extension_module_register("LOCALDAY2", parser_extension_module_time_local_day2);
   wiki_parser_extension_module_register("LOCALDOW", parser_extension_module_time_local_dow);
   wiki_parser_extension_module_register("LOCALDAYNAME", parser_extension_module_time_local_dayname);
   wiki_parser_extension_module_register("LOCALTIME", parser_extension_module_time_local_time);
   wiki_parser_extension_module_register("LOCALHOUR", parser_extension_module_time_local_hour);
   wiki_parser_extension_module_register("LOCALWEEK", parser_extension_module_time_local_week);
   wiki_parser_extension_module_register("LOCALTIMESTAMP", parser_extension_module_time_local_timestamp);
}
