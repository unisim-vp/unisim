INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/kernel/service
sharedir = $(prefix)/share/unisim_lib/unisim/kernel/service
include_HEADERS = service.hh xml_helper.hh

lib_LTLIBRARIES = libunisim_kernel_service.la
libunisim_kernel_service_la_SOURCES = service.cc xml_helper.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
