#ifndef _PARSE_SQL_H
#define _PARSE_SQL_H

void read_sql_file(const char *filename, const char *search, 
      Eina_Bool (*line_callback)(void *data, Eina_List *list), void *data);

#endif
