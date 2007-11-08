INCLUDES=-I$(top_srcdir)/../../../..
lib_LIBRARIES = libunisim_service_tee_memory_access_reporting.a
libunisim_service_tee_memory_access_reporting_a_SOURCES = tee_32.cc tee_64.cc

includedir = $(prefix)/include/unisim/service/tee/memory_access_reporting
sharedir = $(prefix)/share/unisim_lib/unisim/service/tee/memory_access_reporting
include_HEADERS = tee.hh tee.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
