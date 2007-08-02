includedir = $(prefix)/include/unisim/component/c++/memory
libdir= $(prefix)/lib/unisim/component/c++/memory

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_ram
SUBDIRS += ram
INCLUDE_SUBDIRS += ram
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
