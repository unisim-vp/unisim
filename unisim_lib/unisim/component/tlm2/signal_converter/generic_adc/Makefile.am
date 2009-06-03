INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/signal_converter/generic_adc
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/signal_converter/generic_adc
lib_LTLIBRARIES = libunisim_component_tlm2_signal_converter_generic_adc.la
libunisim_component_tlm2_tlm2_signal_converter_generic_adc_la_SOURCES = adc.cc
include_HEADERS = adc.hh config_generator.hh config_spi.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
