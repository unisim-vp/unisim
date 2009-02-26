INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/util/endian
sharedir = $(prefix)/share/unisim_lib/unisim/util/endian
include_HEADERS = endian.hh

lib_LTLIBRARIES = libunisim_util_endian.la
libunisim_util_endian_la_SOURCES = endian.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
	