INCLUDES = -I$(top_srcdir)/../../../..
libdir = $(prefix)/lib/unisim/service/loader/pmac_bootx
includedir = $(prefix)/include/unisim/service/loader/pmac_bootx
include_HEADERS = pmac_bootx.hh

lib_LIBRARIES = libpmac_bootx.a
libpmac_bootx_a_SOURCES = pmac_bootx.cc
install-exec-hook: $(libdir)/libpmac_bootx.a
	rm -f $(prefix)/lib/libunisim_service_loader_pmac_bootx.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_service_loader_pmac_bootx.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_loader_pmac_bootx.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
