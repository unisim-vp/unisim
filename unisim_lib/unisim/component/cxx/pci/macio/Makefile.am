INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/pci/macio
lib_LIBRARIES = libunisim_component_cxx_pci_macio.a
libunisim_component_cxx_pci_macio_a_SOURCES = heathrow_32.cc heathrow_64.cc
include_HEADERS = heathrow.hh heathrow.tcc

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
