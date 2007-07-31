includedir = $(prefix)/include/unisim/component/c++
libdir= $(prefix)/lib/unisim/component/c++

SUBDIRS=memory
INCLUDE_SUBDIRS=memory

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
