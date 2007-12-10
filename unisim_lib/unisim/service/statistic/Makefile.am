INCLUDES=-I$(top_srcdir)/../../..
lib_LIBRARIES = libunisim_service_statistic.a
libunisim_service_statistic_a_SOURCES = statistic_server.cc

includedir = $(prefix)/include/unisim/service/statistic
sharedir = $(prefix)/share/unisim_lib/unisim/service/statistic
include_HEADERS = statistic_server.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
