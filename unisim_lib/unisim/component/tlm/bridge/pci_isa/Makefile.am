INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/bridge/pci_isa
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/bridge/pci_isa
lib_LTLIBRARIES = libunisim_component_tlm_bridge_pci_isa.la
libunisim_component_tlm_bridge_pci_isa_la_SOURCES = bridge_32.cc bridge_64.cc
include_HEADERS = bridge.hh bridge.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
