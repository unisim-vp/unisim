INCLUDES = -I$(top_srcdir)/../../../..
libdir = $(prefix)/lib/unisim/service/debug/symbol_table
includedir = $(prefix)/include/unisim/service/debug/symbol_table
include_HEADERS = symbol_table.hh symbol_table.tcc

lib_LIBRARIES = libsymbol_table.a
libsymbol_table_a_SOURCES = symbol_table_32.cc symbol_table_64.cc
install-exec-hook: $(libdir)/libsymbol_table.a
	rm -f $(prefix)/lib/libunisim_service_debug_symbol_table.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_service_debug_symbol_table.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_debug_symbol_table.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
