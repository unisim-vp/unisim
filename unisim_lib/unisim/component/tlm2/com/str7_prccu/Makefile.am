INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/com/str7_prccu
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/com/str7_prccu
lib_LTLIBRARIES = libunisim_component_tlm2_com_str7_prccu.la
libunisim_component_tlm2_com_str7_prccu_la_SOURCES = str7_prccu.cc
include_HEADERS = str7_prccu.hh str7_prccu.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
