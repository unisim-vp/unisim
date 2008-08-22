includedir = $(prefix)/include/unisim/component/clm/processor/ooosim_mc
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/processor/ooosim_mc

#SUBDIRS=
#INCLUDE_SUBDIRS=

include_HEADERS = \
	config.hh \
	cpu_emulator.hh \
	cpu_simulator_mc.hh \
	iss_interface.hh \
	parameters.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..