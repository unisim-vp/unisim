INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/interconnect/generic_router
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/interconnect/generic_router
lib_LIBRARIES = libunisim_component_tlm2_interconnect_generic_router.a
libunisim_component_tlm2_interconnect_generic_router_a_SOURCES = router.cc variable_mapping.cc
include_HEADERS = router.hh router.tcc config.hh router_dispatcher.hh router_dispatcher.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
