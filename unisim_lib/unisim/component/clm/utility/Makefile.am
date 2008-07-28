includedir = $(prefix)/include/unisim/component/clm/utility
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/utility

#SUBDIRS=
#INCLUDE_SUBDIRS=

include_HEADERS = \
	common.h \
	error.h \
	types.hh \
	utility.h


dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..