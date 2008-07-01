includedir = $(prefix)/include/unisim/component/tlm2
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_simple_bus
SUBDIRS += simple_bus
INCLUDE_SUBDIRS += simple_bus
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
