includedir = $(prefix)/include/unisim/component

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_cxx
SUBDIRS += cxx
INCLUDE_SUBDIRS += cxx
endif

if COND_tlm
SUBDIRS += tlm
INCLUDE_SUBDIRS += tlm
endif

uninstall-hook:
	(rmdir $(includedir) || true)
