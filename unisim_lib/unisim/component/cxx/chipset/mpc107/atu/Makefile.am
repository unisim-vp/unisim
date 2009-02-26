INCLUDES=-I$(top_srcdir)/../../../../../..
includedir = $(prefix)/include/unisim/component/cxx/chipset/mpc107/atu
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/chipset/mpc107/atu

lib_LTLIBRARIES = libunisim_component_cxx_chipset_mpc107_atu.la
libunisim_component_cxx_chipset_mpc107_atu_la_SOURCES = \
	register.cc \
	atu.cc atu_debug.cc

include_HEADERS = \
	register.hh \
	atu.hh atu.tcc
	
uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

dist_share_DATA = COPYING

ROOT_DIR = ../../../../../..
include $(top_srcdir)/../../../../../../m4/unisim.am
	