INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/pci/ide
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/pci/ide
lib_LIBRARIES = libunisim_component_cxx_pci_ide.a
libunisim_component_cxx_pci_ide_a_SOURCES = ide_disk.cc ide_ctrl.cc disk_image.cc
include_HEADERS = disk_image.hh event.hh ide_atareg.hh ide_ctrl.hh ide_disk.hh ide_wdcreg.hh pcidev.hh pci_master.hh pcireg.hh ide_ctrl.tcc ide_disk.tcc pcidev.tcc

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
