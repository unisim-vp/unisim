INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/tlm/isa/i8042
includedir = $(prefix)/include/unisim/component/tlm/isa/i8042
lib_LIBRARIES = libi8042.a
libi8042_a_SOURCES = i8042.cc
include_HEADERS = i8042.hh i8042.tcc

install-exec-hook: $(libdir)/libi8042.a
	rm -f $(prefix)/lib/libunisim_component_tlm_isa_i8042.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_isa_i8042.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_isa_i8042.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
