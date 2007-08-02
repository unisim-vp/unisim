INCLUDES=-I$(top_srcdir)/../../../../../..

libdir= $(prefix)/lib/unisim/component/cxx/memory/flash/am29lv
lib_LIBRARIES = libam29lv.a
libram_a_SOURCES = am29lv.cc config.cc

includedir = $(prefix)/include/unisim/component/cxx/memory/flash/am29lv
include_HEADERS = am29lv.hh am29lv.tcc config.hh types.hh

install-exec-hook: $(libdir)/libam29lv.a
	if test ! -f $(prefix)/lib/libunisim_component_cxx_memory_flash_am29lv.a; then (cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_memory_flash_am29lv.a); fi
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_memory_flash_am29lv.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
