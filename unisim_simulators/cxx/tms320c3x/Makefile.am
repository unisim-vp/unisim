sharedir = $(prefix)/share/unisim_simulators/cxx/tms320c3x
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..

bin_PROGRAMS = tms320c3x-debug

tms320c3x_debug_SOURCES = main.cc
tms320c3x_debug_CPPFLAGS = -DTMS320C3X_DEBUG

if COND_release
bin_PROGRAMS += tms320c3x
tms320c3x_SOURCES = main.cc
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
