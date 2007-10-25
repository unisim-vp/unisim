includedir = $(prefix)/include/unisim/component/cxx/bridge
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/bridge

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_pci_isa
SUBDIRS += pci_isa
INCLUDE_SUBDIRS += pci_isa
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
