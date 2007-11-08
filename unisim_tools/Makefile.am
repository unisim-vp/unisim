sharedir = $(prefix)/share/unisim_tools

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_genisslib
SUBDIRS += genisslib
INCLUDE_SUBDIRS += genisslib
endif

if COND_mkinitrd
SUBDIRS += mkinitrd
INCLUDE_SUBDIRS += mkinitrd
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)


include m4/aminclude.am
