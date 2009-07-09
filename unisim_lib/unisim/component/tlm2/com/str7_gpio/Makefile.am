INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/com/str7_gpio
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/com/str7_gpio
lib_LTLIBRARIES = libunisim_component_tlm2_com_str7_gpio.la
libunisim_component_tlm2_com_str7_gpio_la_SOURCES = str7_gpio.cc
include_HEADERS = str7_gpio.hh str7_gpio.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
