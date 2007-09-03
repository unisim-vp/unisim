INCLUDES=-I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/component/cxx/pci
libdir= $(prefix)/lib/unisim/component/cxx/pci
lib_LIBRARIES = libpci.a
libpci_a_SOURCES = types.cc
include_HEADERS = types.hh types.tcc

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_ide
SUBDIRS += ide
INCLUDE_SUBDIRS += ide
endif

if COND_video
SUBDIRS += video
INCLUDE_SUBDIRS += video
endif

if COND_macio
SUBDIRS += macio
INCLUDE_SUBDIRS += macio
endif

if COND_debug
SUBDIRS += debug
INCLUDE_SUBDIRS += debug
endif

install-exec-hook: $(libdir)/libpci.a
	rm -f $(prefix)/lib/libunisim_component_cxx_pci.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_pci.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_pci.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
