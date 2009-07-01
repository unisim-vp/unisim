sharedir = $(prefix)/share/unisim_simulators/tlm2/str7
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..

bin_PROGRAMS = str7 

str7_SOURCES = main.cc router.cc router_config.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
