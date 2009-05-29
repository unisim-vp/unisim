INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/com/str7_spi
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/com/str7_spi
lib_LTLIBRARIES = libunisim_component_tlm2_com_str7_spi.la
libunisim_component_tlm2_com_str7_spi_la_SOURCES = str7_spi.cc str7_spi_verbose.cc
include_HEADERS = str7_spi.hh str7_spi.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
