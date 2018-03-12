#!/bin/bash

SIMPKG=e5500fpv

UNISIM_TOOLS_GENISSLIB_HEADER_FILES="\
action.hh \
cli.hh \
errtools.hh \
isa.hh \
parser_defs.hh \
riscgenerator.hh \
specialization.hh \
variable.hh \
bitfield.hh \
comment.hh \
fwd.hh \
main.hh \
product.hh \
scanner.hh \
strtools.hh \
vect.hh \
ciscgenerator.hh \
conststr.hh \
generator.hh \
operation.hh \
referencecounting.hh \
sourcecode.hh \
subdecoder.hh \
"

UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES="\
scanner.cc \
parser.cc \
parser_tokens.hh \
"

UNISIM_TOOLS_GENISSLIB_SOURCE_FILES="\
parser.yy \
scanner.ll \
action.cc \
bitfield.cc \
cli.cc \
comment.cc \
conststr.cc \
isa.cc \
main.cc \
operation.cc \
product.cc \
referencecounting.cc \
sourcecode.cc \
strtools.cc \
variable.cc \
generator.cc \
riscgenerator.cc \
ciscgenerator.cc \
subdecoder.cc \
specialization.cc \
errtools.cc \
"

UNISIM_TOOLS_GENISSLIB_DATA_FILES="COPYING INSTALL NEWS README AUTHORS ChangeLog"

UNISIM_TOOLS_GENISSLIB_M4_FILES="\
m4/lexer.m4 \
m4/parser_gen.m4 \
"

GENISSLIB_EXTERNAL_HEADERS="\
cassert \
cctype \
cerrno \
cstdarg \
cstdio \
cstdlib \
cstring \
fstream \
inttypes.h \
iosfwd \
iostream \
limits \
map \
memory \
ostream \
unistd.h \
vector \
"

UNISIM_LIB_SIMULATOR_SOURCE_FILES="\
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/util/debug/symbol_table_64.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/dwarf/class.cc \
unisim/util/debug/dwarf/dwarf64.cc \
unisim/util/debug/dwarf/encoding.cc \
unisim/util/debug/dwarf/ml.cc \
unisim/util/debug/dwarf/filename.cc \
unisim/util/debug/dwarf/leb128.cc \
unisim/util/debug/dwarf/abbrev.cc \
unisim/util/debug/dwarf/dwarf32.cc \
unisim/util/debug/dwarf/register_number_mapping.cc \
unisim/util/debug/dwarf/data_object.cc \
unisim/util/debug/dwarf/c_loc_expr_parser.cc \
unisim/util/blob/section32.cc \
unisim/util/blob/blob32.cc \
unisim/util/blob/section64.cc \
unisim/util/blob/segment32.cc \
unisim/util/blob/segment64.cc \
unisim/util/blob/blob64.cc \
unisim/util/debug/stmt_32.cc \
unisim/util/debug/elf_symtab/elf_symtab32.cc \
unisim/util/debug/elf_symtab/elf_symtab64.cc \
unisim/util/debug/stmt_64.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/type.cc \
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/loader/elf_loader/elf64_loader.cc \
unisim/util/os/linux_os/environment.cc \
unisim/util/lexer/lexer.cc \
unisim/util/xml/xml.cc \
"

