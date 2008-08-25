sharedir = $(prefix)/share/unisim_simulators/clm/ooosim
INCLUDES=-I$(top_srcdir)/../../include -I$(top_builddir)/../..
bin_PROGRAMS = ooosim_1core ooosim_2core
#ooosim_SOURCES = main.cc
#ooosim_CPPFLAGS =

ooosim_1core_SOURCES = main_1core.cc simulator_1core.hh
ooosim_1core_CXXFLAGS = -g3 -O3

ooosim_2core_SOURCES = main_2core.cc simulator_2core.hh
ooosim_2core_CXXFLAGS = -g3 -O3

ooosim_4core_SOURCES = main_4core.cc simulator_4core.hh
ooosim_4core_CXXFLAGS = -g3 -O3

ooosim_8core_SOURCES = main_8core.cc simulator_8core.hh
ooosim_8core_CXXFLAGS = -g3 -O3

ooosim_16core_SOURCES = main_16core.cc simulator_16core.hh
ooosim_16core_CXXFLAGS = -g3 -O3

ooosim_32core_SOURCES = main_32core.cc simulator_32core.hh
ooosim_32core_CXXFLAGS = -g3 -O3

#include_HEADERS = simulator.hh
#noinst_HEADERS  = simulator.hh

if COND_release
#bin_PROGRAMS += ooosim
#ooosim_SOURCES += main.cc
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
