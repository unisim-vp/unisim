sharedir = $(prefix)/share/unisim_simulators/clm

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_ooosim
SUBDIRS += ooosim
INCLUDE_SUBDIRS += ooosim
endif

if COND_ooosim_mc
SUBDIRS += ooosim_mc
INCLUDE_SUBDIRS += ooosim_mc
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)
