INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/util/debug
sharedir = $(prefix)/share/unisim_lib/unisim/util/debug
include_HEADERS = breakpoint.hh breakpoint_registry.hh netstub.hh watchpoint.hh watchpoint_registry.hh \
                  breakpoint_registry.tcc netstub.tcc watchpoint_registry.tcc register.hh simple_register.hh symbol.hh \
                  symbol_table.hh symbol_table.tcc symbol.tcc profile.hh profile.tcc

lib_LTLIBRARIES = libunisim_util_debug.la
libunisim_util_debug_la_SOURCES = breakpoint_registry_32.cc breakpoint_registry_64.cc watchpoint_registry_32.cc \
                                  watchpoint_registry_64.cc netstub.cc symbol_table_32.cc symbol_table_64.cc \
                                  symbol_32.cc symbol_64.cc profile_32.cc profile_64.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
