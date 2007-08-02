includedir = $(prefix)/include/unisim/component/cxx/bridge
libdir= $(prefix)/lib/unisim/component/cxx/bridge

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_pci_isa
SUBDIRS += pci_isa
INCLUDE_SUBDIRS += pci_isa
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
