INCLUDES=-I$(top_srcdir)/../../../..
libdir= $(prefix)/lib/unisim/component/tlm/message
includedir = $(prefix)/include/unisim/component/tlm/message

lib_LIBRARIES = libmessage.a

libmessage_a_SOURCES = \
	interrupt.cc
		
include_HEADERS = \
	interrupt.hh \
	isa.hh \
	memory.hh \
	pci.hh \
	simple_fsb.hh \
	snooping_fsb.hh

install-exec-hook: $(libdir)/libmessage.a
	rm -f $(prefix)/lib/libunisim_component_tlm_message.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_message.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_message.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
