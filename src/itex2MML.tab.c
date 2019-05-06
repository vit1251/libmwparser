/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 5 "itex2MML.y" /* yacc.c:339  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "itex2MML.h"

#define YYSTYPE char *
#define YYPARSE_PARAM_TYPE char **
#define YYPARSE_PARAM ret_str

#define yytext itex2MML_yytext

static int itex2MML_last_error = 0;
 extern int yylex ();

 extern char * yytext;

 static void itex2MML_default_error (const char * msg)
   {
     if (msg)
       fprintf(stderr, "Line: %d Error: %s\n", itex2MML_lineno, msg);
   }

 void (*itex2MML_error) (const char * msg) = itex2MML_default_error;

 static void yyerror (char * s)
   {
     char * msg = itex2MML_copy3 (s, " at token ", yytext);
     if (itex2MML_error)
       (*itex2MML_error) (msg);
     itex2MML_free_string (msg);
     itex2MML_last_error = 1;
   }

 /* Note: If length is 0, then buffer is treated like a string; otherwise only length bytes are written.
  */
 static void itex2MML_default_write (const char * buffer, unsigned long length)
   {
     if (buffer)
       {
	 if (length)
	   fwrite (buffer, 1, length, stdout);
	 else
	   fputs (buffer, stdout);
       }
   }

 static void itex2MML_default_write_mathml (const char * mathml)
   {
     if (itex2MML_write)
       (*itex2MML_write) (mathml, 0);
   }

#ifdef itex2MML_CAPTURE
    static char * itex2MML_output_string = "" ;

    const char * itex2MML_output ()
    {
        char * copy = (char *) malloc(strlen(itex2MML_output_string) +1);
        if (copy)
          {
           if (itex2MML_output_string)
             {
               strcpy(copy, itex2MML_output_string);
               if (itex2MML_output_string != "")
                   free(itex2MML_output_string);
             }
           else
             copy[0] = 0;
          }
        itex2MML_output_string = "";
        return copy;
    }

 static void itex2MML_capture (const char * buffer, unsigned long length)
    {
     if (buffer)
       {
         if (length)
           {
              unsigned long first_length = itex2MML_output_string ? strlen(itex2MML_output_string) : 0;
              char * copy  = (char *) malloc(first_length + length + 1);
              if (copy)
                {
                  if (itex2MML_output_string)
                    {
                       strcpy(copy, itex2MML_output_string);
                       if (itex2MML_output_string != "")
                          free(itex2MML_output_string);
                    }
                  else
                     copy[0] = 0;
                  strncat(copy, buffer, length);
                 }
              itex2MML_output_string = copy;
            }
         else
            {
              char * copy = itex2MML_copy2(itex2MML_output_string, buffer);
              if (itex2MML_output_string != "")
                 free(itex2MML_output_string);
              itex2MML_output_string = copy;
            }
        }
    }

    static void itex2MML_capture_mathml (const char * buffer)
    {
       char * temp = itex2MML_copy2(itex2MML_output_string, buffer);
       if (itex2MML_output_string != "")
         free(itex2MML_output_string);
       itex2MML_output_string = temp;
    }
    void (*itex2MML_write) (const char * buffer, unsigned long length) = itex2MML_capture;
    void (*itex2MML_write_mathml) (const char * mathml) = itex2MML_capture_mathml;
#else
    void (*itex2MML_write) (const char * buffer, unsigned long length) = itex2MML_default_write;
    void (*itex2MML_write_mathml) (const char * mathml) = itex2MML_default_write_mathml;
#endif 

 char * itex2MML_empty_string = "";

 /* Create a copy of a string, adding space for extra chars
  */
 char * itex2MML_copy_string_extra (const char * str, unsigned extra)
   {
     char * copy = (char *) malloc(extra + (str ? strlen (str) : 0) + 1);
     if (copy)
       {
	 if (str)
	   strcpy(copy, str);
	 else
	   copy[0] = 0;
       }
     return copy ? copy : itex2MML_empty_string;
   }

 /* Create a copy of a string, appending two strings
  */
 char * itex2MML_copy3 (const char * first, const char * second, const char * third)
   {
     int  first_length =  first ? strlen( first) : 0;
     int second_length = second ? strlen(second) : 0;
     int  third_length =  third ? strlen( third) : 0;

     char * copy = (char *) malloc(first_length + second_length + third_length + 1);

     if (copy)
       {
	 if (first)
	   strcpy(copy, first);
	 else
	   copy[0] = 0;

	 if (second) strcat(copy, second);
	 if ( third) strcat(copy,  third);
       }
     return copy ? copy : itex2MML_empty_string;
   }

 /* Create a copy of a string, appending a second string
  */
 char * itex2MML_copy2 (const char * first, const char * second)
   {
     return itex2MML_copy3(first, second, 0);
   }

 /* Create a copy of a string
  */
 char * itex2MML_copy_string (const char * str)
   {
     return itex2MML_copy3(str, 0, 0);
   }

 /* Create a copy of a string, escaping unsafe characters for XML
  */
 char * itex2MML_copy_escaped (const char * str)
   {
     unsigned long length = 0;

     const char * ptr1 = str;

     char * ptr2 = 0;
     char * copy = 0;

     if ( str == 0) return itex2MML_empty_string;
     if (*str == 0) return itex2MML_empty_string;

     while (*ptr1)
       {
	 switch (*ptr1)
	   {
	   case '<':  /* &lt;   */
	   case '>':  /* &gt;   */
	     length += 4;
	     break;
	   case '&':  /* &amp;  */
	     length += 5;
	     break;
	   case '\'': /* &apos; */
	   case '"':  /* &quot; */
	   case '-':  /* &#x2d; */
	     length += 6;
	     break;
	   default:
	     length += 1;
	     break;
	   }
	 ++ptr1;
       }

     copy = (char *) malloc (length + 1);

     if (copy)
       {
	 ptr1 = str;
	 ptr2 = copy;

	 while (*ptr1)
	   {
	     switch (*ptr1)
	       {
	       case '<':
		 strcpy (ptr2, "&lt;");
		 ptr2 += 4;
		 break;
	       case '>':
		 strcpy (ptr2, "&gt;");
		 ptr2 += 4;
		 break;
	       case '&':  /* &amp;  */
		 strcpy (ptr2, "&amp;");
		 ptr2 += 5;
		 break;
	       case '\'': /* &apos; */
		 strcpy (ptr2, "&apos;");
		 ptr2 += 6;
		 break;
	       case '"':  /* &quot; */
		 strcpy (ptr2, "&quot;");
		 ptr2 += 6;
		 break;
	       case '-':  /* &#x2d; */
		 strcpy (ptr2, "&#x2d;");
		 ptr2 += 6;
		 break;
	       default:
		 *ptr2++ = *ptr1;
		 break;
	       }
	     ++ptr1;
	   }
	 *ptr2 = 0;
       }
     return copy ? copy : itex2MML_empty_string;
   }

 /* Create a hex character reference string corresponding to code
  */
 char * itex2MML_character_reference (unsigned long int code)
   {
#define ENTITY_LENGTH 10
     char * entity = (char *) malloc(ENTITY_LENGTH);
     sprintf(entity, "&#x%05lx;", code);
     return entity;
   }

 void itex2MML_free_string (char * str)
   {
     if (str && str != itex2MML_empty_string)
       free(str);
   }


#line 342 "itex2MML.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TEXOVER = 258,
    TEXATOP = 259,
    CHAR = 260,
    STARTMATH = 261,
    STARTDMATH = 262,
    ENDMATH = 263,
    MI = 264,
    MIB = 265,
    MN = 266,
    MO = 267,
    SUP = 268,
    SUB = 269,
    MROWOPEN = 270,
    MROWCLOSE = 271,
    LEFT = 272,
    RIGHT = 273,
    BIG = 274,
    BBIG = 275,
    BIGG = 276,
    BBIGG = 277,
    BIGL = 278,
    BBIGL = 279,
    BIGGL = 280,
    BBIGGL = 281,
    FRAC = 282,
    TFRAC = 283,
    OPERATORNAME = 284,
    MATHOP = 285,
    MATHBIN = 286,
    MATHREL = 287,
    MOP = 288,
    MOL = 289,
    MOLL = 290,
    MOF = 291,
    MOR = 292,
    PERIODDELIM = 293,
    OTHERDELIM = 294,
    LEFTDELIM = 295,
    RIGHTDELIM = 296,
    MOS = 297,
    MOB = 298,
    SQRT = 299,
    ROOT = 300,
    BINOM = 301,
    TBINOM = 302,
    UNDER = 303,
    OVER = 304,
    OVERBRACE = 305,
    UNDERLINE = 306,
    UNDERBRACE = 307,
    UNDEROVER = 308,
    TENSOR = 309,
    MULTI = 310,
    ARRAYALIGN = 311,
    COLUMNALIGN = 312,
    ARRAY = 313,
    COLSEP = 314,
    ROWSEP = 315,
    ARRAYOPTS = 316,
    COLLAYOUT = 317,
    COLALIGN = 318,
    ROWALIGN = 319,
    ALIGN = 320,
    EQROWS = 321,
    EQCOLS = 322,
    ROWLINES = 323,
    COLLINES = 324,
    FRAME = 325,
    PADDING = 326,
    ATTRLIST = 327,
    ITALICS = 328,
    BOLD = 329,
    BOXED = 330,
    SLASHED = 331,
    RM = 332,
    BB = 333,
    ST = 334,
    END = 335,
    BBLOWERCHAR = 336,
    BBUPPERCHAR = 337,
    BBDIGIT = 338,
    CALCHAR = 339,
    FRAKCHAR = 340,
    CAL = 341,
    FRAK = 342,
    CLAP = 343,
    LLAP = 344,
    RLAP = 345,
    ROWOPTS = 346,
    TEXTSIZE = 347,
    SCSIZE = 348,
    SCSCSIZE = 349,
    DISPLAY = 350,
    TEXTSTY = 351,
    TEXTBOX = 352,
    TEXTSTRING = 353,
    XMLSTRING = 354,
    CELLOPTS = 355,
    ROWSPAN = 356,
    COLSPAN = 357,
    THINSPACE = 358,
    MEDSPACE = 359,
    THICKSPACE = 360,
    QUAD = 361,
    QQUAD = 362,
    NEGSPACE = 363,
    PHANTOM = 364,
    HREF = 365,
    UNKNOWNCHAR = 366,
    EMPTYMROW = 367,
    STATLINE = 368,
    TOOLTIP = 369,
    TOGGLE = 370,
    FGHIGHLIGHT = 371,
    BGHIGHLIGHT = 372,
    SPACE = 373,
    INTONE = 374,
    INTTWO = 375,
    INTTHREE = 376,
    BAR = 377,
    WIDEBAR = 378,
    VEC = 379,
    WIDEVEC = 380,
    HAT = 381,
    WIDEHAT = 382,
    CHECK = 383,
    WIDECHECK = 384,
    ACUTE = 385,
    WIDEACUTE = 386,
    GRAVE = 387,
    WIDEGRAVE = 388,
    BREVE = 389,
    WIDEBREVE = 390,
    TILDE = 391,
    WIDETILDE = 392,
    DOT = 393,
    DDOT = 394,
    DDDOT = 395,
    DDDDOT = 396,
    UNARYMINUS = 397,
    UNARYPLUS = 398,
    BEGINENV = 399,
    ENDENV = 400,
    MATRIX = 401,
    PMATRIX = 402,
    BMATRIX = 403,
    BBMATRIX = 404,
    VMATRIX = 405,
    VVMATRIX = 406,
    SVG = 407,
    ENDSVG = 408,
    SMALLMATRIX = 409,
    CASES = 410,
    ALIGNED = 411,
    GATHERED = 412,
    SUBSTACK = 413,
    PMOD = 414,
    RMCHAR = 415,
    COLOR = 416,
    BGCOLOR = 417,
    XARROW = 418,
    OPTARGOPEN = 419,
    OPTARGCLOSE = 420
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);



