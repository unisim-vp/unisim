INCLUDES=-I$(top_srcdir)/../../../..
libdir = $(prefix)/lib/unisim/service/tee/memory_access_reporting
lib_LIBRARIES = libtee_memory_access_reporting.a
libtee_memory_access_reporting_a_SOURCES = tee_32.cc tee_64.cc

includedir = $(prefix)/include/unisim/service/tee/memory_access_reporting
include_HEADERS = tee.hh tee.tcc

install-exec-hook: $(libdir)/libtee_memory_access_reporting.a
	rm -f $(prefix)/lib/libunisim_service_tee_memory_access_reporting.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_service_tee_memory_access_reporting.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_tee_memory_access_reporting.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
