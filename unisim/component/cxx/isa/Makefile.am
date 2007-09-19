includedir = $(prefix)/include/unisim/component/cxx/isa
include_HEADERS = types.hh

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_i8042
SUBDIRS += i8042
INCLUDE_SUBDIRS += i8042
endif

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
