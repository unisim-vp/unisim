INCLUDES=-I$(top_srcdir)/../../../..
lib_LTLIBRARIES = libunisim_service_os_ti_c_io.la
libunisim_service_os_ti_c_io_la_SOURCES = ti_c_io_32.cc ti_c_io_64.cc

includedir = $(prefix)/include/unisim/service/os/ti_c_io
sharedir = $(prefix)/share/unisim_lib/unisim/service/os/ti_c_io
include_HEADERS = ti_c_io.hh ti_c_io.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
