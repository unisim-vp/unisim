# SUBDIRS = cache
# INCLUDE_SUBDIRS = cache

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
	exception.hh exception.tcc \
	cache_interface.hh \
	coprocessor_interface.hh
	
nobase_include_HEADERS = \
	cache/cache.hh cache/cache.tcc \
	cache/set.hh cache/set.tcc \
	cache/line.hh cache/line.tcc \
	cache/config.hh \
	cache/types.hh \
	coprocessor/arm966e_s/cp15.hh \
	coprocessor/arm966e_s/cp15.tcc \
	tcm/tcm.hh tcm/tcm.tcc \
	isa/arm32/arm.isa isa/arm32/constructors_dec.isa isa/arm32/actions_dec.isa \
	isa/arm32/branch.isa isa/arm32/coprocessor.isa isa/arm32/data_processing.isa isa/arm32/exception.isa \
	isa/arm32/load_store.isa isa/arm32/load_store_base.isa isa/arm32/load_store_multiple.isa \
	isa/arm32/load_store_v4.isa isa/arm32/load_store_v5e.isa isa/arm32/misc_arithmetic.isa isa/arm32/multiply.isa \
	isa/arm32/semaphore.isa isa/arm32/status_register_access.isa \
	isa/thumb/thumb_arm.isa isa/thumb/thumb_constructors_dec.isa \
	isa/thumb/thumb_actions_dec.isa \
	isa/thumb/thumb_branch.isa \
	isa/thumb/thumb_data_processing.isa \
	isa/thumb/thumb_exception.isa \
	isa/thumb/thumb_load_store_reg.isa \
	isa/thumb/thumb_load_store_multiple.isa
	
nodist_include_HEADERS = \
	armcpu.tcc armcpu.hh \
	thumbcpu.tcc thumbcpu.hh
	
BUILT_SOURCES = armcpu.tcc armcpu.hh \
	thumbcpu.tcc thumbcpu.hh
CLEANFILES = armcpu.tcc armcpu.hh \
	thumbcpu.tcc thumbcpu.hh

armcpu.tcc: armcpu.hh 
armcpu.hh: $(srcdir)/isa/arm32/arm.isa $(srcdir)/isa/arm32/constructors_dec.isa \
	$(srcdir)/isa/arm32/actions_dec.isa $(srcdir)/isa/arm32/branch.isa \
	$(srcdir)/isa/arm32/coprocessor.isa $(srcdir)/isa/arm32/data_processing.isa \
	$(srcdir)/isa/arm32/exception.isa \
	$(srcdir)/isa/arm32/load_store.isa $(srcdir)/isa/arm32/load_store_base.isa \
	$(srcdir)/isa/arm32/load_store_multiple.isa \
	$(srcdir)/isa/arm32/load_store_v4.isa $(srcdir)/isa/arm32/load_store_v5e.isa \
	$(srcdir)/isa/arm32/misc_arithmetic.isa $(srcdir)/isa/arm32/multiply.isa \
	$(srcdir)/isa/arm32/semaphore.isa $(srcdir)/isa/arm32/status_register_access.isa
	$(GENISSLIB_PATH) -o armcpu -w 32 -I $(srcdir)/isa/arm32 $(srcdir)/isa/arm32/arm.isa

thumbcpu.tcc: thumbcpu.hh 
thumbcpu.hh: $(srcdir)/isa/thumb/thumb_arm.isa $(srcdir)/isa/thumb/thumb_constructors_dec.isa \
	$(srcdir)/isa/thumb/thumb_actions_dec.isa \
	$(srcdir)/isa/thumb/thumb_branch.isa \
	$(srcdir)/isa/thumb/thumb_data_processing.isa \
	$(srcdir)/isa/thumb/thumb_exception.isa \
	$(srcdir)/isa/thumb/thumb_load_store_reg.isa \
	$(srcdir)/isa/thumb/thumb_load_store_multiple.isa
	$(GENISSLIB_PATH) -o thumbcpu -w 32 -I $(srcdir)/isa/thumb $(srcdir)/isa/thumb/thumb_arm.isa

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
