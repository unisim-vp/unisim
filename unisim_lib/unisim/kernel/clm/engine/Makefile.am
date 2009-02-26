INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/kernel/clm/engine
sharedir = $(prefix)/share/unisim_lib/unisim/kernel/clm/engine
include_HEADERS = fsc.h simulated_pthread.h unisim_commandline.h unisim_generation.h unisim_graph.h unisim.h unisim_inttypes.h unisim_list.h unisim_lse.h unisim_parameters.h unisim_signal.h unisim_string.h

lib_LTLIBRARIES = libunisim_kernel_clm_engine.la
libunisim_kernel_clm_engine_la_SOURCES = fsc.cpp unisim_commandline.cpp unisim.cpp unisim_module.cpp unisim_parameters.cpp unisim_port.cpp unisim_simulator.cpp unisim_string.cpp

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../
include $(top_srcdir)/../../../../m4/unisim.am
