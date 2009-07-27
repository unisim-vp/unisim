INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/com/str7_adc
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/com/str7_adc
lib_LTLIBRARIES = libunisim_component_tlm2_com_str7_adc.la
libunisim_component_tlm2_com_str7_adc_la_SOURCES = str7_adc.cc
include_HEADERS = str7_adc.hh str7_adc.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
