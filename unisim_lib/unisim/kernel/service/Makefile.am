INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/kernel/service
sharedir = $(prefix)/share/unisim_lib/unisim/kernel/service
include_HEADERS = service.hh

lib_LIBRARIES = libunisim_kernel_service.a
libunisim_kernel_service_a_SOURCES = service.cc

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
