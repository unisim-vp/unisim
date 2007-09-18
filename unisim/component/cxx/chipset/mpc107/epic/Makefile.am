INCLUDES=-I$(top_srcdir)/../../../../../..
includedir = $(prefix)/include/unisim/component/cxx/chipset/mpc107/epic

lib_LIBRARIES = libunisim_component_cxx_chipset_mpc107_epic.a
libunisim_component_cxx_chipset_mpc107_epic_a_SOURCES = \
	register.cc \
	inservice_reg.cc \
	epic_32.cc epic_64.cc epic_32_debug.cc epic_64_debug.cc
		
include_HEADERS = \
	register.hh \
	inservice_reg.hh \
	epic.hh epic.tcc
	
uninstall-hook:
	(rmdir $(includedir) || true)