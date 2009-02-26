INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/pci/video
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/pci/video
lib_LTLIBRARIES = libunisim_component_cxx_pci_video.la
libunisim_component_cxx_pci_video_la_SOURCES = display_32.cc display_64.cc
include_HEADERS = display.hh display.tcc

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
