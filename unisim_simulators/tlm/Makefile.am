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

if COND_arm966e_s
SUBDIRS += arm966e_s
INCLUDE_SUBDIRS += arm966e_s
endif

if COND_arm7tdmi
SUBDIRS += arm7tdmi
INCLUDE_SUBDIRS += arm7tdmi
endif

if COND_embedded_ppc_g4_board
SUBDIRS += embedded_ppc_g4_board
INCLUDE_SUBDIRS += embedded_ppc_g4_board
endif

#if COND_hcs12xemu
#SUBDIRS += hcs12xemu
#INCLUDE_SUBDIRS += hcs12x
#endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)
