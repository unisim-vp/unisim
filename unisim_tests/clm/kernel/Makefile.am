sharedir = $(prefix)/share/unisim_test/clm/kernel
INCLUDES=-I$(top_srcdir)/../.. -I$(top_builddir)/../..
bin_PROGRAMS = test_mconf test_sconf

test_mconf_SOURCES = main_mconf.cc simulator_mconf.hh
test_mconf_CPPFLAGS =

test_sconf_SOURCES = main_sconf.cc simulator_sconf.hh
test_sconf_CPPFLAGS =

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
