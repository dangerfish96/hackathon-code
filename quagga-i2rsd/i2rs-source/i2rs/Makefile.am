## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
AM_CFLAGS = $(WERROR)
DEFS = @DEFS@ $(LOCAL_OPTS) -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

lib_LTLIBRARIES = libi2rs.la
libi2rs_la_LDFLAGS = -version-info 0:0:0
libi2rs_la_LIBADD = ../lib/libzebra.la

sbin_PROGRAMS = i2rsd

libi2rs_la_SOURCES = \
	i2rs_zebra.c i2rs_vty.c

i2rsdheaderdir = $(pkgincludedir)/i2rs

noinst_HEADERS = \
	 i2rs_main.h i2rs_zebra.h i2rs_vty.h
i2rsd_SOURCES = i2rs_main.c

i2rsd_LDADD = libi2rs.la ../lib/libzebra.la @LIBCAP@



