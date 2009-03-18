AM_CPPFLAGS=-D__STDC_CONSTANT_MACROS
INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/processor/tms320
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/processor/tms320

lib_LTLIBRARIES = libunisim_component_cxx_processor_tms320.la

libunisim_component_cxx_processor_tms320_la_SOURCES = \
	cpu_tms320vc33_debug.cc \
	cpu_tms320vc33.cc

include_HEADERS = config.hh \
	cpu.hh \
	cpu.tcc \
	exception.hh \
	exception.tcc

nobase_include_HEADERS = isa/tms320.isa \
	isa/actions_dec.isa \
	isa/2op.isa \
	isa/3op.isa \
	isa/control.isa \
	isa/interlock.isa \
	isa/load_store.isa \
	isa/parallel.isa \
	isa/power.isa

nodist_include_HEADERS = \
	$(top_srcdir)/isa_tms320.tcc $(top_srcdir)/isa_tms320.hh

BUILT_SOURCES = $(top_srcdir)/isa_tms320.tcc $(top_srcdir)/isa_tms320.hh 

CLEANFILES = $(top_srcdir)/isa_tms320.tcc $(top_srcdir)/isa_tms320.hh

$(top_srcdir)/isa_tms320.tcc: $(top_srcdir)/isa_tms320.hh 
$(top_srcdir)/isa_tms320.hh: $(srcdir)/isa/tms320.isa \
	$(srcdir)/isa/actions_dec.isa \
	$(srcdir)/isa/2op.isa \
	$(srcdir)/isa/3op.isa \
	$(srcdir)/isa/control.isa \
	$(srcdir)/isa/interlock.isa \
	$(srcdir)/isa/load_store.isa \
	$(srcdir)/isa/parallel.isa \
	$(srcdir)/isa/power.isa
	cd $(top_srcdir); $(GENISSLIB_PATH) -o isa_tms320 -w 32 -I isa isa/tms320.isa

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
