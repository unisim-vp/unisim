INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/bridge/pci_isa
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/bridge/pci_isa
lib_LIBRARIES = libunisim_component_tlm_bridge_pci_isa.a
libunisim_component_tlm_bridge_pci_isa_a_SOURCES = bridge_32.cc bridge_64.cc
include_HEADERS = bridge.hh bridge.tcc

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
