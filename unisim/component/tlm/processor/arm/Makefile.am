INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
libdir= $(prefix)/lib/unisim/component/tlm/processor/arm
includedir = $(prefix)/include/unisim/component/tlm/processor/arm
lib_LIBRARIES = libarm.a
libarm_a_SOURCES = arm966e_s_bigendian.cc \
	arm966e_s_bigendian_debug.cc \
	arm966e_s_littleendian.cc \
	arm966e_s_littleendian_debug.cc \
	arm9tdmi_bigendian.cc \
	arm9tdmi_bigendian_debug.cc \
	arm9tdmi_littleendian.cc \
	arm9tdmi_littleendian_debug.cc 
include_HEADERS = arm.hh arm.tcc

install-exec-hook: $(libdir)/libarm.a
	rm -f $(prefix)/lib/libunisim_component_tlm_processor_arm.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_tlm_processor_arm.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_tlm_processor_arm.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
