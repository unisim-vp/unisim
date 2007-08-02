includedir = $(prefix)/include/unisim/component/cxx/isa
libdir= $(prefix)/lib/unisim/component/cxx/isa

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_i8042
SUBDIRS += i8042
INCLUDE_SUBDIRS += i8042
endif

uninstall-hook:
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
