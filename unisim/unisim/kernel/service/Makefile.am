INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/kernel/service
include_HEADERS = service.hh

lib_LIBRARIES = libunisim_kernel_service.a
libunisim_kernel_service_a_SOURCES = service.cc

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
