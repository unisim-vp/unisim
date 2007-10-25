includedir = $(prefix)/include/unisim/service/time
sharedir = $(prefix)/share/unisim_lib/unisim/service/time

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_sc_time
SUBDIRS += sc_time
INCLUDE_SUBDIRS += sc_time
endif

if COND_host_time
SUBDIRS += host_time
INCLUDE_SUBDIRS += host_time
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
