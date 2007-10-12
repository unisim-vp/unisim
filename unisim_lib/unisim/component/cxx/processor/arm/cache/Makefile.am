INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/processor/arm/cache

# lib_LIBRARIES = libunisim_component_cxx_processor_arm_cache.a
# libunisim_component_cxx_processor_arm_cache_a_SOURCES =
#	512b_dm_wt_8bls.cc 512b_dm_wt_8bls_debug.cc

include_HEADERS = \
	cache.hh cache.tcc \
	set.hh set.tcc \
	line.hh line.tcc \
	config.hh \
	types.hh

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
