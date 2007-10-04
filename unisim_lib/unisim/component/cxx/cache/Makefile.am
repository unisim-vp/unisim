includedir = $(prefix)/include/unisim/component/cxx/cache
include_HEADERS = cache.hh cache.tcc cache_interface.hh

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

# if COND_arm
# SUBDIRS += arm
# INCLUDE_SUBDIRS += arm
# endif

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