UNISIM_LIB_SIMULATOR_ISA_FILES="\
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/add.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/adde.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addic.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addic_.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addis.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addme.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addze.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mulhw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mulhwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mulli.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mullw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/neg.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfe.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfic.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfme.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfze.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmp.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpL.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpiL.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpl.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpli.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/and.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/andc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/andi_.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/andis_.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cntlzw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/eqv.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/extsb.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/extsh.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/nand.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/nor.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/or.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/orc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/ori.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/oris.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/xor.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/xori.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/xoris.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rlwimi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rlwinm.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rlwnm.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rldicl.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rldicr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/slw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sraw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/srawi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/srw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fadd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fadds.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fdiv.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fdivs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmul.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmuls.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fsub.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fsubs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmadd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmadds.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmsub.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmsubs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fnmadd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fnmadds.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fnmsub.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fnmsubs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fcmpo.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fcmpu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fctiw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fctiwz.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/frsp.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fabs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fnabs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fneg.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mcrfs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mffs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mtfsb0.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mtfsb1.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mtfsf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mtfsfi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fres.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/frsqrte.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fsel.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fsqrt.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fsqrts.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/branch/b.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/branch/bc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/branch/bcctr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/branch/bclr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crand.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crandc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/creqv.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crnand.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crnor.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/cror.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crorc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crxor.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/mcrf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lbz.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lbzu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lbzux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lbzx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lha.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhau.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhaux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhax.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhz.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhzu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhzux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhzx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwz.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwzu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwzux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwzx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwa.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwaux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwax.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/ld.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/ldu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/ldux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/ldx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stb.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stbu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stbux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stbx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sth.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sthu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sthux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sthx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stwux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stwx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/std.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stdu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stdux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stdx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sthbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stwbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lswi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lswx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stswi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stswx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lmw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stmw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfdu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfdux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfdx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfsu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfsux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfsx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfdu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfdux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfdx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfiwx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfsu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfsux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfsx.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/isync.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/lwarx.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/stwcx_.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcba.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcbf.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/dcbi.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcbst.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcbt.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcbtst.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcbz.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/icbi.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/icbt.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/tlbsync.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/syscall/sc.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/rfi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/tw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/twi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mcrxr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mfcr.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mfmsr.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mfspr.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mtspr.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/mftb.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mtcrf.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/mtmsr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/isel.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_move_assist/dlmzb.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/macchw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/macchws.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/macchwsu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/macchwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/machhw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/machhws.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/machhwsu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/machhwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/maclhw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/maclhws.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/maclhwsu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/maclhwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/mulchw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/mulchwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/mulhhw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/mulhhwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/mullhw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/mullhwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/nmacchw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/nmacchws.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/nmachhw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/nmachhws.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/nmaclhw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac/nmaclhws.isa \
unisim/component/cxx/processor/powerpc/isa/book_e/dccci.isa \
unisim/component/cxx/processor/powerpc/isa/book_e/dcread.isa \
unisim/component/cxx/processor/powerpc/isa/book_e/iccci.isa \
unisim/component/cxx/processor/powerpc/isa/book_e/icread.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/mbar.isa \
unisim/component/cxx/processor/powerpc/isa/book_e/msync.isa \
unisim/component/cxx/processor/powerpc/isa/book_e/tlbre.isa \
unisim/component/cxx/processor/powerpc/isa/book_e/tlbsx.isa \
unisim/component/cxx/processor/powerpc/isa/book_e/tlbwe.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/rfci.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/rfmci.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mfdcr.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mfdcrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mfdcrux.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mtdcr.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mtdcrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mtdcrux.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/wrtee.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/wrteei.isa \
unisim/component/cxx/processor/powerpc/isa/book_e/nop.isa \
"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
${UNISIM_LIB_SIMULATOR_ISA_FILES} \
unisim/kernel/api/api.hh \
unisim/util/backtrace/backtrace.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/service/service.hh \
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/interfaces/linux_os.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/register.hh \
unisim/service/interfaces/stmt_lookup.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/blob/blob.hh \
unisim/util/blob/blob.tcc \
unisim/util/blob/section.hh \
unisim/util/blob/section.tcc \
unisim/util/blob/segment.hh \
unisim/util/blob/segment.tcc \
unisim/util/debug/data_object.hh \
unisim/util/debug/dwarf/abbrev.hh \
unisim/util/debug/dwarf/addr_range.hh \
unisim/util/debug/dwarf/addr_range.tcc \
unisim/util/debug/dwarf/attr.hh \
unisim/util/debug/dwarf/attr.tcc \
unisim/util/debug/dwarf/call_frame_prog.hh \
unisim/util/debug/dwarf/call_frame_prog.tcc \
unisim/util/debug/dwarf/call_frame_vm.hh \
unisim/util/debug/dwarf/call_frame_vm.tcc \
unisim/util/debug/dwarf/cfa.hh \
unisim/util/debug/dwarf/cie.hh \
unisim/util/debug/dwarf/cie.tcc \
unisim/util/debug/dwarf/class.hh \
unisim/util/debug/dwarf/c_loc_expr_parser.hh \
unisim/util/debug/dwarf/cu.hh \
unisim/util/debug/dwarf/cu.tcc \
unisim/util/debug/dwarf/data_object.hh \
unisim/util/debug/dwarf/data_object.tcc \
unisim/util/debug/dwarf/die.hh \
unisim/util/debug/dwarf/die.tcc \
unisim/util/debug/dwarf/dwarf.hh \
unisim/util/debug/dwarf/dwarf.tcc \
unisim/util/debug/dwarf/encoding.hh \
unisim/util/debug/dwarf/expr_vm.hh \
unisim/util/debug/dwarf/expr_vm.tcc \
unisim/util/debug/dwarf/fde.hh \
unisim/util/debug/dwarf/fde.tcc \
unisim/util/debug/dwarf/filename.hh \
unisim/util/debug/dwarf/fmt.hh \
unisim/util/debug/dwarf/frame.hh \
unisim/util/debug/dwarf/frame.tcc \
unisim/util/debug/dwarf/fwd.hh \
unisim/util/debug/dwarf/leb128.hh \
unisim/util/debug/dwarf/loc.hh \
unisim/util/debug/dwarf/loc.tcc \
unisim/util/debug/dwarf/macinfo.hh \
unisim/util/debug/dwarf/macinfo.tcc \
unisim/util/debug/dwarf/ml.hh \
unisim/util/debug/dwarf/option.hh \
unisim/util/debug/dwarf/pub.hh \
unisim/util/debug/dwarf/pub.tcc \
unisim/util/debug/dwarf/range.hh \
unisim/util/debug/dwarf/range.tcc \
unisim/util/debug/dwarf/register_number_mapping.hh \
unisim/util/debug/dwarf/stmt_prog.hh \
unisim/util/debug/dwarf/stmt_prog.tcc \
unisim/util/debug/dwarf/stmt_vm.hh \
unisim/util/debug/dwarf/stmt_vm.tcc \
unisim/util/debug/dwarf/subprogram.hh \
unisim/util/debug/dwarf/subprogram.tcc \
unisim/util/debug/dwarf/util.hh \
unisim/util/debug/dwarf/version.hh \
unisim/util/debug/elf_symtab/elf_symtab.hh \
unisim/util/debug/elf_symtab/elf_symtab.tcc \
unisim/util/debug/simple_register.hh \
unisim/util/debug/stmt.hh \
unisim/util/debug/stmt.tcc \
unisim/util/debug/subprogram.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/symbol.tcc \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/type.hh \
unisim/util/dictionary/dictionary.hh \
unisim/util/dictionary/dictionary.tcc \
unisim/util/endian/endian.hh \
unisim/util/inlining/inlining.hh \
unisim/util/lexer/lexer.hh \
unisim/util/lexer/lexer.tcc \
unisim/util/likely/likely.hh \
unisim/util/loader/elf_loader/elf32.h \
unisim/util/loader/elf_loader/elf32_loader.hh \
unisim/util/loader/elf_loader/elf64.h \
unisim/util/loader/elf_loader/elf64_loader.hh \
unisim/util/loader/elf_loader/elf_common.h \
unisim/util/loader/elf_loader/elf_loader.hh \
unisim/util/loader/elf_loader/elf_loader.tcc \
unisim/util/os/linux_os/aux_table.hh \
unisim/util/os/linux_os/calls.tcc \
unisim/util/os/linux_os/environment.hh \
unisim/util/os/linux_os/errno.hh \
unisim/util/os/linux_os/files_flags.hh \
unisim/util/os/linux_os/linux.hh \
unisim/util/os/linux_os/linux.tcc \
unisim/util/os/linux_os/powerpc64.hh \
unisim/util/parser/parser.hh \
unisim/util/parser/parser.tcc \
unisim/util/reg/core/register.hh \
unisim/util/reg/core/register.tcc \
unisim/util/xml/xml.hh \
unisim/component/cxx/memory/sparse/memory.hh \
unisim/component/cxx/processor/powerpc/isa/book_vle/vle.hh \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/integer.hh \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/floating.hh \
"

