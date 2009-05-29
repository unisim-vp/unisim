includedir = $(prefix)/include/unisim/component/tlm2
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_memory
SUBDIRS += memory
INCLUDE_SUBDIRS += memory
endif

if COND_processor
SUBDIRS += processor
INCLUDE_SUBDIRS += processor
endif

if COND_interconnect
SUBDIRS += interconnect
INCLUDE_SUBDIRS += interconnect
endif

if COND_interrupt
SUBDIRS += interrupt
INCLUDE_SUBDIRS += interrupt
endif

if COND_timer
SUBDIRS += timer
INCLUDE_SUBDIRS += timer
endif

if COND_com
# uncomment following lines to activate the com
# SUBDIRS += com
# INCLUDE_SUBDIRS += com
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
