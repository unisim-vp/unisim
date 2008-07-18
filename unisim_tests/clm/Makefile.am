sharedir = $(prefix)/share/unisim_tests/clm

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_kernel
SUBDIRS += kernel
INCLUDE_SUBDIRS += kernel
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)
