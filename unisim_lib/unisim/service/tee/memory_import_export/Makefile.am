INCLUDES=-I$(top_srcdir)/../../../..
lib_LTLIBRARIES = libunisim_service_tee_memory_import_export.la
libunisim_service_tee_memory_import_export_la_SOURCES = memory_import_export_tee.cc

includedir = $(prefix)/include/unisim/service/tee/memory_import_export
sharedir = $(prefix)/share/unisim_lib/unisim/service/tee/memory_import_export
include_HEADERS = memory_import_export_tee.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
