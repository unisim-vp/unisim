INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/chipset/mpc107
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/chipset/mpc107

SUBDIRS=dma epic
INCLUDE_SUBDIRS=dma epic

lib_LIBRARIES = libunisim_component_cxx_chipset_mpc107.a
libunisim_component_cxx_chipset_mpc107_a_SOURCES = \
	address_map_entry.cc \
	config_regs.cc \
	address_maps.cc \
	address_maps_debug.cc \
	pci_controller.cc pci_controller_debug.cc
	
include_HEADERS = \
	address_map_entry.hh \
	config_regs.hh \
	address_maps.hh address_maps.tcc \
	pci_controller.hh pci_controller.tcc
	
uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
