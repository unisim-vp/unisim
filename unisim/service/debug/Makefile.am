includedir = $(prefix)/include/unisim/service/debug
libdir= $(prefix)/lib/unisim/service/debug

SUBDIRS=symbol_table
INCLUDE_SUBDIRS=symbol_table

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)

