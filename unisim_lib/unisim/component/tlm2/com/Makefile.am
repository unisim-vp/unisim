includedir = $(prefix)/include/unisim/component/tlm2/com
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/com

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_str7_spi
SUBDIRS += str7_spi
INCLUDE_SUBDIRS += str7_spi
endif

if COND_str7_uart
SUBDIRS += str7_uart
INCLUDE_SUBDIRS += str7_uart
endif

if COND_str7_i2c
SUBDIRS += str7_i2c
INCLUDE_SUBDIRS += str7_i2c
endif

if COND_str7_gpio
SUBDIRS += str7_gpio
INCLUDE_SUBDIRS += str7_gpio
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
