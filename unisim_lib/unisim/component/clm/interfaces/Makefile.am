includedir = $(prefix)/include/unisim/component/clm/interfaces
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/interfaces

#SUBDIRS=
#INCLUDE_SUBDIRS=

nobase_include_HEADERS = \
	interfaces/instruction_interface.hh \
	interfaces/memreq.h \
	interfaces/memreq_interface.h

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..