includedir = $(prefix)/include/unisim/component/cxx/cache
libdir= $(prefix)/lib/unisim/component/cxx/cache
include_HEADERS = cache.hh cache.tcc

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_mesi
SUBDIRS += mesi
INCLUDE_SUBDIRS += mesi
endif

if COND_insn
SUBDIRS += insn
INCLUDE_SUBDIRS += insn
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
