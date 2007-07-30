INCLUDES = -I$(top_srcdir)/../../../..
libdir = $(prefix)/lib/unisim/service/debug/inline_debugger
includedir = $(prefix)/include/unisim/service/debug/inline_debugger
include_HEADERS = inline_debugger.hh inline_debugger.tcc

lib_LIBRARIES = libinline_debugger.a
libinline_debugger_a_SOURCES = inline_debugger_32.cc inline_debugger_64.cc
install-exec-hook: $(libdir)/libinline_debugger.a
	if test ! -f $(prefix)/lib/libunisim_service_debug_inline_debugger.a; then (cd $(prefix)/lib && $(LN_S) $< libunisim_service_debug_inline_debugger.a); fi
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_debug_inline_debugger.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
