INCLUDES=-I$(top_srcdir)/../../../../..

lib_LIBRARIES = libunisim_component_cxx_memory_ram.a
libunisim_component_cxx_memory_ram_a_SOURCES = memory_32.cc memory_64.cc

includedir = $(prefix)/include/unisim/component/cxx/memory/ram
include_HEADERS = memory.hh memory.tcc

uninstall-hook:
	(rmdir $(includedir) || true)
