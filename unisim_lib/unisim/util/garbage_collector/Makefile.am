INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/util/garbage_collector
sharedir = $(prefix)/share/unisim_lib/unisim/util/garbage_collector
include_HEADERS = garbage_collector.hh

lib_LIBRARIES = libunisim_util_garbage_collector.a
libunisim_util_garbage_collector_a_SOURCES = garbage_collector.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
	