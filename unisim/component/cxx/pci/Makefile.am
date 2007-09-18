INCLUDES=-I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/component/cxx/pci
lib_LIBRARIES = libunisim_component_cxx_pci.a
libunisim_component_cxx_pci_a_SOURCES = types.cc
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

uninstall-hook:
	(rmdir $(includedir) || true)
