includedir = $(prefix)/include/unisim/component
sharedir = $(prefix)/share/unisim_lib/unisim/component

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

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
