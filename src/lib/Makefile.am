MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-Wall -g -O2 -Wpointer-arith -Wmissing-prototypes -Wwrite-strings -Wno-long-long \
-I. \
-I$(top_srcdir)/ \
@EINA_CFLAGS@ \
@XML2_CFLAGS@ \
@ICU_CFLAGS@ \
@PCRE_CFLAGS@ \
@CFLAGS@

LEX = @LEX@ -i

CLEANFILES = \
	expr.c \
	libmwparser_la-expr.h \
	libmwparser_la-expr.c \
        itex2MML.c \
        libmwparser_la-itex2MML.h \
        libmwparser_la-itex2MML.c


lib_LTLIBRARIES = libmwparser.la

itex2MML_yy.tab.c: itex2MML.y
	$(YACC) -d -p itex2MML_yy -b itex2MML_yy $+

itex2MML_yy.c: itex2MML.l
	$(LEX) -Pitex2MML_yy -o $@ $+

libmwparser_la_YFLAGS = -d -k
libmwparser_la_SOURCES = \
expr.y \
expr.l \
itex2MML.h \
itex2MML_yy.tab.c \
itex2MML_yy.c \
wiki_define.h \
wiki_lang.h \
wiki_lang.c \
lexer.h \
lexer.c \
entities.h \
entities.c \
parser.h \
parser.c \
parser_extension.h \
parser_extension_private.h \
parser_extension_private.c \
parser_tag_extensions_private.h \
parser_tag_extensions.h \
parser_tag_extensions.c \
parser_extension_functions.c \
parser_extension_magic_words.c \
parser_extension_time.c \
parser_extension_stats.c \
parser_extension_builtin.c \
parser_output_xhtml.c \
parser_title.c \
util.h \
util.c \
icu_eina_hash.h \
icu_eina_hash.c \
pcre_replace.h \
pcre_replace.c \
md5.h \
md5.c

libmwparser_la_CFLAGS = @CFLAGS@
libmwparser_la_LIBADD = @EINA_LIBS@ @ICU_LIBS@ @PCRE_LIBS@ @XML2_LIBS@ @LIBS@

