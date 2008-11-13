sharedir = $(prefix)/share/unisim_simulators

SUBDIRS=
INCLUDE_SUBDIRS=

# if COND_cxx
# SUBDIRS += cxx
# INCLUDE_SUBDIRS += cxx
# endif
#  
# if COND_cycle_level
# SUBDIRS += cycle_level
# INCLUDE_SUBDIRS += cycle_level
# endif

if COND_clm
SUBDIRS += clm
INCLUDE_SUBDIRS += clm
endif

if COND_tlm
SUBDIRS += tlm
INCLUDE_SUBDIRS += tlm
endif

if COND_tlm2
SUBDIRS += tlm2
INCLUDE_SUBDIRS += tlm2
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

include m4/aminclude.am
