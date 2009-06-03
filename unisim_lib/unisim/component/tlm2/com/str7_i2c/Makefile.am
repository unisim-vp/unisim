INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/com/str7_i2c
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/com/str7_i2c
lib_LTLIBRARIES = libunisim_component_tlm2_com_str7_i2c.la
libunisim_component_tlm2_com_str7_i2c_la_SOURCES = str7_i2c.cc str7_i2c_verbose.cc
include_HEADERS = str7_i2c.hh str7_i2c.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