UNISIM_LIB_SIMULATOR_M4_FILES="\
m4/times.m4 \
m4/endian.m4 \
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/boost_graph.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/with_boost.m4 \
m4/cacti.m4 \
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4 \
m4/pthread.m4 \
"

SIMULATOR_EXTERNAL_HEADERS="\
assert.h \
ctype.h \
cxxabi.h \
errno.h \
fcntl.h \
fenv.h \
float.h \
fstream \
getopt.h \
inttypes.h \
limits.h \
math.h \
signal.h \
stdarg.h \
stdio.h \
stdlib.h \
string.h \
sys/types.h \
unistd.h \
cassert \
cerrno \
cstddef \
cstdio \
cstdlib \
cstring \
stdexcept \
deque \
list \
sstream \
iosfwd \
iostream \
stack \
map \
ostream \
queue \
vector \
string \
"

UNISIM_LIB_SIMULATOR_DATA_FILES="\
"

UNISIM_SIMULATOR_ISA_FILES="\
top_ppc64.isa \
"

UNISIM_SIMULATOR_ISA_FILES="${UNISIM_LIB_SIMULATOR_ISA_FILES} ${UNISIM_SIMULATOR_ISA_FILES}"

UNISIM_SIMULATOR_SOURCE_FILES="\
main.cc \
linuxsystem.cc \
arch.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
${UNISIM_SIMULATOR_ISA_FILES} \
linuxsystem.hh \
arch.hh \
"

