includedir = $(prefix)/include/unisim/component/tlm/processor

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_arm
SUBDIRS += arm
INCLUDE_SUBDIRS += arm
endif

if COND_powerpc
SUBDIRS += powerpc
INCLUDE_SUBDIRS += powerpc
endif

uninstall-hook:
	(rmdir $(includedir) || true)
