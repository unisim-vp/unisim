includedir = $(prefix)/include/unisim/component/tlm/bridge
libdir= $(prefix)/lib/unisim/component/tlm/bridge

SUBDIRS=
INCLUDE_SUBDIRS=

# if COND_simple_fsb_to_mem
# SUBDIRS += simple_fsb_to_mem
# INCLUDE_SUBDIRS += simple_fsb_to_mem
# endif

# if COND_snooping_fsb_to_mem
# SUBDIRS += snooping_fsb_to_mem
# INCLUDE_SUBDIRS += snooping_fsb_to_mem
# endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
