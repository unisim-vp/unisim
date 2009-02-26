INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/pci/debug
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/pci/debug
lib_LTLIBRARIES = libunisim_component_cxx_pci_debug.la
libunisim_component_cxx_pci_debug_la_SOURCES = pci_stub.cc pci_stub_64.cc
include_HEADERS = pci_stub.hh pci_stub.tcc

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
