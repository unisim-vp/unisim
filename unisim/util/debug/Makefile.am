INCLUDES = -I$(top_srcdir)/../../..
libdir = $(prefix)/lib/unisim/util/debug
includedir = $(prefix)/include/unisim/util/debug
include_HEADERS = breakpoint.hh breakpoint_registry.hh netstub.hh watchpoint.hh watchpoint_registry.hh breakpoint_registry.tcc netstub.tcc watchpoint_registry.tcc register.hh simple_register.hh symbol.hh

lib_LIBRARIES = libdebug.a
libdebug_a_SOURCES = breakpoint_registry_32.cc breakpoint_registry_64.cc watchpoint_registry_32.cc watchpoint_registry_64.cc
install-exec-hook: $(libdir)/libdebug.a
	rm -f $(prefix)/lib/libunisim_util_debug.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_util_debug.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_util_debug.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
