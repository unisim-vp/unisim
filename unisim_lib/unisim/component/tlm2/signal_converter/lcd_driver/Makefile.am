INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/signal_converter/lcd_driver
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/signal_converter/lcd_driver
lib_LTLIBRARIES = libunisim_component_tlm2_signal_converter_lcd_driver.la
libunisim_component_tlm2_signal_converter_lcd_driver_la_SOURCES = em6124.cc
include_HEADERS = em6124.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am

