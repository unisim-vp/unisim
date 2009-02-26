INCLUDES=-I$(top_srcdir)/../../..
lib_LTLIBRARIES = libunisim_util_xml.la
libunisim_util_xml_la_SOURCES = xml.cc

includedir = $(prefix)/include/unisim/util/xml
sharedir = $(prefix)/share/unisim_lib/unisim/util/xml
include_HEADERS = xml.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
