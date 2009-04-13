sharedir = $(prefix)/share/unisim_simulators/tlm2/str7
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..

bin_PROGRAMS = str7_verbose str7_debug str7_debug_inline

str7_verbose_SOURCES = main.cc
str7_verbose_CPPFLAGS = -DSTR7_VERBOSE

str7_debug_SOURCES = main.cc
str7_debug_CPPFLAGS = -DSTR7_DEBUG

str7_debug_inline_SOURCES = main.cc
str7_debug_inline_CPPFLAGS = -DSTR7_DEBUG_INLINE

if COND_release
bin_PROGRAMS += str7
str7_SOURCES = main.cc
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
