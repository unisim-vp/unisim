includedir = $(prefix)/include/unisim/component/tlm/pci
libdir= $(prefix)/lib/unisim/component/tlm/pci

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_video
SUBDIRS += video
INCLUDE_SUBDIRS += video
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
