INCLUDES=-I$(top_srcdir)/../../../../..

lib_LIBRARIES = libunisim_component_cxx_cache_mesi.a
libunisim_component_cxx_cache_mesi_a_SOURCES = cache.cc

includedir = $(prefix)/include/unisim/component/cxx/cache/mesi
include_HEADERS = cache.hh cache.tcc

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
