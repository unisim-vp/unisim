INCLUDES=-I$(top_srcdir)/../../../../../..
includedir = $(prefix)/include/unisim/component/cxx/chipset/mpc107/dma
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/chipset/mpc107/dma

lib_LIBRARIES = libunisim_component_cxx_chipset_mpc107_dma.a
libunisim_component_cxx_chipset_mpc107_dma_a_SOURCES = \
	buffer_32.cc buffer_64.cc \
	register.cc \
	dma_32.cc dma_64.cc dma_32_debug.cc dma_64_debug.cc
	
include_HEADERS = \
	buffer.hh buffer.tcc \
	register.hh \
	dma_client_interface.hh \
	dma.hh dma.tcc

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../../../../..
include $(top_srcdir)/../../../../../../m4/unisim.am
	