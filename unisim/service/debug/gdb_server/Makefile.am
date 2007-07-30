INCLUDES = -I$(top_srcdir)/../../../..
libdir = $(prefix)/lib/unisim/service/debug/gdb_server
includedir = $(prefix)/include/unisim/service/debug/gdb_server
include_HEADERS = gdb_server.hh gdb_server.tcc

lib_LIBRARIES = libgdb_server.a
libgdb_server_a_SOURCES = gdb_server_32.cc gdb_server_64.cc
install-exec-hook: $(libdir)/libgdb_server.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_service_debug_gdb_server.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_debug_gdb_server.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
