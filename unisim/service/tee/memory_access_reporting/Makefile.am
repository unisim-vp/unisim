INCLUDES=-I$(top_srcdir)/../../../..
lib_LIBRARIES = libunisim_service_tee_memory_access_reporting.a
libunisim_service_tee_memory_access_reporting_a_SOURCES = tee_32.cc tee_64.cc

includedir = $(prefix)/include/unisim/service/tee/memory_access_reporting
include_HEADERS = tee.hh tee.tcc

uninstall-hook:
	(rmdir $(includedir) || true)
