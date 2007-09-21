includedir = $(prefix)/include/unisim/component/cxx/chipset

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_mpc107
SUBDIRS += mpc107
INCLUDE_SUBDIRS += mpc107
endif

uninstall-hook:
	(rmdir $(includedir) || true)
