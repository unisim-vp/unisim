INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/interrupt/str7_xti
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/interrupt/str7_xti
lib_LTLIBRARIES = libunisim_component_tlm2_interrupt_str7-xti.la
libunisim_component_tlm2_interrupt_str7_xti_la_SOURCES = \
	str7_xti_debug.cc
	
if COND_release
libunisim_component_tlm2_interrupt_str7_xti_la_SOURCES += \
	str7_xti.cc
endif 

include_HEADERS = str7_xti.hh str7_xti.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am

