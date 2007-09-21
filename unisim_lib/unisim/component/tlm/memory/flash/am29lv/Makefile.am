INCLUDES=-I$(top_srcdir)/../../../../../..

lib_LIBRARIES = libunisim_component_tlm_memory_flash_am29lv.a
libunisim_component_tlm_memory_flash_am29lv_a_SOURCES = am29lv.cc

includedir = $(prefix)/include/unisim/component/tlm/memory/flash/am29lv
include_HEADERS = am29lv.hh am29lv.tcc

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../../../..
include $(top_srcdir)/../../../../../../m4/unisim.am
