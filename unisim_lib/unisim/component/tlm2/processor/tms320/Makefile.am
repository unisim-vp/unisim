INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/processor/tms320
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/processor/tms320
lib_LTLIBRARIES = libunisim_component_tlm2_processor_tms320.la
libunisim_component_tlm2_processor_tms320_la_SOURCES = tms320vc33_debug.cc tms320vc33.cc
	
if COND_release
libunisim_component_tlm2_processor_tms320_la_SOURCES += tms320vc33.cc
endif 

include_HEADERS = tms320.hh tms320.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am

