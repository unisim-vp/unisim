INCLUDES=-I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/component/tlm/message
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/message

lib_LTLIBRARIES = libunisim_component_tlm_message.la

libunisim_component_tlm_message_la_SOURCES = \
	interrupt.cc
		
include_HEADERS = \
	interrupt.hh \
	isa.hh \
	memory.hh \
	pci.hh \
	simple_fsb.hh \
	snooping_fsb.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
