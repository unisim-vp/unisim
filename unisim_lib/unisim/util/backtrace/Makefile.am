INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/kernel/debug
sharedir = $(prefix)/share/unisim_lib/unisim/kernel/debug
include_HEADERS = debug.hh

lib_LTLIBRARIES = libunisim_kernel_debug.la
libunisim_kernel_debug_la_SOURCES = debug.cc

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
