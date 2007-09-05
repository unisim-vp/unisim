INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/tlm/fsb/simple_bus
includedir = $(prefix)/include/unisim/component/tlm/fsb/simple_bus
lib_LIBRARIES = libsimplebus.a
libsimplebus_a_SOURCES = bus.cc bus_debug.cc
include_HEADERS = bus.hh bus.tcc

install-exec-hook: $(libdir)/libsimplebus.a
	rm -f $(prefix)/lib/libunisim_component_tlm_fsb_simplebus.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_fsb_simplebus.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_fsb_simplebus.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
