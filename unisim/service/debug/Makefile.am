includedir = $(prefix)/include/unisim/service/debug
libdir= $(prefix)/lib/unisim/service/debug

SUBDIRS=inoine_debugger gdb_server symbol_table
INCLUDE_SUBDIRS=inline_debugger gdb_server symbol_table

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)

