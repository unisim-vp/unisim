includedir = $(prefix)/include/unisim/component/clm/utility
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/utility

#SUBDIRS=
#INCLUDE_SUBDIRS=

include_HEADERS = \
	common.hh \
	error.h \
	types.hh \
	utility.hh


dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
