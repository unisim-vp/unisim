INCLUDES=-I$(top_srcdir)/../../../../..

libdir= $(prefix)/lib/unisim/component/cxx/cache/insn
lib_LIBRARIES = libcache.a
libcache_a_SOURCES = cache.cc

includedir = $(prefix)/include/unisim/component/cxx/cache/insn
include_HEADERS = cache.hh cache.tcc

install-exec-hook: $(libdir)/libcache.a
	rm -f $(prefix)/lib/libunisim_component_cxx_cache_insn.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_cache_insn.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_cache_insn.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