UNISIM_SIMULATOR_EXTRA_FILES="\
"

UNISIM_SIMULATOR_TOP_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
README \
INSTALL \
AUTHORS \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_TESTBENCH_FILES="\
"

function Usage
{
	echo "Usage:"
	echo "  $0 <destination directory>"
}

if [ -z "$1" ]; then
	Usage
	exit -1
fi

UNISIM_DIR=$(cd $(dirname $(dirname $0)); pwd)
mkdir -p "$1"
DEST_DIR=$(cd "$1"; pwd)
UNISIM_TOOLS_DIR=${UNISIM_DIR}/unisim_tools
UNISIM_LIB_DIR=${UNISIM_DIR}/unisim_lib
UNISIM_SIMULATOR_DIR=${UNISIM_DIR}/unisim_simulators/cxx/${SIMPKG}

SIMULATOR_VERSION=$(cat ${UNISIM_SIMULATOR_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-${SIMPKG}-${SIMULATOR_VERSION}

if [ -z "${DISTCOPY}" ]; then
	DISTCOPY=cp
fi

has_to_build() {
	[ ! -e "$1" -o "$2" -nt "$1" ]
}

dist_copy() {
	if has_to_build "$2" "$1"; then
		echo "$1 ==> $2"
		mkdir -p "$(dirname $2)"
		${DISTCOPY} -f "$1" "$2" || exit
		true
	fi
	false
}

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/${SIMPKG}

UNISIM_TOOLS_GENISSLIB_FILES="${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES} ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES} ${UNISIM_TOOLS_GENISSLIB_DATA_FILES}"

for file in ${UNISIM_TOOLS_GENISSLIB_FILES}; do
	dist_copy "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/genisslib/${file}"
done

UNISIM_LIB_SIMULATOR_FILES="${UNISIM_LIB_SIMULATOR_SOURCE_FILES} ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_LIB_SIMULATOR_DATA_FILES}"

for file in ${UNISIM_LIB_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_EXTRA_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
${UNISIM_SIMULATOR_TESTBENCH_FILES} \
"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

for file in ${UNISIM_SIMULATOR_TOP_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/${SIMPKG}/config
mkdir -p ${DEST_DIR}/${SIMPKG}/m4
mkdir -p ${DEST_DIR}/genisslib/config
mkdir -p ${DEST_DIR}/genisslib/m4

# Some imported files (m4 macros) impact configure generation
has_to_build_genisslib_configure=no
has_to_build_simulator_configure=no

for file in ${UNISIM_TOOLS_GENISSLIB_M4_FILES}; do
	dist_copy "${UNISIM_TOOLS_DIR}/${file}" "${DEST_DIR}/genisslib/${file}" && has_to_build_genisslib_configure=yes
done

for file in ${UNISIM_LIB_SIMULATOR_M4_FILES}; do
	dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}" && has_to_build_simulator_configure=yes
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Yves Lhuillier <yves.lhuillier@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - mpc57_simtest: an MPC57 V5 user level simulator
  - GenISSLib (will not be installed): an instruction set simulator generator
See INSTALL for installation instructions.
EOF

cat << EOF > "${DEST_DIR}/INSTALL"
INSTALLATION
------------

Requirements:
  - GNU C++ compiler
  - GNU C++ standard library
  - GNU bash
  - GNU make
  - GNU autoconf
  - GNU automake
  - GNU flex
  - GNU bison
  - libxml2 (http://xmlsoft.org/libxml2) development package (libxml2-devel for Redhat/Mandriva, libxml2-dev for Debian/Ubuntu)


Building instructions:
  $ ./configure
  $ make

Installing (optional):
  $ make install
EOF

CONFIGURE_AC="${DEST_DIR}/configure.ac"
MAKEFILE_AM="${DEST_DIR}/Makefile.am"
CONFIGURE_CROSS="${DEST_DIR}/configure.cross"

has_to_build_configure=no

if has_to_build "${CONFIGURE_AC}" "$0"; then
	echo "Generating configure.ac"
	cat <<EOF > "${CONFIGURE_AC}"
AC_INIT([UNISIM FPV e5500 Floating-Point verification package], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}])
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects tar-pax])
AC_PATH_PROGS(SH, sh)
AC_PROG_INSTALL
AC_PROG_LN_S
AC_CONFIG_SUBDIRS([genisslib]) 
AC_CONFIG_SUBDIRS([${SIMPKG}]) 
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF
	has_to_build_configure=yes
