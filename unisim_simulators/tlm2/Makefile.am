sharedir = $(prefix)/share/unisim_simulators/tlm2

SUBDIRS=
INCLUDE_SUBDIRS=

# if COND_cxx
# SUBDIRS += cxx
# INCLUDE_SUBDIRS += cxx
# endif
 
dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

