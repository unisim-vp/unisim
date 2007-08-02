includedir = $(prefix)/include/unisim/component/cxx/processor
libdir= $(prefix)/lib/unisim/component/cxx/processor

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_powerpc
SUBDIRS += powerpc
INCLUDE_SUBDIRS += powerpc
endif

if COND_arm
SUBDIRS += arm
INCLUDE_SUBDIRS += arm
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
