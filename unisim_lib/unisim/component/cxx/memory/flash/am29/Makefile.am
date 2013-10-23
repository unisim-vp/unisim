INCLUDES=-I$(top_srcdir)/../../../../../..

lib_LTLIBRARIES = libunisim_component_cxx_memory_flash_am29.la
libunisim_component_cxx_memory_flash_am29lv_la_SOURCES = am29.cc config.cc

includedir = $(prefix)/include/unisim/component/cxx/memory/flash/am29
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/memory/flash/am29
include_HEADERS = am29.hh am29.tcc config.hh types.hh

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../../../../..
include $(top_srcdir)/../../../../../../m4/unisim.am
