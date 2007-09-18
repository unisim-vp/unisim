INCLUDES=-I$(top_srcdir)/../../..
lib_LIBRARIES = libunisim_util_xml.a
libunisim_util_xml_a_SOURCES = xml.cc

includedir = $(prefix)/include/unisim/util/xml
include_HEADERS = xml.hh

uninstall-hook:
	(rmdir $(includedir) || true)
