INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/debug/inline_debugger
include_HEADERS = inline_debugger.hh inline_debugger.tcc

lib_LIBRARIES = libunisim_service_debug_inline_debugger.a
libunisim_service_debug_inline_debugger_a_SOURCES = inline_debugger.cc inline_debugger_32.cc inline_debugger_64.cc
uninstall-hook:
	(rmdir $(includedir) || true)
