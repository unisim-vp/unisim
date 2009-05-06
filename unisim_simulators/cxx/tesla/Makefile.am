sharedir = $(prefix)/share/unisim_simulators/cxx/tesla
INCLUDES=-I$(top_srcdir)/../../include -I$(top_builddir)/../..

lib_LTLIBRARIES = libunisimsimulator_cxx_tesla_tesla.la

libunisimsimulator_cxx_tesla_tesla_la_SOURCES = cuda.cc driver_objects.cc exception.cc fatformat.cc
libunisimsimulator_cxx_tesla_tesla_la_LDFLAGS = -version-info 0:1:0

include_HEADERS = device.hh exception.hh device.tcc cuda.h driver.hh driver_objects.hh module.hh module.tcc \
	driver.tcc config.hh kernel.hh kernel.tcc fatformat.hh

dist_share_DATA = COPYING README

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

