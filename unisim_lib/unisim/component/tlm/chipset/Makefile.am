includedir = $(prefix)/include/unisim/component/tlm/chipset
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/chipset

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_mpc107
SUBDIRS += mpc107
INCLUDE_SUBDIRS += mpc107
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
