INCLUDES=-I$(top_srcdir)/../../../../..

libdir= $(prefix)/lib/unisim/component/tlm/memory/ram
lib_LIBRARIES = libram.a
libram_a_SOURCES = memory_32.cc memory_64.cc \
	memory_32_debug.cc memory_64_debug.cc

includedir = $(prefix)/include/unisim/component/tlm/memory/ram
include_HEADERS = memory.hh memory.tcc

install-exec-hook: $(libdir)/libram.a
	rm -f $(prefix)/lib/libunisim_component_tlm_memory_ram.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_memory_ram.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_memory_ram.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
	