INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/time/sc_time
sharedir = $(prefix)/share/unisim_lib/unisim/service/time/sc_time
include_HEADERS = time.hh

lib_LIBRARIES = libunisim_service_time_sc_time.a
libunisim_service_time_sc_time_a_SOURCES = time.cc

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)


ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
