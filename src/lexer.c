#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define U_DISABLE_RENAMING 0
#include "lexer.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/uregex.h>

#include <Eina.h>

#include "parser_tag_extensions.h"
#include "entities.h"

static const Eina_Hash *wiki_lexer_tags = NULL;
static Wiki_Lexer *token_add(Wiki_Lexer *lexer, off_t new_offset);

/* Keep it sync with struct _Wiki_Token */
static const char *Wiki_Token_Str[] =
{
   "WIKI_TOKEN_TEXT",
   "WIKI_TOKEN_AMP",
   "WIKI_TOKEN_WHITESPACE",
   "WIKI_TOKEN_NOWIKI_OPEN",
   "WIKI_TOKEN_NOWIKI_CLOSE",
   "WIKI_TOKEN_NOWIKI_OPEN_CLOSE",
   "WIKI_TOKEN_SQRE_OPEN",
   "WIKI_TOKEN_SQRE_CLOSE",
   "WIKI_TOKEN_CURL_OPEN",
   "WIKI_TOKEN_CURL_CLOSE",
   "WIKI_TOKEN_PIPE",
   "WIKI_TOKEN_EXCL",
   "WIKI_TOKEN_APOSTROPHE",
   "WIKI_TOKEN_QUOTE",
   "WIKI_TOKEN_ENTITY",
   "WIKI_TOKEN_UNIQ_STRING",
   "WIKI_TOKEN_NEWLINE",
/* Block tag */
   "WIKI_TOKEN_NEWPAR",
   "WIKI_TOKEN_EQUAL_SIGN",
   "WIKI_TOKEN_ASTERISK",
   "WIKI_TOKEN_COLON",
   "WIKI_TOKEN_SEMICOLON",
   "WIKI_TOKEN_HASH",
   "WIKI_TOKEN_DASH",
   "WIKI_TOKEN_TAB_OPEN",
   "WIKI_TOKEN_TAB_NEWLINE",
   "WIKI_TOKEN_TAB_CAPTION",
   "WIKI_TOKEN_TAB_CLOSE",
   "WIKI_TOKEN_PRE_OPEN",
   "WIKI_TOKEN_PRE_CLOSE",
   "WIKI_TOKEN_TAG_OPEN",
   "WIKI_TOKEN_TAG_OPEN_CLOSE",
   "WIKI_TOKEN_TAG_CLOSE",
   "WIKI_TOKEN_TAG_UNKNOW_OPEN",
   "WIKI_TOKEN_TAG_UNKNOW_OPEN_CLOSE",
   "WIKI_TOKEN_TAG_UNKNOW_CLOSE",
   "WIKI_TOKEN_EOF",
   "WIKI_TOKEN_NONE"
};

const Eina_Hash *wiki_lexer_tags_get(void)
{
   if(! wiki_lexer_tags)
      wiki_lexer_tags = wiki_parser_tag_extension_init();

   return wiki_lexer_tags;
}

void wiki_lexer_tags_free(void)
{
   wiki_lexer_tags = wiki_parser_tag_extension_free();
}

Wiki_Lexer *wiki_lexer_free(Wiki_Lexer *lexer)
{
   Wiki_Lexer *l = lexer;

   while(lexer) {
      l = l->next;
      free(lexer);
      lexer = l;
   }
   return NULL;
}

static Wiki_Lexer *token_add(Wiki_Lexer *lexer, off_t new_offset)
{
   if(! lexer) return NULL;

   lexer->next = malloc(sizeof(Wiki_Lexer));
   lexer->next->prev = lexer;

   lexer = lexer->next;
   lexer->offset = new_offset;
   lexer->size = 0;
   lexer->token = WIKI_TOKEN_TEXT;
   lexer->next = NULL;
   lexer->block = EINA_FALSE;
   lexer->parse = WIKI_PARSER_TAG_CONTENT_PARSE_ALL;
   lexer->tag_ext = NULL;
   
   return lexer;
}


