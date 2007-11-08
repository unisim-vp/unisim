includedir = $(prefix)/include/unisim/component/cxx/isa
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/isa
include_HEADERS = types.hh

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_i8042
SUBDIRS += i8042
INCLUDE_SUBDIRS += i8042
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
