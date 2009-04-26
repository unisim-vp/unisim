INCLUDES=-I$(top_srcdir)/../../../.. -I$(top_builddir)/../../../..
includedir = $(prefix)/include/unisim/component/tlm2/interrupt
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm2/interrupt
lib_LTLIBRARIES = libunisim_component_tlm2_interrupt.la
libunisim_component_tlm2_interrupt_la_SOURCES = \
	master_stub.cc

include_HEADERS = types.hh master_stub.hh

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_str7_xti
SUBDIRS += str7_xti
INCLUDE_SUBDIRS += str7_xti
endif

if COND_str7_eic
SUBDIRS += str7_eic
INCLUDE_SUBDIRS += str7_eic
endif

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am

