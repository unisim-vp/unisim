INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/cxx/chipset/mpc107
includedir = $(prefix)/include/unisim/component/cxx/chipset/mpc107

SUBDIRS=dma epic
INCLUDE_SUBDIRS=dma epic

lib_LIBRARIES = libmpc107.a
libmpc107_a_SOURCES = \
	address_map_entry.cc \
	config_regs.cc \
	address_maps.cc \
	pci_controller.cc pci_controller_debug.cc
	
include_HEADERS = \
	address_map_entry.hh \
	config_regs.hh \
	address_maps.hh address_maps.tcc \
	pci_controller.hh pci_controller.tcc
	
install-exec-hook: $(libdir)/libmpc107.a
	rm -f $(prefix)/lib/libunisim_component_cxx_chipset_mpc107.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_chipset_mpc107.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_chipset_mpc107.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
