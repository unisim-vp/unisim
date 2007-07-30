INCLUDES=-I$(top_srcdir)/../../..
libdir = $(prefix)/lib/unisim/util/xml
lib_LIBRARIES = libxml.a
libxml_a_SOURCES = xml.cc

includedir = $(prefix)/include/unisim/util/xml
include_HEADERS = xml.hh

install-exec-hook: $(libdir)/libxml.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_util_xml.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_util_xml.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
