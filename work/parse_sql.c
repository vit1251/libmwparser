#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

#include "parse_sql.h"

//#define BUF_READ 1073741824
#define BUF_READ 4096

static const char *read_sql_line(const char *buf,
      Eina_Bool (*line_callback)(void *data, Eina_List *values), 
      void *data, Eina_Bool *cont)
{
   Eina_List *list = NULL;
   size_t len = 0;
   int i = 0;
   int field = 0;
   int in_str = 0;
   int in_values = 0;
   const char *s = buf;
   const char *last = NULL;

   if(! buf) return NULL;
   if(! cont) return NULL;
   len = strlen(buf);

   i = 0;
   while(*cont == EINA_TRUE && buf[i] && i < len) {
      switch(buf[i]) {
	 case '\\':
	    i++;
	    break;
	 case '\'':
	    if(in_str) {
	       in_str--;
	    } else {
	       in_str++;
	    }
	    break;
	 case '(':
	    if(! in_values && ! in_str) {
	       in_values++;
	       s = buf + i + 1;
               last = s;
	    }
	    break;
	 case ')':
	 case ',':
	    if(in_values && ! in_str) {
	       char *tmp = strndup(s, (buf + i) - s);
	       if(tmp) {
		  list = eina_list_append(list, tmp);
	       }
     	       if(buf[i] == ',') {
		  field++;
		  s = buf + i + 1;
	       } else {
		  in_values--;
		  field = 0;
		  if(line_callback) {
		     Eina_Bool c = line_callback(data, list);
		     *cont = c;
		  }
		  else if(list) {
		     Eina_List *l = NULL;
		     EINA_LIST_FOREACH(list, l, tmp) {
			printf("%s\t", tmp);
		     }
		     printf("\n");
		  }
		  if(list) {
		     EINA_LIST_FREE(list, tmp) {
			free(tmp);
		     }
		     list = NULL;
		  }
	       }
	    }
	    break;
         case '\n':
            last = NULL;
            break;
      }
      i++;
   }

   if(list) {
      char *tmp = NULL;
      EINA_LIST_FREE(list, tmp) {
	 free(tmp);
      }
      list = NULL;
   }

   return last;
}

void read_sql_file(const char *filename, const char *search, 
      Eina_Bool (*line_callback)(void *data, Eina_List *list), void *data)
{
   gzFile gz = NULL;
   Eina_Bool cont = EINA_TRUE;

   if(! filename) return;
   if(! search) return;

   gz = gzopen(filename, "rb");
   if(gz) {
      char *buf = calloc(BUF_READ + 1, sizeof(char));
      char *read;
      size_t len = 0; 

      while(cont == EINA_TRUE &&
	    (read = gzgets(gz, buf, BUF_READ)) != Z_NULL) {
	 char *p = NULL;
         const char *last = NULL;
	 if((p = strstr(buf, search))) {
//	    printf("FOUND: %s in : %s\n\n\n\n", search, p);
	    last = read_sql_line(p, line_callback, data, &cont); 
            if(last) {
               len = strlen(last);
               memmove(buf, last, len);
               memset(buf + len, 0x0, BUF_READ - len);
            }
            while(cont == EINA_TRUE && last && read != Z_NULL) {
               read = gzgets(gz, buf + len, BUF_READ - len); 
               if(read != Z_NULL) {
                  p = buf;
                  last = read_sql_line(p, line_callback, data, &cont); 
                  if(last) {
                     len = strlen(last);
                     memmove(buf, last, len);
                     memset(buf + len, 0x0, BUF_READ - len);
                  } else
                     len = 0;
               }
            }
            len = 0;
	 }
//	 buf = calloc(BUF_READ + 1, sizeof(char));
      }

      if(buf)
	 free(buf);
      gzclose(gz);
   }
}

#ifdef PARSE_SQL_MAIN
int main(int argc, char **argv)
{
   eina_init();

   if(argc > 2) {
      read_sql_file(argv[1], argv[2], NULL, NULL);
   } else {
      printf("Usage: %s file.mysql insert_line_match\n", basename(argv[0]));
   }

   eina_shutdown();

   return 0;
}
#endif
