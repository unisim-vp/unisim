sharedir = $(prefix)/share/unisim_tools

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_genisslib
SUBDIRS += genisslib
INCLUDE_SUBDIRS += genisslib
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)


include m4/aminclude.am
