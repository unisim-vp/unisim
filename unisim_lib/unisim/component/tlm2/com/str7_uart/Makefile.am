INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/com/str7_uart
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/com/str7_uart
lib_LTLIBRARIES = libunisim_component_tlm2_com_str7_uart.la
libunisim_component_tlm2_com_str7_uart_la_SOURCES = str7_uart.cc
include_HEADERS = str7_uart.hh str7_uart.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
