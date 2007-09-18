INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/processor/arm
lib_LIBRARIES = libunisim_component_tlm_processor_arm.a
libunisim_component_tlm_processor_arm_a_SOURCES = arm966e_s_bigendian.cc \
	arm966e_s_bigendian_debug.cc \
	arm966e_s_littleendian.cc \
	arm966e_s_littleendian_debug.cc \
	arm9tdmi_bigendian.cc \
	arm9tdmi_bigendian_debug.cc \
	arm9tdmi_littleendian.cc \
	arm9tdmi_littleendian_debug.cc 
include_HEADERS = arm.hh arm.tcc

uninstall-hook:
	(rmdir $(includedir) || true)
