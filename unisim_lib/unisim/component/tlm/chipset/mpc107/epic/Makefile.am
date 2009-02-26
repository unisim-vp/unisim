INCLUDES=-I$(top_srcdir)/../../../../../..
includedir = $(prefix)/include/unisim/component/tlm/chipset/mpc107/epic
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/chipset/mpc107/epic

lib_LTLIBRARIES = libunisim_component_tlm_chipset_mpc107_epic.la

libunisim_component_tlm_chipset_mpc107_epic_la_CXXFLAGS =-DSC_INCLUDE_DYNAMIC_PROCESSES

libunisim_component_tlm_chipset_mpc107_epic_la_SOURCES = \
	timer.cc \
	epic_32.cc epic_32_debug.cc epic_64.cc epic_64_debug.cc
		
include_HEADERS = \
	timer.hh \
	epic.hh epic.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../../..
include $(top_srcdir)/../../../../../../m4/unisim.am
	