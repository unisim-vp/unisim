includedir = $(prefix)/include/unisim/service/time

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

uninstall-hook:
	(rmdir $(includedir) || true)
