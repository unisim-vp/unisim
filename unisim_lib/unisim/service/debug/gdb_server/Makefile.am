INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/debug/gdb_server
sharedir = $(prefix)/share/unisim_lib/unisim/service/debug/gdb_server
include_HEADERS = gdb_server.hh gdb_server.tcc

lib_LTLIBRARIES = libunisim_service_debug_gdb_server.la
libunisim_service_debug_gdb_server_la_SOURCES = gdb_server.cc gdb_server_32.cc gdb_server_64.cc

dist_share_DATA = COPYING gdb_powerpc.xml gdb_armv4l.xml gdb_armv5b.xml

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
