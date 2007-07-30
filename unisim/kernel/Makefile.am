includedir = $(prefix)/include/unisim/kernel
libdir= $(prefix)/lib/unisim/kernel
SUBDIRS=tlm service
INCLUDE_SUBDIRS=tlm service

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
