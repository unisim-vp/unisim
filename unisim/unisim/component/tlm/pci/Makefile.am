includedir = $(prefix)/include/unisim/component/tlm/pci

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_macio
SUBDIRS += macio
INCLUDE_SUBDIRS += macio
endif

if COND_video
SUBDIRS += video
INCLUDE_SUBDIRS += video
endif

if COND_ide
SUBDIRS += ide
INCLUDE_SUBDIRS += ide
endif

if COND_debug
SUBDIRS += debug
INCLUDE_SUBDIRS += debug
endif

if COND_bus
SUBDIRS += bus
INCLUDE_SUBDIRS += bus
endif

uninstall-hook:
	(rmdir $(includedir) || true)
