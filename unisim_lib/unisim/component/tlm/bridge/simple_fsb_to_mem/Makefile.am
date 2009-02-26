INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/bridge/simple_fsb_to_mem
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/bridge/simple_fsb_to_mem
lib_LTLIBRARIES = libunisim_component_tlm_bridge_simple_fsb_to_mem.la
libunisim_component_tlm_bridge_simple_fsb_to_mem_la_SOURCES = addr32_burstsize32.cc \
	addr32_burstsize32_debug.cc
include_HEADERS = bridge.hh bridge.tcc config.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
