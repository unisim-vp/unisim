includedir = $(prefix)/include/unisim/component/tlm2/interconnect
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/interconnect

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_generic_bus
SUBDIRS += generic_bus
INCLUDE_SUBDIRS += generic_bus
endif

if COND_generic_router
SUBDIRS += generic_router
INCLUDE_SUBDIRS += generic_router
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
