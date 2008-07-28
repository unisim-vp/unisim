includedir = $(prefix)/include/unisim/component/clm/memory
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/memory

#SUBDIRS=
#INCLUDE_SUBDIRS=

nobase_include_HEADERS = \
	dram/dram_components.hh \
	dram/dram.hh \
	mem_common.h \
	memory_container.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..