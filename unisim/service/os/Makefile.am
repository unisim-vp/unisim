includedir = $(prefix)/include/unisim/service/os
libdir = $(prefix)/lib/unisim/service/os

SUBDIRS=linux_os
INCLUDE_SUBDIRS=linux_os

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
