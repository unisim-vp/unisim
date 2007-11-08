includedir = $(prefix)/include/unisim/service/tee
sharedir = $(prefix)/share/unisim_lib/unisim/service/tee

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_memory_access_reporting
SUBDIRS += memory_access_reporting
INCLUDE_SUBDIRS += memory_access_reporting
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
