includedir = $(prefix)/include/unisim/service/debug
sharedir = $(prefix)/share/unisim_lib/unisim/service/debug
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

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

