includedir = $(prefix)/include/unisim/component/tlm2/com
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/com

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_str7_spi
SUBDIRS += str7_spi
INCLUDE_SUBDIRS += str7_spi
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
