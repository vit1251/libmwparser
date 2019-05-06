rm -f config.cache

AC_INIT([wikidump_server], [0.1], [saitmoh@gmail.com])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.06 dist-bzip2 silent-rules])
AM_CONFIG_HEADER(config.h)
AC_USE_SYSTEM_EXTENSIONS

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_C_BIGENDIAN

AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_SYS_LARGEFILE

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL
AM_SILENT_RULES

LIBS="-lm"

PKG_CHECK_MODULES([EINA], [
	 eina
])

PKG_CHECK_MODULES([PCRE], [
	libpcre
])

PKG_CHECK_MODULES([SQLITE3], [
	sqlite3,
	zlib
])

PKG_CHECK_MODULES([XML2], [
	libxml-2.0
])

PKG_CHECK_MODULES([CURL], [
        libcurl
])

PKG_CHECK_MODULES([MICROHTTPD], [
        libmicrohttpd
])

#PKG_CHECK_MODULES([], [
#])

# TODO: libicu version must be checked
AC_PATH_PROG(icu_config, icu-config, no)
if test "$icu_config" = "no"; then
    AC_MSG_ERROR([Cannot find icu-config. The ICU library is needed.])
fi
ICU_CFLAGS=`$icu_config --cppflags`
ICU_LIBS=`$icu_config --ldflags-libsonly --ldflags-icuio`
AC_SUBST([ICU_CFLAGS])
AC_SUBST([ICU_LIBS])

AC_CHECK_HEADER(bzlib.h, bz2=yes, bz2=no)
if test "$bz2" = "no"; then
    AC_MSG_ERROR([Cannot find bz2. The libbz2-dev library is needed.])
else
    BZ2_CFLAGS=
fi
AC_CHECK_LIB([bz2], [BZ2_bzread], bz2=yes, bz2=no)
if test "$bz2" = "no"; then
    AC_MSG_ERROR([Cannot find bz2. The libbz2-dev library is needed.])
else
    BZ2_LIBS=-lbz2
fi
AC_SUBST([BZ2_CFLAGS])
AC_SUBST([BZ2_LIBS])


AC_OUTPUT([
Makefile
src/Makefile
src/lib/Makefile
src/bin/Makefile
po/Makefile
])

