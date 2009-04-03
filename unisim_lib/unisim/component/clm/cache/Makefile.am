includedir = $(prefix)/include/unisim/component/clm/cache
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/cache

#SUBDIRS=
#INCLUDE_SUBDIRS=

include_HEADERS = \
	cache_coherency_protocol.hh \
	cache_common.hh \
	cache_common_mc.hh \
	cache_container_counter.hh \
	cache_container.hh \
	cache_wb.hh \
	cache_wb_mc.hh \
	cache_mperfect.hh \
	mesi.h \
	MSHR_container.hh \
	MSHR_container_prefetcher.hh \
	msi.h

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..