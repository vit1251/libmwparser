#ifndef _ICU_EINA_HASH_H
#define _ICU_EINA_HASH_H

#include <Eina.h>
#include <unicode/ustring.h>

Eina_Hash *icu_eina_hash_new(Eina_Free_Cb data_free_cb);
Eina_Bool icu_eina_hash_add(Eina_Hash *hash, const UChar *key, const void *data);
Eina_Bool icu_eina_hash_del(Eina_Hash *hash, const UChar *key);
void *icu_eina_hash_modify(Eina_Hash *hash, const UChar *key, const void *data);
void *icu_eina_hash_find(const Eina_Hash *hash, const UChar *key);


#endif
