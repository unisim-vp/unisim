sharedir = $(prefix)/share/unisim_simulators/tlm/ppcemu
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..
bin_PROGRAMS = ppcemu-debug
ppcemu_debug_SOURCES = main.cc
ppcemu_debug_CPPFLAGS = -DDEBUG_PPCEMU

if COND_release
bin_PROGRAMS += ppcemu
ppcemu_SOURCES = main.cc
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
