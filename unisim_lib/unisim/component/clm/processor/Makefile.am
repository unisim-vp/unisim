includedir = $(prefix)/include/unisim/component/clm/processor
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/processor

#SUBDIRS=
#INCLUDE_SUBDIRS=

nobase_include_HEADERS = \
	ooosim/config.hh \
	ooosim/cpu_emulator.hh \
	ooosim/cpu_simulator.hh \
	ooosim/iss_interface.hh \
	ooosim/parameters.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..