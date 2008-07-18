sharedir = $(prefix)/share/unisim_test/clm/kernel
INCLUDES=-I$(top_srcdir)/../.. -I$(top_builddir)/../..
bin_PROGRAMS = sstest
sstest_SOURCES = main.cc simulator.hh
sstest_CPPFLAGS =

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
