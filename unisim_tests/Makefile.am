sharedir = $(prefix)/share/unisim_tests

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_clm
SUBDIRS += clm
INCLUDE_SUBDIRS += clm
endif

if COND_tlm2
SUBDIRS += tlm2
INCLUDE_SUBDIRS += tlm2
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

include m4/aminclude.am
