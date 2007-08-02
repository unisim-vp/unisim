includedir = $(prefix)/include/unisim
libdir= $(prefix)/lib/unisim

SUBDIRS =
INCLUDE_SUBDIRS =

if COND_kernel
SUBDIRS += kernel
INCLUDE_SUBDIRS += kernel
endif

if COND_util
SUBDIRS += util
INCLUDE_SUBDIRS += util
endif

if COND_service
SUBDIRS += service
INCLUDE_SUBDIRS += service
endif

if COND_component
SUBDIRS += component
INCLUDE_SUBDIRS += component
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
