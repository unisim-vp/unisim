INCLUDES=-I$(top_srcdir)/../../../../..
libdir= $(prefix)/lib/unisim/component/cxx/pci/debug
includedir = $(prefix)/include/unisim/component/cxx/pci/debug
lib_LIBRARIES = libdebug.a
libdebug_a_SOURCES = pci_stub.cc
include_HEADERS = pci_stub.hh pci_stub.tcc

install-exec-hook: $(libdir)/libdebug.a
	rm -f $(prefix)/lib/libunisim_component_cxx_pci_debug.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_pci_debug.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_pci_debug.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
