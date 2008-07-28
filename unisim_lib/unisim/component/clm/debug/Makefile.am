includedir = $(prefix)/include/unisim/component/clm
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm

#SUBDIRS=
#INCLUDE_SUBDIRS=

include_HEADERS = \
	log.hh \
	stop_at_cycle.sim \
	svg_memreq.h \
	svg_memreq_interface.hh \
	svg_memreq_service.h \
	svg_memreq.sim

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..