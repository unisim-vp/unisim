includedir = $(prefix)/include/unisim/component/cxx/memory/flash
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/memory/flash

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_am29lv
SUBDIRS += am29lv
INCLUDE_SUBDIRS += am29lv
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
