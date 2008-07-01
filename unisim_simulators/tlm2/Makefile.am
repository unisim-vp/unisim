sharedir = $(prefix)/share/unisim_simulators/tlm

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_simple_bus_test
SUBDIRS += simple_bus_test
INCLUDE_SUBDIRS += simple_bus_test
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)