fi

if has_to_build "${MAKEFILE_AM}" "$0"; then
	echo "Generating Makefile.am"
	cat <<EOF > "${MAKEFILE_AM}"
SUBDIRS=genisslib ${SIMPKG}
EXTRA_DIST = configure.cross
EOF
	has_to_build_configure=yes
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

if has_to_build "${CONFIGURE_CROSS}" "$0"; then
	echo "Building configure.cross"
	cat << EOF_CONFIGURE_CROSS > "${CONFIGURE_CROSS}"
#!/bin/bash
HERE=\$(pwd)
MY_DIR=\$(cd \$(dirname \$0); pwd)

# remove --host from command line arguments
host=""
help=""
i=0
j=0
for arg in "\$@"
do
	case "\${arg}" in
		--host=*)
			host=\$(printf "%s" "\${arg}" | cut -f 2- -d '=')
			;;
		--help=* | --help)
			help="yes"
			args[\${j}]=\${arg}
			j=\$((\${j}+1))
			;;
		*)
			args[\${j}]=\${arg}
			j=\$((\${j}+1))
			;;
	esac
	i=\$((\${i}+1))
done

if test "\${help}" != "yes"; then
	if test -z "\${host}"; then
		echo "ERROR: No canonical name for the host system type was specified. Use --host=<canonical name> to specify a host system type (e.g. --host=i586-pc-mingw32)"
		exit -1
	fi
fi

if test "\${help}" = "yes"; then
	echo "=== configure help for genisslib"
