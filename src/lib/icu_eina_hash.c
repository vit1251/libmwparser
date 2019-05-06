#include <Eina.h>
#include <unicode/ustring.h>

#include "icu_eina_hash.h"
#include "util.h"

static unsigned int icu_eina_hash_key_length(const void *key)
{
   const UChar *k = key;
   if(! k) return 0;
   return (u_strlen(k) + 1) * sizeof(UChar);
}

static int icu_eina_hash_key_cmp(const void *key1, int len1, const void *key2, int len2)
{
   const UChar * k1 = key1, *k2 = key2;
   return u_strcmp(k1, k2);
}

void *icu_eina_hash_find(const Eina_Hash *hash, const UChar *key)
{
   int len = 0, hash_num = 0;

   if(! hash || ! key) return NULL;

   len = icu_eina_hash_key_length(key);
   hash_num = eina_hash_superfast((const char *) key, len);

   return eina_hash_find_by_hash(hash, key, len, hash_num);
}

void *icu_eina_hash_modify(Eina_Hash *hash, const UChar *key, const void *data)
{
   int len = 0, hash_num = 0;

   if(! hash || ! key) return NULL;

   len = icu_eina_hash_key_length(key);
   hash_num = eina_hash_superfast((const char *) key, len);

   return eina_hash_modify_by_hash(hash, key, len, hash_num, data);
}

Eina_Bool icu_eina_hash_add(Eina_Hash *hash, const UChar *key, const void *data)
{
   int len = 0, hash_num = 0;

   if(! hash || ! key) return EINA_FALSE;

   len = icu_eina_hash_key_length(key);
   hash_num = eina_hash_superfast((const char *) key, len);

   return eina_hash_add_by_hash(hash, key, len, hash_num, data);
}

Eina_Bool icu_eina_hash_del(Eina_Hash *hash, const UChar *key)
{
   int len = 0, hash_num = 0;

   if(! hash || ! key) return EINA_FALSE;

   len = icu_eina_hash_key_length(key);
   hash_num = eina_hash_superfast((const char *) key, len);

   return eina_hash_del_by_key_hash(hash, key, len, hash_num);
}

Eina_Hash *icu_eina_hash_new(Eina_Free_Cb data_free_cb)
{
   return eina_hash_new(icu_eina_hash_key_length, 
	 icu_eina_hash_key_cmp,  EINA_KEY_HASH(eina_hash_superfast), 
	 data_free_cb, 8);
}
