INCLUDES=-I$(top_srcdir)/../../..
lib_LIBRARIES = libunisim_service_power.a
libunisim_service_power_a_SOURCES = cache_power_estimator.cc cacti4_2.cc

includedir = $(prefix)/include/unisim/service/power
include_HEADERS = cache_power_estimator.hh cacti4_2.hh

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
