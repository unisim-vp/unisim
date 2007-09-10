includedir = $(prefix)/include/unisim/service/tee
libdir = $(prefix)/lib/unisim/service/tee
servicedir = $(prefix)/share/unisim/service/tee

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_memory_access_reporting
SUBDIRS += memory_access_reporting
INCLUDE_SUBDIRS += memory_access_reporting
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
	(rmdir $(servicedir) || true)
