INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/loader/coff_loader
sharedir = $(prefix)/share/unisim_lib/unisim/service/loader/coff_loader
include_HEADERS = coff_loader.hh coff_loader.tcc
nobase_include_HEADERS = ti/ti.hh ti/ti.tcc

lib_LTLIBRARIES = libunisim_service_loader_coff_loader.la
libunisim_service_loader_coff_loader_la_SOURCES = coff_loader.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
	