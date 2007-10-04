INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
# GENISSLIB=$(top_builddir)/../../../../../build_tool/genisslib/genisslib
includedir = $(prefix)/include/unisim/component/cxx/processor/arm

lib_LIBRARIES = libunisim_component_cxx_processor_arm.a
libunisim_component_cxx_processor_arm_a_SOURCES = \
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
	cache_interface.hh \
	arm.isa constructors_dec.isa actions_dec.isa \
	branch.isa coprocessor.isa data_processing.isa exception.isa \
	load_store.isa load_store_base.isa load_store_multiple.isa \
	load_store_v4.isa load_store_v5e.isa misc_arithmetic.isa multiply.isa \
	semaphore.isa status_register_access.isa
nodist_include_HEADERS = armcpu.tcc armcpu.hh

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
	cd $(srcdir); $(GENISSLIB_PATH) -o armcpu -w 32 -I$(srcdir) $(srcdir)/arm.isa

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
