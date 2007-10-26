sharedir = $(prefix)/share/unisim_simulators/tlm/armemu-nocache
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..
bin_PROGRAMS = armemu-nocache
armemu_nocache_SOURCES = main.cc

share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
