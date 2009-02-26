INCLUDES=-I$(top_srcdir)/../../../../..

lib_LTLIBRARIES = libunisim_component_tlm2_memory_ram.la
libunisim_component_tlm2_memory_ram_la_SOURCES = memory.cc memory_debug.cc

includedir = $(prefix)/include/unisim/component/tlm2/memory/ram
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/memory/ram
include_HEADERS = memory.hh memory.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
	