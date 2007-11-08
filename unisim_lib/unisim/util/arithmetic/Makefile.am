includedir = $(prefix)/include/unisim/util/arithmetic
sharedir = $(prefix)/share/unisim_lib/unisim/util/arithmetic
include_HEADERS = arithmetic.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
	