sharedir = $(prefix)/share/unisim_simulators/cxx

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_tms320c3x
SUBDIRS += tms320c3x
INCLUDE_SUBDIRS += tms320c3x
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)
