INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/pci/video
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/pci/video
lib_LIBRARIES = libunisim_component_tlm_pci_video.a
libunisim_component_tlm_pci_video_a_SOURCES = display_32.cc display_64.cc
include_HEADERS = display.hh display.tcc

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
