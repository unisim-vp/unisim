INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/loader/pmac_linux_kernel_loader
sharedir = $(prefix)/share/unisim_lib/unisim/service/loader/pmac_linux_kernel_loader
include_HEADERS = pmac_linux_kernel_loader.hh

lib_LIBRARIES = libunisim_service_loader_pmac_linux_kernel_loader.a
libunisim_service_loader_pmac_linux_kernel_loader_a_SOURCES = pmac_linux_kernel_loader.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
