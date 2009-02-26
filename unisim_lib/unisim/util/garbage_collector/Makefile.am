INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/util/garbage_collector
sharedir = $(prefix)/share/unisim_lib/unisim/util/garbage_collector
include_HEADERS = garbage_collector.hh

lib_LTLIBRARIES = libunisim_util_garbage_collector.la
libunisim_util_garbage_collector_la_SOURCES = garbage_collector.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
	