INCLUDES=-I$(top_srcdir)/../../..
libdir = $(prefix)/lib/unisim/service/power
lib_LIBRARIES = libpower.a
libpower_a_SOURCES = cache_power_estimator.cc cacti4_2.cc

includedir = $(prefix)/include/unisim/service/power
include_HEADERS = cache_power_estimator.hh cacti4_2.hh

install-exec-hook: $(libdir)/libpower.a
	rm -f $(prefix)/lib/libunisim_service_power.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_service_power.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_power.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
