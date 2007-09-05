includedir = $(prefix)/include/unisim/component/tlm
libdir= $(prefix)/lib/unisim/component/tlm

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_message
SUBDIRS += message
INCLUDE_SUBDIRS += message
endif

if COND_debug
SUBDIRS += debug
INCLUDE_SUBDIRS += debug
endif

if COND_memory
SUBDIRS += memory
INCLUDE_SUBDIRS += memory
endif

if COND_chipset
SUBDIRS += chipset
INCLUDE_SUBDIRS += chipset
endif

if COND_isa
SUBDIRS += isa
INCLUDE_SUBDIRS += isa
endif

if COND_pci
SUBDIRS += pci
INCLUDE_SUBDIRS += pci
endif

if COND_processor
SUBDIRS += processor
INCLUDE_SUBDIRS += processor
endif

if COND_bridge
SUBDIRS += bridge
INCLUDE_SUBDIRS += bridge
endif

if COND_fsb
SUBDIRS += fsb
INCLUDE_SUBDIRS += fsb
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
