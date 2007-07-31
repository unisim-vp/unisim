includedir = $(prefix)/include/unisim/component/tlm
libdir= $(prefix)/lib/unisim/component/tlm

SUBDIRS=message debug memory
INCLUDE_SUBDIRS=message debug memory

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
