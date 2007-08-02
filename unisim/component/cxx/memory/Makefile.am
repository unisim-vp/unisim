includedir = $(prefix)/include/unisim/component/cxx/memory
libdir= $(prefix)/lib/unisim/component/cxx/memory

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_ram
SUBDIRS += ram
INCLUDE_SUBDIRS += ram
endif

if COND_flash
SUBDIRS += flash
INCLUDE_SUBDIRS += flash
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
