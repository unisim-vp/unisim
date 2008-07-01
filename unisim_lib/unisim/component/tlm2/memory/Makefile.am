includedir = $(prefix)/include/unisim/component/tlm2/memory
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/memory

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_ram
SUBDIRS += ram
INCLUDE_SUBDIRS += ram
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
