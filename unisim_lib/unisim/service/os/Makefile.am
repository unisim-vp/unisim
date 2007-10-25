includedir = $(prefix)/include/unisim/service/os
sharedir = $(prefix)/share/unisim_lib/unisim/service/os

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_linux_os
SUBDIRS += linux_os
INCLUDE_SUBDIRS += linux_os
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
