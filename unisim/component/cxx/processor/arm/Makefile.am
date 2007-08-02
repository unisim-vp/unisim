INCLUDES=-I$(top_srcdir)/../../../../..
GENISSLIB=$(top_srcdir)/../../../../../build_tool/genisslib/genisslib
libdir= $(prefix)/lib/unisim/component/cxx/processor/arm
includedir = $(prefix)/include/unisim/component/cxx/processor/arm

BUILT_SOURCES = armcpu.tcc armcpu.hh
CLEANFILES = armcpu.tcc armcpu.hh

armcpu.tcc: armcpu.hh 
armcpu.hh: $(srcdir)/arm.isa $(srcdir)/constructors_dec.isa \
	$(srcdir)/actions_dec.isa $(srcdir)/branch.isa \
	$(srcdir)/coprocessor.isa $(srcdir)/data_processing.isa \
	$(srcdir)/exception.isa \
	$(srcdir)/load_store.isa $(srcdir)/load_store_base.isa \
	$(srcdir)/load_store_multiple.isa \
	$(srcdir)/load_store_v4.isa $(srcdir)/load_store_v5e.isa \
	$(srcdir)/misc_arithmetic.isa $(srcdir)/multiply.isa \
	$(srcdir)/semaphore.isa $(srcdir)/status_register_access.isa
	$(GENISSLIB) -o armcpu -w 32 -I$(srcdir) $(srcdir)/arm.isa

lib_LIBRARIES = libarm.a
libarm_a_SOURCES = \
	cpu_arm9tdmi_littleendian.cc \
	cpu_arm9tdmi_bigendian.cc \
	cpu_arm9tdmi_littleendian_debug.cc \
	cpu_arm9tdmi_bigendian_debug.cc \
	cpu_arm966e_s_littleendian.cc \
	cpu_arm966e_s_bigendian.cc \
	cpu_arm966e_s_littleendian_debug.cc \
	cpu_arm966e_s_bigendian_debug.cc

include_HEADERS = config.hh cpu.hh cpu.tcc \
	exception.hh exception.tcc \
	armcpu.hh armcpu.tcc \
	arm.isa constructors_dec.isa actions_dec.isa \
	branch.isa coprocessor.isa data_processing.isa exception.isa \
	load_store.isa load_store_base.isa load_store_multiple.isa \
	load_store_v4.isa load_store_v5e.isa misc_arithmetic.isa multiply.isa \
	semaphore.isa status_register_access.isa

install-exec-hook: $(libdir)/libarm.a
	rm -f $(prefix)/lib/libunisim_component_cxx_processor_arm.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_processor_arm.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_processor_arm.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
