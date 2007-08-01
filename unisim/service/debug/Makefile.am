includedir = $(prefix)/include/unisim/service/debug
libdir= $(prefix)/lib/unisim/service/debug

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_inline_debugger
SUBDIRS += inline_debugger
INCLUDE_SUBDIRS += inline_debugger
endif

if COND_gdb_server
SUBDIRS += gdb_server
INCLUDE_SUBDIRS += gdb_server
endif

if COND_symbol_table
SUBDIRS += symbol_table
INCLUDE_SUBDIRS += symbol_table
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)

