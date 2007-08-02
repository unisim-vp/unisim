includedir = $(prefix)/include/unisim/component/c++
libdir= $(prefix)/lib/unisim/component/c++

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_memory
SUBDIRS += memory
INCLUDE_SUBDIRS += memory
endif

if COND_cache
SUBDIRS += cache
INCLUDE_SUBDIRS += cache
endif

if COND_tlb
SUBDIRS += tlb
INCLUDE_SUBDIRS += tlb
endif

if COND_processor
SUBDIRS += processor
INCLUDE_SUBDIRS += processor
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
