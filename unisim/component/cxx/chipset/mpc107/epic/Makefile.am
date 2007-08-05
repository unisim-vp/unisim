INCLUDES=-I$(top_srcdir)/../../../../../..
libdir= $(prefix)/lib/unisim/component/cxx/chipset/mpc107/epic
includedir = $(prefix)/include/unisim/component/cxx/chipset/mpc107/epic

lib_LIBRARIES = libepic.a
libepic_a_SOURCES = \
	register.cc \
	inservice_reg.cc \
	epic_32.cc epic_64.cc epic_32_debug.cc epic_64_debug.cc
		
include_HEADERS = \
	register.hh \
	inservice_reg.hh \
	epic.hh epic.tcc
	
install-exec-hook: $(libdir)/libepic.a
	rm -f $(prefix)/lib/libunisim_component_cxx_chipset_mpc107_epic.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_chipset_mpc107_epic.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_chipset_mpc107_epic.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)