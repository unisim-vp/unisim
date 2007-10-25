INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/pci/debug
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/pci/debug
lib_LIBRARIES = libunisim_component_cxx_pci_debug.a
libunisim_component_cxx_pci_debug_a_SOURCES = pci_stub.cc
include_HEADERS = pci_stub.hh pci_stub.tcc

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

share_DATA = COPYING

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
