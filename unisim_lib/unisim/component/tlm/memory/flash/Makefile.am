includedir = $(prefix)/include/unisim/component/tlm/memory/flash
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/memory/flash

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_am29lv
SUBDIRS += am29lv
INCLUDE_SUBDIRS += am29lv
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
