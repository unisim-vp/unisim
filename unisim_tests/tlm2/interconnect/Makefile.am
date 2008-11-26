sharedir = $(prefix)/share/unisim_tests/tlm2/interconnect

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_generic_bus
SUBDIRS += generic_bus
INCLUDE_SUBDIRS += generic_bus
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)
