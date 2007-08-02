INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/cxx/pci/ide
includedir = $(prefix)/include/unisim/component/cxx/pci/ide
lib_LIBRARIES = libide.a
libide_a_SOURCES = ide_disk.cc ide_ctrl.cc disk_image.cc
include_HEADERS = disk_image.hh event.hh ide_atareg.hh ide_ctrl.hh ide_disk.hh ide_wdcreg.hh pcidev.hh pci_master.hh pcireg.hh ide_ctrl.tcc ide_disk.tcc

install-exec-hook: $(libdir)/libide.a
	rm -f $(prefix)/lib/libunisim_component_cxx_pci_ide.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_pci_ide.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_pci_ide.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
