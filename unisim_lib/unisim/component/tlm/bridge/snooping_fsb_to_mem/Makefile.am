INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/bridge/snooping_fsb_to_mem
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/bridge/snooping_fsb_to_mem
lib_LIBRARIES = libunisim_component_tlm_bridge_snooping_fsb_to_mem.a
libunisim_component_tlm_bridge_snooping_fsb_to_mem_a_SOURCES = addr32_burstsize32.cc \
	addr32_burstsize32_debug.cc
include_HEADERS = bridge.hh bridge.tcc config.hh

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
