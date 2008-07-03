sharedir = $(prefix)/share/unisim_tests/tlm2/bus/generic_bus
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..
bin_PROGRAMS = generic_bus_test

generic_bus_test_SOURCES = main.cc bus_test.hh
generic_bus_test_CPPFLAGS =

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
