sharedir = $(prefix)/share/unisim_simulators/cxx

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_tesla
SUBDIRS += tesla
INCLUDE_SUBDIRS += tesla
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)
