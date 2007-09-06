INCLUDES = -I$(top_srcdir)/../../../..
libdir = $(prefix)/lib/unisim/service/time/sc_time
includedir = $(prefix)/include/unisim/service/time/sc_time
include_HEADERS = time.hh

lib_LIBRARIES = libtime.a
libtime_a_SOURCES = time.cc
install-exec-hook: $(libdir)/libtime.a
	rm -f $(prefix)/lib/libunisim_service_time_sc_time.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_service_time_sc_time.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_time_sc_time.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
