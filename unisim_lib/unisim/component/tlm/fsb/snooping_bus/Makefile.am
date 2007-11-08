INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/fsb/snooping_bus
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/fsb/snooping_bus
lib_LIBRARIES = libunisim_component_tlm_fsb_snooping_bus.a
libunisim_component_tlm_fsb_snooping_bus_a_SOURCES = bus_addr32_size32_procs1.cc
include_HEADERS = bus.hh bus.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
