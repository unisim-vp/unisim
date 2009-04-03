sharedir = $(prefix)/share/unisim_simulators/clm/ooosim
INCLUDES=-I$(top_srcdir)/../../include -I$(top_builddir)/../..
bin_PROGRAMS = ooosim_1core ooosim_ideal_d1 ooosim_ideal_d2 ooosim_ideal_d4 
# ooosim_2core 
#ooosim_4core ooosim_8core ooosim_16core ooosim_32core ooosim_64core ooosim_128core
#ooosim_SOURCES = main.cc
#ooosim_CPPFLAGS =

#ooosim_1core_SOURCES = ooosim_cpu_emulator.cc cpu_simulator.cc main_1core.cc simulator.hh
ooosim_1core_SOURCES = ooosim_cpu_emulator.cc main_1core.cc simulator.hh
#ooosim_1core_SOURCES = ooosim_cpu_emulator.cc main_1core.cc simulator.hh

#ooosim_2core_SOURCES = ooosim_cpu_emulator.cc cpu_simulator.cc main_2core.cc simulator_2core.hh
#ooosim_2core_SOURCES = ooosim_cpu_emulator.cc main_2core.cc simulator_2core.hh

#ooosim_4core_SOURCES = ooosim_cpu_emulator.cc cpu_simulator.cc main_4core.cc simulator_4core.hh
#ooosim_4core_SOURCES = main_4core.cc ooosim_cpu_emulator.cc simulator_4core.hh

#ooosim_8core_SOURCES = ooosim_cpu_emulator.cc cpu_simulator.cc main_8core.cc simulator_8core.hh

#ooosim_16core_SOURCES = ooosim_cpu_emulator.cc cpu_simulator.cc main_16core.cc simulator_16core.hh

#ooosim_32core_SOURCES = ooosim_cpu_emulator.cc cpu_simulator.cc main_32core.cc simulator_32core.hh

#ooosim_64core_SOURCES = ooosim_cpu_emulator.cc cpu_simulator.cc main_64core.cc simulator_64core.hh

#ooosim_128core_SOURCES = ooosim_cpu_emulator.cc cpu_simulator.cc main_128core.cc simulator_128core.hh

#include_HEADERS = simulator.hh
#noinst_HEADERS  = simulator.hh

#ooosim_ideal_SOURCES = ooosim_cpu_emulator.cc ooosim_ideal.cc simulator_ideal.hh
ooosim_ideal_d1_SOURCES = ooosim_cpu_emulator.cc ooosim_ideal_d1.cc simulator_ideal.hh
ooosim_ideal_d2_SOURCES = ooosim_cpu_emulator.cc ooosim_ideal_d2.cc simulator_ideal.hh
ooosim_ideal_d4_SOURCES = ooosim_cpu_emulator.cc ooosim_ideal_d4.cc simulator_ideal.hh
#ooosim_ideal_d8_SOURCES = ooosim_cpu_emulator.cc ooosim_ideal_d8.cc simulator_ideal.hh
#ooosim_ideal_d16_SOURCES = ooosim_cpu_emulator.cc ooosim_ideal_d16.cc simulator_ideal.hh
#ooosim_ideal_d32_SOURCES = ooosim_cpu_emulator.cc ooosim_ideal_d32.cc simulator_ideal.hh

if COND_release
#bin_PROGRAMS += ooosim
#ooosim_SOURCES += main.cc
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
