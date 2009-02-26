INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/loader/linux_loader
sharedir = $(prefix)/share/unisim_lib/unisim/service/loader/linux_loader
include_HEADERS = linux_loader.hh linux_loader.tcc

lib_LTLIBRARIES = libunisim_service_loader_linux_loader.la
libunisim_service_loader_linux_loader_la_SOURCES = linux32_loader.cc linux64_loader.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
