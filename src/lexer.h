#ifndef _WIKI_LEXER_H
#define _WIKI_LEXER_H

#define _FILE_OFFSET_BITS 64

#include <stdlib.h>
#include <stdio.h>
#include <Eina.h>
#include "unicode/utypes.h"
#include "parser_tag_extensions.h"

enum {
   WIKI_LEXER_ERROR_NONE,
   WIKI_LEXER_ERROR_NO_FILENAME,
   WIKI_LEXER_ERROR_NO_FILE_STREAM,
   WIKI_LEXER_ERROR_CANT_OPEN_FILE,
   WIKI_LEXER_ERROR_ALLOCATE_LEXER
};

typedef enum _Wiki_Token Wiki_Token;
enum _Wiki_Token
{
   WIKI_TOKEN_TEXT,
   WIKI_TOKEN_AMP,
   WIKI_TOKEN_WHITESPACE,
   WIKI_TOKEN_NOWIKI_OPEN,
   WIKI_TOKEN_NOWIKI_CLOSE,
   WIKI_TOKEN_NOWIKI_OPEN_CLOSE,
   WIKI_TOKEN_SQRE_OPEN,
   WIKI_TOKEN_SQRE_CLOSE,
   WIKI_TOKEN_CURL_OPEN,
   WIKI_TOKEN_CURL_CLOSE,
   WIKI_TOKEN_PIPE,
   WIKI_TOKEN_EXCL,
   WIKI_TOKEN_APOSTROPHE,
   WIKI_TOKEN_QUOTE,
   WIKI_TOKEN_ENTITY,
   WIKI_TOKEN_UNIQ_STRING,
   WIKI_TOKEN_NEWLINE,
/* Keep block token here */
   WIKI_TOKEN_NEWPAR,
   WIKI_TOKEN_EQUAL_SIGN,
   WIKI_TOKEN_ASTERISK,
   WIKI_TOKEN_COLON,
   WIKI_TOKEN_SEMICOLON,
   WIKI_TOKEN_HASH,
   WIKI_TOKEN_DASH,
   WIKI_TOKEN_TAB_OPEN,
   WIKI_TOKEN_TAB_NEWLINE,
   WIKI_TOKEN_TAB_CAPTION,
   WIKI_TOKEN_TAB_CLOSE,
   WIKI_TOKEN_PRE_OPEN,
   WIKI_TOKEN_PRE_CLOSE,
   WIKI_TOKEN_TAG_OPEN,
   WIKI_TOKEN_TAG_OPEN_CLOSE,
   WIKI_TOKEN_TAG_CLOSE,
   WIKI_TOKEN_TAG_UNKNOW_OPEN,
   WIKI_TOKEN_TAG_UNKNOW_OPEN_CLOSE,
   WIKI_TOKEN_TAG_UNKNOW_CLOSE,
   WIKI_TOKEN_EOF,
   WIKI_TOKEN_NONE
};

typedef struct _Wiki_Lexer Wiki_Lexer;
struct _Wiki_Lexer
{
   Wiki_Token  token;
   off_t       offset;
   size_t      size;
   const Wiki_Parser_Tag_Extension *tag_ext;
   Wiki_Lexer *prev;
   Wiki_Lexer *next;
   Wiki_Parser_Tag_Content_Type parse;
   Eina_Bool   block;
};

Wiki_Lexer *wiki_lexer_free(Wiki_Lexer *lexer);
Wiki_Lexer *wiki_lexer_get(Wiki_Lexer *lexer, const UChar *buf, size_t size, off_t start_offset, int *error, Eina_Bool with_angl);
Wiki_Lexer *wiki_lexer_template_get(Wiki_Lexer *lexer, const UChar *buf, size_t size, off_t start_offset, int *error);

Wiki_Lexer *wiki_lexer_filename_new(const char *filename, int *error, Eina_Bool with_angl);
Wiki_Lexer *wiki_lexer_file_new(FILE *file, int *error, Eina_Bool with_angl);
Wiki_Lexer *wiki_lexer_buf_new(const UChar *buf, size_t size, int *error, Eina_Bool with_angl);

const char *wiki_lexer_token_name_get(Wiki_Token token);

void wiki_lexer_dump_with_file(Wiki_Lexer *lexer, FILE *file);
void wiki_lexer_dump_with_filename(Wiki_Lexer *lexer, const char *filename);
void wiki_lexer_dump_with_buf(Wiki_Lexer *lexer, const UChar *buf, size_t size);

const Eina_Hash *wiki_lexer_tags_get(void);
void wiki_lexer_tags_free(void);
#endif
