sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/cache/insn
INCLUDES=-I$(top_srcdir)/../../../../..

lib_LIBRARIES = libunisim_component_cxx_cache_insn.a
libunisim_component_cxx_cache_insn_a_SOURCES = cache.cc

includedir = $(prefix)/include/unisim/component/cxx/cache/insn
include_HEADERS = cache.hh cache.tcc

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