Wiki_Lexer *wiki_lexer_get(Wiki_Lexer *lexer, const UChar *buf, size_t size, off_t start_offset, int *error, Eina_Bool with_angl)
{
   Wiki_Lexer *l = lexer;
   off_t offset = start_offset;
   const UChar *p = buf;
   const char *pnone = NULL;
   int eof = 0, tab_open = 0, nowiki = 0, tag_inline = 0;

   if(! buf) return lexer;

   if(lexer) {
      while(l->next)
	 l = l->next;
   } else {
      lexer = calloc(1, sizeof(Wiki_Lexer));
      if(! lexer) return NULL;
      l = lexer;
   }
 
   if(! wiki_lexer_tags)
      wiki_lexer_tags = wiki_parser_tag_extension_init();

   while(p >= buf && p <= buf + size && l) {
      if(wiki_lexer_tags && *p == '<') {
	 const Wiki_Parser_Tag_Extension *te = NULL;
	 const UChar *e = NULL, *s = NULL;
	 UChar *tag_u = NULL;
	 Wiki_Token open = 0, close = 0, openclose = 0;
	 int closed = 0, end_tag = 0;
	 int brhr = 0;
	 Eina_Bool block = EINA_FALSE;
	 Wiki_Parser_Tag_Content_Type parse = WIKI_PARSER_TAG_CONTENT_PARSE_ALL;

	 s = p + 1;

	 while(0 && s && s < buf + size && u_isWhitespace(*s)) {
	    s++;
	 }

	 if(s && icu_search_size("^[^/a-zA-Z]", s, 1, 0)) {
      	    s = NULL;
	 }

	 if(s && *s == '/') end_tag = 1;
	 if(s) e = s;
	 if(end_tag) {
	    while(e && e < buf + size 
		  && ! (u_isWhitespace(*e) || *e == '>')) {
	       e++;
	    }
	 } else {
	    while(e && e < buf + size 
		  && ! (u_isWhitespace(*e) || *e == '/' || *e == '\\' || *e == '>')) {
	       e++;
	    }
	 }
	 
	 if(s && s < e && s < buf + size && e < buf + size) {
	    if(end_tag)
	       tag_u = u_strndup(s + 1, e - (s + 1));
	    else
	       tag_u = u_strndup(s, e - s);
	    if(tag_u) {
	       char *tag = u_strToUTF8_new(tag_u);
	       tag = strtolower(tag);
	       if(tag) {
		  te = eina_hash_find(wiki_lexer_tags, tag);
		  if(te) {
		     block = te->block;
		     parse = te->type;
		     if(te->type == WIKI_PARSER_TAG_CONTENT_PARSE_NONE) {
			if(end_tag && pnone == te->name) {
			   pnone = NULL;
			   nowiki = 0;
			}
			else if(end_tag == 0 && nowiki == 0) {
			   pnone = te->name;
			   nowiki = 1;
			}
		     }
		     if(strcmp(tag, "nowiki") == 0) {
			open = WIKI_TOKEN_NOWIKI_OPEN;
			openclose = WIKI_TOKEN_NOWIKI_OPEN_CLOSE;
			close = WIKI_TOKEN_NOWIKI_CLOSE;
		     } else if(strcmp(tag, "pre") == 0) {
			open = WIKI_TOKEN_PRE_OPEN;
			openclose = WIKI_TOKEN_TAG_OPEN_CLOSE;
			close = WIKI_TOKEN_PRE_CLOSE;
		     } else {
			open = WIKI_TOKEN_TAG_OPEN;
			openclose = WIKI_TOKEN_TAG_OPEN_CLOSE;
			close = WIKI_TOKEN_TAG_CLOSE;

			if(strcmp(tag, "br") == 0 || strcmp(tag, "hr") == 0)
			   brhr = 1;
		     }
		     if(with_angl == EINA_FALSE && te->type != WIKI_PARSER_TAG_CONTENT_PARSE_NONE) {
			s = e = NULL;
		     }
		  } else {
		     s = e = NULL;
		  }
		  if(tag) {
		     free(tag);
		  }
	       } else {
		  open = WIKI_TOKEN_TAG_UNKNOW_OPEN;
		  openclose = WIKI_TOKEN_TAG_UNKNOW_OPEN_CLOSE;
		  close = WIKI_TOKEN_TAG_UNKNOW_CLOSE;
		  e = s = NULL;
	       }
	    }
	    if(nowiki == 2) {
	       e = s = NULL;
	       ulog("WIKI_TAG: ignore '%S', nowiki: %d, end_tag = %d, %p\n", tag_u, nowiki, end_tag, pnone);
	    } else 
	       ulog("WIKI_TAG: '%S', nowiki: %d, end_tag = %d, %p\n", tag_u, nowiki, end_tag, pnone);
	    free(tag_u);
	 }

	 if(e && *e != '>') {
	    const UChar *c = u_strstr(e, ICU_STR_ANGL_CLOSE);
	    if(c && c > e) {
	       e = c;
	       if(! end_tag && *(c - 1) == '/') { //u_strFindFirst(s, e - s, ICU_STR_SLASH, -1)) {
		  closed = 1;
		  if(nowiki == 1)
		     nowiki = 0;
	       }
	       c = u_strFindLast(p, e - p, ICU_STR_ANGL_OPEN, -1);
	       if(c != p) {
		  int num = 1;
		  /* TODO: check this: when there are tag is in a open tag skip
		   * the tag */
		  // s = e = NULL;
		  c = p + 1;
		  while(c && c < buf + size && num > 0) {
		     c++;
		     if(*c == '<') {
			num++;
			if(u_strncmp(c + 1, ICU_STR_NOWIKI, 6) != 0) 
			   break;
		     }
		     if(*c == '>') num--;
		  }
		  if(c && c < buf + size && num == 0)
		     e = c;
		  else
		     e = s = NULL;
	       }
	    } else
	       e = s = NULL;
	 } 

	 if(e && *e != '>')
	    e = s = NULL;
	 
      	 if(nowiki == 0 && e) {
	    int num = 0;
	    const UChar *c = s;
	    while(c < e) {
	       if(*c == '{') num++;
	       if(*c == '}') num--;
	       c++;
	    }
	    // if(num != 0)
	    //   s = e = NULL;
	 }

	 if(e && s) {
	    e++;
	    if(l->size) {
	       offset = l->offset + l->size;
	       l = token_add(l, offset);
	    }

	    l->block = block;
	    l->parse = parse;
	    l->offset = offset;
	    l->size = e - p;

	    if(te)
	       l->tag_ext = te;

	    if(end_tag) {
	       l->token = close;
	       if(tag_inline && block == EINA_FALSE)
		  tag_inline--;
	    } else if(closed || brhr)
	       l->token = openclose;
	    else {
	       if(block == EINA_FALSE) {
		  tag_inline++;
	       } else
		  tag_inline = 0;
	       l->token = open;
	    }

	    offset = l->offset + l->size;
	    l = token_add(l, offset);

	    p = e;
	    if(nowiki == 1) 
	       nowiki = 2;

	    continue;
	 }
      }
      if(nowiki > 0) {
	 l->size++;
	 p++;
	 continue;
      }
      switch (*p) {
	 case '[':
	 case ']':
	 case '}':
	 case '*':
	 case '#':
	 case '=':
	 case ':':
	 case ';':
//	 case '"':
	 case '\'':

	    if(l->size) {
	       offset = l->offset + l->size;
	       l = token_add(l, offset);
	    }

	    l->offset = offset;
	    l->size = 1;

	    if      (*p == '[')  l->token = WIKI_TOKEN_SQRE_OPEN;
	    else if (*p == ']')  l->token = WIKI_TOKEN_SQRE_CLOSE;
	    else if (*p == '}')  l->token = WIKI_TOKEN_CURL_CLOSE;
  	    else if (*p == '\'') l->token = WIKI_TOKEN_APOSTROPHE;
	    else if (*p == '=')  l->token = WIKI_TOKEN_EQUAL_SIGN;
	    else if (*p == '#')  l->token = WIKI_TOKEN_HASH;
	    else if (*p == ':')  l->token = WIKI_TOKEN_COLON;
	    else if (*p == '*')  l->token = WIKI_TOKEN_ASTERISK;
	    else if (*p == ';')  l->token = WIKI_TOKEN_SEMICOLON;
	    else if (*p == '"')  l->token = WIKI_TOKEN_QUOTE;

  	    offset++;
	    l = token_add(l, offset);
	    break;

	 case '!':
	    /* Is CSS ? */
	    if(icu_search("^! *important", p, UREGEX_CASE_INSENSITIVE)) {
	       l->size++;
	    } else {
	       if(l->size) {
		  offset = l->offset + l->size;
		  l = token_add(l, offset);
	       }

	       l->token = WIKI_TOKEN_EXCL;
	       l->offset = offset;
	       l->size = 1;

	       offset++;
	       l = token_add(l, offset);
	    }
	    break;
	 case '{':
	    if(p + 1 < buf + size) {
	       char c = *(p + 1);
	       if(c == '|' 
		     && ((l->prev && l->prev->token == WIKI_TOKEN_TAB_CLOSE)
		     || (l->prev && l->prev->token == WIKI_TOKEN_COLON)
		     || ((l->prev && l->prev->token == WIKI_TOKEN_NEWLINE)
			|| ((p > buf && (*(p - 1) == '\n'
				 /* 
				  * TEST CASE FOR prev == ':'
			       * http://fr.wikipedia.org/wiki/Mod%C3%A8le:Cases_quantiques_par_sous-couches_%C3%A9lectroniques 
			       * */
			      || (*(p - 1) == ':' && ((p - 1) == buf || *(p - 2) == '\n'))
				 ))) || p == buf))) {
		  if(l->size) {
		     offset = l->offset + l->size;
		     l = token_add(l, offset);
		  }
		  tab_open++;
		  l->token = WIKI_TOKEN_TAB_OPEN;
		  l->block = EINA_TRUE;
		  l->offset = offset;
		  l->size = 2;

		  offset += 2;
		  l = token_add(l, offset);
		  p++;
	       } else if(1 || c == '{') {
		  if(l->size) {
		     offset = l->offset + l->size;
		     l = token_add(l, offset);
		  }
		  l->token = WIKI_TOKEN_CURL_OPEN;
		  l->offset = offset;
		  l->size = 1;

		  offset++;
		  l = token_add(l, offset);
/*		  
		  l->size = 2;

		  offset += 2;
		  l = token_add(l, offset);
		  p++;
		  */
	       } else {
		  /* Use as text */
		  l->size++;
	       }
	    }
	    break;
	 case '|':
	    if(p + 1 < buf + size) {
	       char c = *(p + 1);
	       if(c == '-' && ((p == buf || *(p - 1) == '\n')
			|| (l->prev && l->prev->token == WIKI_TOKEN_NEWLINE))) {
		  if(l->size) {
		     offset = l->offset + l->size;
		     l = token_add(l, offset);
		  }
		  l->token = WIKI_TOKEN_TAB_NEWLINE;
		  l->block = EINA_TRUE;
		  l->offset = offset;
		  l->size = 2;

		  p++;
		  while(p + 1 < buf + size && *(p + 1) == '-') {
		     l->size++;
		     p++;
		  }

		  offset += l->size;
		  l = token_add(l, offset);
	       } else if(c == '+' && ((p == buf || *(p - 1) == '\n')
			|| (l->prev && l->prev->token == WIKI_TOKEN_NEWLINE))) {
		  if(l->size) {
		     offset = l->offset + l->size;
		     l = token_add(l, offset);
		  }
		  l->token = WIKI_TOKEN_TAB_CAPTION;
		  l->block = EINA_TRUE;
		  l->offset = offset;
		  l->size = 2;

		  offset += 2;
		  l = token_add(l, offset);
		  p++;
	       } else if(with_angl && c == '}' && ((p == buf || *(p - 1) == '\n')
			|| (l->prev && l->prev->token == WIKI_TOKEN_NEWLINE))) {
		  if(l->size) {
		     offset = l->offset + l->size;
		     l = token_add(l, offset);
		  }
		  l->token = WIKI_TOKEN_TAB_CLOSE;
		  l->block = EINA_TRUE;
		  l->offset = offset;
		  l->size = 2;
		  if(tab_open > 0)
		     tab_open--;

		  offset += 2;
		  l = token_add(l, offset);
		  p++;
	       } else {
		  if(l->size) {
		     offset = l->offset + l->size;
		     l = token_add(l, offset);
		  }
		  l->token = WIKI_TOKEN_PIPE;
		  l->offset = offset;
		  l->size = 1;

		  offset++;
		  l = token_add(l, offset);
	       }
	    } else {
	       if(l->size) {
		  offset = l->offset + l->size;
		  l = token_add(l, offset);
	       }
	       l->token = WIKI_TOKEN_PIPE;
	       l->offset = offset;
	       l->size = 1;

	       offset++;
	       l = token_add(l, offset);
	    }

	    break;
	 case '-':
	    if((p > buf && *(p - 1) == '\n') || p == buf) {
	       int n = 0;
	       while(*p == '-' && p < buf + size) {
		  n++; 
		  p++;
	       }
	       if(n >= 4) {
		  if(l->size) {
		     offset = l->offset + l->size;
		     l = token_add(l, offset);
		  }

		  l->token = WIKI_TOKEN_DASH;
		  l->offset = offset;
		  l->size = n;
		  offset += n;
		  p--;
		  l = token_add(l, offset);
	       } else {
		  l->size++;
		  p -= (n );
	       }
	    } else
	       l->size++;

	    break;
	 case '&':
	    {
	       const UChar *c = p + 1;
	       c = icu_search("^([#a-zA-Z0-9]+);",c, 0);
	       if(c) {
		  while(*c != ';')
		     c++;
	       }
	       if(c && *c == ';' && *(p + 1) != '#') {
		  char *t = u_strnToUTF8_new(p, (c + 1) - p);
		  if(t) {
		     if(! html_entity_to_hex(t))
			c = NULL;
		     free(t);
		  } else {
		     c = NULL;
		  }
	       }

	       if(c && *c == ';') {
		  if(l->size) {
		     offset = l->offset + l->size;
		     l = token_add(l, offset);
		  }

		  l->token = WIKI_TOKEN_ENTITY;
		  l->offset = offset;
		  l->size = (c + 1) - p;

		  offset = l->offset + l->size;
		  l = token_add(l, offset);
		  p = c;
	       } else {
		  if(l->size) {
		     offset = l->offset + l->size;
		     l = token_add(l, offset);
		  }

		  l->token = WIKI_TOKEN_AMP;
		  l->offset = offset;
		  l->size = 1;

		  offset = l->offset + l->size;
		  l = token_add(l, offset);
	       }
	    }
	    break;
	 case '\n':
	    if(tag_inline && !(*(p + 1) == '|' || *(p + 1) == '{' || *(p + 1) == '\n')) {
	       l->size++;
	    } else {
	       tag_inline = 0;
	       if(l->size) {
		  offset = l->offset + l->size;
		  l = token_add(l, offset);
	       }

	       l->token = WIKI_TOKEN_NEWLINE;
	       l->size = 1;

	       {
		  int n = 0;
		  const UChar *e = p, *s = p;
		  while(e < buf + size && (u_isspace(*e) || *e == '\n')) {
		     if(*e == '\n') {
			n++;
		     s = e;
		     }
		     e++;
		  }

		  if(n > 1 && s) {
		     l->size = (s + 1) - p;
		     p = s;
		     l->token = WIKI_TOKEN_NEWPAR;
		  }
	       }

	       offset = l->offset + l->size;
	       l = token_add(l, offset);
	    }
	    break;
	 case '\x7f':
	    if(icu_search("^\x7fUNIQ[^\x7f]*-QINU\x7f", p, 0)) {
	       const UChar *e = icu_search("\x7f", p + 1, 0);
	       if(l) {
		  if(l->size) {
		     offset = l->offset + l->size;
   		     l = token_add(l, offset);
   		  }
	       }
	       if(e) {
		  e++;
		  l->token = WIKI_TOKEN_UNIQ_STRING;
		  l->offset = offset;
		  l->size = e - p;

		  offset = l->offset + l->size;
		  l = token_add(l, offset);
		  p = e - 1;
	       }
	    } else 
	       l->size++;
	    break;
	 case '\0':
	    if(l) {
	       if(l->size) {
		  offset = l->offset + l->size;
		  l = token_add(l, offset);
	       }

	       l->token = WIKI_TOKEN_EOF;
	       l->offset = offset;
	       l->size = 1;
	    }
	    l = NULL; 
	    eof = 1;

	    break;
	 case '\t':
	 case ' ':
	    if(1) {
	       size_t s = 0;
	       while(p < buf + size && u_isspace(*p) && *p != '\n') {
		  s++;
		  p++;

	       }
	       if(l->size == 0 && tab_open > 0 && l->prev 
		     && (l->prev->token == WIKI_TOKEN_NEWLINE || l->prev->token == WIKI_TOKEN_NEWPAR)
		     && p < buf + size && (*p == '{' || *p == '|' || *p == '!')) {
		  l->prev->size += s;
		  l->prev->token = WIKI_TOKEN_NEWLINE;
		  l->offset = l->prev->offset + l->prev->size;
		  offset = l->offset;
		  l->size = 0;
	       } else if(l->size == 0 && l->prev 
		     && (l->prev->token == WIKI_TOKEN_NEWLINE || l->prev->token == WIKI_TOKEN_NEWPAR)) {
		  if(0 && l->size) {
		     offset = l->offset + l->size;
		     l = token_add(l, offset);
		  }
		  l->token = WIKI_TOKEN_WHITESPACE;
		  l->size += s;

		  offset = l->offset + l->size;
		  l = token_add(l, offset);
	       } else if(l->size > 0) {
		  l->size += s;
	       } else {
		  l->token = WIKI_TOKEN_WHITESPACE;
		  l->size += s;

		  offset = l->offset + l->size;
		  l = token_add(l, offset);
	       }
	       p--;
	    } else 
	       l->size++;
	    break;
	 default:
	    if(l)
	       l->size++;
	    break;
      }
      if(! l && ! eof) {
	 *error = WIKI_LEXER_ERROR_ALLOCATE_LEXER;
	 break;
      }       
      p++;
   }

   return lexer;
}

