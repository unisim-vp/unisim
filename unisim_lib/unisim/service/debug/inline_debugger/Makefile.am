INCLUDES = -I$(top_srcdir)/../../../.. -I$(top_builddir)/../../../..
includedir = $(prefix)/include/unisim/service/debug/inline_debugger
sharedir = $(prefix)/share/unisim_lib/unisim/service/debug/inline_debugger
include_HEADERS = inline_debugger.hh inline_debugger.tcc

lib_LTLIBRARIES = libunisim_service_debug_inline_debugger.la
libunisim_service_debug_inline_debugger_la_SOURCES = inline_debugger.cc inline_debugger_32.cc inline_debugger_64.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
	
