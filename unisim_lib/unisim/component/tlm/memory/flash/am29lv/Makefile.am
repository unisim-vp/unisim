INCLUDES=-I$(top_srcdir)/../../../../../..

lib_LTLIBRARIES = libunisim_component_tlm_memory_flash_am29lv.la
libunisim_component_tlm_memory_flash_am29lv_la_SOURCES = am29lv.cc

includedir = $(prefix)/include/unisim/component/tlm/memory/flash/am29lv
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/memory/flash/am29lv
include_HEADERS = am29lv.hh am29lv.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../../..
include $(top_srcdir)/../../../../../../m4/unisim.am
