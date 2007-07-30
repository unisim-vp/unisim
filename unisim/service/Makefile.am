includedir = $(prefix)/include/unisim/service
libdir = $(prefix)/lib/unisim/service
SUBDIRS=interfaces logger debug
INCLUDE_SUBDIRS=interfaces logger debug

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
