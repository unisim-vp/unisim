INCLUDES=-I$(top_srcdir)/../../../../..

lib_LTLIBRARIES = libunisim_component_cxx_memory_ram.la
libunisim_component_cxx_memory_ram_la_SOURCES = memory_32.cc memory_64.cc

includedir = $(prefix)/include/unisim/component/cxx/memory/ram
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/memory/ram
include_HEADERS = memory.hh memory.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
