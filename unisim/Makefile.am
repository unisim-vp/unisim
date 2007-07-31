includedir = $(prefix)/include/unisim
libdir= $(prefix)/lib/unisim
SUBDIRS=kernel util service component
INCLUDE_SUBDIRS=kernel util service component

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
