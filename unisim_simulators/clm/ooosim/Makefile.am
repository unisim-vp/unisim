sharedir = $(prefix)/share/unisim_simulators/clm/ooosim
INCLUDES=-I$(top_srcdir)/../../include -I$(top_builddir)/../..
bin_PROGRAMS = ooosim_1core ooosim_2core ooosim_4core ooosim_8core ooosim_16core ooosim_32core ooosim_64core ooosim_128core
#ooosim_SOURCES = main.cc
#ooosim_CPPFLAGS =

ooosim_1core_SOURCES = ooosim_cpu_emulator.cc cpu_simulator.cc main_1core.cc simulator.hh
#ooosim_1core_SOURCES = ooosim_cpu_emulator.cc main_1core.cc simulator.hh

ooosim_2core_SOURCES = ooosim_cpu_emulator.cc cpu_simulator.cc main_2core.cc simulator_2core.hh
#ooosim_2core_SOURCES = ooosim_cpu_emulator.cc main_2core.cc simulator_2core.hh

ooosim_4core_SOURCES = main_4core.cc ooosim_cpu_emulator.cc simulator_4core.hh
#ooosim_4core_SOURCES = main_4core.cc ooosim_cpu_emulator.cc simulator_4core.hh

ooosim_8core_SOURCES = main_8core.cc ooosim_cpu_emulator.cc simulator_8core.hh

ooosim_16core_SOURCES = main_16core.cc ooosim_cpu_emulator.cc simulator_16core.hh

ooosim_32core_SOURCES = main_32core.cc ooosim_cpu_emulator.cc simulator_32core.hh

ooosim_64core_SOURCES = main_64core.cc ooosim_cpu_emulator.cc simulator_64core.hh

ooosim_128core_SOURCES = main_128core.cc ooosim_cpu_emulator.cc simulator_128core.hh

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
