INCLUDES=-I$(top_srcdir)/../../../../..

lib_LIBRARIES = libunisim_component_tlm_memory_ram.a
libunisim_component_tlm_memory_ram_a_SOURCES = memory_32.cc memory_64.cc \
	memory_32_debug.cc memory_64_debug.cc

includedir = $(prefix)/include/unisim/component/tlm/memory/ram
include_HEADERS = memory.hh memory.tcc

uninstall-hook:
	(rmdir $(includedir) || true)
	
ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
	