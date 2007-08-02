INCLUDES=-I$(top_srcdir)/../../../../..

libdir= $(prefix)/lib/unisim/component/cxx/cache/mesi
lib_LIBRARIES = libcache.a
libcache_a_SOURCES = cache.cc

includedir = $(prefix)/include/unisim/component/cxx/cache/mesi
include_HEADERS = cache.hh cache.tcc

install-exec-hook: $(libdir)/libcache.a
	rm -f $(prefix)/lib/libunisim_component_cxx_cache_mesi.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_cache_mesi.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_cache_mesi.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