else
	echo "=== configuring in genisslib (\${HERE}/genisslib)"
	echo "\$(basename \$0): running \${MY_DIR}/genisslib/configure \${args[@]}"
fi
if test ! -d \${HERE}/genisslib; then
	mkdir "\${HERE}/genisslib"
fi
cd "\${HERE}/genisslib"
\${MY_DIR}/genisslib/configure --disable-option-checking "\${args[@]}"
STATUS="\$?"
cd "\${HERE}"
if test \${STATUS} -ne 0; then
	exit \${STATUS}
fi

if test "\${help}" = "yes"; then
	echo "=== configure help for ${SIMPKG}"
else
	echo "=== configuring in ${SIMPKG} (\${HERE}/${SIMPKG}) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/${SIMPKG}/configure \$@"
fi

if test ! -d \${HERE}/${SIMPKG}; then
	mkdir \${HERE}/${SIMPKG}
fi
cd \${HERE}/${SIMPKG}
\${MY_DIR}/${SIMPKG}/configure "\$@"
STATUS="\$?"
cd "\${HERE}"
if test \${STATUS} -ne 0; then
	exit \${STATUS}
fi

if test "\${help}" = "yes"; then
	exit 0
fi

echo "\$(basename \$0): creating Makefile.cross"
cat << EOF_MAKEFILE_CROSS > Makefile.cross
#!/usr/bin/make -f
all: ${SIMPKG}-all
clean: genisslib-clean ${SIMPKG}-clean
distclean: genisslib-distclean ${SIMPKG}-distclean
	rm -f \${HERE}/Makefile.cross
install: ${SIMPKG}-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
${SIMPKG}-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
${SIMPKG}-clean:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
${SIMPKG}-distclean:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} distclean
${SIMPKG}-install:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} install
EOF_MAKEFILE_CROSS

chmod +x Makefile.cross

echo "\$(basename \$0): run 'make -f \${HERE}/Makefile.cross' or '\${HERE}/Makefile.cross' to build for \${host} host system type"
EOF_CONFIGURE_CROSS
	chmod +x "${CONFIGURE_CROSS}"
fi  # has to build configure cross

# GENISSLIB

GENISSLIB_CONFIGURE_AC="${DEST_DIR}/genisslib/configure.ac"
GENISSLIB_MAKEFILE_AM="${DEST_DIR}/genisslib/Makefile.am"

if has_to_build "${GENISSLIB_CONFIGURE_AC}" "$0"; then
	echo "Generating GENISSLIB configure.ac"
	cat <<EOF > "${GENISSLIB_CONFIGURE_AC}"
AC_INIT([UNISIM GENISSLIB], [${GENISSLIB_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects tar-pax])
AC_PATH_PROGS(SH, sh)
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_LANG([C++])
AC_CHECK_HEADERS([${GENISSLIB_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))
UNISIM_CHECK_LEXER_GENERATOR
UNISIM_CHECK_PARSER_GENERATOR
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF
	has_to_build_genisslib_configure=yes
fi

if has_to_build "${GENISSLIB_MAKEFILE_AM}" "$0"; then
	AM_GENISSLIB_VERSION=$(printf ${GENISSLIB_VERSION} | sed -e 's/\./_/g')
	echo "Generating GENISSLIB Makefile.am"
	cat <<EOF > "${GENISSLIB_MAKEFILE_AM}"
ACLOCAL_AMFLAGS=-I m4
BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}
CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}
AM_YFLAGS = -d -p yy
AM_LFLAGS = -l
AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)
noinst_PROGRAMS = genisslib
genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}
genisslib_CPPFLAGS = -DGENISSLIB_VERSION=\"${GENISSLIB_VERSION}\"
genisslib_CXXFLAGS = -O1 -Wno-error
noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}
EXTRA_DIST = ${UNISIM_TOOLS_GENISSLIB_M4_FILES}
# The following lines are a workaround caused by a bugFix in AUTOMAKE 1.12
# Note that parser_tokens.hh has been added to BUILT_SOURCES above
# assumption: parser.cc and either parser.h or parser.hh are generated at the same time
\$(top_builddir)/parser_tokens.hh: \$(top_builddir)/parser.cc
	if test -f "\$(top_builddir)/parser.h"; then\
		cp -f "\$(top_builddir)/parser.h" "\$(top_builddir)/parser_tokens.hh";\
	elif test -f "\$(top_builddir)/parser.hh"; then\
		cp -f "\$(top_builddir)/parser.hh" "\$(top_builddir)/parser_tokens.hh";\
	fi
