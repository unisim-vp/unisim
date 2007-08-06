includedir = $(prefix)/include/unisim/component/tlm/chipset
libdir= $(prefix)/lib/unisim/component/tlm/chipset

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_mpc107
SUBDIRS += mpc107
INCLUDE_SUBDIRS += mpc107
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