/* Copy the second part of user declarations.  */

#line 556 "itex2MML.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  196
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   4152

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  166
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  115
/* YYNRULES -- Number of rules.  */
#define YYNRULES  319
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  563

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   420

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   286,   286,   289,   290,   291,   292,   293,   295,   297,
     298,   299,   312,   326,   330,   336,   355,   369,   388,   402,
     421,   435,   454,   468,   478,   488,   495,   502,   506,   510,
     515,   516,   517,   518,   519,   523,   527,   528,   529,   530,
     531,   532,   533,   534,   535,   536,   537,   538,   539,   540,
     541,   542,   543,   544,   545,   546,   547,   548,   549,   550,
     551,   552,   553,   554,   555,   556,   557,   558,   559,   560,
     561,   562,   563,   564,   565,   566,   567,   568,   569,   570,
     571,   572,   573,   574,   575,   576,   577,   578,   579,   580,
     581,   582,   583,   584,   585,   586,   587,   588,   592,   596,
     604,   605,   606,   607,   609,   614,   619,   625,   629,   633,
     638,   643,   647,   651,   656,   660,   664,   669,   673,   677,
     682,   686,   690,   695,   700,   705,   710,   715,   720,   725,
     731,   735,   739,   743,   745,   751,   753,   759,   760,   761,
     766,   771,   776,   780,   785,   789,   793,   797,   802,   807,
     812,   817,   822,   827,   833,   844,   852,   860,   868,   876,
     884,   891,   899,   904,   909,   914,   919,   924,   929,   934,
     939,   944,   949,   954,   959,   964,   967,   972,   976,   982,
     985,   990,   994,  1000,  1004,  1008,  1016,  1019,  1024,  1028,
    1034,  1039,  1042,  1047,  1051,  1057,  1062,  1066,  1070,  1074,
    1078,  1082,  1086,  1091,  1099,  1106,  1114,  1124,  1133,  1141,
    1145,  1151,  1156,  1160,  1164,  1169,  1176,  1184,  1189,  1196,
    1210,  1217,  1231,  1238,  1246,  1251,  1256,  1261,  1265,  1270,
    1274,  1279,  1284,  1289,  1294,  1299,  1303,  1308,  1312,  1317,
    1321,  1326,  1330,  1335,  1339,  1344,  1348,  1353,  1358,  1365,
    1373,  1380,  1388,  1395,  1403,  1413,  1422,  1426,  1430,  1434,
    1438,  1442,  1446,  1450,  1454,  1458,  1462,  1466,  1476,  1483,
    1487,  1491,  1496,  1501,  1506,  1510,  1518,  1522,  1528,  1532,
    1536,  1540,  1544,  1548,  1552,  1556,  1560,  1564,  1569,  1574,
    1579,  1584,  1589,  1594,  1599,  1604,  1609,  1614,  1621,  1625,
    1631,  1635,  1640,  1644,  1650,  1658,  1662,  1668,  1672,  1677,
    1680,  1684,  1692,  1696,  1702,  1706,  1710,  1714,  1719,  1724
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TEXOVER", "TEXATOP", "CHAR",
  "STARTMATH", "STARTDMATH", "ENDMATH", "MI", "MIB", "MN", "MO", "SUP",
  "SUB", "MROWOPEN", "MROWCLOSE", "LEFT", "RIGHT", "BIG", "BBIG", "BIGG",
  "BBIGG", "BIGL", "BBIGL", "BIGGL", "BBIGGL", "FRAC", "TFRAC",
  "OPERATORNAME", "MATHOP", "MATHBIN", "MATHREL", "MOP", "MOL", "MOLL",
  "MOF", "MOR", "PERIODDELIM", "OTHERDELIM", "LEFTDELIM", "RIGHTDELIM",
  "MOS", "MOB", "SQRT", "ROOT", "BINOM", "TBINOM", "UNDER", "OVER",
  "OVERBRACE", "UNDERLINE", "UNDERBRACE", "UNDEROVER", "TENSOR", "MULTI",
  "ARRAYALIGN", "COLUMNALIGN", "ARRAY", "COLSEP", "ROWSEP", "ARRAYOPTS",
  "COLLAYOUT", "COLALIGN", "ROWALIGN", "ALIGN", "EQROWS", "EQCOLS",
  "ROWLINES", "COLLINES", "FRAME", "PADDING", "ATTRLIST", "ITALICS",
  "BOLD", "BOXED", "SLASHED", "RM", "BB", "ST", "END", "BBLOWERCHAR",
  "BBUPPERCHAR", "BBDIGIT", "CALCHAR", "FRAKCHAR", "CAL", "FRAK", "CLAP",
  "LLAP", "RLAP", "ROWOPTS", "TEXTSIZE", "SCSIZE", "SCSCSIZE", "DISPLAY",
  "TEXTSTY", "TEXTBOX", "TEXTSTRING", "XMLSTRING", "CELLOPTS", "ROWSPAN",
  "COLSPAN", "THINSPACE", "MEDSPACE", "THICKSPACE", "QUAD", "QQUAD",
  "NEGSPACE", "PHANTOM", "HREF", "UNKNOWNCHAR", "EMPTYMROW", "STATLINE",
  "TOOLTIP", "TOGGLE", "FGHIGHLIGHT", "BGHIGHLIGHT", "SPACE", "INTONE",
  "INTTWO", "INTTHREE", "BAR", "WIDEBAR", "VEC", "WIDEVEC", "HAT",
  "WIDEHAT", "CHECK", "WIDECHECK", "ACUTE", "WIDEACUTE", "GRAVE",
  "WIDEGRAVE", "BREVE", "WIDEBREVE", "TILDE", "WIDETILDE", "DOT", "DDOT",
  "DDDOT", "DDDDOT", "UNARYMINUS", "UNARYPLUS", "BEGINENV", "ENDENV",
  "MATRIX", "PMATRIX", "BMATRIX", "BBMATRIX", "VMATRIX", "VVMATRIX", "SVG",
  "ENDSVG", "SMALLMATRIX", "CASES", "ALIGNED", "GATHERED", "SUBSTACK",
  "PMOD", "RMCHAR", "COLOR", "BGCOLOR", "XARROW", "OPTARGOPEN",
  "OPTARGCLOSE", "$accept", "doc", "xmlmmlTermList", "char", "expression",
  "compoundTermList", "compoundTerm", "closedTerm", "left", "right",
  "bigdelim", "unrecognized", "unaryminus", "unaryplus", "mi", "mib", "mn",
  "mob", "mo", "space", "statusline", "tooltip", "toggle", "fghighlight",
  "bghighlight", "color", "mathrlap", "mathllap", "mathclap", "textstring",
  "displaystyle", "textstyle", "textsize", "scriptsize",
  "scriptscriptsize", "italics", "slashed", "boxed", "bold", "roman",
  "rmchars", "bbold", "bbchars", "bbchar", "frak", "frakletters",
  "frakletter", "cal", "calletters", "calletter", "thinspace", "medspace",
  "thickspace", "quad", "qquad", "negspace", "phantom", "href", "tensor",
  "multi", "subsupList", "subsupTerm", "mfrac", "pmod", "texover",
  "texatop", "binom", "munderbrace", "munderline", "moverbrace", "bar",
  "vec", "dot", "ddot", "dddot", "ddddot", "acute", "grave", "breve",
  "tilde", "check", "hat", "msqrt", "mroot", "munder", "mover",
  "munderover", "emptymrow", "mathenv", "columnAlignList", "substack",
  "array", "arrayopts", "anarrayopt", "collayout", "colalign", "rowalign",
  "align", "eqrows", "eqcols", "rowlines", "collines", "frame", "padding",
  "tableRowList", "tableRow", "simpleTableRow", "optsTableRow", "rowopts",
  "arowopt", "tableCell", "cellopts", "acellopt", "rowspan", "colspan", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420
};
# endif

