includedir = $(prefix)/include/unisim/component/tlm2/signal_converter
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/signal_converter

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_generic_adc
SUBDIRS += generic_adc
INCLUDE_SUBDIRS += generic_adc
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
