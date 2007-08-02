INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/cxx/pci/video
includedir = $(prefix)/include/unisim/component/cxx/pci/video
lib_LIBRARIES = libvideo.a
libvideo_a_SOURCES = display_32.cc display_64.cc
include_HEADERS = display.hh display.tcc

install-exec-hook: $(libdir)/libvideo.a
	rm -f $(prefix)/lib/libunisim_component_cxx_pci_video.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_pci_video.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_pci_video.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
