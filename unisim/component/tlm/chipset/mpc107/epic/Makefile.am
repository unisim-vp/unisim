INCLUDES=-I$(top_srcdir)/../../../../../..
includedir = $(prefix)/include/unisim/component/tlm/chipset/mpc107/epic

lib_LIBRARIES = libunisim_component_tlm_chipset_mpc107_epic.a

libunisim_component_tlm_chipset_mpc107_epic_a_CXXFLAGS =-DSC_INCLUDE_DYNAMIC_PROCESSES

libunisim_component_tlm_chipset_mpc107_epic_a_SOURCES = \
	timer.cc \
	epic_32.cc epic_32_debug.cc epic_64.cc epic_64_debug.cc
		
include_HEADERS = \
	timer.hh \
	epic.hh epic.tcc
		
uninstall-hook:
	(rmdir $(includedir) || true)
	