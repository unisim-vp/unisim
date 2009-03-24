sharedir = $(prefix)/share/unisim_simulators/tlm2/s12x
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..
bin_PROGRAMS = s12xemu
s12xemu_SOURCES = main.cc rtb_unisim.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
