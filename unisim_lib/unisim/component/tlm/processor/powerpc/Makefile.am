INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/processor/powerpc
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/processor/powerpc
lib_LTLIBRARIES = libunisim_component_tlm_processor_powerpc.la
libunisim_component_tlm_processor_powerpc_la_SOURCES = mpc755_debug.cc mpc7447a_debug.cc

if COND_release
libunisim_component_tlm_processor_powerpc_la_SOURCES += mpc755.cc mpc7447a.cc
endif

include_HEADERS = powerpc.hh powerpc.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
