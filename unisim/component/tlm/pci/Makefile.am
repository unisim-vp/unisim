includedir = $(prefix)/include/unisim/component/tlm/pci
libdir= $(prefix)/lib/unisim/component/tlm/pci

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_macio
SUBDIRS += macio
INCLUDE_SUBDIRS += macio
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