#define YYPACT_NINF -354

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-354)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      71,  -354,  1659,  1815,    19,    71,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  3834,  3834,  3524,    44,   116,   166,   169,
     172,    17,   103,   184,   187,  3834,  3834,   -77,   -49,    16,
      18,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  2282,  3834,  3834,  3834,  3834,  3834,  3834,  3834,
    3834,  3834,  3834,    -4,    76,  3834,  3834,  3834,  3834,   -27,
       7,   -26,     0,  3834,  3834,  3834,  3834,  3834,  3834,  3834,
    3834,    41,  -354,  -354,  -354,  -354,  -354,  -354,  3834,    70,
    -354,  -354,    78,    84,  3834,   118,   124,   113,  3834,  3834,
    3834,  3834,  3834,  3834,  3834,  3834,  3834,  3834,  3834,  3834,
    3834,  3834,  3834,  3834,  3834,  3834,  3834,  3834,  -354,  -354,
     -50,   183,  3834,  -354,   132,   149,  2438,  1971,  -354,   215,
    3524,  -354,  -354,  -354,  -354,  -354,   220,  -354,   222,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,    65,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  2127,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  1028,   161,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  3834,  3834,
    -354,  -354,  -354,  -354,  3524,  -354,  3834,  3834,  3834,  3834,
    3834,  -354,  -354,  -354,  3834,   201,   224,  3834,  2594,  -354,
    -354,  -354,  -354,    85,  -354,   137,  -354,  -354,  -354,  -354,
     147,  -354,  -354,   158,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  3834,  3834,  3834,  3834,
    3834,  3834,   125,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,    -8,  2749,  2749,  2749,  2749,  2749,  2749,
     -83,  2749,  2749,  2749,  2749,  2749,  -354,  3524,  3524,  3524,
    -354,  -354,  -354,  3834,  3834,  1189,  3834,  3834,  3834,  3834,
    -354,  -354,  3524,  3524,  -354,  -354,  -354,  -354,  1344,  -354,
    -354,  -354,  -354,  -354,  3834,  3834,  3679,   224,   224,  -354,
     165,   233,   235,   236,   237,  3524,    -1,  -354,   194,  -354,
    -354,   -66,    90,  -354,    57,  -354,    -5,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,   174,   177,   198,   -24,   -23,   -22,
     -21,   -20,   -19,   105,  -354,   -18,   -17,   -16,   -15,     4,
    3524,  3524,  1501,   245,   247,  3524,  3524,   146,  -354,   248,
     250,   251,   254,  2904,  3059,  3834,  -354,  -354,  3834,   255,
     175,  -354,  3834,   224,    83,   176,   -41,  -354,  2749,  3214,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,   185,   198,  -354,
      -7,   126,   123,   127,   122,   128,   130,  -354,   119,   121,
     129,   117,  -354,  3989,  3834,  3834,  3369,  3369,  -354,  -354,
    -354,  3834,  3834,  3834,  3834,  -354,  -354,  -354,  -354,  3834,
    -354,    -3,   181,   205,   207,   214,   216,   217,   218,   219,
     221,   225,   226,    96,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,    56,  -354,   227,
     229,  -354,  -354,    -9,  -354,  -354,  -354,  -354,  -354,   167,
      -6,  -354,  2749,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,   224,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  2749,  -354,  3214,
    -354,  -354,  -354,  3524,  -354,   212,  2749,   -14,   186,   120,
     194,  3524,   239,   -13,   261,  -354,  -354,   173,   262,  -354,
     228,  -354,  -354
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       3,     8,     0,     0,     0,     2,     4,     5,     9,   133,
     134,   135,   139,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   148,   140,   141,   145,   149,   146,   144,   143,   142,
     147,   136,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   196,   197,   198,   199,   200,   201,     0,     0,
     130,   256,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   131,   132,
       0,     0,     0,   177,     0,     0,     0,     0,    13,    29,
       0,   138,   103,    31,    32,    34,    33,    35,   137,    36,
      79,    89,    90,    91,    92,    93,    94,    64,    63,    62,
      80,    65,    66,    67,    68,    69,    70,    76,    77,    71,
      72,    73,    74,    75,    78,    81,    82,    83,    84,    85,
      86,    87,    88,    37,    38,    39,   102,    95,    96,    40,
      58,    59,    57,    45,    46,    48,    49,    50,    51,    54,
      55,    56,    53,    52,    47,    41,    42,    43,    44,    60,
      61,   100,   101,    30,    10,     0,     1,     6,     7,    28,
      33,   137,    27,     0,    29,   106,   105,   104,   112,   110,
     111,   115,   113,   114,   118,   116,   117,   121,   119,   120,
     123,   122,   125,   124,   127,   126,   129,   128,     0,     0,
     150,   151,   152,   153,     0,   247,     0,     0,     0,     0,
       0,   226,   225,   224,     0,     0,     0,     0,   309,   171,
     174,   173,   172,     0,   176,     0,   183,   184,   185,   180,
       0,   195,   192,     0,   190,   187,   164,   163,   162,   168,
     169,   170,   166,   167,   165,   202,     0,     0,     0,     0,
       0,     0,     0,   227,   228,   229,   230,   245,   246,   243,
     244,   235,   236,   237,   238,   239,   240,   241,   242,   231,
     232,   233,   234,     0,   309,   309,   309,   309,   309,   309,
       0,   309,   309,   309,   309,   309,   217,     0,     0,     0,
     252,    11,    14,     0,     0,     0,     0,     0,     0,     0,
     178,    12,     0,     0,    98,    97,   215,   216,     0,   249,
     222,   223,   251,   253,     0,     0,     0,     0,   205,   209,
       0,     0,     0,     0,     0,   310,     0,   298,   300,   301,
     302,     0,     0,   181,     0,   193,     0,   188,   203,   155,
     156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   270,     0,     0,     0,     0,     0,
     160,   161,     0,    26,    25,     0,     0,     0,    99,    22,
      20,    18,    16,     0,     0,     0,   255,   213,     0,   212,
       0,   210,     0,     0,     0,     0,     0,   274,   309,   309,
     175,   179,   182,   191,   194,   186,   189,     0,     0,   272,
       0,     0,     0,     0,     0,     0,     0,   269,     0,     0,
       0,     0,   273,     0,     0,     0,     0,     0,   109,   108,
     107,     0,     0,     0,     0,   218,   220,   248,   214,     0,
     204,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   276,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   307,   308,     0,   305,     0,
       0,   314,   315,     0,   312,   316,   317,   299,   303,     0,
       0,   271,   309,   257,   259,   260,   262,   261,   263,   264,
     265,   266,   258,   250,   254,    24,    23,   219,   221,    21,
      19,    17,    15,   211,     0,   207,   208,   288,   289,   290,
     291,   292,   293,   294,   295,   296,   297,   309,   277,   309,
     306,   318,   319,     0,   313,     0,   309,     0,     0,     0,
     304,   311,     0,     0,     0,   206,   275,     0,     0,   268,
       0,   267,   154
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -354,  -354,  -354,   316,   317,    15,  -108,   576,  -354,  -205,
    -354,  -354,  -354,  -354,  -354,    -2,  -354,    66,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
      72,  -354,  -354,  -251,  -354,  -354,  -253,  -354,  -354,  -254,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -244,  -345,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -104,
    -354,  -354,  -354,  -147,  -354,  -353,  -349,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -280,   -91,  -211,  -354,  -354,  -158,
     -84,  -354,  -156,  -354,  -354
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,     6,     7,   355,   118,   119,   120,   398,
     121,   122,   123,   124,   125,   200,   127,   201,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   362,   259,   153,   366,   265,   154,   364,   262,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     348,   349,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   430,
     192,   193,   473,   474,   475,   476,   477,   478,   479,   480,
     481,   482,   483,   484,   356,   357,   358,   359,   487,   488,
     360,   493,   494,   495,   496
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint16 yytable[] =
{
     126,   126,   350,   411,   363,   411,   365,   543,   303,   322,
     367,   246,   524,   126,   420,   417,   383,   117,   195,   196,
     442,   230,   464,   465,   377,   378,   379,   380,   381,   382,
     203,   385,   386,   387,   388,   389,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   375,   231,
     501,   501,   253,   260,   464,   465,   220,   221,   261,   418,
     489,   490,   485,   491,   418,   411,   486,   492,   128,   128,
     384,   376,   539,   502,   546,   425,     1,     2,     3,   263,
     264,   128,   205,   206,   207,   264,   255,   322,   256,   257,
     258,   248,   489,   490,   330,   322,   304,   305,   306,   307,
     308,   309,   310,   410,   311,   312,   313,   314,   247,   525,
     424,   422,   537,   426,   232,   126,   233,   411,   126,   464,
     465,   431,   432,   433,   434,   435,   436,   438,   439,   440,
     441,   554,   558,   254,   485,   325,   556,   423,   486,   274,
     491,   261,   222,   223,   492,   463,   464,   465,   466,   467,
     468,   469,   470,   471,   472,   208,   209,   210,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   276,   462,
     421,   256,   257,   258,   323,   324,   277,   335,   345,   346,
     418,   412,   278,   128,   448,   449,   128,   450,   345,   346,
     280,   460,   282,   126,   345,   346,   281,   526,   315,   345,
     346,   126,   555,   411,   317,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   345,   346,   347,   322,   256,   257,
     258,   318,   547,   224,   225,   330,   226,   227,   323,   324,
     322,   261,   126,   326,   327,   328,   329,   345,   346,   464,
     465,   517,   518,   264,   374,   113,   126,   322,   413,   338,
     414,   415,   416,   419,   427,   429,   428,   549,   437,   444,
     445,   128,   451,   452,   499,   453,   553,   454,   459,   128,
     504,   506,   503,   509,   512,   505,   510,   527,   507,   528,
     548,   508,   322,   322,   322,   511,   529,   545,   530,   531,
     532,   533,   552,   534,   560,   322,   322,   535,   536,   541,
     128,   542,   126,   126,   126,   126,   126,   126,   562,   126,
     126,   126,   126,   126,   128,   126,   126,   126,   557,   559,
     561,   197,   198,   126,   500,   361,   538,   497,   550,   540,
     126,   126,   390,   391,   392,   498,   126,   544,   322,   322,
       0,     0,     0,     0,     0,     0,     0,   403,   404,     0,
       0,     0,     0,   126,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     128,   128,   128,   128,   128,   128,     0,   128,   128,   128,
     128,   128,     0,   128,   128,   128,     0,     0,   126,   126,
     126,   128,     0,   126,   126,     0,     0,     0,   128,   128,
       0,   126,   126,     0,   128,     0,     0,     0,     0,     0,
     446,   447,     0,     0,     0,     0,   126,   126,     0,     0,
       0,   128,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   322,   126,   126,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   128,   128,   128,     0,
       0,   128,   128,     0,     0,     0,     0,     0,     0,   128,
     128,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   128,   128,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     126,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   128,   128,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   126,     0,   126,     0,     0,
       0,   126,     0,     0,   126,     0,     0,     0,     0,   126,
       0,     0,     0,     0,     0,     0,     0,     0,   551,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   128,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   199,
     202,   204,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   228,   229,   128,     0,   128,     0,     0,     0,   128,
       0,     0,   128,     0,     0,     0,     0,   128,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,     0,
       0,   249,   250,   251,   252,     0,     0,     0,     0,   266,
     267,   268,   269,   270,   271,   272,   273,     0,     0,     0,
       0,     0,     0,     0,   275,     0,     0,     0,     0,     0,
     279,     0,     0,     0,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   302,     0,     0,     0,     0,   316,     0,
       0,     0,   320,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   336,   337,     0,     0,     0,     0,
       0,     0,   339,   340,   341,   342,   343,     0,     0,     0,
     344,     0,     0,   351,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   368,   369,   370,   371,   372,   373,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   393,
     394,     0,   399,   400,   401,   402,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     406,   407,   409,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   457,     0,     0,   458,     0,     0,     0,   461,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   514,
     515,   516,     0,     0,     0,     0,     0,   519,   520,   521,
     522,   332,   333,     0,     0,   523,     0,     9,    10,    11,
      12,    13,    14,    15,   334,    16,     0,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,     0,     0,    54,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,    61,    62,    63,    64,    65,     0,
      66,    67,    68,    69,    70,    71,     0,     0,     0,     0,
       0,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,     0,     0,     0,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   111,   112,   113,   114,
     115,   116,   395,   396,     0,     0,     0,     0,     9,    10,
      11,    12,    13,    14,    15,     0,    16,   397,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,    64,    65,
       0,    66,    67,    68,    69,    70,    71,     0,     0,     0,
       0,     0,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,     0,     0,
       0,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   111,   112,   113,
     114,   115,   116,     9,    10,    11,    12,    13,    14,    15,
       0,    16,     0,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
      61,    62,    63,    64,    65,     0,    66,    67,    68,    69,
      70,    71,     0,     0,     0,     0,     0,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,     0,     0,     0,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   111,   112,   113,   114,   115,   116,     0,   405,
       9,    10,    11,    12,    13,    14,    15,     0,    16,     0,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,     0,     0,    54,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,    61,    62,    63,
      64,    65,     0,    66,    67,    68,    69,    70,    71,     0,
       0,     0,     0,     0,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
       0,     0,     0,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   111,
     112,   113,   114,   115,   116,     0,   443,     8,     9,    10,
      11,    12,    13,    14,    15,     0,    16,     0,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,    64,    65,
       0,    66,    67,    68,    69,    70,    71,     0,     0,     0,
       0,     0,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,     0,     0,
       0,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   111,   112,   113,
     114,   115,   116,   194,     9,    10,    11,    12,    13,    14,
      15,     0,    16,     0,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,     0,     0,    54,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
       0,    61,    62,    63,    64,    65,     0,    66,    67,    68,
      69,    70,    71,     0,     0,     0,     0,     0,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,     0,     0,     0,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   111,   112,   113,   114,   115,   116,   321,
       9,    10,    11,    12,    13,    14,    15,     0,    16,     0,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,     0,     0,    54,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,    61,    62,    63,
      64,    65,     0,    66,    67,    68,    69,    70,    71,     0,
       0,     0,     0,     0,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
       0,     0,     0,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   111,
     112,   113,   114,   115,   116,   331,     9,    10,    11,    12,
      13,    14,    15,     0,    16,     0,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,     0,     0,    54,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,    61,    62,    63,    64,    65,     0,    66,
      67,    68,    69,    70,    71,     0,     0,     0,     0,     0,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,     0,     0,     0,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   111,   112,   113,   114,   115,
     116,     9,    10,    11,    12,     0,     0,    15,     0,    16,
       0,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,     0,     0,
      54,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    55,    56,    57,    58,    59,
      60,     0,     0,     0,     0,     0,     0,     0,    61,    62,
      63,    64,    65,     0,    66,    67,    68,    69,    70,    71,
       0,     0,     0,     0,     0,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,     0,     0,     0,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     111,   112,   113,   114,   115,   116,   234,     9,    10,    11,
      12,     0,     0,    15,     0,    16,     0,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,     0,     0,    54,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,    61,    62,    63,    64,    65,     0,
      66,    67,    68,    69,    70,    71,     0,     0,     0,     0,
       0,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,     0,     0,     0,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   111,   112,   113,   114,
     115,   116,   319,     9,    10,    11,    12,    13,    14,    15,
       0,    16,     0,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
       0,     0,    54,     0,     0,   352,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
      61,    62,    63,    64,    65,   353,    66,    67,    68,    69,
      70,    71,     0,     0,   354,     0,     0,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,     0,     0,     0,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   111,   112,   113,   114,   115,   116,     9,    10,
      11,    12,    13,    14,    15,     0,    16,     0,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,    64,    65,
     353,    66,    67,    68,    69,    70,    71,     0,     0,   354,
       0,     0,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,     0,     0,
       0,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   111,   112,   113,
     114,   115,   116,     9,    10,    11,    12,    13,    14,    15,
     455,    16,     0,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
      61,    62,    63,    64,    65,     0,    66,    67,    68,    69,
      70,    71,     0,     0,     0,     0,     0,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,     0,     0,     0,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   111,   112,   113,   114,   115,   116,     9,    10,
      11,    12,    13,    14,    15,   456,    16,     0,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,    64,    65,
       0,    66,    67,    68,    69,    70,    71,     0,     0,     0,
       0,     0,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,     0,     0,
       0,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   111,   112,   113,
     114,   115,   116,     9,    10,    11,    12,    13,    14,    15,
       0,    16,     0,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
      61,    62,    63,    64,    65,     0,    66,    67,    68,    69,
      70,    71,     0,     0,   354,     0,     0,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,     0,     0,     0,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   111,   112,   113,   114,   115,   116,     9,    10,
      11,    12,    13,    14,    15,     0,    16,   397,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,    64,    65,
       0,    66,    67,    68,    69,    70,    71,     0,     0,     0,
       0,     0,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,     0,     0,
       0,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   111,   112,   113,
     114,   115,   116,     9,    10,    11,    12,    13,    14,    15,
       0,    16,     0,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
      61,    62,    63,    64,    65,     0,    66,    67,    68,    69,
      70,    71,     0,     0,     0,     0,     0,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,     0,     0,     0,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   111,   112,   113,   114,   115,   116,     9,    10,
      11,    12,   408,     0,    15,     0,    16,     0,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,    64,    65,
       0,    66,    67,    68,    69,    70,    71,     0,     0,     0,
       0,     0,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,     0,     0,
       0,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   111,   112,   113,
     114,   115,   116,     9,    10,    11,    12,     0,     0,    15,
       0,    16,     0,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
      61,    62,    63,    64,    65,     0,    66,    67,    68,    69,
      70,    71,     0,     0,     0,     0,     0,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,     0,     0,     0,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   111,   112,   113,   114,   115,   116,     9,    10,
      11,    12,     0,     0,    15,     0,    16,     0,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,    64,    65,
       0,    66,    67,    68,    69,    70,    71,     0,     0,     0,
       0,     0,    72,    73,    74,    75,    76,    77,    78,    79,
      80,   513,    82,    83,    84,    85,    86,    87,     0,     0,
       0,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   111,   112,   113,
     114,   115,   116
};

