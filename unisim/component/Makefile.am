includedir = $(prefix)/include/unisim/component
libdir= $(prefix)/lib/unisim/component

SUBDIRS=c++ tlm
INCLUDE_SUBDIRS=c++ tlm

# SUBDIRS=
# INCLUDE_SUBDIRS=
# 
# if COND_c++
# SUBDIRS += c++
# INCLUDE_SUBDIRS += c++
# endif
# 
# if COND_tlm
# SUBDIRS += tlm
# INCLUDE_SUBDIRS += tlm
# endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
