INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
libdir= $(prefix)/lib/unisim/component/tlm/processor/powerpc
includedir = $(prefix)/include/unisim/component/tlm/processor/powerpc
lib_LIBRARIES = libpowerpc.a
libpowerpc_a_SOURCES = mpc755.cc mpc7447a.cc
include_HEADERS = powerpc.hh powerpc.tcc

install-exec-hook: $(libdir)/libpowerpc.a
	rm -f $(prefix)/lib/libunisim_component_tlm_processor_powerpc.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_processor_powerpc.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_processor_powerpc.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
