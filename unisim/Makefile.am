includedir = $(prefix)/include/unisim
libdir= $(prefix)/lib/unisim
SUBDIRS=kernel util service build_tool component
INCLUDE_SUBDIRS=kernel util service build_tool component

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
