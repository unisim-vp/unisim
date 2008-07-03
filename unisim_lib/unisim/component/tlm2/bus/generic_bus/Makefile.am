INCLUDES=-I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/component/tlm2/simple_bus
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/simple_bus
lib_LIBRARIES = libunisim_component_tlm2_simple_bus_bus.a
libunisim_component_tlm2_simple_bus_bus_a_SOURCES = bus_test.cc
include_HEADERS = bus.hh bus.tcc bus_types.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
