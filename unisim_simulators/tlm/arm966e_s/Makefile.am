sharedir = $(prefix)/share/unisim_simulators/tlm/arm966e_s
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..

bin_PROGRAMS = arm966e_s-debug

arm966e_s_debug_SOURCES = main.cc
arm966e_s_debug_CPPFLAGS = -DARM966E_S_DEBUG

if COND_release
bin_PROGRAMS += arm966e_s
arm966e_s_SOURCES = main.cc
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
