includedir = $(prefix)/include/unisim/service
libdir = $(prefix)/lib/unisim/service

SUBDIRS=interfaces logger os debug loader sdl
INCLUDE_SUBDIRS=interfaces logger os debug loader sdl

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
