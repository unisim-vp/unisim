includedir = $(prefix)/include/unisim/util/hash_table
sharedir = $(prefix)/share/unisim_lib/unisim/util/hash_table
include_HEADERS = hash_table.hh

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
	