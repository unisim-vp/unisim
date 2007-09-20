INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/isa/i8042
lib_LIBRARIES = libunisim_component_cxx_isa_i8042.a
libunisim_component_cxx_isa_i8042_a_SOURCES = i8042.cc
include_HEADERS = i8042.hh

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
