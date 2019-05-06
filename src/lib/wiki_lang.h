#ifndef _WIKI_LANG_H
#define _WIKI_LANG_H

#include <Eina.h>

typedef struct _Wiki_Lang_Magic_Word
{
   const char *locale;
   const char *name;
} Wiki_Lang_Magic_Word;

typedef struct _Wiki_Lang_Message
{
   const char *name;
   const char *message;
} Wiki_Lang_Message;

typedef struct _Wiki_Lang
{
   const char *code;
   const char *english;
   const char *dir;
   const char *local;
   const Wiki_Lang_Magic_Word *magic_words;
   const Wiki_Lang_Message *messages;
} Wiki_Lang;

Eina_Hash *wiki_languages_init(void);
const char *wiki_lang_magic_word_get(Eina_Hash *hash, const char *lang, const char *word);
const char *wiki_lang_message_get(Eina_Hash *hash, const char *lang, const char *name);

#endif
