sharedir = $(prefix)/share/unisim_simulators/clm/ooosim
INCLUDES=-I$(top_srcdir)/../../include -I$(top_builddir)/../..
bin_PROGRAMS = ooosim_mc_2core ooosim_mc_4core ooosim_mc_8core ooosim_mc_16core ooosim_mc_32core ooosim_mc_64core ooosim_mc_128core

ooosim_mc_2core_SOURCES = main_mc_2core.cc ooosim_mc_cpu_emulator.cc simulator_mc_2core.hh
#ooosim_mc_2core_SOURCES = ooosim_mc_cpu_emulator.cc cpu_simulator_mc.cc main_mc_2core.cc simulator_mc_2core.hh

ooosim_mc_4core_SOURCES = main_mc_4core.cc ooosim_mc_cpu_emulator.cc simulator_mc_4core.hh

ooosim_mc_8core_SOURCES = main_mc_8core.cc ooosim_mc_cpu_emulator.cc simulator_mc_8core.hh

ooosim_mc_16core_SOURCES = main_mc_16core.cc ooosim_mc_cpu_emulator.cc simulator_mc_16core.hh

ooosim_mc_32core_SOURCES = main_mc_32core.cc ooosim_mc_cpu_emulator.cc simulator_mc_32core.hh

ooosim_mc_64core_SOURCES = main_mc_64core.cc ooosim_mc_cpu_emulator.cc simulator_mc_64core.hh

ooosim_mc_128core_SOURCES = main_mc_128core.cc ooosim_mc_cpu_emulator.cc simulator_mc_128core.hh

include_HEADERS = \
simulator.hh \
simulator_mc_128core.hh \
simulator_mc_16core.hh \
simulator_mc_2core.hh \
simulator_mc_32core.hh \
simulator_mc_4core.hh \
simulator_mc_64core.hh \
simulator_mc_8core.hh

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
