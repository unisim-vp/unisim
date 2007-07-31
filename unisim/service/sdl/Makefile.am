INCLUDES=-I$(top_srcdir)/../../..
libdir = $(prefix)/lib/unisim/service/sdl
lib_LIBRARIES = libsdl.a
libsdl_a_SOURCES = sdl.cc

includedir = $(prefix)/include/unisim/service/sdl
include_HEADERS = sdl.hh sdl.tcc

install-exec-hook: $(libdir)/libsdl.a
	rm -f $(prefix)/lib/libunisim_service_sdl.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_service_sdl.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_sdl.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
