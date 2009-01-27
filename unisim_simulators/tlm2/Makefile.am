sharedir = $(prefix)/share/unisim_simulators/tlm2

SUBDIRS=
INCLUDE_SUBDIRS=

# if COND_cxx
# SUBDIRS += cxx
# INCLUDE_SUBDIRS += cxx
# endif
 
if COND_hcs12xemu
SUBDIRS += hcs12xemu
INCLUDE_SUBDIRS += hcs12x
endif

if COND_arm7tdmi
SUBDIRS += arm7tdmi
INCLUDE_SUBDIRS += arm7tdmi
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

