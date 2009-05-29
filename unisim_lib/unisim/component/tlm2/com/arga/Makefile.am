INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/com/arga
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/com/arga
lib_LTLIBRARIES = libunisim_component_tlm2_com_arga.la
libunisim_component_tlm2_com_arga_la_SOURCES = arga.cc
include_HEADERS = arga.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
