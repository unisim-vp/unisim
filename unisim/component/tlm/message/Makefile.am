INCLUDES=-I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/component/tlm/message

lib_LIBRARIES = libunisim_component_tlm_message.a

libunisim_component_tlm_message_a_SOURCES = \
	interrupt.cc
		
include_HEADERS = \
	interrupt.hh \
	isa.hh \
	memory.hh \
	pci.hh \
	simple_fsb.hh \
	snooping_fsb.hh

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
