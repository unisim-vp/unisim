includedir = $(prefix)/include/unisim/component/cxx/processor
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/processor

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_powerpc
SUBDIRS += powerpc
INCLUDE_SUBDIRS += powerpc
endif

if COND_arm
SUBDIRS += arm
INCLUDE_SUBDIRS += arm
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
