INCLUDES = -I$(top_srcdir)/../../..
libdir = $(prefix)/lib/unisim/kernel/service
includedir = $(prefix)/include/unisim/kernel/service
include_HEADERS = service.hh

lib_LIBRARIES = libservices.a
libservices_a_SOURCES = service.cc

install-exec-hook: $(libdir)/libservices.a
	if test ! -f $(prefix)/lib/libunisim_kernel_services.a; then (cd $(prefix)/lib && $(LN_S) $< libunisim_kernel_services.a); fi
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_kernel_services.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
