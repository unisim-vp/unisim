includedir = $(prefix)/include/unisim/component/cxx/scheduler
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/scheduler

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_cuda_scheduler
SUBDIRS += cuda_scheduler
INCLUDE_SUBDIRS += cuda_scheduler
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
