includedir = $(prefix)/include/unisim/kernel
sharedir = $(prefix)/share/unisim_lib/unisim/kernel
SUBDIRS=
INCLUDE_SUBDIRS=

if COND_clm
SUBDIRS += clm
INCLUDE_SUBDIRS += clm
endif

if COND_tlm
SUBDIRS += tlm
INCLUDE_SUBDIRS += tlm
endif

if COND_tlm2
SUBDIRS += tlm2
INCLUDE_SUBDIRS += tlm2
endif

if COND_logger
SUBDIRS += logger
INCLUDE_SUBDIRS += logger
endif

if COND_service
SUBDIRS += service
INCLUDE_SUBDIRS += service
endif

if COND_debug
SUBDIRS += debug
INCLUDE_SUBDIRS += debug
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
