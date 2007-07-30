includedir = $(prefix)/include/unisim
libdir= $(prefix)/lib/unisim
SUBDIRS=kernel util service
INCLUDE_SUBDIRS=kernel util service

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
