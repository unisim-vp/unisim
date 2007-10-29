sharedir = $(prefix)/share/unisim_simulators/tlm/armemu
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..
bin_PROGRAMS = armemu-debug armemu-nocache-debug

armemu_debug_SOURCES = main.cc
armemu_debug_CPPFLAGS = -DARMEMU_DEBUG
armemu_nocache_debug_SOURCES = main.cc
armemu_nocache_debug_CPPFLAGS = -DARMEMU_DEBUG -DARMEMU_NOCACHE

if COND_release
bin_PROGRAMS += armemu armemu-nocache
armemu_SOURCES = main.cc
armemu_nocache_SOURCES = main.cc
armemu_nocache_CPPFLAGS = -DARMEMU_NOCACHE
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
