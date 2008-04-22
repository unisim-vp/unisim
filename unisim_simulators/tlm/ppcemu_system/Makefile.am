sharedir = $(prefix)/share/unisim_simulators/tlm/ppcemu_system
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..
bin_PROGRAMS = ppcemu-system-debug

if COND_release
bin_PROGRAMS += ppcemu-system
ppcemu_system_SOURCES = main.cc
endif

ppcemu_system_debug_SOURCES = main.cc
ppcemu_system_debug_CPPFLAGS = -DDEBUG_PPCEMU_SYSTEM

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
