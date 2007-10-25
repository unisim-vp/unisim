includedir = $(prefix)/include/unisim/component/tlm/memory
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/memory

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_ram
SUBDIRS += ram
INCLUDE_SUBDIRS += ram
endif

if COND_flash
SUBDIRS += flash
INCLUDE_SUBDIRS += flash
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
