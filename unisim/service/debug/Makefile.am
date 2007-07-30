includedir = $(prefix)/include/unisim/service/debug
libdir= $(prefix)/lib/unisim/service/debug
SUBDIRS=gdb_server inline_debugger symbol_table
INCLUDE_SUBDIRS=gdb_server inline_debugger symbol_table

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)

