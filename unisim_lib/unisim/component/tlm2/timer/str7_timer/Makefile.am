INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/timer/str7_timer
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/timer/str7_timer
lib_LTLIBRARIES = libunisim_component_tlm2_timer_str7_timer.la
libunisim_component_tlm2_timer_str7_timer_la_SOURCES = tim.cc 
include_HEADERS = tim.hh tim.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
