INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/loader/pmac_bootx
sharedir = $(prefix)/share/unisim_lib/unisim/service/loader/pmac_bootx
include_HEADERS = pmac_bootx.hh

lib_LIBRARIES = libunisim_service_loader_pmac_bootx.a
libunisim_service_loader_pmac_bootx_a_SOURCES = pmac_bootx.cc

dist_share_DATA = COPYING device_tree_pmac_g3.xml device_tree_pmac_g4.xml

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
	