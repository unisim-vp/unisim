includedir = $(prefix)/include/unisim/component/tlm2/processor
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/processor

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_arm
SUBDIRS += arm
INCLUDE_SUBDIRS += arm
endif

if COND_hcs12x
SUBDIRS += hcs12x
INCLUDE_SUBDIRS += hcs12x
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
