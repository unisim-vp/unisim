includedir = $(prefix)/include/unisim/component/tlm/fsb
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/fsb

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_simple_bus
SUBDIRS += simple_bus
INCLUDE_SUBDIRS += simple_bus
endif

if COND_snooping_bus
SUBDIRS += snooping_bus
INCLUDE_SUBDIRS += snooping_bus
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
