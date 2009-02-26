INCLUDES=-I$(top_srcdir)/../../../../../..

lib_LTLIBRARIES = libunisim_component_cxx_memory_flash_am29lv.la
libunisim_component_cxx_memory_flash_am29lv_la_SOURCES = am29lv.cc config.cc

includedir = $(prefix)/include/unisim/component/cxx/memory/flash/am29lv
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/memory/flash/am29lv
include_HEADERS = am29lv.hh am29lv.tcc config.hh types.hh

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../../../../..
include $(top_srcdir)/../../../../../../m4/unisim.am
