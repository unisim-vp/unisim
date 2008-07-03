sharedir = $(prefix)/share/unisim_tests/tlm2

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_bus
SUBDIRS += bus
INCLUDE_SUBDIRS += bus
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)
