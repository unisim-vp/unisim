INCLUDES=-I$(top_srcdir)/../../..
lib_LTLIBRARIES = libunisim_util_time.la
libunisim_util_time_la_SOURCES = time.cc

includedir = $(prefix)/include/unisim/util/time
sharedir = $(prefix)/share/unisim_lib/unisim/util/time
include_HEADERS = time.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
