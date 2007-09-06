INCLUDES = -I$(top_srcdir)/../../..
libdir = $(prefix)/lib/unisim/util/endian
includedir = $(prefix)/include/unisim/util/endian
include_HEADERS = endian.hh

lib_LIBRARIES = libendian.a
libendian_a_SOURCES = endian.cc

install-exec-hook: $(libdir)/libendian.a
	rm -f $(prefix)/lib/libunisim_util_endian.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_util_endian.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_util_endian.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
