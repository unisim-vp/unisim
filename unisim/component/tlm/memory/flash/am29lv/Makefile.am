INCLUDES=-I$(top_srcdir)/../../../../../..

libdir= $(prefix)/lib/unisim/component/tlm/memory/flash/am29lv
lib_LIBRARIES = libam29lv.a
libam29lv_a_SOURCES = am29lv.cc

includedir = $(prefix)/include/unisim/component/tlm/memory/flash/am29lv
include_HEADERS = am29lv.hh am29lv.tcc

install-exec-hook: $(libdir)/libam29lv.a
	rm -f $(prefix)/lib/libunisim_component_tlm_memory_flash_am29lv.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_memory_flash_am29lv.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_memory_flash_am29lv.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
