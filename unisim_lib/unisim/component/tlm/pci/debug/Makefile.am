INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/pci/debug
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/pci/debug
lib_LIBRARIES = libunisim_component_tlm_pci_debug.a
libunisim_component_tlm_pci_debug_a_SOURCES = pci_stub_32.cc pci_stub_64.cc
include_HEADERS = pci_stub.hh pci_stub.tcc

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
