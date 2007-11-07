INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/processor/arm
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/processor/arm
lib_LIBRARIES = libunisim_component_tlm_processor_arm.a
libunisim_component_tlm_processor_arm_a_SOURCES = \
	arm966e_s_debug.cc \
	arm9tdmi_debug.cc \
	arm9tdmi_nocache_debug.cc
	
if COND_release
libunisim_component_tlm_processor_arm_a_SOURCES += \
	arm966e_s.cc \
	arm9tdmi.cc \
	arm9tdmi_nocache.cc
endif 

include_HEADERS = arm.hh arm.tcc

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
