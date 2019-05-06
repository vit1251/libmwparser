/*
 * Molty from php/ext/pcre
 */
#include <pcre.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pcre_replace.h"

#define PREG_REPLACE_EVAL (1<<0)

#define PCRE_BACKTRACE_LIMIT 100000
#define PCRE_RECURSION_LIMIT 100000

static int pcre_compile_options(char *match, int *pcre_options);

enum {
   PHP_PCRE_NO_ERROR = 0,
   PHP_PCRE_INTERNAL_ERROR,
   PHP_PCRE_BACKTRACK_LIMIT_ERROR,
   PHP_PCRE_RECURSION_LIMIT_ERROR,
   PHP_PCRE_BAD_UTF8_ERROR,
   PHP_PCRE_BAD_UTF8_OFFSET_ERROR
};

static int is_numeric_string(const char *str, size_t len)
{
   const char *p = str;
   int ret = 1;
   if(!str) return 0;

   while(p < str + len) {
      if(! isdigit(*p))
	 ret = 1;
      p++;
   }

   return ret;
}

static int preg_get_backref(const char **str, int *backref)
{
   register char in_brace = 0;
   register const char *walk = *str;
   
   if (walk[1] == 0)
      return 0;
   
   if (*walk == '$' && walk[1] == '{') {
      in_brace = 1;
      walk++;
   }
   walk++;
   
   if (*walk >= '0' && *walk <= '9') {
      *backref = *walk - '0';
      walk++;
   } else
      return 0;
   
   if (*walk && *walk >= '0' && *walk <= '9') {
      *backref = *backref * 10 + *walk - '0';
      walk++;
   }
   
   if (in_brace) {
      if (*walk == 0 || *walk != '}')
	 return 0;
      else
	 walk++;
   }
   
   *str = walk;
   return 1;	
}

static char **make_subpats_table(int num_subpats, pcre *re, pcre_extra *extra)
{
   int rc, name_cnt = 0, name_size, ni = 0;
   char *name_table;
   unsigned short name_idx;
   char **subpat_names = calloc(num_subpats, sizeof(char *));

   if(!subpat_names) return NULL;

   rc = pcre_fullinfo(re, extra, PCRE_INFO_NAMECOUNT, &name_cnt);
   if (rc < 0) {
      fprintf(stderr, "Internal pcre_fullinfo() error %d\n", rc);
      free(subpat_names);
      return NULL;
   }
   if(name_cnt >0) {
      int rc1, rc2;
      rc1 = pcre_fullinfo(re, extra, PCRE_INFO_NAMETABLE, &name_table);
      rc2 = pcre_fullinfo(re, extra, PCRE_INFO_NAMEENTRYSIZE, &name_size);
      rc = rc2 ? rc2 : rc1;
      if (rc < 0) {
	 fprintf(stderr, "Internal pcre_fullinfo() error %d\n", rc);
	 free(subpat_names);
	 return NULL;
      }

      while (ni++ < name_cnt) {
	 name_idx = 0xff * name_table[0] + name_table[1];
	 subpat_names[name_idx] = name_table + 2;
	 if (is_numeric_string(subpat_names[name_idx], strlen(subpat_names[name_idx])) > 0) {
	    fprintf(stderr, "Numeric named subpatterns are not allowed\n");
	    free(subpat_names);
	    return NULL;
	 }
	 name_table += name_size;
      }
   }

   return subpat_names;
}

static int pcre_compile_options(char *match, int *pcre_options) {
   int res = 0, options = *pcre_options;
   char *p;

   if(! match || match[0] != '/') return 0;

   p = rindex(match, '/');
   if(p) {
      *p = '\0';
      p++;
   } else {
      fprintf(stderr, "NOT FOUND\n");
      return 0;
   }
   while(p && *p != 0) {
      switch (*p) {
	 case 'i':	options |= PCRE_CASELESS;		break;
	 case 'm':	options |= PCRE_MULTILINE;		break;
	 case 's':	options |= PCRE_DOTALL;			break;
	 case 'x':	options |= PCRE_EXTENDED;		break;
	 case 'A':	options |= PCRE_ANCHORED;		break;
	 case 'D':	options |= PCRE_DOLLAR_ENDONLY;		break;
	 case 'S':	fprintf(stderr, "Modifier 'S' not implemented\n");
//			do_study  = 1;				
			break;
	 case 'U':	options |= PCRE_UNGREEDY;		break;
	 case 'X':	options |= PCRE_EXTRA;			break;
	 case 'u':	options |= PCRE_UTF8;			break;
	 case 'e':	fprintf(stderr, "Modifier 'e' not implemented\n");
//			poptions |= PREG_REPLACE_EVAL;	
			break;
	 case ' ':
	 case '\n':
			break;
			
	 default:
			fprintf(stderr, "Unknown modifier '%c'\n", *p);
			
      }
      p++;
   }

   *pcre_options = options;
   return res;
}

