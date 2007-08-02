INCLUDES=-I$(top_srcdir)/../../../../..
GENISSLIB=$(top_srcdir)/../../../../../build_tool/genisslib/genisslib
libdir= $(prefix)/lib/unisim/component/c++/processor/powerpc
includedir = $(prefix)/include/unisim/component/c++/processor/powerpc
lib_LIBRARIES = libpowerpc.a
libpowerpc_a_SOURCES = cpu.cc exception.cc mpc755.cc mpc7447a.cc
include_HEADERS = powerpc.hh cpu.hh exception.hh mmu.hh powerpc_types.hh fpu.hh floating.hh config.hh branch.isa condition.isa floating.isa integer.isa loadstore.isa misc.isa altivec.isa ppc.isa powerpc.tcc fpu.tcc cpu.tcc mmu.tcc exception.tcc

BUILT_SOURCES = powerpc.hh
CLEANFILES = powerpc.tcc powerpc.hh

powerpc.hh: $(srcdir)/branch.isa $(srcdir)/condition.isa $(srcdir)/floating.isa $(srcdir)/integer.isa \
	$(srcdir)/loadstore.isa $(srcdir)/misc.isa $(srcdir)/altivec.isa $(srcdir)/ppc.isa
	$(GENISSLIB) -I$(srcdir) -o powerpc -w 32 $(srcdir)/ppc.isa

install-exec-hook: $(libdir)/libpowerpc.a
	rm -f $(prefix)/lib/libunisim_component_cxx_processor_powerpc.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_component_cxx_processor_powerpc.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_component_cxx_processor_powerpc.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
