INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/interconnect/generic_bus
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/interconnect/generic_bus
lib_LTLIBRARIES = libunisim_component_tlm2_interconnect_generic_bus.la
libunisim_component_tlm2_interconnect_generic_bus_la_SOURCES = bus_test.cc
include_HEADERS = bus.hh bus.tcc bus_types.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
