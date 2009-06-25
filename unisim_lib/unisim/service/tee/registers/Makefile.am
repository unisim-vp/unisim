INCLUDES=-I$(top_srcdir)/../../../..
lib_LTLIBRARIES = libunisim_service_tee_registers.la
libunisim_service_tee_registers_la_SOURCES = registers_tee.cc

includedir = $(prefix)/include/unisim/service/tee/registers
sharedir = $(prefix)/share/unisim_lib/unisim/service/tee/registers
include_HEADERS = registers_tee.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
