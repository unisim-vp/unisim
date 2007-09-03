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

if COND_processor
SUBDIRS += processor
INCLUDE_SUBDIRS += processor
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
