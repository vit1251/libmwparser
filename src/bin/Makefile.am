MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = -Wall -O2 -g -Wpointer-arith -Wmissing-prototypes -Wwrite-strings -Wno-long-long

#MWPARSER_LIB=$(top_builddir)/src/lib/libmwparser.la
MWPARSER_LIB=$(top_builddir)/src/lib/.libs/libmwparser.a \
@EINA_LIBS@ \
@ICU_LIBS@ \
@XML2_LIBS@ \
@PCRE_LIBS@ 

INCLUDES = \
-I. \
-I$(top_srcdir)/ \
-I$(top_srcdir)/src/lib \
@EINA_CFLAGS@ \
@XML2_CFLAGS@ \
@ICU_CFLAGS@ \
@PCRE_CFLAGS@ \
@SQLITE3_CFLAGS@ \
@BZ2_CFLAGS@ \
@MICROHTTPD_CFLAGS@ \
@CFLAGS@

bin_PROGRAMS = wikidump_sqlite \
               wikidump_server \
               wikidump_cgi \
               wikidump_fs \
               parse_sql

wikidump_sqlite_SOURCES = \
wiki_static_pages.h \
wikidump.h \
wikidump.c \
parse_sql.h \
parse_sql.c \
wikidump_server_common.h \
wikidump_server_common.c \
wikipedia_ui.c \
wikipedia_ui.h \
wikidump_sqlite.h \
wikidump_sqlite.c 

wikidump_sqlite_CFLAGS = -DWIKIDUMP_SQLITE_MAIN @EINA_CFLAGS@ @XML2_CFLAGS@ @BZ2_CFLAGS@ @SQLITE3_CFLAGS@ @ICU_CFLAGS@ @CFLAGS@
wikidump_sqlite_LDADD = \
${MWPARSER_LIB} \
@BZ2_LIBS@ \
@SQLITE3_LIBS@ @LIBS@

wikidump_sqlite_DEPENDENCIES = \
$(top_builddir)/src/lib/libmwparser.la

wikidump_fs_SOURCES = \
wikidump.h \
wikidump.c \
parse_sql.h \
parse_sql.c \
wikidump_fs.h \
wikidump_fs.c 

wikidump_fs_CFLAGS = -DWIKIDUMP_SQLITE_MAIN @EINA_CFLAGS@ @XML2_CFLAGS@ @BZ2_CFLAGS@ @ICU_CFLAGS@ @CFLAGS@
wikidump_fs_LDADD = \
${MWPARSER_LIB} \
@BZ2_LIBS@  @LIBS@ -lz

wikidump_fs_DEPENDENCIES = \
$(top_builddir)/src/lib/libmwparser.la

wikidump_server_SOURCES = \
wiki_static_pages.h \
wikidump.h \
wikidump.c \
wikidump_sqlite.h \
wikidump_sqlite.c \
wikidump_server_common.h \
wikidump_server_common.c \
wikipedia_ui.c \
wikipedia_ui.h \
wikidump_server.c

wikidump_server_CFLAGS = -DWIKIDUMP_SQLITE @CFLAGS@
wikidump_server_LDADD = \
${MWPARSER_LIB} \
@MICROHTTPD_LIBS@ \
@SQLITE3_LIBS@ @LIBS@

wikidump_cgi_SOURCES = \
wiki_static_pages.h \
wikidump.h \
wikidump.c \
wikidump_sqlite.h \
wikidump_sqlite.c \
wikidump_server_common.h \
wikidump_server_common.c \
wikipedia_ui.c \
wikipedia_ui.h \
wikidump_cgi.c

wikidump_cgi_CFLAGS = -DWIKIDUMP_SQLITE @CFLAGS@
wikidump_cgi_LDADD = \
${MWPARSER_LIB} \
@SQLITE3_LIBS@ @LIBS@

parse_sql_SOURCES = \
parse_sql.h \
parse_sql.c 

parse_sql_CPPFLAGS = -DPARSE_SQL_MAIN @EINA_CFLAGS@
parse_sql_LDADD = \
@EINA_LIBS@ -lz
