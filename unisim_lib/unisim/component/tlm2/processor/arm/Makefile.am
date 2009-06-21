AM_CPPFLAGS=-D__STDC_CONSTANT_MACROS
INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/processor/arm
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/processor/arm
lib_LTLIBRARIES = libunisim_component_tlm2_processor_arm.la
libunisim_component_tlm2_processor_arm_la_SOURCES = \
	arm966e_s_debug.cc \
	arm7tdmi.cc \
	arm966e_s.cc

include_HEADERS = arm.hh arm.tcc arm7tdmi.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am

