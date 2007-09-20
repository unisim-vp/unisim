INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/processor/powerpc
lib_LIBRARIES = libunisim_component_tlm_processor_powerpc.a
libunisim_component_tlm_processor_powerpc_a_SOURCES = mpc755.cc mpc7447a.cc
include_HEADERS = powerpc.hh powerpc.tcc

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
