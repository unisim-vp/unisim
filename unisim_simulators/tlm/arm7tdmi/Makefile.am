sharedir = $(prefix)/share/unisim_simulators/tlm/arm7tdmi
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..

bin_PROGRAMS = arm7tdmi-debug

arm7tdmi_debug_SOURCES = main.cc
arm7tdmi_debug_CPPFLAGS = -DARM7TDMI_DEBUG

if COND_release
bin_PROGRAMS += arm7tdmi
arm7tdmi_SOURCES = main.cc
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
