INCLUDES=-I$(top_srcdir)/../../..
lib_LIBRARIES = libunisim_service_sdl.a
libunisim_service_sdl_a_SOURCES = sdl.cc

includedir = $(prefix)/include/unisim/service/sdl
include_HEADERS = sdl.hh sdl.tcc

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
