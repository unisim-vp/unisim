INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/tlm/bridge/snooping_fsb_to_mem
includedir = $(prefix)/include/unisim/component/tlm/bridge/snooping_fsb_to_mem
lib_LIBRARIES = libsnooping_fsb_to_mem.a
libsnooping_fsb_to_mem_a_SOURCES = addr32_burstsize32.cc \
	addr32_burstsize32_debug.cc
include_HEADERS = snooping_fsb_to_mem.hh snooping_fsb_to_mem.tcc

install-exec-hook: $(libdir)/libsnooping_fsb_to_mem.a
	rm -f $(prefix)/lib/libunisim_component_tlm_bridge_snooping_fsb_to_mem.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_bridge_snooping_fsb_to_mem.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_bridge_snooping_fsb_to_mem.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
