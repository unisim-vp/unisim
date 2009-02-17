sharedir = $(prefix)/share/unisim_simulators/cxx/tesla
INCLUDES=-I$(top_srcdir)/../../include -I$(top_builddir)/../..

lib_LTLIBRARIES = libunisimsimulator_cxx_tesla_tesla.la

libunisimsimulator_cxx_tesla_tesla_la_SOURCES = cuda.cc driver_objects.cc exception.cc fatformat.cc

#libunisimsimulator_cxx_tesla_tesla_la_LIBADD =  \
#	$(with_unisim_lib)/libunisim_component_cxx_processor_tesla.la \
#	$(with_unisim_lib)/libunisim_kernel_service.la \
#	$(with_unisim_lib)/libunisim_component_cxx_memory_ram.la

#with_unisim_lib = /home/scollang/usr/unisim/lib

include_HEADERS = cuda.h driver.hh driver_objects.hh module.hh module.tcc \
	driver.tcc config.hh kernel.hh kernel.tcc fatformat.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

