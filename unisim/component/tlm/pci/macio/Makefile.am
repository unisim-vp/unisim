INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/tlm/pci/macio
includedir = $(prefix)/include/unisim/component/tlm/pci/macio
lib_LIBRARIES = libmacio.a
libmacio_a_SOURCES = heathrow_32.cc heathrow_64.cc
include_HEADERS = heathrow.hh heathrow.tcc

install-exec-hook: $(libdir)/libmacio.a
	rm -f $(prefix)/lib/libunisim_component_tlm_pci_macio.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_pci_macio.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_pci_macio.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
