INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/isa/i8042
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/isa/i8042
lib_LTLIBRARIES = libunisim_component_tlm_isa_i8042.la
libunisim_component_tlm_isa_i8042_la_SOURCES = i8042.cc
include_HEADERS = i8042.hh i8042.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
