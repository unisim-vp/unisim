INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/tlm/pci/ide
includedir = $(prefix)/include/unisim/component/tlm/pci/ide
lib_LIBRARIES = libide.a
libide_a_SOURCES = pci_dev_32.cc pci_dev_64.cc pci_ide_module_32.cc pci_ide_module_64.cc
include_HEADERS = pci_dev.hh pci_dev.tcc pci_ide_module.hh pci_ide_module.tcc

install-exec-hook: $(libdir)/libide.a
	rm -f $(prefix)/lib/libunisim_component_tlm_pci_ide.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_pci_ide.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_pci_ide.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
