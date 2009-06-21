INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/interrupt/str7_eic
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/interrupt/str7_eic
lib_LTLIBRARIES = libunisim_component_tlm2_interrupt_str7_eic.la
	
libunisim_component_tlm2_interrupt_str7_eic_la_SOURCES = \
	str7_eic.cc

include_HEADERS = str7_eic.hh str7_eic.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am

