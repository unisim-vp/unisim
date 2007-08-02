includedir = $(prefix)/include/unisim/component/cxx/pci
libdir= $(prefix)/lib/unisim/component/cxx/pci

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_ide
SUBDIRS += ide
INCLUDE_SUBDIRS += ide
endif

if COND_net
SUBDIRS += net
INCLUDE_SUBDIRS += net
endif

if COND_video
SUBDIRS += video
INCLUDE_SUBDIRS += video
endif

if COND_macio
SUBDIRS += macio
INCLUDE_SUBDIRS += macio
endif


uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
