includedir = $(prefix)/include/unisim/component
libdir= $(prefix)/lib/unisim/component

SUBDIRS=c++
INCLUDE_SUBDIRS=c++

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
