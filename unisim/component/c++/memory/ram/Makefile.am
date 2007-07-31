INCLUDES=-I$(top_srcdir)/../../../../..

libdir= $(prefix)/lib/unisim/component/c++/memory/ram
lib_LIBRARIES = libram.a
libram_a_SOURCES = memory_32.cc memory_64.cc

includedir = $(prefix)/include/unisim/component/c++/memory/ram
include_HEADERS = memory.hh memory.tcc

install-exec-hook: $(libdir)/libram.a
	if test ! -f $(prefix)/lib/libunisim_component_c++_memory_ram.a; then (cd $(prefix)/lib && $(LN_S) $< libunisim_component_c++_memory_ram.a); fi
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_c++_memory_ram.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
