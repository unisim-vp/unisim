INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/time/sc_time
include_HEADERS = time.hh

lib_LIBRARIES = libunisim_service_time_sc_time.a
libunisim_service_time_sc_time_a_SOURCES = time.cc
uninstall-hook:
	(rmdir $(includedir) || true)
