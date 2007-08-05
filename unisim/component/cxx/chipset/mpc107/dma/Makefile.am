INCLUDES=-I$(top_srcdir)/../../../../../..
libdir= $(prefix)/lib/unisim/component/cxx/chipset/mpc107/dma
includedir = $(prefix)/include/unisim/component/cxx/chipset/mpc107/dma

lib_LIBRARIES = libdma.a
libdma_a_SOURCES = \
	buffer_32.cc buffer_64.cc \
	register.cc \
	dma_32.cc dma_64.cc dma_32_debug.cc dma_64_debug.cc
	
include_HEADERS = \
	buffer.hh buffer.tcc \
	register.hh \
	dma_client_interface.hh \
	dma.hh dma.tcc

install-exec-hook: $(libdir)/libdma.a
	rm -f $(prefix)/lib/libunisim_component_cxx_chipset_mpc107_dma.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_chipset_mpc107_dma.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_chipset_mpc107_dma.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)