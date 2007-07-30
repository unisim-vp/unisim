INCLUDES = -I$(top_srcdir)/../../../..
libdir = $(prefix)/lib/unisim/service/loader/linux_loader
includedir = $(prefix)/include/unisim/service/loader/linux_loader
include_HEADERS = linux_loader.hh linux_loader.tcc

lib_LIBRARIES = liblinux_loader.a
liblinux_loader_a_SOURCES = linux32_loader.cc linux64_loader.cc
install-exec-hook: $(libdir)/liblinux_loader.a
	if test ! -f $(prefix)/lib/libunisim_service_loader_linux_loader.a; then (cd $(prefix)/lib && $(LN_S) $< libunisim_service_loader_linux_loader.a); fi
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_loader_linux_loader.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
