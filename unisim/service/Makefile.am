includedir = $(prefix)/include/unisim/service
libdir = $(prefix)/lib/unisim/service
servicedir = $(prefix)/share/unisim/service

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_interfaces
SUBDIRS += interfaces
INCLUDE_SUBDIRS += interfaces
endif

if COND_logger
SUBDIRS += logger
INCLUDE_SUBDIRS += logger
endif

if COND_os
SUBDIRS += os
INCLUDE_SUBDIRS += os
endif

if COND_debug
SUBDIRS += debug
INCLUDE_SUBDIRS += debug
endif

if COND_loader
SUBDIRS += loader
INCLUDE_SUBDIRS += loader
endif

if COND_sdl
SUBDIRS += sdl
INCLUDE_SUBDIRS += sdl
endif

if COND_power
SUBDIRS += power
INCLUDE_SUBDIRS += power
endif

if COND_time
SUBDIRS += time
INCLUDE_SUBDIRS += time
endif

if COND_tee
SUBDIRS += tee
INCLUDE_SUBDIRS += tee
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
	(rmdir $(servicedir) || true)
