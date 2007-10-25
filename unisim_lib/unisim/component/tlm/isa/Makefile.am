includedir = $(prefix)/include/unisim/component/tlm/isa
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/isa

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_i8042
SUBDIRS += i8042
INCLUDE_SUBDIRS += i8042
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
