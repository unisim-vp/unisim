includedir = $(prefix)/include/unisim/component/c++/memory
libdir= $(prefix)/lib/unisim/component/c++/memory

SUBDIRS=ram
INCLUDE_SUBDIRS=ram

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
