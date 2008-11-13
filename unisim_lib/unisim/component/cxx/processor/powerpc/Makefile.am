INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/processor/powerpc
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/processor/powerpc
lib_LIBRARIES = libunisim_component_cxx_processor_powerpc.a
libunisim_component_cxx_processor_powerpc_a_SOURCES = cpu.cc exception.cc mpc755_debug.cc mpc7447a_debug.cc

if COND_release
libunisim_component_cxx_processor_powerpc_a_SOURCES += mpc755.cc mpc7447a.cc
endif

include_HEADERS = cpu.hh exception.hh \
	floating.hh config.hh branch.isa condition.isa floating.isa \
	integer.isa loadstore.isa misc.isa altivec.isa ppc.isa perf.isa \
	sim_branch.isa sim_dependencies.isa sim_latencies.isa sim_load.isa sim_ppc.isa\
	cpu.tcc exception.tcc
nodist_include_HEADERS = $(top_srcdir)/powerpc.hh $(top_srcdir)/powerpc.tcc
BUILT_SOURCES = $(top_srcdir)/powerpc.hh $(top_srcdir)/powerpc.tcc
CLEANFILES = $(top_srcdir)/powerpc.tcc $(top_srcdir)/powerpc.hh

$(top_srcdir)/powerpc.tcc: $(top_srcdir)/powerpc.hh
$(top_srcdir)/powerpc.hh: $(srcdir)/branch.isa $(srcdir)/condition.isa $(srcdir)/floating.isa $(srcdir)/integer.isa \
	$(srcdir)/loadstore.isa $(srcdir)/misc.isa $(srcdir)/altivec.isa $(srcdir)/ppc.isa $(srcdir)/perf.isa \
	$(srcdir)/sim_branch.isa $(srcdir)/sim_dependencies.isa $(srcdir)/sim_latencies.isa $(srcdir)/sim_load.isa $(srcdir)/sim_ppc.isa
	cd $(top_srcdir); $(GENISSLIB_PATH) -I . -o powerpc -w 32 ppc.isa

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
