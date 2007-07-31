includedir = $(prefix)/include/unisim/component/c++
libdir= $(prefix)/lib/unisim/component/c++

SUBDIRS=memory cache
INCLUDE_SUBDIRS=memory cache

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
