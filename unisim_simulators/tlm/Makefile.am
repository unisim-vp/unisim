sharedir = $(prefix)/share/unisim_simulators/tlm

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_ppcemu_system
SUBDIRS += ppcemu_system
INCLUDE_SUBDIRS += ppcemu_system
endif

if COND_ppcemu
SUBDIRS += ppcemu
INCLUDE_SUBDIRS += ppcemu
endif

if COND_armemu
SUBDIRS += armemu
INCLUDE_SUBDIRS += armemu
endif

if COND_armemu_nocache
SUBDIRS += armemu_nocache
INCLUDE_SUBDIRS += armemu_nocache
endif

if COND_arm966e_s
SUBDIRS += arm966e_s
INCLUDE_SUBDIRS += arm966e_s
endif

if COND_embedded_ppc_g4_board
SUBDIRS += embedded_ppc_g4_board
INCLUDE_SUBDIRS += embedded_ppc_g4_board
endif

share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)