Wiki_Lexer *wiki_lexer_filename_new(const char *filename, int *error, Eina_Bool with_angl)
{
   Wiki_Lexer *lexer = NULL;
   FILE *file;

   if(!filename) {
      *error = WIKI_LEXER_ERROR_NO_FILENAME;
      return NULL;
   }

   file = fopen(filename, "rb");
   if(file) {
      lexer = wiki_lexer_file_new(file, error, with_angl);
      fclose(file);
   } else {
      *error = WIKI_LEXER_ERROR_CANT_OPEN_FILE;
   }

   return lexer;
}

Wiki_Lexer *wiki_lexer_file_new(FILE *file, int *error, Eina_Bool  with_angl)
{
   Wiki_Lexer *lexer = NULL;
   char buf[4096];
   off_t offset = 0;
   int read;

   *error = WIKI_LEXER_ERROR_NONE;
   if(! file) {
      *error = WIKI_LEXER_ERROR_NO_FILE_STREAM;
      return NULL;
   }

   rewind(file);
//   offset = ftell(file);
   while((read = fread(buf, sizeof(char), 4096, file))) {
      UChar *tmp = u_strndupC(buf, read);
      if(tmp) {
	 tmp = icu_replace("&lt;", ICU_STR_ANGL_OPEN, tmp, 0);
	 tmp = icu_replace("&gt;", ICU_STR_ANGL_CLOSE, tmp, 0);
	 lexer = wiki_lexer_get(lexer, tmp, u_strlen(tmp), offset, error, with_angl);
	 if(*error) break;
	 offset += u_strlen(tmp); //ftell(file);
	 free(tmp);
      } else exit(1);
   }
   /* Add WIKI_TOKEN_EOF */
   if(lexer && *error == WIKI_LEXER_ERROR_NONE)
      lexer = wiki_lexer_get(lexer, (UChar *)"\0", 1, offset, error, with_angl);
 else exit(3);
   return lexer;
}

