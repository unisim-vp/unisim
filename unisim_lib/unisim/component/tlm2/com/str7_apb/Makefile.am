INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/com/str7_apb
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/com/str7_apb
lib_LTLIBRARIES = libunisim_component_tlm2_com_str7_apb.la
libunisim_component_tlm2_com_str7_apb_la_SOURCES = str7_apb.cc
include_HEADERS = str7_apb.hh str7_apb.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
