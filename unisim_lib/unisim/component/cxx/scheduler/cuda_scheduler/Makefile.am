INCLUDES=-I$(top_srcdir)/../../../../..

lib_LTLIBRARIES = libunisim_component_cxx_scheduler_cuda_scheduler.la
libunisim_component_cxx_scheduler_cuda_scheduler_la_SOURCES = cuda_scheduler.cc

includedir = $(prefix)/include/unisim/component/cxx/scheduler/cuda_scheduler
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/scheduler/cuda_scheduler
include_HEADERS = cuda_scheduler.hh cuda_scheduler.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
