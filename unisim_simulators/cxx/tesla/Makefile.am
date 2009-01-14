sharedir = $(prefix)/share/unisim_simulators/cxx/tesla
INCLUDES=-I$(top_srcdir)/../../include -I$(top_builddir)/../..

lib_LIBRARIES = libunisimsimulator_cxx_tesla_tesla.a
libunisimsimulator_cxx_tesla_tesla_a_SOURCES = cuda.cc driver_objects.cc exception.cc

libunisimsimulator_cxx_tesla_tesla_a_LIBADD =  \
	$(with_unisim_lib)/libunisim_component_cxx_processor_tesla.a \
	$(with_unisim_lib)/libunisim_kernel_service.a \
	$(with_unisim_lib)/libunisim_component_cxx_memory_ram.a

with_unisim_lib = /home/scollang/usr/unisim/lib
#libunisimsimulator_cxx_tesla_tesla_a_ARFLAGS = -rpath '$(with_unisim_lib)'

#if COND_release
#libunisim_component_cxx_processor_powerpc_a_SOURCES += mpc755.cc mpc7447a.cc
#endif

include_HEADERS = cuda.h driver.hh driver_objects.hh module.hh module.tcc \
	driver.tcc config.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
