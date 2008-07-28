sharedir = $(prefix)/share/unisim_simulators/clm/ooosim
INCLUDES=-I$(top_srcdir)/../../include -I$(top_builddir)/../..
bin_PROGRAMS = ooosim
ooosim_SOURCES = main.cc
ooosim_CPPFLAGS =

include_HEADERS = simulator.hh
noinst_HEADERS = simulator.hh

if COND_release
#bin_PROGRAMS += ooosim
#ooosim_SOURCES += main.cc
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
