includedir = $(prefix)/include/unisim/component/tlm/memory
libdir= $(prefix)/lib/unisim/component/tlm/memory

SUBDIRS=ram
INCLUDE_SUBDIRS=ram

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
