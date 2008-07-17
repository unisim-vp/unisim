includedir = $(prefix)/include/unisim/component/tlm2/bus
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/bus

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_generic_bus
SUBDIRS += generic_bus
INCLUDE_SUBDIRS += generic_bus
endif

if COND_simple_router
SUBDIRS += simple_router
INCLUDE_SUBDIRS += simple_router
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
