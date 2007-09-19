INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/bridge/pci_isa
lib_LIBRARIES = libunisim_component_cxx_bridge_pci_isa.a
libunisim_component_cxx_bridge_pci_isa_a_SOURCES = bridge_32.cc bridge_64.cc
include_HEADERS = bridge.hh bridge.tcc

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
