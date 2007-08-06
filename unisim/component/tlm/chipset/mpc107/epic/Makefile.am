INCLUDES=-I$(top_srcdir)/../../../../../..
libdir= $(prefix)/lib/unisim/component/tlm/chipset/mpc107/epic
includedir = $(prefix)/include/unisim/component/tlm/chipset/mpc107/epic

lib_LIBRARIES = libepic.a

libepic_a_CXXFLAGS =-DSC_INCLUDE_DYNAMIC_PROCESSES

libepic_a_SOURCES = \
	timer.cc \
	epic_32.cc epic_32_debug.cc epic_64.cc epic_64_debug.cc
		
include_HEADERS = \
	timer.hh \
	epic.hh epic.tcc
		
install-exec-hook: $(libdir)/libepic.a
	rm -f $(prefix)/lib/libunisim_component_tlm_chipset_mpc107_epic.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_chipset_mpc107_epic.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_chipset_mpc107_epic.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
	