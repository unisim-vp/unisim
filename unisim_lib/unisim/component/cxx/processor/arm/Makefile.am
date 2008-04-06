# SUBDIRS = cache
# INCLUDE_SUBDIRS = cache

AM_CPPFLAGS=-D__STDC_CONSTANT_MACROS
INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/processor/arm
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/processor/arm

lib_LIBRARIES = libunisim_component_cxx_processor_arm.a

libunisim_component_cxx_processor_arm_a_SOURCES = \
	cpu_arm9tdmi_debug.cc \
	cpu_arm9tdmi_nocache_debug.cc \
	cpu_arm966e_s_debug.cc \
	cache/32kb_dm_wb_32bls.cc cache/32kb_dm_wb_32bls_debug.cc \
	cache/96kb_dm_wb_32bls.cc cache/96kb_dm_wb_32bls_debug.cc \
	coprocessor/arm966e_s/cp15.cc \
	tcm/tcm.cc

if COND_release
libunisim_component_cxx_processor_arm_a_SOURCES += \
	cpu_arm9tdmi.cc \
	cpu_arm9tdmi_nocache.cc \
	cpu_arm966e_s.cc \
	cache/512b_dm_wb_8bls.cc cache/512b_dm_wb_8bls_debug.cc \
	cache/512b_dm_wt_8bls.cc cache/512b_dm_wt_8bls_debug.cc \
	cache/32kb_dm_wt_32bls.cc cache/32kb_dm_wt_32bls_debug.cc \
	cache/64kb_dm_wb_32bls.cc cache/64kb_dm_wb_32bls_debug.cc \
	cache/64kb_dm_wt_32bls.cc cache/64kb_dm_wt_32bls_debug.cc \
	cache/96kb_dm_wt_32bls.cc cache/96kb_dm_wt_32bls_debug.cc 
endif

include_HEADERS = config.hh cpu.hh cpu.tcc \
	cpu.step_instruction.tcc cpu.step_cycle.tcc \
	cpu.soclib_step_cycle.tcc \
	exception.hh exception.tcc \
	cache_interface.hh \
	coprocessor_interface.hh \
	memory_op.hh \
	instruction.hh instruction.tcc

nobase_include_HEADERS = \
	cache/cache.hh cache/cache.tcc \
	cache/set.hh cache/set.tcc \
	cache/line.hh cache/line.tcc \
	cache/config.hh \
	cache/types.hh \
	coprocessor/arm966e_s/cp15.hh \
	coprocessor/arm966e_s/cp15.tcc \
	tcm/tcm.hh tcm/tcm.tcc \
	isa/arm32/arm32.isa isa/arm32/constructors_dec.isa isa/arm32/actions_dec.isa \
	isa/arm32/branch.isa isa/arm32/coprocessor.isa isa/arm32/coprocessor_v5e.isa \
	isa/arm32/data_processing.isa isa/arm32/data_processing_v5e.isa \
	isa/arm32/exception.isa \
	isa/arm32/load_store.isa isa/arm32/load_store_base.isa isa/arm32/load_store_multiple.isa \
	isa/arm32/load_store_v4.isa isa/arm32/load_store_v5e.isa isa/arm32/misc_arithmetic.isa\
	isa/arm32/multiply.isa isa/arm32/multiply_v5e.isa \
	isa/arm32/semaphore.isa isa/arm32/status_register_access.isa \
	isa/arm32/dependency.isa \
	isa/arm32/profiling.isa \
	isa/arm32/specialization.isa \
	isa/thumb/thumb.isa isa/thumb/constructors_dec.isa \
	isa/thumb/actions_dec.isa \
	isa/thumb/branch.isa \
	isa/thumb/data_processing.isa \
	isa/thumb/exception.isa \
	isa/thumb/load_store_reg.isa \
	isa/thumb/load_store_multiple.isa \
	isa/thumb/profiling.isa

nodist_include_HEADERS = \
	isa_arm32.tcc isa_arm32.hh \
	isa_thumb.tcc isa_thumb.hh

BUILT_SOURCES = isa_arm32.tcc isa_arm32.hh \
	isa_thumb.tcc isa_thumb.hh
CLEANFILES = isa_arm32.tcc isa_arm32.hh \
	isa_thumb.tcc isa_thumb.hh

isa_arm32.tcc: isa_arm32.hh 
isa_arm32.hh: $(srcdir)/isa/arm32/arm32.isa $(srcdir)/isa/arm32/constructors_dec.isa \
	$(srcdir)/isa/arm32/actions_dec.isa $(srcdir)/isa/arm32/branch.isa \
	$(srcdir)/isa/arm32/coprocessor.isa $(srcdir)/isa/arm32/coprocessor_v5e.isa \
	$(srcdir)/isa/arm32/data_processing.isa $(srcdir)/isa/arm32/data_processing_v5e.isa \
	$(srcdir)/isa/arm32/exception.isa \
	$(srcdir)/isa/arm32/load_store.isa $(srcdir)/isa/arm32/load_store_base.isa \
	$(srcdir)/isa/arm32/load_store_multiple.isa \
	$(srcdir)/isa/arm32/load_store_v4.isa $(srcdir)/isa/arm32/load_store_v5e.isa \
	$(srcdir)/isa/arm32/misc_arithmetic.isa \
	$(srcdir)/isa/arm32/multiply.isa $(srcdir)/isa/arm32/multiply_v5e.isa \
	$(srcdir)/isa/arm32/semaphore.isa $(srcdir)/isa/arm32/status_register_access.isa \
	$(srcdir)/isa/arm32/dependency.isa \
	$(srcdir)/isa/arm32/profiling.isa \
	$(srcdir)/isa/arm32/specialization.isa
	$(GENISSLIB_PATH) -o isa_arm32 -w 32 -I $(srcdir)/isa/arm32 $(srcdir)/isa/arm32/arm32.isa

isa_thumb.tcc: isa_thumb.hh 
isa_thumb.hh: $(srcdir)/isa/thumb/thumb.isa $(srcdir)/isa/thumb/constructors_dec.isa \
	$(srcdir)/isa/thumb/actions_dec.isa \
	$(srcdir)/isa/thumb/branch.isa \
	$(srcdir)/isa/thumb/data_processing.isa \
	$(srcdir)/isa/thumb/exception.isa \
	$(srcdir)/isa/thumb/load_store_reg.isa \
	$(srcdir)/isa/thumb/load_store_multiple.isa \
	$(srcdir)/isa/thumb/profiling.isa
	$(GENISSLIB_PATH) -o isa_thumb -w 32 -I $(srcdir)/isa/thumb $(srcdir)/isa/thumb/thumb.isa

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
