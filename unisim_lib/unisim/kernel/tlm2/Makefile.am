includedir = $(prefix)/include/unisim/kernel/tlm2
sharedir = $(prefix)/share/unisim_lib/unisim/kernel/tlm2
include_HEADERS = tlm.hh

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
