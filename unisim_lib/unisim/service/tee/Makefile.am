includedir = $(prefix)/include/unisim/service/tee
sharedir = $(prefix)/share/unisim_lib/unisim/service/tee

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_memory_access_reporting
SUBDIRS += memory_access_reporting
INCLUDE_SUBDIRS += memory_access_reporting
endif

if COND_memory_import_export
SUBDIRS += memory_import_export
INCLUDE_SUBDIRS += memory_import_export
endif

if COND_registers
SUBDIRS += registers
INCLUDE_SUBDIRS += registers
endif

if COND_symbol_table_lookup
SUBDIRS += symbol_table_lookup
INCLUDE_SUBDIRS += symbol_table_lookup
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
