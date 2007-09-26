INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/chipset/mpc107

SUBDIRS=epic
INCLUDE_SUBDIRS=epic

lib_LIBRARIES = libunisim_component_tlm_chipset_mpc107.a
libunisim_component_tlm_chipset_mpc107_a_SOURCES = \
	mpc107_fsb32_pci32.cc mpc107_fsb32_pci32_debug.cc
	
#	mpc107_fsb32_pci64.cc mpc107_fsb32_pci64_debug.cc
#	mpc107_fsb64_pci32.cc mpc107_fsb64_pci32_debug.cc
#	mpc107_fsb64_pci64.cc mpc107_fsb64_pci64_debug.cc

libunisim_component_tlm_chipset_mpc107_a_CXXFLAGS = -DSC_INCLUDE_DYNAMIC_PROCESSES
	
include_HEADERS = \
	mpc107.hh mpc107.tcc
		
uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
