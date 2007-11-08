INCLUDES=-I$(top_srcdir)/../../..
lib_LIBRARIES = libunisim_service_sdl.a
libunisim_service_sdl_a_SOURCES = sdl.cc

includedir = $(prefix)/include/unisim/service/sdl
sharedir = $(prefix)/share/unisim_lib/unisim/service/sdl
include_HEADERS = sdl.hh sdl.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
