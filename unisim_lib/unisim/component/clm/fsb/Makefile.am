includedir = $(prefix)/include/unisim/component/clm/fsb
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/fsb

#SUBDIRS=
#INCLUDE_SUBDIRS=

include_HEADERS = \
	bus.hh \
	bus_multiqueue.hh \
	bus_multiqueue2_mc.hh \
	bus_multiqueue3.hh \
	bus_multiqueue3_mc.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..