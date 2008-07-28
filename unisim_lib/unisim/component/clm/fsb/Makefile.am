includedir = $(prefix)/include/unisim/component/clm/fsb
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/fsb

#SUBDIRS=
#INCLUDE_SUBDIRS=

include_HEADERS = \
	bus.hh \
	bus_multiqueue.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..