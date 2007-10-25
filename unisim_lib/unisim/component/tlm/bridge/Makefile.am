includedir = $(prefix)/include/unisim/component/tlm/bridge
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/bridge

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_simple_fsb_to_mem
SUBDIRS += simple_fsb_to_mem
INCLUDE_SUBDIRS += simple_fsb_to_mem
endif

if COND_snooping_fsb_to_mem
SUBDIRS += snooping_fsb_to_mem
INCLUDE_SUBDIRS += snooping_fsb_to_mem
endif

if COND_pci_isa
SUBDIRS += pci_isa
INCLUDE_SUBDIRS += pci_isa
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
