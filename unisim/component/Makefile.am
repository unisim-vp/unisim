includedir = $(prefix)/include/unisim/component
libdir= $(prefix)/lib/unisim/component

SUBDIRS=c++ tlm
INCLUDE_SUBDIRS=c++ tlm

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
