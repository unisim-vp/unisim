INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/loader/s19_loader
sharedir = $(prefix)/share/unisim_lib/unisim/service/loader/elf_loader
include_HEADERS = s19_loader.hh s19_loader.tcc

lib_LIBRARIES = libunisim_service_loader_s19_loader.a
libunisim_service_loader_s19_loader_a_SOURCES = s19_loader.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
	
