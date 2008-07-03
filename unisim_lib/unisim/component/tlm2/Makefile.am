includedir = $(prefix)/include/unisim/component/tlm2
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_bus
SUBDIRS += bus
INCLUDE_SUBDIRS += bus
endif

if COND_memory
SUBDIRS += memory
INCLUDE_SUBDIRS += memory
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
