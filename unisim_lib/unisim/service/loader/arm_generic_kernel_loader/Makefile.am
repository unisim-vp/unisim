INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/loader/arm_generic_kernel_loader
sharedir = $(prefix)/share/unisim_lib/unisim/service/loader/arm_generic_kernel_loader
include_HEADERS = arm_generic_kernel_loader.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
