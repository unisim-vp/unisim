INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/processor/tms320c3x
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/processor/tms320c3x
lib_LTLIBRARIES = libunisim_component_tlm2_processor_tms320c3x.la
libunisim_component_tlm2_processor_tms320c3x_la_SOURCES = tms320c3xvc33_debug.cc
	
if COND_release
libunisim_component_tlm2_processor_tms320c3x_la_SOURCES += tms320c3xvc33.cc
endif 

include_HEADERS = tms320c3x.hh tms320c3x.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am

