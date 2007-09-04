INCLUDES=-I$(top_srcdir)/../../../../..
#libdir= $(prefix)/lib/unisim/component/tlm/pci/bus
includedir = $(prefix)/include/unisim/component/tlm/pci/bus
#lib_LIBRARIES = libbus.a
#libbus_a_SOURCES = bus_32.cc bus_64.cc
include_HEADERS = bus.hh # bus.tcc

#install-exec-hook: $(libdir)/libbus.a
#	rm -f $(prefix)/lib/libunisim_component_tlm_pci_bus.a
#	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_pci_bus.a)
uninstall-hook:
#	rm -f $(prefix)/lib/libunisim_component_tlm_pci_bus.a
#	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
