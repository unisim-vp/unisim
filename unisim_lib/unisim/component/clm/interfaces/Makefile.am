INCLUDES=-I$(top_srcdir)/../../../.. -I$(top_builddir)/../../../..
includedir = $(prefix)/include/unisim/component/clm/interfaces
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/interfaces

#SUBDIRS=
#INCLUDE_SUBDIRS=
lib_LTLIBRARIES = libunisim_component_clm_interfaces_memreq.la
libunisim_component_clm_interfaces_memreq_la_SOURCES = memreq.cc


#if COND_release
#libunisim_component_cxx_processor_powerpc_a_SOURCES += mpc755.cc mpc7447a.cc
#endif

#nobase_include_HEADERS =

nobase_include_HEADERS = \
	instruction_interface.hh \
	memreq.hh \
	memreq.tcc \
	memreq_mc.hh \
	memreq_interface.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
