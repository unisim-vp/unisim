includedir = $(prefix)/include/unisim/util/simfloat
include_HEADERS = floating_gccopt.hh floating.hh integer_gccopt.hh integer.hh floating.tcc integer.tcc

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
