AM_CPPFLAGS=-D__STDC_CONSTANT_MACROS
INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/processor/arm
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/processor/arm
lib_LTLIBRARIES = libunisim_component_tlm2_processor_arm.la
libunisim_component_tlm2_processor_arm_la_SOURCES = \
	arm966es/arm966e_s.cc \
	arm7tdmi/arm7tdmi.cc \
	arm926ejs/arm926ejs.cc

include_HEADERS = arm.hh arm.tcc 
nobase_include_HEADERS = \
	arm7tdmi/arm7tdmi.hh \
	arm9tdmi/arm9tdmi.hh \
	arm926ejs/arm926ejs.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am