void wiki_lexer_dump_with_file(Wiki_Lexer *lexer, FILE *file)
{
   Wiki_Lexer *l;
   char buf[4096];
   UChar *tmp = NULL;
   int i = 0, read = 0;
   off_t offset_s = 0, offset_n = 0;
   if(! lexer) return;
   if(! file) return;

   rewind(file);

   l = lexer;
   while(l) {
      if(l->offset + l->size > offset_n) {
	 if(feof(file)) {
	    if(l->token != WIKI_TOKEN_EOF) {
	       fprintf(stderr, "EOF but still have lexer. The lexer sould not be for the file\n");
	       break;
	    }
	 }
	 if(l->token != WIKI_TOKEN_EOF) {
	    read = fread(buf, sizeof(char), 4096, file);
	    offset_s = offset_n;
	    if(tmp) {
	       free(tmp);
	       tmp = NULL;
	    }
	    if(read) {
	       tmp = u_strndupC(buf, read);
	       tmp = icu_replace("&lt;", ICU_STR_ANGL_OPEN, tmp, 0);
	       tmp = icu_replace("&gt;", ICU_STR_ANGL_CLOSE, tmp, 0);
	    }
	    if(tmp)
	       offset_n += u_strlen(tmp);
	 }
      }
      if(tmp) {
	 UChar *text = NULL;
	 text = u_strndup(tmp + (l->offset - offset_s), l->size);
	 uprintf("i: %04d, offset: %d, size: %d, type: %s, data: %S\n", i, (int) l->offset, (int) l->size, wiki_lexer_token_name_get(l->token), text);
	 if(text)
	    free(text);
      }
      i++;
      l = l->next;
   }
}

