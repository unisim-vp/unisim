INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/tlm/fsb/snooping_bus
includedir = $(prefix)/include/unisim/component/tlm/fsb/snooping_bus
lib_LIBRARIES = libsnoopingbus.a
libsnoopingbus_a_SOURCES = bus_addr32_size32_procs1.cc
include_HEADERS = bus.hh bus.tcc

install-exec-hook: $(libdir)/libsnoopingbus.a
	rm -f $(prefix)/lib/libunisim_component_tlm_fsb_snoopingbus.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_fsb_snoopingbus.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_fsb_snoopingbus.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
