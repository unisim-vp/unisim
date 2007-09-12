INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/tlm/bridge/simple_fsb_to_mem
includedir = $(prefix)/include/unisim/component/tlm/bridge/simple_fsb_to_mem
lib_LIBRARIES = libbridge.a
libbridge_a_SOURCES = addr32_burstsize32.cc \
	addr32_burstsize32_debug.cc
include_HEADERS = bridge.hh bridge.tcc

install-exec-hook: $(libdir)/libbridge.a
	rm -f $(prefix)/lib/libunisim_component_tlm_bridge_simple_fsb_to_mem_bridge.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_bridge_simple_fsb_to_mem_simple_fsb_to_mem.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_bridge_simple_fsb_to_mem.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
