includedir = $(prefix)/include/unisim/component/cxx/memory/flash
libdir= $(prefix)/lib/unisim/component/cxx/memory/flash

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_am29lv
SUBDIRS += am29lv
INCLUDE_SUBDIRS += am29lv
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
