INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/pci/ide
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/pci/ide
lib_LTLIBRARIES = libunisim_component_tlm_pci_ide.la
libunisim_component_tlm_pci_ide_la_SOURCES = pci_dev_32.cc pci_dev_64.cc pci_ide_module_32.cc pci_ide_module_64.cc
include_HEADERS = pci_dev.hh pci_dev.tcc pci_ide_module.hh pci_ide_module.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