void wiki_lexer_dump_with_filename(Wiki_Lexer *lexer, const char *filename)
{
   if(! lexer) return;
   if(! filename) return;

   FILE *file = fopen(filename, "rb");
   if(file) {
      wiki_lexer_dump_with_file(lexer, file);
      fclose(file);
   }
}

void wiki_lexer_dump_with_buf(Wiki_Lexer *lexer, const UChar *buf, size_t size)
{
   Wiki_Lexer *l;
   int i = 0;
   if(! lexer) return;
   if(! buf) return;

   ulog("LEXER DUMP: size = %d, buf size = %d\n", size, u_strlen(buf));
   l = lexer;
   while(l) {
      if(l->offset + l->size > size && l->token != WIKI_TOKEN_EOF) {
	 ulog("EOF but still have lexer. The lexer should not be for the buffer\n");
	 break;
      }
      UChar *tmp = u_strndup(buf + l->offset, l->size);
      ulog("i: %04d, offset: %d, size: %d, type: %s, data: %S\n", i, (int) l->offset, (int) l->size, wiki_lexer_token_name_get(l->token), tmp);
      if(tmp)
	 free(tmp);
      if(l->token == WIKI_TOKEN_EOF) 
	 break;
      i++;
      l = l->next;
   }
}

const char *wiki_lexer_token_name_get(Wiki_Token token)
{
   if(token >=0 && token <= WIKI_TOKEN_EOF) {
      return Wiki_Token_Str[token];
   }

   return NULL;
}

