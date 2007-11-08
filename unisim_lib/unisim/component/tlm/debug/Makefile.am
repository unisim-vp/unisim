includedir = $(prefix)/include/unisim/component/tlm/debug
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/debug
include_HEADERS = transaction_spy.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