/*
 *
 */
char *pcre_replace(const char *match, const char *replace, char *text)
{
   pcre *re = NULL;
   pcre_extra *extra = NULL;
   pcre_extra  extra_data;
   const char *re_error;
   int re_error_offset, error_code, exoptions = 0, g_notempty = 0, count = 0;
   int pcre_options = 0, *offsets, size_offsets, start_offset;
   size_t text_len, match_len, alloc_len, result_len = 0, replace_len, 
	  new_len, eval_result_len;
   int num_subpats, eval, rc, replace_count = 0, is_callable_replace = 0, 
       limit = -1, backref;
   char **subpat_names;
   const char *replace_start = NULL, *replace_end = NULL, *walk;
   char  *result = NULL, *match_part, *mmatch,
	 *new_buf, *walkbuf, *piece, *eval_result, walk_last;

   if(! replace) return text;
   if(! match) return text;
   if(! text) return text;

   mmatch = strdup(match);
   if(! mmatch) 
      return text;

   pcre_compile_options(mmatch, &pcre_options);
//   fprintf(stderr, "match : -%s-\n", mmatch + 1);
   re = pcre_compile(mmatch + 1, pcre_options, &re_error, &re_error_offset, NULL);
   if(mmatch)
      free(mmatch);

   if(!re) {
      fprintf(stderr, "pcre_compile @%u: %s in %s\n", re_error_offset, re_error, match);
      return text;
   }

   if (extra == NULL) {
      extra_data.flags = PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
      extra = &extra_data;
   }
   extra->match_limit = PCRE_BACKTRACE_LIMIT;
   extra->match_limit_recursion = PCRE_RECURSION_LIMIT;

   eval = pcre_options & PREG_REPLACE_EVAL;

   rc = pcre_fullinfo(re, extra, PCRE_INFO_CAPTURECOUNT, &num_subpats);
   if (rc < 0) {
      fprintf(stderr, "Internal pcre_fullinfo() error %d\n", rc);
      pcre_free(re);
      return text;
   }
   num_subpats++;
   size_offsets = num_subpats * 3;

   subpat_names = make_subpats_table(num_subpats, re, extra);
   if (!subpat_names) {
      pcre_free(re);
      return text;
   }

   offsets = calloc(size_offsets, sizeof(int));
   if(!offsets) {
      free(subpat_names);
      pcre_free(re);
      return text;
   }

   text_len = strlen(text);

   alloc_len = 2 * text_len + 1;
   result = calloc(alloc_len, sizeof(char));
   if(!result) {
      free(offsets);
      free(subpat_names);
      pcre_free(re);
      return text;
   }
   
   /* Initialize */
   match_part = NULL;
   result_len = 0;
   replace_start = replace;
   replace_len = strlen(replace);
   replace_end = replace_start + replace_len;
   start_offset = 0;
   error_code = PHP_PCRE_NO_ERROR;
   eval_result = NULL;
   eval_result_len = 0;
//   extra = NULL;

   while(1) {
      count = pcre_exec(re, extra, text, text_len, start_offset,
	    exoptions|g_notempty, offsets, size_offsets);

//      fprintf(stderr, "Matched count : %d\n", count);
      exoptions |= PCRE_NO_UTF8_CHECK;

      if (count == 0) {
	 fprintf(stderr, "Matched, but too many substrings\n");
	 count = size_offsets/3;
      }

      piece = text + start_offset;

      if (count > 0 && (limit == -1 || limit > 0)) {
	 if (replace_count) {
	    ++replace_count;
	 }
	 match_part = text + offsets[0];
	 
	 new_len = result_len + offsets[0] - start_offset;

	 if (eval) {
	    /*
	    eval_result_len = preg_do_eval(replace, replace_len, text,
		  offsets, count, &eval_result);
	    new_len += eval_result_len;
	    */
//	    fprintf(stderr, "Do Eval not set\n");
	 } else if (is_callable_replace) {
	    /*
	    eval_result_len = preg_do_repl_func(replace, text, offsets, subpat_names, count, &eval_result TSRMLS_CC);
	    new_len += eval_result_len;
	    */
//	    fprintf(stderr, "Eval callback not set\n");
	 } else {
	    walk = (char *) replace;
	    walk_last = 0;
	    while (walk < replace_end) {
	       if ('\\' == *walk || '$' == *walk) {
		  if (walk_last == '\\') {
		     walk++;
		     walk_last = 0;
		     continue;
		  }
		  if (preg_get_backref(&walk, &backref)) {
		     if (backref < count)
			new_len += offsets[(backref<<1)+1] - offsets[backref<<1];
		     continue;
		  }
	       }
	       new_len++;
	       walk++;
	       walk_last = walk[-1];
	    }
	 }

	 if (new_len + 1 > alloc_len) {
	    alloc_len = 1 + alloc_len + 2 * new_len;
	    new_buf = calloc(alloc_len + 1, sizeof(char));
	    if(! new_buf) goto END;
	    memcpy(new_buf, result, result_len);
	    free(result);
	    result = new_buf;
	 }

	 memcpy(&result[result_len], piece, match_part - piece);
	 result_len += match_part - piece;

	 walkbuf = result + result_len;

	 if (eval || is_callable_replace) {
	    /*
	    memcpy(walkbuf, eval_result, eval_result_len);
	    result_len += eval_result_len;
	    free(eval_result);
	    */
//	    fprintf(stderr, "%s:%d: Not implemented\n", __FUNCTION__, __LINE__);
	 } else {
	    walk = replace;
	    walk_last = 0;
	    while (walk < replace_end) {
	       if ('\\' == *walk || '$' == *walk) {
		  if (walk_last == '\\') {
		     *(walkbuf-1) = *walk++;
		     walk_last = 0;
		     continue;
		  }
		  if (preg_get_backref(&walk, &backref)) {
		     if (backref < count) {
			match_len = offsets[(backref<<1)+1] - offsets[backref<<1];
			memcpy(walkbuf, text + offsets[backref<<1], match_len);
			walkbuf += match_len;
		     }
		     continue;
		  }
	       }
	       *walkbuf++ = *walk++;
	       walk_last = walk[-1];
	    }
	    *walkbuf = '\0';
	    result_len += walkbuf - (result + result_len);
	 }
	 
	 if (limit != -1)
	    limit--;
	 
      } else if (count == PCRE_ERROR_NOMATCH || limit == 0) {
	 if (g_notempty != 0 && start_offset < text_len) {
	    offsets[0] = start_offset;
	    offsets[1] = start_offset + 1;
	    memcpy(&result[result_len], piece, 1);
	    (result_len)++;
	 } else {
	    new_len = result_len + text_len - start_offset;
	    if (new_len + 1 > alloc_len) {
	       alloc_len = new_len + 1; /* now we know exactly how long it is */
	       new_buf = calloc(alloc_len, sizeof(char));
	       if(! new_buf) goto END;
	       memcpy(new_buf, result, result_len);
	       free(result);
	       result = new_buf;
	    }
	    /* stick that last bit of string on our output */
	    memcpy(&result[result_len], piece, text_len - start_offset);
	    result_len += text_len - start_offset;
	    result[result_len] = '\0';
	    break;
	 }
      } else {
	 // pcre_handle_exec_error(count);
	 fprintf(stderr, "pcre_replace error: %d\n", count);
	 free(result);
	 result = NULL;
	 break;
      }

      g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY | PCRE_ANCHORED : 0;
      start_offset = offsets[1];
   }

END:
   free(subpat_names);
   free(offsets);
   free(text);
   pcre_free(re);

//   fprintf(stderr, "res_len: %d, count: %d, replace_count: %d\n",
//	 (int) result_len, count, replace_count);
   return result;
}

