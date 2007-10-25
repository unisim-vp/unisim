INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/time/host_time
include_HEADERS = time.hh

lib_LIBRARIES = libunisim_service_time_host_time.a
libunisim_service_time_host_time_a_SOURCES = time.cc
uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
