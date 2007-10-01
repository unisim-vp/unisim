INCLUDES=-I$(top_srcdir)/../../../../..

lib_LIBRARIES = libunisim_component_cxx_cache_arm.a
libunisim_component_cxx_cache_arm_a_SOURCES = \
	512b_dm_wt_8bls.cc 512b_dm_wt_8bls_debug.cc

includedir = $(prefix)/include/unisim/component/cxx/cache/arm
include_HEADERS = cache.hh cache.tcc \
	set.hh set.tcc \
	line.hh line.tcc \
	config.hh \
	types.hh \
	cache_interface.hh

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
