INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/pci/ide
lib_LIBRARIES = libunisim_component_tlm_pci_ide.a
libunisim_component_tlm_pci_ide_a_SOURCES = pci_dev_32.cc pci_dev_64.cc pci_ide_module_32.cc pci_ide_module_64.cc
include_HEADERS = pci_dev.hh pci_dev.tcc pci_ide_module.hh pci_ide_module.tcc

uninstall-hook:
	(rmdir $(includedir) || true)
