sharedir = $(prefix)/share/unisim_tests/tlm2/bus/simple_router
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..
bin_PROGRAMS = simple_router_test

simple_router_test_SOURCES = main.cc router_test.hh
simple_router_test_CPPFLAGS =

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
