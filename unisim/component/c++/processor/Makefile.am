includedir = $(prefix)/include/unisim/component/c++/processor
libdir= $(prefix)/lib/unisim/component/c++/processor

SUBDIRS=powerpc arm
INCLUDE_SUBDIRS=powerpc arm

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
