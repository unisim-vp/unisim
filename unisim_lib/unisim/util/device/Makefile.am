includedir = $(prefix)/include/unisim/util/device
sharedir = $(prefix)/share/unisim_lib/unisim/util/device
include_HEADERS = register.hh register_map.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
