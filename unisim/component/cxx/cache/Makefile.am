includedir = $(prefix)/include/unisim/component/c++/cache
libdir= $(prefix)/lib/unisim/component/c++/cache
include_HEADERS = cache.hh cache.tcc

SUBDIRS=mesi insn
INCLUDE_SUBDIRS=mesi insn

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
