INCLUDES=-I$(top_srcdir)/../../../../../..

lib_LTLIBRARIES = libunisim_component_tlm_memory_flash_am29.la
libunisim_component_tlm_memory_flash_am29_la_SOURCES = am29.cc

includedir = $(prefix)/include/unisim/component/tlm/memory/flash/am29
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/memory/flash/am29
include_HEADERS = am29.hh am29.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../../..
include $(top_srcdir)/../../../../../../m4/unisim.am
