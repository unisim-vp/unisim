sharedir = $(prefix)/share/unisim_simulators/clm

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_ooosim
SUBDIRS += ooosim
INCLUDE_SUBDIRS += ooosim
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)
