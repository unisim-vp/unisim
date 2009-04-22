INCLUDES=-I$(top_srcdir)/../../../..
lib_LTLIBRARIES = libunisim_service_tee_symbol_table_lookup.la
libunisim_service_tee_symbol_table_lookup_la_SOURCES = tee_32.cc tee_64.cc

includedir = $(prefix)/include/unisim/service/tee/symbol_table_lookup
sharedir = $(prefix)/share/unisim_lib/unisim/service/tee/symbol_table_lookup
include_HEADERS = tee.hh tee.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
