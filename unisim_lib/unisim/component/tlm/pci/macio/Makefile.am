INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/pci/macio
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/pci/macio
lib_LTLIBRARIES = libunisim_component_tlm_pci_macio.la
libunisim_component_tlm_pci_macio_la_SOURCES = heathrow_32.cc heathrow_64.cc
include_HEADERS = heathrow.hh heathrow.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
