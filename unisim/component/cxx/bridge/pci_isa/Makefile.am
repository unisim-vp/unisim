INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/cxx/bridge/pci_isa
includedir = $(prefix)/include/unisim/component/cxx/bridge/pci_isa
lib_LIBRARIES = libpci_isa.a
libpci_isa_a_SOURCES = bridge_32.cc bridge_64.cc
include_HEADERS = bridge.hh bridge.tcc

install-exec-hook: $(libdir)/libpci_isa.a
	rm -f $(prefix)/lib/libunisim_component_cxx_bridge_pci_isa.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_bridge_pci_isa.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_bridge_pci_isa.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
