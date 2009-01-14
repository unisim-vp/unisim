INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/processor/tesla
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/processor/tesla
lib_LIBRARIES = libunisim_component_cxx_processor_tesla.a

libunisim_component_cxx_processor_tesla_a_SOURCES = cpu.cc g80_debug.cc flags.cc disasm.cc


include_HEADERS = cpu.hh cpu.tcc config.hh disasm.hh exec.hh exec.tcc \
	flags.hh flags.tcc instruction.hh instruction.tcc register.hh register.tcc \
	exception.hh exception.tcc \
	isa/opcode/tesla.isa isa/src1/src1.isa isa/src2/src2.isa isa/src3/src3.isa \
	isa/opcode/mov.isa isa/opcode/integer.isa isa/opcode/fp32.isa isa/opcode/fp32.isa \
	isa/dest/dest.isa

nodist_include_HEADERS = $(top_srcdir)/tesla_opcode.hh $(top_srcdir)/tesla_opcode.tcc \
	$(top_srcdir)/tesla_src1.hh $(top_srcdir)/tesla_src1.tcc \
	$(top_srcdir)/tesla_src2.hh $(top_srcdir)/tesla_src2.tcc \
	$(top_srcdir)/tesla_src3.hh $(top_srcdir)/tesla_src3.tcc \
	$(top_srcdir)/tesla_dest.hh $(top_srcdir)/tesla_dest.tcc

BUILT_SOURCES = $(top_srcdir)/tesla_opcode.hh $(top_srcdir)/tesla_opcode.tcc \
	$(top_srcdir)/tesla_src1.hh $(top_srcdir)/tesla_src1.tcc \
	$(top_srcdir)/tesla_src2.hh $(top_srcdir)/tesla_src2.tcc \
	$(top_srcdir)/tesla_src3.hh $(top_srcdir)/tesla_src3.tcc \
	$(top_srcdir)/tesla_dest.hh $(top_srcdir)/tesla_dest.tcc
CLEANFILES = $(top_srcdir)/tesla_opcode.tcc $(top_srcdir)/tesla_opcode.hh \
	$(top_srcdir)/tesla_src1.hh $(top_srcdir)/tesla_src1.tcc \
	$(top_srcdir)/tesla_src2.hh $(top_srcdir)/tesla_src2.tcc \
	$(top_srcdir)/tesla_src3.hh $(top_srcdir)/tesla_src3.tcc \
	$(top_srcdir)/tesla_dest.hh $(top_srcdir)/tesla_dest.tcc

$(top_srcdir)/tesla_opcode.tcc: $(top_srcdir)/tesla_opcode.hh
$(top_srcdir)/tesla_opcode.hh: $(srcdir)/isa/opcode/tesla.isa \
	$(srcdir)/isa/opcode/fp32.isa $(srcdir)/isa/opcode/mov.isa \
	$(srcdir)/isa/opcode/integer.isa
	cd $(top_srcdir); $(GENISSLIB_PATH) -I . -o tesla_opcode -w 64 isa/opcode/tesla.isa

$(top_srcdir)/tesla_src1.tcc: $(top_srcdir)/tesla_src1.hh
$(top_srcdir)/tesla_src1.hh: $(srcdir)/isa/src1/src1.isa
	cd $(top_srcdir); $(GENISSLIB_PATH) -I . -o tesla_src1 -w 64 isa/src1/src1.isa

$(top_srcdir)/tesla_src2.tcc: $(top_srcdir)/tesla_src2.hh
$(top_srcdir)/tesla_src2.hh: $(srcdir)/isa/src2/src2.isa
	cd $(top_srcdir); $(GENISSLIB_PATH) -I . -o tesla_src2 -w 64 isa/src2/src2.isa

$(top_srcdir)/tesla_src3.tcc: $(top_srcdir)/tesla_src3.hh
$(top_srcdir)/tesla_src3.hh: $(srcdir)/isa/src3/src3.isa
	cd $(top_srcdir); $(GENISSLIB_PATH) -I . -o tesla_src3 -w 64 isa/src3/src3.isa

$(top_srcdir)/tesla_dest.tcc: $(top_srcdir)/tesla_dest.hh
$(top_srcdir)/tesla_dest.hh: $(srcdir)/isa/dest/dest.isa
	cd $(top_srcdir); $(GENISSLIB_PATH) -I . -o tesla_dest -w 64 isa/dest/dest.isa


dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
