includedir = $(prefix)/include/unisim/component/c++
libdir= $(prefix)/lib/unisim/component/c++

SUBDIRS=memory cache tlb processor
INCLUDE_SUBDIRS=memory cache tlb processor

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
