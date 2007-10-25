INCLUDES=-I$(top_srcdir)/../../..
lib_LIBRARIES = libunisim_util_xml.a
libunisim_util_xml_a_SOURCES = xml.cc

includedir = $(prefix)/include/unisim/util/xml
sharedir = $(prefix)/share/unisim_lib/unisim/util/xml
include_HEADERS = xml.hh

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
