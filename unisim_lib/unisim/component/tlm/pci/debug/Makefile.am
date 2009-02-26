INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/pci/debug
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/pci/debug
lib_LTLIBRARIES = libunisim_component_tlm_pci_debug.la
libunisim_component_tlm_pci_debug_la_SOURCES = pci_stub_32.cc pci_stub_64.cc
include_HEADERS = pci_stub.hh pci_stub.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
