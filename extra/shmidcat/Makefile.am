ACLOCAL_AMFLAGS=-I m4
INCLUDES = -I$(top_srcdir)
includedir = $(prefix)/include
sharedir = $(prefix)/share/libshmidcat
include_HEADERS = shmidcat.h

lib_LTLIBRARIES = libshmidcat.la
libshmidcat_la_SOURCES = shmidcat.c

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
	