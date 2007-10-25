includedir = $(prefix)/include/unisim/util/simfloat
sharedir = $(prefix)/share/unisim_lib/unisim/util/simfloat
include_HEADERS = floating_gccopt.hh floating.hh integer_gccopt.hh integer.hh floating.tcc integer.tcc

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
