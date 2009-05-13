includedir = $(prefix)/include/unisim/component/tlm2/timer
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/timer

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_str7_timer
SUBDIRS += str7_timer
INCLUDE_SUBDIRS += str7_timer
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