static const yytype_int16 yycheck[] =
{
       2,     3,   246,   348,   255,   350,   260,    16,    58,   117,
     263,    15,    15,    15,    80,    16,    99,     2,     3,     0,
      16,    98,    63,    64,   304,   305,   306,   307,   308,   309,
      15,   311,   312,   313,   314,   315,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    56,    98,
      57,    57,    79,    79,    63,    64,    39,    40,    84,    60,
     101,   102,   415,   416,    60,   410,   415,   416,     2,     3,
     153,    79,    16,    80,    80,    80,     5,     6,     7,    79,
      85,    15,    38,    39,    40,    85,    79,   195,    81,    82,
      83,    15,   101,   102,   160,   203,   146,   147,   148,   149,
     150,   151,   152,   347,   154,   155,   156,   157,   112,   112,
     364,   362,    16,   366,    98,   117,    98,   462,   120,    63,
      64,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   160,   487,   120,    16,    80,   487,    98,
     493,    84,    39,    40,   493,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    39,    40,    41,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    98,   413,
      80,    81,    82,    83,    13,    14,    98,    16,    13,    14,
      60,    16,    98,   117,    38,    39,   120,    41,    13,    14,
      72,    16,    79,   195,    13,    14,    72,    16,    15,    13,
      14,   203,    16,   548,    72,    39,    40,    41,    39,    40,
      41,    39,    40,    41,    13,    14,    15,   325,    81,    82,
      83,    72,   502,    39,    40,   160,    39,    40,    13,    14,
     338,    84,   234,    13,    14,    13,    14,    13,    14,    63,
      64,   446,   447,    85,   119,   160,   248,   355,    15,   234,
      15,    15,    15,    59,    80,    57,    79,   537,   153,    14,
      13,   195,    14,    13,    79,    14,   546,    13,    13,   203,
     147,   149,   146,   154,   157,   148,   155,    72,   150,    72,
     524,   151,   390,   391,   392,   156,    72,   120,    72,    72,
      72,    72,    80,    72,   121,   403,   404,    72,    72,    72,
     234,    72,   304,   305,   306,   307,   308,   309,    80,   311,
     312,   313,   314,   315,   248,   317,   318,   319,    79,    58,
      58,     5,     5,   325,   428,   253,   473,   418,   539,   487,
     332,   333,   317,   318,   319,   419,   338,   493,   446,   447,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   332,   333,    -1,
      -1,    -1,    -1,   355,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     304,   305,   306,   307,   308,   309,    -1,   311,   312,   313,
     314,   315,    -1,   317,   318,   319,    -1,    -1,   390,   391,
     392,   325,    -1,   395,   396,    -1,    -1,    -1,   332,   333,
      -1,   403,   404,    -1,   338,    -1,    -1,    -1,    -1,    -1,
     395,   396,    -1,    -1,    -1,    -1,   418,   419,    -1,    -1,
      -1,   355,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   551,   446,   447,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   390,   391,   392,    -1,
      -1,   395,   396,    -1,    -1,    -1,    -1,    -1,    -1,   403,
     404,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   418,   419,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     502,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   446,   447,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   537,    -1,   539,    -1,    -1,
      -1,   543,    -1,    -1,   546,    -1,    -1,    -1,    -1,   551,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   543,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   502,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,
      14,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    26,   537,    -1,   539,    -1,    -1,    -1,   543,
      -1,    -1,   546,    -1,    -1,    -1,    -1,   551,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    -1,
      -1,    55,    56,    57,    58,    -1,    -1,    -1,    -1,    63,
      64,    65,    66,    67,    68,    69,    70,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    -1,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,    -1,    -1,    -1,    -1,   112,    -1,
      -1,    -1,   116,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   228,   229,    -1,    -1,    -1,    -1,
      -1,    -1,   236,   237,   238,   239,   240,    -1,    -1,    -1,
     244,    -1,    -1,   247,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   276,   277,   278,   279,   280,   281,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   323,
     324,    -1,   326,   327,   328,   329,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     344,   345,   346,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   405,    -1,    -1,   408,    -1,    -1,    -1,   412,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   443,
     444,   445,    -1,    -1,    -1,    -1,    -1,   451,   452,   453,
     454,     3,     4,    -1,    -1,   459,    -1,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    -1,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    73,    74,    75,    76,    77,    78,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,    -1,
      92,    93,    94,    95,    96,    97,    -1,    -1,    -1,    -1,
      -1,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    -1,    -1,    -1,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   158,   159,   160,   161,
     162,   163,     3,     4,    -1,    -1,    -1,    -1,     9,    10,
      11,    12,    13,    14,    15,    -1,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,    77,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,
      -1,    92,    93,    94,    95,    96,    97,    -1,    -1,    -1,
      -1,    -1,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   158,   159,   160,
     161,   162,   163,     9,    10,    11,    12,    13,    14,    15,
      -1,    17,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,
      76,    77,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    88,    89,    90,    -1,    92,    93,    94,    95,
      96,    97,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    -1,    -1,    -1,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   158,   159,   160,   161,   162,   163,    -1,   165,
       9,    10,    11,    12,    13,    14,    15,    -1,    17,    -1,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    74,    75,    76,    77,    78,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    88,
      89,    90,    -1,    92,    93,    94,    95,    96,    97,    -1,
      -1,    -1,    -1,    -1,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
      -1,    -1,    -1,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   158,
     159,   160,   161,   162,   163,    -1,   165,     8,     9,    10,
      11,    12,    13,    14,    15,    -1,    17,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,    77,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,
      -1,    92,    93,    94,    95,    96,    97,    -1,    -1,    -1,
      -1,    -1,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   158,   159,   160,
     161,   162,   163,     8,     9,    10,    11,    12,    13,    14,
      15,    -1,    17,    -1,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      75,    76,    77,    78,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    88,    89,    90,    -1,    92,    93,    94,
      95,    96,    97,    -1,    -1,    -1,    -1,    -1,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   158,   159,   160,   161,   162,   163,     8,
       9,    10,    11,    12,    13,    14,    15,    -1,    17,    -1,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    74,    75,    76,    77,    78,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    88,
      89,    90,    -1,    92,    93,    94,    95,    96,    97,    -1,
      -1,    -1,    -1,    -1,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
      -1,    -1,    -1,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   158,
     159,   160,   161,   162,   163,     8,     9,    10,    11,    12,
      13,    14,    15,    -1,    17,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    75,    76,    77,    78,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    88,    89,    90,    -1,    92,
      93,    94,    95,    96,    97,    -1,    -1,    -1,    -1,    -1,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,    -1,    -1,    -1,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   158,   159,   160,   161,   162,
     163,     9,    10,    11,    12,    -1,    -1,    15,    -1,    17,
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,    77,
      78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,
      88,    89,    90,    -1,    92,    93,    94,    95,    96,    97,
      -1,    -1,    -1,    -1,    -1,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    -1,    -1,    -1,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     158,   159,   160,   161,   162,   163,   164,     9,    10,    11,
      12,    -1,    -1,    15,    -1,    17,    -1,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    73,    74,    75,    76,    77,    78,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,    -1,
      92,    93,    94,    95,    96,    97,    -1,    -1,    -1,    -1,
      -1,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    -1,    -1,    -1,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   158,   159,   160,   161,
     162,   163,   164,     9,    10,    11,    12,    13,    14,    15,
      -1,    17,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    -1,    -1,    61,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,
      76,    77,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    -1,    -1,   100,    -1,    -1,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    -1,    -1,    -1,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   158,   159,   160,   161,   162,   163,     9,    10,
      11,    12,    13,    14,    15,    -1,    17,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,    77,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    -1,    -1,   100,
      -1,    -1,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   158,   159,   160,
     161,   162,   163,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,
      76,    77,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    88,    89,    90,    -1,    92,    93,    94,    95,
      96,    97,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    -1,    -1,    -1,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   158,   159,   160,   161,   162,   163,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,    77,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,
      -1,    92,    93,    94,    95,    96,    97,    -1,    -1,    -1,
      -1,    -1,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   158,   159,   160,
     161,   162,   163,     9,    10,    11,    12,    13,    14,    15,
      -1,    17,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,
      76,    77,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    88,    89,    90,    -1,    92,    93,    94,    95,
      96,    97,    -1,    -1,   100,    -1,    -1,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    -1,    -1,    -1,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   158,   159,   160,   161,   162,   163,     9,    10,
      11,    12,    13,    14,    15,    -1,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,    77,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,
      -1,    92,    93,    94,    95,    96,    97,    -1,    -1,    -1,
      -1,    -1,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   158,   159,   160,
     161,   162,   163,     9,    10,    11,    12,    13,    14,    15,
      -1,    17,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,
      76,    77,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    88,    89,    90,    -1,    92,    93,    94,    95,
      96,    97,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    -1,    -1,    -1,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   158,   159,   160,   161,   162,   163,     9,    10,
      11,    12,    13,    -1,    15,    -1,    17,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,    77,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,
      -1,    92,    93,    94,    95,    96,    97,    -1,    -1,    -1,
      -1,    -1,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   158,   159,   160,
     161,   162,   163,     9,    10,    11,    12,    -1,    -1,    15,
      -1,    17,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,
      76,    77,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    88,    89,    90,    -1,    92,    93,    94,    95,
      96,    97,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    -1,    -1,    -1,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   158,   159,   160,   161,   162,   163,     9,    10,
      11,    12,    -1,    -1,    15,    -1,    17,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,    77,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,
      -1,    92,    93,    94,    95,    96,    97,    -1,    -1,    -1,
      -1,    -1,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   158,   159,   160,
     161,   162,   163
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     5,     6,     7,   167,   168,   169,   170,     8,     9,
      10,    11,    12,    13,    14,    15,    17,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    58,    73,    74,    75,    76,    77,
      78,    86,    87,    88,    89,    90,    92,    93,    94,    95,
      96,    97,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   158,   159,   160,   161,   162,   163,   171,   172,   173,
     174,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   210,   213,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   256,   257,     8,   171,     0,   169,   170,   173,
     181,   183,   173,   171,   173,    38,    39,    40,    39,    40,
      41,    39,    40,    41,    39,    40,    41,    39,    40,    41,
      39,    40,    39,    40,    39,    40,    39,    40,   173,   173,
      98,    98,    98,    98,   164,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,    15,   112,    15,   173,
     173,   173,   173,    79,   160,    79,    81,    82,    83,   209,
      79,    84,   215,    79,    85,   212,   173,   173,   173,   173,
     173,   173,   173,   173,    98,   173,    98,    98,    98,   173,
      72,    72,    79,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,    58,   146,   147,   148,   149,   150,   151,
     152,   154,   155,   156,   157,    15,   173,    72,    72,   164,
     173,     8,   172,    13,    14,   171,    13,    14,    13,    14,
     160,     8,     3,     4,    16,    16,   173,   173,   171,   173,
     173,   173,   173,   173,   173,    13,    14,    15,   226,   227,
     226,   173,    61,    91,   100,   171,   270,   271,   272,   273,
     276,   206,   208,   209,   214,   215,   211,   212,   173,   173,
     173,   173,   173,   173,   119,    56,    79,   270,   270,   270,
     270,   270,   270,    99,   153,   270,   270,   270,   270,   270,
     171,   171,   171,   173,   173,     3,     4,    18,   175,   173,
     173,   173,   173,   171,   171,   165,   173,   173,    13,   173,
     226,   227,    16,    15,    15,    15,    15,    16,    60,    59,
      80,    80,   209,    80,   215,    80,   212,    80,    79,    57,
     255,   145,   145,   145,   145,   145,   145,   153,   145,   145,
     145,   145,    16,   165,    14,    13,   171,   171,    38,    39,
      41,    14,    13,    14,    13,    16,    16,   173,   173,    13,
      16,   173,   226,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   261,   262,   274,   275,   101,
     102,   261,   262,   277,   278,   279,   280,   271,   276,    79,
     255,    57,    80,   146,   147,   148,   149,   150,   151,   154,
     155,   156,   157,   112,   173,   173,   173,   175,   175,   173,
     173,   173,   173,   173,    15,   112,    16,    72,    72,    72,
      72,    72,    72,    72,    72,    72,    72,    16,   259,    16,
     275,    72,    72,    16,   278,   120,    80,   270,   226,   270,
     272,   171,    80,   270,   145,    16,    16,    79,   145,    58,
     121,    58,    80
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   166,   167,   168,   168,   168,   168,   168,   169,   170,
     170,   170,   170,   171,   171,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   174,   174,   174,   175,   175,   175,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   185,   186,   187,   188,   189,   190,
     191,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   205,   206,   206,   207,
     207,   208,   208,   209,   209,   209,   210,   210,   211,   211,
     212,   213,   213,   214,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   224,   225,   225,   225,   226,
     226,   227,   227,   227,   227,   228,   228,   229,   230,   230,
     231,   231,   232,   232,   233,   234,   235,   236,   236,   237,
     237,   238,   239,   240,   241,   242,   242,   243,   243,   244,
     244,   245,   245,   246,   246,   247,   247,   248,   249,   249,
     250,   250,   251,   251,   252,   252,   253,   254,   254,   254,
     254,   254,   254,   254,   254,   254,   254,   254,   254,   254,
     254,   255,   255,   256,   257,   257,   258,   258,   259,   259,
     259,   259,   259,   259,   259,   259,   259,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   270,
     271,   271,   272,   272,   273,   274,   274,   275,   275,   276,
     276,   276,   277,   277,   278,   278,   278,   278,   279,   280
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     1,     1,     2,     2,     1,     2,
       2,     3,     3,     1,     2,     5,     3,     5,     3,     5,
       3,     5,     3,     5,     5,     3,     3,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       1,     1,     1,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     2,    10,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     4,     2,     1,     2,     4,
       2,     1,     2,     1,     1,     1,     4,     2,     1,     2,
       1,     4,     2,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     2,     3,     5,     3,     8,     6,     6,     1,
       2,     4,     2,     2,     3,     3,     3,     2,     5,     5,
       5,     5,     3,     3,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     5,     3,
       5,     3,     2,     3,     5,     4,     1,     5,     5,     5,
       5,     5,     5,     5,     5,     5,     5,     9,     8,     4,
       3,     2,     1,     4,     4,     8,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     3,
       1,     1,     1,     3,     5,     1,     2,     1,     1,     0,
       1,     5,     1,     2,     1,     1,     1,     1,     2,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 286 "itex2MML.y" /* yacc.c:1646  */
    {/* all processing done in body*/}
#line 2809 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 289 "itex2MML.y" /* yacc.c:1646  */
    {/* nothing - do nothing*/}
#line 2815 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 290 "itex2MML.y" /* yacc.c:1646  */
    {/* proc done in body*/}
#line 2821 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 291 "itex2MML.y" /* yacc.c:1646  */
    {/* all proc. in body*/}
#line 2827 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 292 "itex2MML.y" /* yacc.c:1646  */
    {/* all proc. in body*/}
#line 2833 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 293 "itex2MML.y" /* yacc.c:1646  */
    {/* all proc. in body*/}
#line 2839 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 295 "itex2MML.y" /* yacc.c:1646  */
    {printf("%s", (yyvsp[0]));}
#line 2845 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 297 "itex2MML.y" /* yacc.c:1646  */
    {/* empty math group - ignore*/}
#line 2851 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 298 "itex2MML.y" /* yacc.c:1646  */
    {/* ditto */}
#line 2857 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 299 "itex2MML.y" /* yacc.c:1646  */
    {
  char ** r = (char **) ret_str;
  char * s = itex2MML_copy3("<math xmlns='http://www.w3.org/1998/Math/MathML' display='inline'>", (yyvsp[-1]), "</math>");
  itex2MML_free_string((yyvsp[-1]));
  if (r) {
    (*r) = (s == itex2MML_empty_string) ? 0 : s;
  }
  else {
    if (itex2MML_write_mathml)
      (*itex2MML_write_mathml) (s);
    itex2MML_free_string(s);
  }
}
#line 2875 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 312 "itex2MML.y" /* yacc.c:1646  */
    {
  char ** r = (char **) ret_str;
  char * s = itex2MML_copy3("<math xmlns='http://www.w3.org/1998/Math/MathML' display='block'>", (yyvsp[-1]), "</math>");
  itex2MML_free_string((yyvsp[-1]));
  if (r) {
    (*r) = (s == itex2MML_empty_string) ? 0 : s;
  }
  else {
    if (itex2MML_write_mathml)
      (*itex2MML_write_mathml) (s);
    itex2MML_free_string(s);
  }
}
#line 2893 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 326 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 2902 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 330 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2((yyvsp[-1]), (yyvsp[0]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 2912 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 336 "itex2MML.y" /* yacc.c:1646  */
    {
  if (itex2MML_displaymode == 1) {
    char * s1 = itex2MML_copy3("<munderover>", (yyvsp[-4]), " ");
    char * s2 = itex2MML_copy3((yyvsp[-2]), " ", (yyvsp[0]));
    (yyval) = itex2MML_copy3(s1, s2, "</munderover>");
    itex2MML_free_string(s1);
    itex2MML_free_string(s2);
  }
  else {
    char * s1 = itex2MML_copy3("<msubsup>", (yyvsp[-4]), " ");
    char * s2 = itex2MML_copy3((yyvsp[-2]), " ", (yyvsp[0]));
    (yyval) = itex2MML_copy3(s1, s2, "</msubsup>");
    itex2MML_free_string(s1);
    itex2MML_free_string(s2);
  }
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 2936 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 355 "itex2MML.y" /* yacc.c:1646  */
    {
  if (itex2MML_displaymode == 1) {
    char * s1 = itex2MML_copy3("<munder>", (yyvsp[-2]), " ");
    (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</munder>");
    itex2MML_free_string(s1);
  }
  else {
    char * s1 = itex2MML_copy3("<msub>", (yyvsp[-2]), " ");
    (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</msub>");
    itex2MML_free_string(s1);
  }
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 2955 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 369 "itex2MML.y" /* yacc.c:1646  */
    {
  if (itex2MML_displaymode == 1) {
    char * s1 = itex2MML_copy3("<munderover>", (yyvsp[-4]), " ");
    char * s2 = itex2MML_copy3((yyvsp[0]), " ", (yyvsp[-2]));
    (yyval) = itex2MML_copy3(s1, s2, "</munderover>");
    itex2MML_free_string(s1);
    itex2MML_free_string(s2);
  }
  else {
    char * s1 = itex2MML_copy3("<msubsup>", (yyvsp[-4]), " ");
    char * s2 = itex2MML_copy3((yyvsp[0]), " ", (yyvsp[-2]));
    (yyval) = itex2MML_copy3(s1, s2, "</msubsup>");
    itex2MML_free_string(s1);
    itex2MML_free_string(s2);
  }
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 2979 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 388 "itex2MML.y" /* yacc.c:1646  */
    {
  if (itex2MML_displaymode == 1) {
    char * s1 = itex2MML_copy3("<mover>", (yyvsp[-2]), " ");
    (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</mover>");
    itex2MML_free_string(s1);
  }
  else {
    char * s1 = itex2MML_copy3("<msup>", (yyvsp[-2]), " ");
    (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</msup>");
    itex2MML_free_string(s1);
  }
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 2998 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 402 "itex2MML.y" /* yacc.c:1646  */
    {
  if (itex2MML_displaymode == 1) {
    char * s1 = itex2MML_copy3("<munderover>", (yyvsp[-4]), " ");
    char * s2 = itex2MML_copy3((yyvsp[-2]), " ", (yyvsp[0]));
    (yyval) = itex2MML_copy3(s1, s2, "</munderover>");
    itex2MML_free_string(s1);
    itex2MML_free_string(s2);
  }
  else {
    char * s1 = itex2MML_copy3("<msubsup>", (yyvsp[-4]), " ");
    char * s2 = itex2MML_copy3((yyvsp[-2]), " ", (yyvsp[0]));
    (yyval) = itex2MML_copy3(s1, s2, "</msubsup>");
    itex2MML_free_string(s1);
    itex2MML_free_string(s2);
  }
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3022 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 421 "itex2MML.y" /* yacc.c:1646  */
    {
  if (itex2MML_displaymode == 1) {
    char * s1 = itex2MML_copy3("<munder>", (yyvsp[-2]), " ");
    (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</munder>");
    itex2MML_free_string(s1);
  }
  else {
    char * s1 = itex2MML_copy3("<msub>", (yyvsp[-2]), " ");
    (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</msub>");
    itex2MML_free_string(s1);
  }
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3041 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 435 "itex2MML.y" /* yacc.c:1646  */
    {
  if (itex2MML_displaymode == 1) {
    char * s1 = itex2MML_copy3("<munderover>", (yyvsp[-4]), " ");
    char * s2 = itex2MML_copy3((yyvsp[0]), " ", (yyvsp[-2]));
    (yyval) = itex2MML_copy3(s1, s2, "</munderover>");
    itex2MML_free_string(s1);
    itex2MML_free_string(s2);
  }
  else {
    char * s1 = itex2MML_copy3("<msubsup>", (yyvsp[-4]), " ");
    char * s2 = itex2MML_copy3((yyvsp[0]), " ", (yyvsp[-2]));
    (yyval) = itex2MML_copy3(s1, s2, "</msubsup>");
    itex2MML_free_string(s1);
    itex2MML_free_string(s2);
  }
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3065 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 454 "itex2MML.y" /* yacc.c:1646  */
    {
  if (itex2MML_displaymode == 1) {
    char * s1 = itex2MML_copy3("<mover>", (yyvsp[-2]), " ");
    (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</mover>");
    itex2MML_free_string(s1);
  }
  else {
    char * s1 = itex2MML_copy3("<msup>", (yyvsp[-2]), " ");
    (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</msup>");
    itex2MML_free_string(s1);
  }
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3084 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 468 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<msubsup>", (yyvsp[-4]), " ");
  char * s2 = itex2MML_copy3((yyvsp[-2]), " ", (yyvsp[0]));
  (yyval) = itex2MML_copy3(s1, s2, "</msubsup>");
  itex2MML_free_string(s1);
  itex2MML_free_string(s2);
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3099 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 478 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<msubsup>", (yyvsp[-4]), " ");
  char * s2 = itex2MML_copy3((yyvsp[0]), " ", (yyvsp[-2]));
  (yyval) = itex2MML_copy3(s1, s2, "</msubsup>");
  itex2MML_free_string(s1);
  itex2MML_free_string(s2);
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3114 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 488 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<msub>", (yyvsp[-2]), " ");
  (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</msub>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3126 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 495 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<msup>", (yyvsp[-2]), " ");
  (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</msup>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3138 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 502 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<msub><mo></mo>", (yyvsp[0]), "</msub>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3147 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 506 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<msup><mo></mo>", (yyvsp[0]), "</msup>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3156 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 510 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3165 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 519 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mi>", (yyvsp[0]), "</mi>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3174 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 523 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mn>", (yyvsp[0]), "</mn>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3183 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 588 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[-1]));
}
#line 3192 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 592 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow>", (yyvsp[-1]), "</mrow>");
  itex2MML_free_string((yyvsp[-1]));
}
#line 3201 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 596 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mrow>", (yyvsp[-2]), (yyvsp[-1]));
  (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</mrow>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3214 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 609 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo>", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3224 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 614 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo>", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3234 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 619 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy_string("");
  itex2MML_free_string((yyvsp[0]));
}
#line 3244 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 625 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo>", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3253 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 629 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo>", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3262 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 633 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("");
  itex2MML_free_string((yyvsp[0]));
}
#line 3271 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 638 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"1.2em\" minsize=\"1.2em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3281 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 643 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo maxsize=\"1.2em\" minsize=\"1.2em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3290 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 647 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo maxsize=\"1.2em\" minsize=\"1.2em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3299 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 651 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"1.8em\" minsize=\"1.8em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3309 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 656 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo maxsize=\"1.8em\" minsize=\"1.8em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3318 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 660 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo maxsize=\"1.8em\" minsize=\"1.8em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3327 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 664 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"2.4em\" minsize=\"2.4em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3337 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 669 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo maxsize=\"2.4em\" minsize=\"2.4em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3346 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 118:
#line 673 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo maxsize=\"2.4em\" minsize=\"2.4em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3355 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 677 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"3em\" minsize=\"3em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3365 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 682 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo maxsize=\"3em\" minsize=\"3em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3374 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 686 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo maxsize=\"3em\" minsize=\"3em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3383 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 690 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"1.2em\" minsize=\"1.2em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3393 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 695 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"1.2em\" minsize=\"1.2em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3403 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 700 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"1.8em\" minsize=\"1.8em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3413 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 125:
#line 705 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"1.8em\" minsize=\"1.8em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3423 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 710 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"2.4em\" minsize=\"2.4em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3433 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 715 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"2.4em\" minsize=\"2.4em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3443 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 720 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"3em\" minsize=\"3em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3453 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 725 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo maxsize=\"3em\" minsize=\"3em\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3463 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 731 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("<merror><mtext>Unknown character</mtext></merror>");
}
#line 3471 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 131:
#line 735 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("<mo lspace=\"verythinmathspace\" rspace=\"0em\">&minus;</mo>");
}
#line 3479 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 132:
#line 739 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("<mo lspace=\"verythinmathspace\" rspace=\"0em\">+</mo>");
}
#line 3487 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 745 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn=2;
  (yyval) = itex2MML_copy3("<mi>", (yyvsp[0]), "</mi>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3497 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 136:
#line 753 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo lspace=\"thinmathspace\" rspace=\"thinmathspace\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3507 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 139:
#line 761 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo>", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3517 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 140:
#line 766 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo>", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3527 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 771 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mstyle scriptlevel=\"0\"><mo>", (yyvsp[0]), "</mo></mstyle>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3537 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 142:
#line 776 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo stretchy=\"false\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3546 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 780 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo stretchy=\"false\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3556 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 144:
#line 785 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo stretchy=\"false\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3565 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 145:
#line 789 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo stretchy=\"false\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3574 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 146:
#line 793 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mo>", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3583 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 147:
#line 797 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn=2;
  (yyval) = itex2MML_copy3("<mo lspace=\"mediummathspace\" rspace=\"mediummathspace\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3593 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 148:
#line 802 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo lspace=\"0em\" rspace=\"thinmathspace\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3603 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 149:
#line 807 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo lspace=\"verythinmathspace\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3613 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 150:
#line 812 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo lspace=\"0em\" rspace=\"thinmathspace\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3623 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 151:
#line 817 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo lspace=\"thinmathspace\" rspace=\"thinmathspace\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3633 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 152:
#line 822 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo lspace=\"mediummathspace\" rspace=\"mediummathspace\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3643 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 153:
#line 827 "itex2MML.y" /* yacc.c:1646  */
    {
  itex2MML_rowposn = 2;
  (yyval) = itex2MML_copy3("<mo lspace=\"thickmathspace\" rspace=\"thickmathspace\">", (yyvsp[0]), "</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3653 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 154:
#line 833 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mspace height=\"", (yyvsp[-7]), "ex\" depth=\"");
  char * s2 = itex2MML_copy3((yyvsp[-4]), "ex\" width=\"", (yyvsp[-1]));
  (yyval) = itex2MML_copy3(s1, s2, "em\"></mspace>");
  itex2MML_free_string(s1);
  itex2MML_free_string(s2);
  itex2MML_free_string((yyvsp[-7]));
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-1]));
}
#line 3668 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 155:
#line 844 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<maction actiontype=\"statusline\">", (yyvsp[0]), "<mtext>");
  (yyval) = itex2MML_copy3(s1, (yyvsp[-1]), "</mtext></maction>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3680 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 156:
#line 852 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<maction actiontype=\"tooltip\">", (yyvsp[0]), "<mtext>");
  (yyval) = itex2MML_copy3(s1, (yyvsp[-1]), "</mtext></maction>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3692 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 157:
#line 860 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<maction actiontype=\"toggle\" selection=\"2\">", (yyvsp[-1]), " ");
  (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</maction>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3704 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 158:
#line 868 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<maction actiontype=\"highlight\" other='color=", (yyvsp[-1]), "'>");
  (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</maction>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3716 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 159:
#line 876 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<maction actiontype=\"highlight\" other='background=", (yyvsp[-1]), "'>");
  (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</maction>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3728 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 160:
#line 884 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mstyle mathcolor=", (yyvsp[-1]), ">");
  (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</mstyle>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3740 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 161:
#line 891 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mstyle mathbackground=", (yyvsp[-1]), ">");
  (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</mstyle>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3752 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 899 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mpadded width=\"0\">", (yyvsp[0]), "</mpadded>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3761 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 163:
#line 904 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mpadded width=\"0\" lspace=\"-100%width\">", (yyvsp[0]), "</mpadded>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3770 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 164:
#line 909 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mpadded width=\"0\" lspace=\"-50%width\">", (yyvsp[0]), "</mpadded>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3779 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 165:
#line 914 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mtext>", (yyvsp[0]), "</mtext>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3788 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 166:
#line 919 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mstyle displaystyle=\"true\">", (yyvsp[0]), "</mstyle>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3797 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 167:
#line 924 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mstyle displaystyle=\"false\">", (yyvsp[0]), "</mstyle>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3806 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 929 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mstyle scriptlevel=\"0\">", (yyvsp[0]), "</mstyle>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3815 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 934 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mstyle scriptlevel=\"1\">", (yyvsp[0]), "</mstyle>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3824 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 170:
#line 939 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mstyle scriptlevel=\"2\">", (yyvsp[0]), "</mstyle>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3833 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 171:
#line 944 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mstyle mathvariant=\"italic\">", (yyvsp[0]), "</mstyle>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3842 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 172:
#line 949 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<menclose notation=\"updiagonalstrike\">", (yyvsp[0]), "</menclose>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3851 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 173:
#line 954 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<menclose notation=\"box\">", (yyvsp[0]), "</menclose>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3860 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 174:
#line 959 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mstyle mathvariant=\"bold\">", (yyvsp[0]), "</mstyle>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3869 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 964 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mi mathvariant=\"normal\">", (yyvsp[-1]), "</mi>");
  itex2MML_free_string((yyvsp[-1]));
}
#line 3878 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 967 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mi mathvariant=\"normal\">", (yyvsp[0]), "</mi>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3887 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 177:
#line 972 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3896 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 178:
#line 976 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2((yyvsp[-1]), (yyvsp[0]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3906 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 179:
#line 982 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mi>", (yyvsp[-1]), "</mi>");
  itex2MML_free_string((yyvsp[-1]));
}
#line 3915 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 180:
#line 985 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mi>", (yyvsp[0]), "</mi>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3924 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 181:
#line 990 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3933 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 182:
#line 994 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2((yyvsp[-1]), (yyvsp[0]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 3943 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 183:
#line 1000 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("&", (yyvsp[0]), "opf;");
  itex2MML_free_string((yyvsp[0]));
}
#line 3952 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 184:
#line 1004 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("&", (yyvsp[0]), "opf;");
  itex2MML_free_string((yyvsp[0]));
}
#line 3961 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 185:
#line 1008 "itex2MML.y" /* yacc.c:1646  */
    {
  /* Blackboard digits 0-9 correspond to Unicode characters 0x1D7D8-0x1D7E1 */
  char * end = (yyvsp[0]) + 1;
  int code = 0x1D7D8 + strtoul((yyvsp[0]), &end, 10);
  (yyval) = itex2MML_character_reference(code);
  itex2MML_free_string((yyvsp[0]));
}
#line 3973 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 186:
#line 1016 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mi>", (yyvsp[-1]), "</mi>");
  itex2MML_free_string((yyvsp[-1]));
}
#line 3982 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 187:
#line 1019 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mi>", (yyvsp[0]), "</mi>");
  itex2MML_free_string((yyvsp[0]));
}
#line 3991 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 188:
#line 1024 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4000 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 189:
#line 1028 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2((yyvsp[-1]), (yyvsp[0]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4010 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 190:
#line 1034 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("&", (yyvsp[0]), "fr;");
  itex2MML_free_string((yyvsp[0]));
}
#line 4019 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 191:
#line 1039 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mi>", (yyvsp[-1]), "</mi>");
  itex2MML_free_string((yyvsp[-1]));
}
#line 4028 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 192:
#line 1042 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mi>", (yyvsp[0]), "</mi>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4037 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 193:
#line 1047 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4046 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 194:
#line 1051 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2((yyvsp[-1]), (yyvsp[0]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4056 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 195:
#line 1057 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("&", (yyvsp[0]), "scr;");
  itex2MML_free_string((yyvsp[0]));
}
#line 4065 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 196:
#line 1062 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("<mspace width=\"thinmathspace\"></mspace>");
}
#line 4073 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 197:
#line 1066 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("<mspace width=\"mediummathspace\"></mspace>");
}
#line 4081 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 198:
#line 1070 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("<mspace width=\"thickmathspace\"></mspace>");
}
#line 4089 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 199:
#line 1074 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("<mspace width=\"1em\"></mspace>");
}
#line 4097 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 200:
#line 1078 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("<mspace width=\"2em\"></mspace>");
}
#line 4105 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 201:
#line 1082 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("<mspace width=\"-0.1667 em\"></mspace>");
}
#line 4113 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 202:
#line 1086 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mphantom>", (yyvsp[0]), "</mphantom>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4122 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 203:
#line 1091 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mrow xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:type=\"simple\" xlink:href=\"", (yyvsp[-1]), "\">");
  (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</mrow>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4134 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 204:
#line 1099 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mmultiscripts>", (yyvsp[-3]), (yyvsp[-1]));
  (yyval) = itex2MML_copy2(s1, "</mmultiscripts>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-3]));
  itex2MML_free_string((yyvsp[-1]));
}
#line 4146 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 205:
#line 1106 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mmultiscripts>", (yyvsp[-1]), (yyvsp[0]));
  (yyval) = itex2MML_copy2(s1, "</mmultiscripts>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4158 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 206:
#line 1114 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mmultiscripts>", (yyvsp[-3]), (yyvsp[-1]));
  char * s2 = itex2MML_copy3("<mprescripts></mprescripts>", (yyvsp[-5]), "</mmultiscripts>");
  (yyval) = itex2MML_copy2(s1, s2);
  itex2MML_free_string(s1);
  itex2MML_free_string(s2);
  itex2MML_free_string((yyvsp[-5]));
  itex2MML_free_string((yyvsp[-3]));
  itex2MML_free_string((yyvsp[-1]));
}
#line 4173 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 207:
#line 1124 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy2("<mmultiscripts>", (yyvsp[-1]));
  char * s2 = itex2MML_copy3("<mprescripts></mprescripts>", (yyvsp[-3]), "</mmultiscripts>");
  (yyval) = itex2MML_copy2(s1, s2);
  itex2MML_free_string(s1);
  itex2MML_free_string(s2);
  itex2MML_free_string((yyvsp[-3]));
  itex2MML_free_string((yyvsp[-1]));
}
#line 4187 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 208:
#line 1133 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mmultiscripts>", (yyvsp[-3]), (yyvsp[-1]));
  (yyval) = itex2MML_copy2(s1, "</mmultiscripts>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-3]));
  itex2MML_free_string((yyvsp[-1])); 
}
#line 4199 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 209:
#line 1141 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4208 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 210:
#line 1145 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3((yyvsp[-1]), " ", (yyvsp[0]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4218 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 211:
#line 1151 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3((yyvsp[-2]), " ", (yyvsp[0]));
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4228 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 212:
#line 1156 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2((yyvsp[0]), " <none></none>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4237 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 213:
#line 1160 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("<none></none> ", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4246 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 214:
#line 1164 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("<none></none> ", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4255 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 215:
#line 1169 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mfrac>", (yyvsp[-1]), (yyvsp[0]));
  (yyval) = itex2MML_copy2(s1, "</mfrac>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4267 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 216:
#line 1176 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mstyle displaystyle=\"false\"><mfrac>", (yyvsp[-1]), (yyvsp[0]));
  (yyval) = itex2MML_copy2(s1, "</mfrac></mstyle>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4279 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 217:
#line 1184 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3( "<mo lspace=\"mediummathspace\">(</mo><mo rspace=\"thinmathspace\">mod</mo>", (yyvsp[0]), "<mo rspace=\"mediummathspace\">)</mo>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4288 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 1189 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mfrac><mrow>", (yyvsp[-3]), "</mrow><mrow>");
  (yyval) = itex2MML_copy3(s1, (yyvsp[-1]), "</mrow></mfrac>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-3]));
  itex2MML_free_string((yyvsp[-1]));
}
#line 4300 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 1196 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mrow>", (yyvsp[-4]), "<mfrac><mrow>");
  char * s2 = itex2MML_copy3((yyvsp[-3]), "</mrow><mrow>", (yyvsp[-1]));
  char * s3 = itex2MML_copy3("</mrow></mfrac>", (yyvsp[0]), "</mrow>");
  (yyval) = itex2MML_copy3(s1, s2, s3);
  itex2MML_free_string(s1);
  itex2MML_free_string(s2);
  itex2MML_free_string(s3);
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-3]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4318 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 220:
#line 1210 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mfrac linethickness=\"0\"><mrow>", (yyvsp[-3]), "</mrow><mrow>");
  (yyval) = itex2MML_copy3(s1, (yyvsp[-1]), "</mrow></mfrac>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-3]));
  itex2MML_free_string((yyvsp[-1]));
}
#line 4330 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 1217 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mrow>", (yyvsp[-4]), "<mfrac linethickness=\"0\"><mrow>");
  char * s2 = itex2MML_copy3((yyvsp[-3]), "</mrow><mrow>", (yyvsp[-1]));
  char * s3 = itex2MML_copy3("</mrow></mfrac>", (yyvsp[0]), "</mrow>");
  (yyval) = itex2MML_copy3(s1, s2, s3);
  itex2MML_free_string(s1);
  itex2MML_free_string(s2);
  itex2MML_free_string(s3);
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-3]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4348 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 222:
#line 1231 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mrow><mo>(</mo><mfrac linethickness=\"0\">", (yyvsp[-1]), (yyvsp[0]));
  (yyval) = itex2MML_copy2(s1, "</mfrac><mo>)</mo></mrow>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4360 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 223:
#line 1238 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mrow><mo>(</mo><mstyle displaystyle=\"false\"><mfrac linethickness=\"0\">", (yyvsp[-1]), (yyvsp[0]));
  (yyval) = itex2MML_copy2(s1, "</mfrac></mstyle><mo>)</mo></mrow>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4372 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 224:
#line 1246 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<munder>", (yyvsp[0]), "<mo>&UnderBrace;</mo></munder>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4381 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 225:
#line 1251 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<munder>", (yyvsp[0]), "<mo>&#x00332;</mo></munder>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4390 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 1256 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&OverBrace;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4399 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 227:
#line 1261 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo stretchy=\"false\">&#x000AF;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4408 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 228:
#line 1265 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&#x000AF;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4417 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 229:
#line 1270 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo stretchy=\"false\">&RightVector;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4426 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 230:
#line 1274 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&RightVector;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4435 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 231:
#line 1279 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&dot;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4444 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 1284 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&Dot;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4453 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 233:
#line 1289 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&tdot;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4462 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 234:
#line 1294 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&DotDot;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4471 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 1299 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo stretchy=\"false\">&acute;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4480 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 236:
#line 1303 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&acute;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4489 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 237:
#line 1308 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo stretchy=\"false\">&grave;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4498 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 238:
#line 1312 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&grave;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4507 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 239:
#line 1317 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo stretchy=\"false\">&breve;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4516 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 240:
#line 1321 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&breve;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4525 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1326 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo stretchy=\"false\">&tilde;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4534 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 242:
#line 1330 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&tilde;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4543 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 243:
#line 1335 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo stretchy=\"false\">&#x2c7;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4552 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 244:
#line 1339 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&#x2c7;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4561 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 245:
#line 1344 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo stretchy=\"false\">&#x5E;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4570 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 246:
#line 1348 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mover>", (yyvsp[0]), "<mo>&#x5E;</mo></mover>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4579 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 1353 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<msqrt>", (yyvsp[0]), "</msqrt>");
  itex2MML_free_string((yyvsp[0]));
}
#line 4588 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 248:
#line 1358 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mroot>", (yyvsp[0]), (yyvsp[-2]));
  (yyval) = itex2MML_copy2(s1, "</mroot>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4600 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 249:
#line 1365 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mroot>", (yyvsp[0]), (yyvsp[-1]));
  (yyval) = itex2MML_copy2(s1, "</mroot>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4612 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 1373 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<munder><mo>", (yyvsp[-4]), "</mo><mrow>");
  (yyval) = itex2MML_copy3(s1, (yyvsp[-2]), "</mrow></munder>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-2]));
}
#line 4624 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 251:
#line 1380 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<munder>", (yyvsp[0]), (yyvsp[-1]));
  (yyval) = itex2MML_copy2(s1, "</munder>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4636 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 1388 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mover><mo>", (yyvsp[-1]), "</mo>");
  (yyval) =  itex2MML_copy3(s1, (yyvsp[0]), "</mover>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4648 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 1395 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mover>", (yyvsp[0]), (yyvsp[-1]));
  (yyval) = itex2MML_copy2(s1, "</mover>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4660 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 254:
#line 1403 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<munderover><mo>", (yyvsp[-4]), "</mo><mrow>");
  char * s2 = itex2MML_copy3(s1, (yyvsp[-2]), "</mrow>");
  (yyval) = itex2MML_copy3(s2, (yyvsp[0]), "</munderover>");
  itex2MML_free_string(s1);
  itex2MML_free_string(s2);
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4675 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 255:
#line 1413 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<munderover>", (yyvsp[0]), (yyvsp[-2]));
  (yyval) = itex2MML_copy3(s1, (yyvsp[-1]), "</munderover>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4688 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 256:
#line 1422 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("<mrow></mrow>");
}
#line 4696 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 257:
#line 1426 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow><mtable rowspacing=\"0.5ex\">", (yyvsp[-2]), "</mtable></mrow>");
  itex2MML_free_string((yyvsp[-2]));
}
#line 4705 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 258:
#line 1430 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow><mtable rowspacing=\"1.0ex\">", (yyvsp[-2]), "</mtable></mrow>");
  itex2MML_free_string((yyvsp[-2]));
}
#line 4714 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 259:
#line 1434 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow><mo>(</mo><mrow><mtable rowspacing=\"0.5ex\">", (yyvsp[-2]), "</mtable></mrow><mo>)</mo></mrow>");
  itex2MML_free_string((yyvsp[-2]));
}
#line 4723 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 260:
#line 1438 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow><mo>[</mo><mrow><mtable rowspacing=\"0.5ex\">", (yyvsp[-2]), "</mtable></mrow><mo>]</mo></mrow>");
  itex2MML_free_string((yyvsp[-2]));
}
#line 4732 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 261:
#line 1442 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow><mo>&VerticalBar;</mo><mrow><mtable rowspacing=\"0.5ex\">", (yyvsp[-2]), "</mtable></mrow><mo>&VerticalBar;</mo></mrow>");
  itex2MML_free_string((yyvsp[-2]));
}
#line 4741 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 262:
#line 1446 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow><mo>{</mo><mrow><mtable rowspacing=\"0.5ex\">", (yyvsp[-2]), "</mtable></mrow><mo>}</mo></mrow>");
  itex2MML_free_string((yyvsp[-2]));
}
#line 4750 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 263:
#line 1450 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow><mo>&DoubleVerticalBar;</mo><mrow><mtable rowspacing=\"0.5ex\">", (yyvsp[-2]), "</mtable></mrow><mo>&DoubleVerticalBar;</mo></mrow>");
  itex2MML_free_string((yyvsp[-2]));
}
#line 4759 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 264:
#line 1454 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mstyle scriptlevel=\"2\"><mrow><mtable rowspacing=\"0.5ex\">", (yyvsp[-2]), "</mtable></mrow></mstyle>");
  itex2MML_free_string((yyvsp[-2]));
}
#line 4768 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 265:
#line 1458 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow><mo>{</mo><mrow><mtable columnalign=\"left left\">", (yyvsp[-2]), "</mtable></mrow></mrow>");
  itex2MML_free_string((yyvsp[-2]));
}
#line 4777 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 266:
#line 1462 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow><mtable columnalign=\"right left right left right left right left right left\" columnspacing=\"0em\">", (yyvsp[-2]), "</mtable></mrow>");
  itex2MML_free_string((yyvsp[-2]));
}
#line 4786 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 267:
#line 1466 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mtable rowspacing=\"0.5ex\" align=\"", (yyvsp[-6]), "\" columnalign=\"");
  char * s2 = itex2MML_copy3(s1, (yyvsp[-4]), "\">");
  (yyval) = itex2MML_copy3(s2, (yyvsp[-2]), "</mtable>");
  itex2MML_free_string(s1);
  itex2MML_free_string(s2);
  itex2MML_free_string((yyvsp[-6]));
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-2]));
}
#line 4801 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 268:
#line 1476 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mtable rowspacing=\"0.5ex\" columnalign=\"", (yyvsp[-4]), "\">");
  (yyval) = itex2MML_copy3(s1, (yyvsp[-2]), "</mtable>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-4]));
  itex2MML_free_string((yyvsp[-2]));
}
#line 4813 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 269:
#line 1483 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<semantics><annotation-xml encoding=\"SVG1.1\">", (yyvsp[-1]), "</annotation-xml></semantics>");
  itex2MML_free_string((yyvsp[-1]));
}
#line 4822 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 270:
#line 1487 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string(" ");
}
#line 4830 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 271:
#line 1491 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3((yyvsp[-1]), " ", (yyvsp[0]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4840 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 272:
#line 1496 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4849 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 273:
#line 1501 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow><mtable columnalign=\"center\" rowspacing=\"0.5ex\">", (yyvsp[-1]), "</mtable></mrow>");
  itex2MML_free_string((yyvsp[-1]));
}
#line 4858 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 274:
#line 1506 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mrow><mtable>", (yyvsp[-1]), "</mtable></mrow>");
  itex2MML_free_string((yyvsp[-1]));
}
#line 4867 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 275:
#line 1510 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mrow><mtable ", (yyvsp[-3]), ">");
  (yyval) = itex2MML_copy3(s1, (yyvsp[-1]), "</mtable></mrow>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-3]));
  itex2MML_free_string((yyvsp[-1]));
}
#line 4879 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 276:
#line 1518 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4888 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 277:
#line 1522 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3((yyvsp[-1]), " ", (yyvsp[0]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4898 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 278:
#line 1528 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4907 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 279:
#line 1532 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4916 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 280:
#line 1536 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4925 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 281:
#line 1540 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4934 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 282:
#line 1544 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4943 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 283:
#line 1548 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4952 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 284:
#line 1552 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4961 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 285:
#line 1556 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4970 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 286:
#line 1560 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4979 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 287:
#line 1564 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4988 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 288:
#line 1569 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("columnalign=", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 4997 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 289:
#line 1574 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("columnalign=", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5006 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 290:
#line 1579 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("rowalign=", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5015 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 291:
#line 1584 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("align=", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5024 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 292:
#line 1589 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("equalrows=", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5033 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 293:
#line 1594 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("equalcolumns=", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5042 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 294:
#line 1599 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("rowlines=", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5051 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 295:
#line 1604 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("columnlines=", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5060 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 296:
#line 1609 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("frame=", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5069 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 297:
#line 1614 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("rowspacing=", (yyvsp[0]), " columnspacing=");
  (yyval) = itex2MML_copy2(s1, (yyvsp[0]));
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[0]));
}
#line 5080 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 298:
#line 1621 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5089 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 299:
#line 1625 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3((yyvsp[-2]), " ", (yyvsp[0]));
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5099 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 300:
#line 1631 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mtr>", (yyvsp[0]), "</mtr>");
  itex2MML_free_string((yyvsp[0]));
}
#line 5108 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 301:
#line 1635 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5117 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 302:
#line 1640 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5126 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 303:
#line 1644 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3((yyvsp[-2]), " ", (yyvsp[0]));
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5136 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 304:
#line 1650 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mtr ", (yyvsp[-2]), ">");
  (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</mtr>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5148 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 305:
#line 1658 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5157 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 306:
#line 1662 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3((yyvsp[-1]), " ", (yyvsp[0]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5167 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 307:
#line 1668 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5176 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 308:
#line 1672 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5185 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 309:
#line 1677 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string("<mtd></mtd>");
}
#line 5193 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 310:
#line 1680 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3("<mtd>", (yyvsp[0]), "</mtd>");
  itex2MML_free_string((yyvsp[0]));
}
#line 5202 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 311:
#line 1684 "itex2MML.y" /* yacc.c:1646  */
    {
  char * s1 = itex2MML_copy3("<mtd ", (yyvsp[-2]), ">");
  (yyval) = itex2MML_copy3(s1, (yyvsp[0]), "</mtd>");
  itex2MML_free_string(s1);
  itex2MML_free_string((yyvsp[-2]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5214 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 312:
#line 1692 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5223 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 313:
#line 1696 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy3((yyvsp[-1]), " ", (yyvsp[0]));
  itex2MML_free_string((yyvsp[-1]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5233 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 314:
#line 1702 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5242 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 315:
#line 1706 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5251 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 316:
#line 1710 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5260 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 317:
#line 1714 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy_string((yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5269 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 318:
#line 1719 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("rowspan=", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5278 "itex2MML.tab.c" /* yacc.c:1646  */
    break;

  case 319:
#line 1724 "itex2MML.y" /* yacc.c:1646  */
    {
  (yyval) = itex2MML_copy2("columnspan=", (yyvsp[0]));
  itex2MML_free_string((yyvsp[0]));
}
#line 5287 "itex2MML.tab.c" /* yacc.c:1646  */
    break;


#line 5291 "itex2MML.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1729 "itex2MML.y" /* yacc.c:1906  */


char * itex2MML_parse (const char * buffer, unsigned long length)
{
  char * mathml = 0;

  int result;

  itex2MML_last_error = 0;
  itex2MML_setup (buffer, length);
  itex2MML_restart ();

  result = itex2MML_yyparse (&mathml);

  if ((result && mathml) || itex2MML_last_error) /* shouldn't happen? */
    {
      itex2MML_free_string (mathml);
      mathml = 0;
    }
  return mathml;
}

int itex2MML_filter (const char * buffer, unsigned long length)
{
  itex2MML_setup (buffer, length);
  itex2MML_restart ();

  return itex2MML_yyparse (0);
}
/*
#define ITEX_DELIMITER_DOLLAR 0
#define ITEX_DELIMITER_DOUBLE 1
#define ITEX_DELIMITER_SQUARE 2

static char * itex2MML_last_error = 0;

static void itex2MML_keep_error (const char * msg)
{
  if (itex2MML_last_error)
    {
      itex2MML_free_string (itex2MML_last_error);
      itex2MML_last_error = 0;
    }
  itex2MML_last_error = itex2MML_copy_escaped (msg);
}

int itex2MML_html_filter (const char * buffer, unsigned long length)
{
  itex2MML_do_html_filter (buffer, length, 0);
}

int itex2MML_strict_html_filter (const char * buffer, unsigned long length)
{
  itex2MML_do_html_filter (buffer, length, 1);
}

int itex2MML_do_html_filter (const char * buffer, unsigned long length, const int forbid_markup)
{
  int result = 0;

  int type = 0;
  int skip = 0;
  int match = 0;

  const char * ptr1 = buffer;
  const char * ptr2 = 0;

  const char * end = buffer + length;

  char * mathml = 0;

  void (*save_error_fn) (const char * msg) = itex2MML_error;

  itex2MML_error = itex2MML_keep_error;

 _until_math:
  ptr2 = ptr1;

  while (ptr2 < end)
    {
      if (*ptr2 == '$') break;
      if ((*ptr2 == '\\') && (ptr2 + 1 < end))
	{
	  if (*(ptr2+1) == '[') break;
	}
      ++ptr2;
    }
  if (itex2MML_write && ptr2 > ptr1)
    (*itex2MML_write) (ptr1, ptr2 - ptr1);

  if (ptr2 == end) goto _finish;

 _until_html:
  ptr1 = ptr2;

  if (ptr2 + 1 < end)
    {
      if ((*ptr2 == '\\') && (*(ptr2+1) == '['))
	{
	  type = ITEX_DELIMITER_SQUARE;
	  ptr2 += 2;
	}
      else if ((*ptr2 == '$') && (*(ptr2+1) == '$'))
	{
	  type = ITEX_DELIMITER_DOUBLE;
	  ptr2 += 2;
	}
      else
	{
	  type = ITEX_DELIMITER_DOLLAR;
	  ptr2 += 2;
	}
    }
  else goto _finish;

  skip = 0;
  match = 0;

  while (ptr2 < end)
    {
      switch (*ptr2)
	{
	case '<':
	case '>':
	  if (forbid_markup == 1) skip = 1;
	  break;

	case '\\':
	  if (ptr2 + 1 < end)
	    {
	      if (*(ptr2 + 1) == '[')
		{
		  skip = 1;
		}
	      else if (*(ptr2 + 1) == ']')
		{
		  if (type == ITEX_DELIMITER_SQUARE)
		    {
		      ptr2 += 2;
		      match = 1;
		    }
		  else
		    {
		      skip = 1;
		    }
		}
	    }
	  break;

	case '$':
	  if (type == ITEX_DELIMITER_SQUARE)
	    {
	      skip = 1;
	    }
	  else if (ptr2 + 1 < end)
	    {
	      if (*(ptr2 + 1) == '$')
		{
		  if (type == ITEX_DELIMITER_DOLLAR)
		    {
		      ptr2++;
		      match = 1;
		    }
		  else
		    {
		      ptr2 += 2;
		      match = 1;
		    }
		}
	      else
		{
		  if (type == ITEX_DELIMITER_DOLLAR)
		    {
		      ptr2++;
		      match = 1;
		    }
		  else
		    {
		      skip = 1;
		    }
		}
	    }
	  else
	    {
	      if (type == ITEX_DELIMITER_DOLLAR)
		{
		  ptr2++;
		  match = 1;
		}
	      else
		{
		  skip = 1;
		}
	    }
	  break;

	default:
	  break;
	}
      if (skip || match) break;

      ++ptr2;
    }
  if (skip)
    {
      if (type == ITEX_DELIMITER_DOLLAR)
	{
	  if (itex2MML_write)
	    (*itex2MML_write) (ptr1, 1);
	  ptr1++;
	}
      else
	{
	  if (itex2MML_write)
	    (*itex2MML_write) (ptr1, 2);
	  ptr1 += 2;
	}
      goto _until_math;
    }
  if (match)
    {
      mathml = itex2MML_parse (ptr1, ptr2 - ptr1);

      if (mathml)
	{
	  if (itex2MML_write_mathml)
	    (*itex2MML_write_mathml) (mathml);
	  itex2MML_free_string (mathml);
	  mathml = 0;
	}
      else
	{
	  ++result;
	  if (itex2MML_write)
	    {
	      if (type == ITEX_DELIMITER_DOLLAR)
		(*itex2MML_write) ("<math xmlns='http://www.w3.org/1998/Math/MathML' display='inline'><merror><mtext>", 0);
	      else
		(*itex2MML_write) ("<math xmlns='http://www.w3.org/1998/Math/MathML' display='block'><merror><mtext>", 0);

	      (*itex2MML_write) (itex2MML_last_error, 0);
	      (*itex2MML_write) ("</mtext></merror></math>", 0);
	    }
	}
      ptr1 = ptr2;

      goto _until_math;
    }
  if (itex2MML_write)
    (*itex2MML_write) (ptr1, ptr2 - ptr1);

 _finish:
  if (itex2MML_last_error)
    {
      itex2MML_free_string (itex2MML_last_error);
      itex2MML_last_error = 0;
    }
  itex2MML_error = save_error_fn;

  return result;
}

*/
