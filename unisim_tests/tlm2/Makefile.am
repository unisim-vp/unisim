sharedir = $(prefix)/share/unisim_tests/tlm2

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_interconnect
SUBDIRS += interconnect
INCLUDE_SUBDIRS += interconnect
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)
