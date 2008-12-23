INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/kernel/debug
sharedir = $(prefix)/share/unisim_lib/unisim/kernel/debug
include_HEADERS = debug.hh

lib_LIBRARIES = libunisim_kernel_debug.a
libunisim_kernel_debug_a_SOURCES = debug.cc

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
