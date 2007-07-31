INCLUDES = -I$(top_srcdir)/../../../..
libdir = $(prefix)/lib/unisim/service/loader/pmac_linux_kernel_loader
includedir = $(prefix)/include/unisim/service/loader/pmac_linux_kernel_loader
include_HEADERS = pmac_linux_kernel_loader.hh

lib_LIBRARIES = libpmac_linux_kernel_loader.a
libpmac_linux_kernel_loader_a_SOURCES = pmac_linux_kernel_loader.cc
install-exec-hook: $(libdir)/libpmac_linux_kernel_loader.a
	rm -f $(prefix)/lib/libunisim_service_loader_pmac_linux_kernel_loader.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_service_loader_pmac_linux_kernel_loader.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_loader_pmac_linux_kernel_loader.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
