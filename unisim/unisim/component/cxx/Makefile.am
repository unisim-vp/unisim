includedir = $(prefix)/include/unisim/component/cxx

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

if COND_pci
SUBDIRS += pci
INCLUDE_SUBDIRS += pci
endif

if COND_isa
SUBDIRS += isa
INCLUDE_SUBDIRS += isa
endif

if COND_bridge
SUBDIRS += bridge
INCLUDE_SUBDIRS += bridge
endif

if COND_chipset
SUBDIRS += chipset
INCLUDE_SUBDIRS += chipset
endif

uninstall-hook:
	(rmdir $(includedir) || true)
