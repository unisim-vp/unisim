includedir = $(prefix)/include/unisim/service/os

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_linux_os
SUBDIRS += linux_os
INCLUDE_SUBDIRS += linux_os
endif

uninstall-hook:
	(rmdir $(includedir) || true)
