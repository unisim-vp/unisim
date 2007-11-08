INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/kernel/service
sharedir = $(prefix)/share/unisim_lib/unisim/kernel/service
include_HEADERS = service.hh xml_helper.hh

lib_LIBRARIES = libunisim_kernel_service.a
libunisim_kernel_service_a_SOURCES = service.cc xml_helper.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