Wiki_Lexer *wiki_lexer_buf_new(const UChar *buf, size_t size, int *error, Eina_Bool with_angl)
{
   Wiki_Lexer *lexer = NULL;

   if(!buf) return NULL;

   lexer = wiki_lexer_get(lexer, buf, size, 0, error, with_angl);

   return lexer;
}

#ifdef TEST_LEXER
int main(int argc, char **argv)
{
   Wiki_Lexer *l;
   const char cwiki[] = "Test éwith [[wikilink|description]],                       é à çù                  <!-- >TOTO  \n </noincl-->                                  on {fait} un test{{template|parameter's|{{nested}}=booh}}, \n\n \n       \n<noinclude>   <hr /> é à \r </div>\nnew</noinclude> paragraphs, <html>, {| tables |- |}";
   int error, i;
   UChar *wiki;

   wiki = calloc(1024, sizeof(UChar));
   u_uastrcpy(wiki, cwiki);

   icu_util_init();

   for(i = 1; i < argc; i++) {
      l = wiki_lexer_filename_new(argv[i], &error, EINA_TRUE);
      if(l && error == WIKI_LEXER_ERROR_NONE) {
	 wiki_lexer_dump_with_filename(l, argv[i]);
      } else {
	 printf("%d, %p\n", error, l);
	 break;
      }
      if(l) 
	 l = wiki_lexer_free(l);
   }

   if(argc == 1) {
      l = wiki_lexer_buf_new(wiki, u_strlen(wiki) + 1, &error);
      if(l && error == WIKI_LEXER_ERROR_NONE) 
	 wiki_lexer_dump_with_buf(l, wiki, u_strlen(wiki) + 1);
      if(l) 
	 l = wiki_lexer_free(l);
   }

   icu_util_shutdown();

   return 0;
}
#endif
