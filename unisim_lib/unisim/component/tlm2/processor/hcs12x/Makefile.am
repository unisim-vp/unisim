INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/processor/hcs12x
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/processor/hcs12x
lib_LIBRARIES = libunisim_component_tlm2_processor_hcs12x.a
libunisim_component_tlm2_processor_hcs12x_a_SOURCES = hcs12x.cc xint.cc
	
include_HEADERS = hcs12x.hh xint.hh tlm_types.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
