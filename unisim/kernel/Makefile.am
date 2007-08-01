includedir = $(prefix)/include/unisim/kernel
libdir= $(prefix)/lib/unisim/kernel

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_tlm
SUBDIRS += tlm
INCLUDE_SUBDIRS += tlm
endif

if COND_service
SUBDIRS += service
INCLUDE_SUBDIRS += service
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
