includedir = $(prefix)/include/unisim/component/tlm/isa
libdir= $(prefix)/lib/unisim/component/tlm/isa

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_i8042
SUBDIRS += i8042
INCLUDE_SUBDIRS += i8042
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
