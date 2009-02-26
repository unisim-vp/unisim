INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/time/host_time
sharedir = $(prefix)/share/unisim_lib/unisim/service/time/host_time
include_HEADERS = time.hh

lib_LTLIBRARIES = libunisim_service_time_host_time.la
libunisim_service_time_host_time_la_SOURCES = time.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
