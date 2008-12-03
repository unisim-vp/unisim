includedir = $(prefix)/include/unisim/component/clm/interfaces
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/interfaces

#SUBDIRS=
#INCLUDE_SUBDIRS=

nobase_include_HEADERS = \
	instruction_interface.hh \
	memreq.hh \
	memreq.tcc \
	memreq_mc.hh \
	memreq_interface.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..