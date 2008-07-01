sharedir = $(prefix)/share/unisim_simulators/tlm2/simple_bus_test
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..
bin_PROGRAMS = bus_test

bus_test_SOURCES = main.cc bus_test.hh
bus_test_CPPFLAGS =

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
