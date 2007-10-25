INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
# GENISSLIB=$(top_builddir)/../../../../../build_tool/genisslib/genisslib
includedir = $(prefix)/include/unisim/component/cxx/processor/powerpc
lib_LIBRARIES = libunisim_component_cxx_processor_powerpc.a
libunisim_component_cxx_processor_powerpc_a_SOURCES = cpu.cc exception.cc mpc755.cc mpc7447a.cc
include_HEADERS = cpu.hh exception.hh mmu.hh powerpc_types.hh \
	fpu.hh floating.hh config.hh branch.isa condition.isa floating.isa \
	integer.isa loadstore.isa misc.isa altivec.isa ppc.isa \
	fpu.tcc cpu.tcc mmu.tcc exception.tcc
nodist_include_HEADERS = powerpc.hh powerpc.tcc
BUILT_SOURCES = powerpc.hh powerpc.tcc
CLEANFILES = powerpc.tcc powerpc.hh

powerpc.tcc: powerpc.hh
powerpc.hh: $(srcdir)/branch.isa $(srcdir)/condition.isa $(srcdir)/floating.isa $(srcdir)/integer.isa \
	$(srcdir)/loadstore.isa $(srcdir)/misc.isa $(srcdir)/altivec.isa $(srcdir)/ppc.isa
	cd $(srcdir); $(GENISSLIB_PATH) -I $(srcdir) -o powerpc -w 32 $(srcdir)/ppc.isa

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
