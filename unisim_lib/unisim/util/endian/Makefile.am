INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/util/endian
include_HEADERS = endian.hh

lib_LIBRARIES = libunisim_util_endian.a
libunisim_util_endian_a_SOURCES = endian.cc

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
	