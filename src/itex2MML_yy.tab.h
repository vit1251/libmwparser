
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TEXATOP = 258,
     TEXOVER = 259,
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
/* Tokens.  */
#define TEXATOP 258
#define TEXOVER 259
#define CHAR 260
#define STARTMATH 261
#define STARTDMATH 262
#define ENDMATH 263
#define MI 264
#define MIB 265
#define MN 266
#define MO 267
#define SUP 268
#define SUB 269
#define MROWOPEN 270
#define MROWCLOSE 271
#define LEFT 272
#define RIGHT 273
#define BIG 274
#define BBIG 275
#define BIGG 276
#define BBIGG 277
#define BIGL 278
#define BBIGL 279
#define BIGGL 280
#define BBIGGL 281
#define FRAC 282
#define TFRAC 283
#define OPERATORNAME 284
#define MATHOP 285
#define MATHBIN 286
#define MATHREL 287
#define MOP 288
#define MOL 289
#define MOLL 290
#define MOF 291
#define MOR 292
#define PERIODDELIM 293
#define OTHERDELIM 294
#define LEFTDELIM 295
#define RIGHTDELIM 296
#define MOS 297
#define MOB 298
#define SQRT 299
#define ROOT 300
#define BINOM 301
#define TBINOM 302
#define UNDER 303
#define OVER 304
#define OVERBRACE 305
#define UNDERLINE 306
#define UNDERBRACE 307
#define UNDEROVER 308
#define TENSOR 309
#define MULTI 310
#define ARRAYALIGN 311
#define COLUMNALIGN 312
#define ARRAY 313
#define COLSEP 314
#define ROWSEP 315
#define ARRAYOPTS 316
#define COLLAYOUT 317
#define COLALIGN 318
#define ROWALIGN 319
#define ALIGN 320
#define EQROWS 321
#define EQCOLS 322
#define ROWLINES 323
#define COLLINES 324
#define FRAME 325
#define PADDING 326
#define ATTRLIST 327
#define ITALICS 328
#define BOLD 329
#define BOXED 330
#define SLASHED 331
#define RM 332
#define BB 333
#define ST 334
#define END 335
#define BBLOWERCHAR 336
#define BBUPPERCHAR 337
#define BBDIGIT 338
#define CALCHAR 339
#define FRAKCHAR 340
#define CAL 341
#define FRAK 342
#define CLAP 343
#define LLAP 344
#define RLAP 345
#define ROWOPTS 346
#define TEXTSIZE 347
#define SCSIZE 348
#define SCSCSIZE 349
#define DISPLAY 350
#define TEXTSTY 351
#define TEXTBOX 352
#define TEXTSTRING 353
#define XMLSTRING 354
#define CELLOPTS 355
#define ROWSPAN 356
#define COLSPAN 357
#define THINSPACE 358
#define MEDSPACE 359
#define THICKSPACE 360
#define QUAD 361
#define QQUAD 362
#define NEGSPACE 363
#define PHANTOM 364
#define HREF 365
#define UNKNOWNCHAR 366
#define EMPTYMROW 367
#define STATLINE 368
#define TOOLTIP 369
#define TOGGLE 370
#define FGHIGHLIGHT 371
#define BGHIGHLIGHT 372
#define SPACE 373
#define INTONE 374
#define INTTWO 375
#define INTTHREE 376
#define BAR 377
#define WIDEBAR 378
#define VEC 379
#define WIDEVEC 380
#define HAT 381
#define WIDEHAT 382
#define CHECK 383
#define WIDECHECK 384
#define ACUTE 385
#define WIDEACUTE 386
#define GRAVE 387
#define WIDEGRAVE 388
#define BREVE 389
#define WIDEBREVE 390
#define TILDE 391
#define WIDETILDE 392
#define DOT 393
#define DDOT 394
#define DDDOT 395
#define DDDDOT 396
#define UNARYMINUS 397
#define UNARYPLUS 398
#define BEGINENV 399
#define ENDENV 400
#define MATRIX 401
#define PMATRIX 402
#define BMATRIX 403
#define BBMATRIX 404
#define VMATRIX 405
#define VVMATRIX 406
#define SVG 407
#define ENDSVG 408
#define SMALLMATRIX 409
#define CASES 410
#define ALIGNED 411
#define GATHERED 412
#define SUBSTACK 413
#define PMOD 414
#define RMCHAR 415
#define COLOR 416
#define BGCOLOR 417
#define XARROW 418
#define OPTARGOPEN 419
#define OPTARGCLOSE 420




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE itex2MML_yylval;


