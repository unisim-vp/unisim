sharedir = $(prefix)/share/unisim_simulators/tlm/hcs12xemu
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..
bin_PROGRAMS = hcs12xemu
hcs12xemu_SOURCES = main.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
