includedir = $(prefix)/include/unisim/component/clm
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_cache
SUBDIRS += cache
INCLUDE_SUBDIRS += cache
endif

if COND_debug
SUBDIRS += debug
INCLUDE_SUBDIRS += debug
endif

if COND_fsb
SUBDIRS += fsb
INCLUDE_SUBDIRS += fsb
endif

if COND_interfaces
SUBDIRS += interfaces
INCLUDE_SUBDIRS += interfaces
endif

if COND_memory
SUBDIRS += memory
INCLUDE_SUBDIRS += memory
endif

if COND_pipeline
SUBDIRS += pipeline
INCLUDE_SUBDIRS += pipeline
endif


if COND_processor
SUBDIRS += processor
INCLUDE_SUBDIRS += processor
endif

if COND_utility
SUBDIRS += utility
INCLUDE_SUBDIRS += utility
endif


dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..