# The following line disable some C++ flags that prevent the flex generated file to compile properly
\$(top_builddir)/genisslib-scanner.o: CXXFLAGS += -O1 -Wno-error
EOF
	has_to_build_genisslib_configure=yes
fi

if [ "${has_to_build_genisslib_configure}" = "yes" ]; then
	echo "Building GENISSLIB configure"
	${SHELL} -c "cd ${DEST_DIR}/genisslib && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

# Simulator

SIMULATOR_CONFIGURE_AC="${DEST_DIR}/${SIMPKG}/configure.ac"
SIMULATOR_MAKEFILE_AM="${DEST_DIR}/${SIMPKG}/Makefile.am"

if has_to_build "${SIMULATOR_CONFIGURE_AC}" "$0"; then
	echo "Generating ${SIMPKG} configure.ac"
	cat <<EOF > "${SIMULATOR_CONFIGURE_AC}"
AC_INIT([UNISIM FPV e5500 Floating-Point verification], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}-core])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects tar-pax])
AC_PATH_PROGS(SH, sh)
AC_PROG_CXX
CXXFLAGS="\$CXXFLAGS -std=c++11"
AC_PROG_INSTALL
LT_INIT
AC_SUBST(LIBTOOL_DEPS)
AC_PROG_LN_S
AC_LANG([C++])
AM_PROG_CC_C_O
AC_CHECK_HEADERS([${SIMULATOR_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))
case "\${host}" in
	*mingw*)
		CPPFLAGS="-U__STRICT_ANSI__ \${CPPFLAGS}"
		;;
	*)
		;;
esac
UNISIM_CHECK_LIBXML2(main)
GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib
AC_SUBST(GENISSLIB_PATH)
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/unisim-${SIMPKG}-${SIMULATOR_VERSION}"], [path of shared data relative to bin directory])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF
	has_to_build_simulator_configure=yes
fi

if has_to_build "${SIMULATOR_MAKEFILE_AM}" "$0"; then
	AM_SIMULATOR_VERSION=$(printf ${SIMULATOR_VERSION} | sed -e 's/\./_/g')
	echo "Generating ${SIMPKG} Makefile.am"
	cat <<EOF > "${SIMULATOR_MAKEFILE_AM}"
ACLOCAL_AMFLAGS=-I m4
AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: \$(LIBTOOL_DEPS)
	\$(SHELL) ./config.status libtool

# Program
bin_PROGRAMS = unisim-${SIMPKG}-${SIMULATOR_VERSION}
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_SIMULATOR_SOURCE_FILES}
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_LDFLAGS = -static-libtool-libs
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la

# Static Library
noinst_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES}
libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -static
nodist_libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = top_ppc64.cc

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/top_ppc64.hh\
	\$(top_builddir)/top_ppc64.cc\

CLEANFILES=\
	\$(top_builddir)/top_ppc64.hh\
	\$(top_builddir)/top_ppc64.cc\

\$(top_builddir)/top_ppc64.cc: \$(top_builddir)/top_ppc64.hh
\$(top_builddir)/top_ppc64.hh: ${UNISIM_SIMULATOR_ISA_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/top_ppc64 -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_ppc64.isa

EOF
	has_to_build_simulator_configure=yes
fi

if [ "${has_to_build_simulator_configure}" = "yes" ]; then
	echo "Building ${SIMPKG} configure"
	${SHELL} -c "cd ${DEST_DIR}/${SIMPKG} && aclocal -I m4 && libtoolize --force && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"

