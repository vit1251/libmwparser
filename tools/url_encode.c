#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>

static char *url_desencode_size(const char *url, size_t size);
static char *url_desencode(const char *url);
static char *url_encode_size(const char *url, size_t size);
static int is_hexdigit(unsigned char c); 
static char *url_encode(const char *url);

static char *url_encode(const char *url)
{
   return url_encode_size(url, (url) ? strlen(url) : 0);
}

static int is_hexdigit(unsigned char c) 
{
   if((c >= '0' && c <= '9') 
	 || (c >= 'A' && c <= 'Z')
	 || (c >= 'a' && c <= 'z'))
      return 1;
   else
      return 0;
}

static char *url_encode_size(const char *url, size_t size)
{
   char *content = NULL;
   char *tmp = NULL;
   unsigned int i = 0, pos = 0;
   size_t s = size;

   if(! url || size <= 0) return NULL;

   tmp = url_desencode_size(url, size);
   if(! tmp) return NULL;

   s = strlen(tmp);
   content = calloc((s * 4) + 1, sizeof(char));
   if(! content) return NULL;

   while(i < s) {
      unsigned char c = tmp[i];
      if(tmp[i] == ' ') {
	 content[pos] = '_';
	 pos++;
      } else if(tmp[i] == '.' 
	    || tmp[i] == ':'
	    || tmp[i] == '-'
	    || tmp[i] == '_') {
	 content[pos] = tmp[i];
	 pos++;
      } else if(! is_hexdigit(c)) {
	 snprintf(content + pos , 4, "%%%02X", c);
	 pos += 3;
      } else {
	 content[pos] = tmp[i];
	 pos++;
      }
      i++;
   }

   free(tmp);
   
   return content;
}

static char *url_desencode(const char *url)
{
   return url_desencode_size(url, (url) ? strlen(url) : 0);
}

static char *url_desencode_size(const char *url, size_t size)
{
   char *content = NULL;
   unsigned int i = 0, pos = 0;

   if(! url || size <= 0) return NULL;

   content = calloc(size + 1, sizeof(char));
   if(! content) return NULL;

   while(url[i] && i < size) {
      if(url[i] == '_') {
	 content[pos] = ' ';
	 pos++;
      } else if(i + 2 < size && url[i] == '%' 
	    && is_hexdigit(url[i + 1]) && is_hexdigit(url[i + 2])) {
	 char tmp[3];
	 tmp[0] = url[i + 1];
	 tmp[1] = url[i + 2];
	 tmp[2] = '\0';
	 content[pos] = strtoul(tmp, NULL, 16);
	 pos++;
	 i += 2;
      } else {
	 content[pos] = url[i];
	 pos++;
      }

      i++;
   }

   return content;
}

void usage(void)
{
   printf("Usage: (d)encode or (un)escape char from stdin\n\
         url_encode [-h|--help|-d]\n\
         url_dencode [-h|--help]\n\n");
}

#define BUF_SIZE 1024
int main(int argc, char *argv[]) 
{
   bool dencode = false;
   char buf[BUF_SIZE + 1];
   size_t r = 0;

   memset(buf, 0x0, sizeof(buf));
   if(argc > 1) {
      int i = 0;
      for(i = 1; i < argc; i++) {
         if(strcmp(argv[i], "-d") == 0)
           dencode = true;
         else if(strncmp(argv[i], "-h", 2) == 0
              || strncmp(argv[i], "--h", 3) == 0) {
            usage();
            return 0;
         } else {
            usage();
            return 1;
         } 
      }
   }

   if(dencode == false) {
      char *tmp = strdup(argv[0]);
      if(tmp) {
         char *b = basename(tmp);
         if(b && strcmp(b, "url_dencode") == 0)
            dencode = true;
         free(tmp);
      }
   }

   while(! feof(stdin) &&
         (r = fread(buf, sizeof(char), BUF_SIZE, stdin)) > 0) {
      char *tmp = NULL;
      if(dencode == true) {
         tmp = url_desencode_size(buf, r);
      } else {
         tmp = url_encode_size(buf, r);
      }
      if(tmp) {
         printf("%s", tmp);
         free(tmp);
      } else {
         printf("%s", buf);
      }
      memset(buf, 0x0, sizeof(buf));
   }

   printf("\n");
   return 0;
}
