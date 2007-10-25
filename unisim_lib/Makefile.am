sharedir = $(prefix)/share/unisim_lib
SUBDIRS = unisim
INCLUDE_SUBDIRS = unisim

share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

include m4/aminclude.am
