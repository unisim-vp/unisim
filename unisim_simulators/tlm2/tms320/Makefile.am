sharedir = $(prefix)/share/unisim_simulators/tlm2/tms320
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..

bin_PROGRAMS = tms320-debug

tms320_debug_SOURCES = main.cc
tms320_debug_CPPFLAGS = -DARM7TDMI_DEBUG

if COND_release
bin_PROGRAMS += tms320
tms320_SOURCES = main.cc
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
