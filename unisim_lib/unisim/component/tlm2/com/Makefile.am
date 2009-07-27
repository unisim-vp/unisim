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

if COND_str7_adc
SUBDIRS += str7_adc
INCLUDE_SUBDIRS += str7_adc
endif

if COND_str7_apb
SUBDIRS += str7_apb
INCLUDE_SUBDIRS += str7_apb
endif

if COND_str7_emi
SUBDIRS += str7_emi
INCLUDE_SUBDIRS += str7_emi
endif

if COND_str7_prccu
SUBDIRS += str7_prccu
INCLUDE_SUBDIRS += str7_prccu
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
