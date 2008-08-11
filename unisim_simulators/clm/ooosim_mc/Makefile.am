sharedir = $(prefix)/share/unisim_simulators/clm/ooosim
INCLUDES=-I$(top_srcdir)/../../include -I$(top_builddir)/../..
bin_PROGRAMS = ooosim_mc_2core ooosim_mc_4core ooosim_mc_8core #ooosim_mc_16core ooosim_mc_32core

ooosim_mc_2core_SOURCES = main_mc_2core.cc simulator_mc_2core.hh
ooosim_mc_2core_CPPFLAGS =

ooosim_mc_4core_SOURCES = main_mc_4core.cc simulator_mc_4core.hh
ooosim_mc_4core_CPPFLAGS =

ooosim_mc_8core_SOURCES = main_mc_8core.cc simulator_mc_8core.hh
ooosim_mc_8core_CPPFLAGS =

#ooosim_mc_16core_SOURCES = main_mc_16core.cc simulator_mc_16core.hh
#ooosim_mc_16core_CPPFLAGS =

#ooosim_mc_32core_SOURCES = main_mc_32core.cc simulator_mc_32core.hh
#ooosim_mc_32core_CPPFLAGS =

#include_HEADERS = simulator.hh
#noinst_HEADERS = simulator.hh

if COND_release
#bin_PROGRAMS += ooosim
#ooosim_SOURCES += main.cc
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
