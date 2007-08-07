INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/tlm/chipset/mpc107
includedir = $(prefix)/include/unisim/component/tlm/chipset/mpc107

SUBDIRS=epic
INCLUDE_SUBDIRS=epic

lib_LIBRARIES = libmpc107.a
libmpc107_a_SOURCES = \
	mpc107_fsb32_pci32.cc mpc107_fsb32_pci32_debug.cc \
	mpc107_fsb32_pci64.cc mpc107_fsb32_pci64_debug.cc \
	mpc107_fsb64_pci32.cc mpc107_fsb64_pci32_debug.cc \
	mpc107_fsb64_pci64.cc mpc107_fsb64_pci64_debug.cc
libmpc107_a_CXXFLAGS = -DSC_INCLUDE_DYNAMIC_PROCESSES
	
include_HEADERS = \
	mpc107.hh mpc107.tcc
		
install-exec-hook: $(libdir)/libmpc107.a
	rm -f $(prefix)/lib/libunisim_component_tlm_chipset_mpc107.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_chipset_mpc107.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_chipset_mpc107.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
