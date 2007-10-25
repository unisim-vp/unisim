includedir = $(prefix)/include/unisim/component/cxx/tlb
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/tlb
include_HEADERS = tlb.hh
uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

share_DATA = COPYING

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
