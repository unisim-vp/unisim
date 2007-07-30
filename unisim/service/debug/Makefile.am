includedir = $(prefix)/include/unisim/service/debug
libdir= $(prefix)/lib/unisim/service/debug
SUBDIRS=gdb_server inline_debugger
INCLUDE_SUBDIRS=gdb_server inline_debugger

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)

