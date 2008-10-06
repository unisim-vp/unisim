INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm2/processor/arm
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/processor/arm
lib_LIBRARIES = libunisim_component_tlm2_processor_arm.a
libunisim_component_tlm2_processor_arm_a_SOURCES = \
	arm7tdmi_debug.cc \
	arm966e_s_debug.cc
	
if COND_release
libunisim_component_tlm2_processor_arm_a_SOURCES += \
	arm7tdmi.cc \
	arm966e_s.cc
endif 

include_HEADERS = arm.hh arm.tcc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am

