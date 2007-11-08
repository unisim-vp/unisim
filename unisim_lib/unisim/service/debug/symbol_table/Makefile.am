INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/debug/symbol_table
sharedir = $(prefix)/share/unisim_lib/unisim/service/debug/symbol_table
include_HEADERS = symbol_table.hh symbol_table.tcc

lib_LIBRARIES = libunisim_service_debug_symbol_table.a
libunisim_service_debug_symbol_table_a_SOURCES = symbol_table_32.cc symbol_table_64.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
	