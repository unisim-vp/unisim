INCLUDES=-I$(top_srcdir)/../../..
lib_LTLIBRARIES = libunisim_service_power.la
libunisim_service_power_la_SOURCES = cache_power_estimator.cc

includedir = $(prefix)/include/unisim/service/power
sharedir = $(prefix)/share/unisim_lib/unisim/service/power
include_HEADERS = cache_power_estimator.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
