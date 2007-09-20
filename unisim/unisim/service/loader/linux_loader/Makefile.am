INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/loader/linux_loader
include_HEADERS = linux_loader.hh linux_loader.tcc

lib_LIBRARIES = libunisim_service_loader_linux_loader.a
libunisim_service_loader_linux_loader_a_SOURCES = linux32_loader.cc linux64_loader.cc
uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
