INCLUDES=-I$(top_srcdir)/../../../..
libdir = $(prefix)/lib/unisim/service/os/linux_os
lib_LIBRARIES = liblinux_os.a
liblinux_os_a_SOURCES = linux_os_32.cc linux_os_exception.cc

includedir = $(prefix)/include/unisim/service/os/linux_os
include_HEADERS = linux_os.hh linux_os.tcc linux_os_exception.hh

install-exec-hook: $(libdir)/liblinux_os.a
	if test ! -f $(prefix)/lib/libunisim_service_os_linux_os.a; then (cd $(prefix)/lib && $(LN_S) $< libunisim_service_os_linux_os.a); fi
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_os_linux_os.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
