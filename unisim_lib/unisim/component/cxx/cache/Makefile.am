INCLUDES=-I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/component/cxx/cache
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/cache
include_HEADERS = cache.hh cache.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
