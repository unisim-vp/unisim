includedir = $(prefix)/include/unisim/component
sharedir = $(prefix)/share/unisim_lib/unisim/component

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_engine
SUBDIRS += engine
INCLUDE_SUBDIRS += engine
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
