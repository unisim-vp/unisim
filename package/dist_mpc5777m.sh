#!/bin/bash
function Usage
{
	echo "Usage:"
	echo "  $0 <destination directory>"
}

if [ -z "$1" ]; then
	Usage
	exit -1
fi

HERE=$(pwd)
MY_DIR=$(cd $(dirname $0); pwd)
DEST_DIR=$1
UNISIM_TOOLS_DIR=${MY_DIR}/../unisim_tools
UNISIM_LIB_DIR=${MY_DIR}/../unisim_lib
UNISIM_SIMULATORS_DIR=${MY_DIR}/../unisim_simulators/tlm2/mpc5777m

MPC5777M_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-mpc5777m-${MPC5777M_VERSION}

if test -z "${DISTCOPY}"; then
    DISTCOPY=cp
fi

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
subdecoder.hh"

UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES="\
scanner.cc \
parser.cc \
parser_tokens.hh"

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
errtools.cc"

UNISIM_TOOLS_GENISSLIB_DATA_FILES="COPYING INSTALL NEWS README AUTHORS ChangeLog"

UNISIM_TOOLS_GENISSLIB_M4_FILES="\
m4/lexer.m4 \
m4/parser_gen.m4"

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
vector"

UNISIM_LIB_MPC5777M_SOURCE_FILES="\
unisim/kernel/service/service.cc \
unisim/kernel/service/xml_helper.cc \
unisim/kernel/tlm2/tlm.cc \
unisim/kernel/tlm2/simulator.cc \
unisim/kernel/tlm2/clock.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/util/backtrace/backtrace.cc \
unisim/kernel/api/api.cc \
unisim/util/xml/xml.cc \
unisim/util/debug/profile_32.cc \
unisim/util/debug/profile_64.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/symbol_table_64.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/watchpoint_registry_64.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/debug/breakpoint_registry_64.cc \
unisim/util/debug/type.cc \
unisim/util/debug/stmt_32.cc \
unisim/util/debug/stmt_64.cc \
unisim/util/debug/dwarf/abbrev.cc \
unisim/util/debug/dwarf/attr.cc \
unisim/util/debug/dwarf/class.cc \
unisim/util/debug/dwarf/dwarf32.cc \
unisim/util/debug/dwarf/dwarf64.cc \
unisim/util/debug/dwarf/encoding.cc \
unisim/util/debug/dwarf/filename.cc \
unisim/util/debug/dwarf/leb128.cc \
unisim/util/debug/dwarf/ml.cc \
unisim/util/debug/dwarf/register_number_mapping.cc \
unisim/util/debug/dwarf/data_object.cc \
unisim/util/debug/dwarf/c_loc_expr_parser.cc \
unisim/util/blob/blob32.cc \
unisim/util/blob/blob64.cc \
unisim/util/blob/section32.cc \
unisim/util/blob/section64.cc \
unisim/util/blob/segment32.cc \
unisim/util/blob/segment64.cc \
unisim/util/debug/elf_symtab/elf_symtab32.cc \
unisim/util/debug/elf_symtab/elf_symtab64.cc \
unisim/util/debug/coff_symtab/coff_symtab32.cc \
unisim/kernel/service/endian.cc \
unisim/util/queue/queue.cc \
unisim/util/garbage_collector/garbage_collector.cc \
unisim/util/random/random.cc \
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/loader/elf_loader/elf64_loader.cc \
unisim/util/loader/coff_loader/coff_loader32.cc \
unisim/util/os/linux_os/environment.cc \
unisim/util/os/linux_os/linux.cc \
unisim/util/lexer/lexer.cc \
unisim/util/ieee754/ieee754.cc \
unisim/util/reg/core/register.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/debug/debugger/debugger32.cc \
unisim/service/debug/profiler/profiler.cc \
unisim/service/debug/profiler/profiler32.cc \
unisim/service/time/host_time/time.cc \
unisim/service/time/sc_time/time.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/loader/elf_loader/elf64_loader.cc \
unisim/service/loader/raw_loader/raw_loader32.cc \
unisim/service/loader/s19_loader/s19_loader.cc \
unisim/service/loader/coff_loader/coff_loader32.cc \
unisim/service/loader/multiformat_loader/multiformat_loader.cc \
unisim/service/loader/multiformat_loader/multiformat_loader32.cc \
unisim/service/tee/loader/tee.cc \
unisim/service/tee/symbol_table_lookup/tee_32.cc \
unisim/service/tee/blob/tee_32.cc \
unisim/service/tee/stmt_lookup/tee_32.cc \
unisim/service/tee/backtrace/tee_32.cc \
unisim/service/telnet/telnet.cc \
unisim/service/os/linux_os/powerpc_linux32.cc \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/floating.cc \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/cpu.cc \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/cpu.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc \
unisim/component/tlm2/memory/ram/memory.cc \
unisim/component/tlm2/memory/ram/memory_debug.cc \
unisim/component/tlm2/interconnect/generic_router/variable_mapping.cc \
unisim/component/tlm2/com/serial_terminal/serial_terminal.cc \
"

UNISIM_LIB_MPC5777M_ISA_FILES="\
unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.isa \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/integer.hh \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/add.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/adde.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addme.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addze.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/and.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/andc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmp.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpl.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cntlzw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divw_sat.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divwu_sat.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/eqv.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/extsb.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/extsh.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/isel.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lbzux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lbzx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhaux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhax.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhzux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhzx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwzux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwzx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mcrxr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mfcr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mtcrf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mulhw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mulhwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mullw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/nand.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/neg.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/nor.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/or.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/orc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/slw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sraw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/srawi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/srw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stbux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stbx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sthbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sthux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sthx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stwbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stwux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stwx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfe.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfme.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfze.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/tw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/xor.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsabs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsadd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efscfh.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efscfsf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efscfsi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efscfuf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efscfui.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efscmpeq.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efscmpgt.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efscmplt.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efscth.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsctsf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsctsi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsctsiz.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsctuf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsctui.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsctuiz.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsdiv.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsmadd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsmsub.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsmax.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsmin.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsmul.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsnabs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsneg.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsnmadd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsnmsub.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efssqrt.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efssub.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efststeq.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efststgt.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efststlt.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcba.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcbf.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcbst.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcbt.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcbtst.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/dcbz.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/icbi.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/icbt.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/lbarx.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/lharx.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/lwarx.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/mbar.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/mtmsr.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/stbcx_.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/sthcx_.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/stwcx_.isa \
unisim/component/cxx/processor/powerpc/isa/book_ii/wait.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/dcbi.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mfdcr.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mtdcr.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mfmsr.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mfspr.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/mtspr.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/wrtee.isa \
unisim/component/cxx/processor/powerpc/isa/book_iii_e/wrteei.isa \
unisim/component/cxx/processor/powerpc/isa/book_e/msync.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/vle.hh \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_add.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_addi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_and.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_andc.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_andi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_bc.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_bclri.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_bctr.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_bgeni.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_b.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_blr.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_bmaski.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_bseti.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_btsti.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_cmp.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_cmph.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_cmphl.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_cmpi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_cmpl.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_cmpli.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_dnh.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_extsb.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_extsh.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_extzb.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_extzh.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_illegal.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_isync.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_lbz.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_lhz.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_li.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_lwz.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_mfar.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_mfctr.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_mflr.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_mr.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_mtar.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_mtctr.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_mtlr.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_mullw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_neg.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_not.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_or.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_rfci.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_rfdi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_rfi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_rfmci.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_sc.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_slw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_slwi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_sraw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_srawi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_srw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_srwi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_stb.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_sth.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_stw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_sub.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_subf.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/se_subi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_add16i.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_add2i_.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_add2is.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_addi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_addic.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_and2i_.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_and2is_.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_andi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_b.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_bc.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_cmp16i.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_cmph.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_cmph16i.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_cmphl.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_cmphl16i.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_cmpi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_cmpl16i.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_cmpli.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_crand.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_crandc.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_creqv.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_crnand.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_crnor.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_cror.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_crorc.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_crxor.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_dnh.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lbz.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lbzu.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lha.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lhau.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lhz.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lhzu.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_li.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lis.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lmw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lwz.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lwzu.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_mcrf.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_mull2i.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_mulli.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_or2i.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_or2is.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_ori.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_rlw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_rlwi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_rlwimi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_rlwinm.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_sc.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_slwi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_srwi.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_stb.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_stbu.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_sth.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_sthu.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_stmw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_stw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_stwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_subfic.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_xori.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lmvgprw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_stmvgprw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lmvsprw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_stmvsprw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lmvsrrw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_stmvsrrw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lmvcsrrw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_stmvcsrrw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lmvdsrrw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_stmvdsrrw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_lmvmcsrrw.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/e_stmvmcsrrw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/lsp.hh \
unisim/component/cxx/processor/powerpc/isa/lsp/zabsw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zabsws.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zaddd.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zadddss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zadddus.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zaddhesw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zaddheuw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zaddhosw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zaddhouw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zaddwgsf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zaddwgsi.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zaddwgui.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zaddwss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zaddwus.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zbrminc.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zcircinc.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zcntlsw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zdivwsf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zldd.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlddx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zldh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zldhx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zldw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zldwx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlhgwsf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlhgwsfx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlhhe.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlhhex.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlhhos.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlhhosx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlhhou.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlhhoux.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlhhsplat.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlhhsplatx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwgsfd.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwgsfdx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhed.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhedx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhgwsfd.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhgwsfdx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhosd.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhosdx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhoud.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhoudx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhsplatd.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhsplatdx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhsplatwd.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhsplatwdx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwhx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlww.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwwosd.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwwosdx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zlwwx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmhg.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmheogwsmf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmhegwsmf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmhogwsmf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmhias.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmhi.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmheosfras.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmhesfras.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmhosfras.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmhsfas.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmheosfr.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmhesfr.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmhosfr.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmhsf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwgias.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwgi.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwgsmfr.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwgsmf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwlsuis.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwlsis.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwluia.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwluis.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwsfras.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwsfas.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwsfr.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zmwsf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/znegws.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zpkswgshfrs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zpkswgswfrs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zrndwh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zrndwhss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsatsdsw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsatsduw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsatswsh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsatswuh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsatswuw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsatuduw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsatuwsh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsatuwsw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsatuwuh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zslwiss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zslwius.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zslwss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zslwus.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstdd.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstddx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstdh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstdhx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstdw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstdwx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsthe.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsthex.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstho.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsthox.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstwhed.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstwhedx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstwh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstwhod.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstwhodx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstwhx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstww.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zstwwx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfd.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfdss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfdus.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfhesw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfheuw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfhosw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfhouw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfwgsf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfwgsi.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfwgui.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfwss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zsubfwus.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zunpkwgsf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvabsh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvabshs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddhss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddhus.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddhx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddhxss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddih.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddsubfh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddsubfhss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddsubfhx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddsubfhxss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddsubfw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddsubfwss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddwss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvaddwus.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvcmpeqh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvcmpgths.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvcmpgthu.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvcmplths.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvcmplthu.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvcntlsh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvcntlzh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvdotphg.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvdotphgwasmf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvdotphgwssmf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvdotphi.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvdotphsfrs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvdotphsfs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvdotphxgwasmf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmergehih.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmergehiloh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmergelohih.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmergeloh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhias.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhi.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhllgwsmf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhsfraahs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhsfranhs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhsfaahs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhsfanhs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhsfras.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhsfas.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhsfrh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhsfh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhsfr.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhsf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhsihs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhuihs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhuih.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhulgwsmf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhuugwsmf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvmhxlgwsmf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvnegh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvnegho.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvneghos.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvneghs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvpkshgwshfrs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvpkswshfrs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvpkswshs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvpkswuhs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvpkuwuhs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvrlhi.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvrlh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsatshuh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsatuhsh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvselh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvslhi.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvslh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvslhiss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvslhius.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvslhss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvslhus.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsplatfih.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsplatih.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsrhis.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsrhiu.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsrhs.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsrhu.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfaddh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfaddhss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfaddhx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfaddhxss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfaddw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfaddwss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfhss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfhus.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfhx.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfhxss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfw.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfwss.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubfwus.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvsubifh.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvunpkhgwsf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvunpkhsf.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvunpkhsi.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zvunpkhui.isa \
unisim/component/cxx/processor/powerpc/isa/lsp/zxtrw.isa \
unisim/component/cxx/processor/powerpc/isa/mpu/mpure.isa \
unisim/component/cxx/processor/powerpc/isa/mpu/mpusync.isa \
unisim/component/cxx/processor/powerpc/isa/mpu/mpuwe.isa \
"

UNISIM_LIB_MPC5777M_HEADER_FILES="${UNISIM_LIB_MPC5777M_ISA_FILES} \
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/tlm2/tlm.hh \
unisim/kernel/tlm2/simulator.hh \
unisim/kernel/tlm2/clock.hh \
unisim/kernel/tlm2/master_id.hh \
unisim/kernel/tlm2/tlm_serial.hh \
unisim/util/backtrace/backtrace.hh \
unisim/kernel/api/api.hh \
unisim/util/likely/likely.hh \
unisim/util/inlining/inlining.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/debug/memory_access_type.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/event.hh \
unisim/util/debug/profile.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/stmt.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/data_object.hh \
unisim/util/debug/type.hh \
unisim/util/debug/data_object_initializer.hh \
unisim/util/debug/subprogram.hh \
unisim/util/debug/fetch_insn_event.hh \
unisim/util/debug/commit_insn_event.hh \
unisim/util/debug/trap_event.hh \
unisim/util/debug/dwarf/abbrev.hh \
unisim/util/debug/dwarf/attr.hh \
unisim/util/debug/dwarf/call_frame_vm.hh \
unisim/util/debug/dwarf/class.hh \
unisim/util/debug/dwarf/die.hh \
unisim/util/debug/dwarf/encoding.hh \
unisim/util/debug/dwarf/fde.hh \
unisim/util/debug/dwarf/fmt.hh \
unisim/util/debug/dwarf/version.hh \
unisim/util/debug/dwarf/option.hh \
unisim/util/debug/dwarf/cfa.hh \
unisim/util/debug/dwarf/leb128.hh \
unisim/util/debug/dwarf/macinfo.hh \
unisim/util/debug/dwarf/pub.hh \
unisim/util/debug/dwarf/stmt_prog.hh \
unisim/util/debug/dwarf/addr_range.hh \
unisim/util/debug/dwarf/call_frame_prog.hh \
unisim/util/debug/dwarf/cie.hh \
unisim/util/debug/dwarf/cu.hh \
unisim/util/debug/dwarf/dwarf.hh \
unisim/util/debug/dwarf/expr_vm.hh \
unisim/util/debug/dwarf/filename.hh \
unisim/util/debug/dwarf/fwd.hh \
unisim/util/debug/dwarf/loc.hh \
unisim/util/debug/dwarf/ml.hh \
unisim/util/debug/dwarf/range.hh \
unisim/util/debug/dwarf/stmt_vm.hh \
unisim/util/debug/dwarf/register_number_mapping.hh \
unisim/util/debug/dwarf/frame.hh \
unisim/util/debug/dwarf/util.hh \
unisim/util/debug/dwarf/data_object.hh \
unisim/util/debug/dwarf/subprogram.hh \
unisim/util/debug/dwarf/c_loc_expr_parser.hh \
unisim/util/blob/blob.hh \
unisim/util/blob/section.hh \
unisim/util/blob/segment.hh \
unisim/util/debug/elf_symtab/elf_symtab.hh \
unisim/util/debug/coff_symtab/coff_symtab.hh \
unisim/util/endian/endian.hh \
unisim/util/garbage_collector/garbage_collector.hh \
unisim/util/hash_table/hash_table.hh \
unisim/util/xml/xml.hh \
unisim/util/queue/queue.hh \
unisim/util/simfloat/floating.hh \
unisim/util/simfloat/integer.hh \
unisim/util/simfloat/host_floating.hh \
unisim/util/ieee754/ieee754.hh \
unisim/util/device/register.hh \
unisim/util/random/random.hh \
unisim/util/allocator/allocator.hh \
unisim/util/loader/elf_loader/elf_common.h \
unisim/util/loader/elf_loader/elf_loader.hh \
unisim/util/loader/elf_loader/elf32.h \
unisim/util/loader/elf_loader/elf64.h \
unisim/util/loader/elf_loader/elf32_loader.hh \
unisim/util/loader/elf_loader/elf64_loader.hh \
unisim/util/loader/coff_loader/coff_loader.hh \
unisim/util/loader/coff_loader/ti/ti.hh \
unisim/util/os/linux_os/aux_table.hh \
unisim/util/os/linux_os/environment.hh \
unisim/util/os/linux_os/files_flags.hh \
unisim/util/os/linux_os/linux.hh \
unisim/util/os/linux_os/ppc.hh \
unisim/util/os/linux_os/errno.hh \
unisim/util/dictionary/dictionary.hh \
unisim/util/lexer/lexer.hh \
unisim/util/parser/parser.hh \
unisim/util/reg/core/register.hh \
unisim/util/cache/cache.hh \
unisim/service/interfaces/debug_yielding.hh \
unisim/service/interfaces/debug_selecting.hh \
unisim/service/interfaces/debug_event.hh \
unisim/service/interfaces/debug_info_loading.hh \
unisim/service/interfaces/memory_access_reporting.hh \
unisim/service/interfaces/disassembly.hh \
unisim/service/interfaces/loader.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/symbol_table_lookup.hh \
unisim/service/interfaces/stmt_lookup.hh \
unisim/service/interfaces/time.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/profiling.hh \
unisim/service/interfaces/register.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/linux_os.hh \
unisim/service/interfaces/synchronizable.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/interfaces/blob.hh \
unisim/service/interfaces/char_io.hh \
unisim/service/interfaces/keyboard.hh \
unisim/service/interfaces/led_board.hh \
unisim/service/interfaces/backtrace.hh \
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/interfaces/subprogram_lookup.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/debug/debugger/debugger.hh \
unisim/service/debug/profiler/profiler.hh \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/elf_loader/elf64_loader.hh \
unisim/service/loader/raw_loader/raw_loader.hh \
unisim/service/loader/s19_loader/s19_loader.hh \
unisim/service/loader/coff_loader/coff_loader.hh \
unisim/service/loader/multiformat_loader/multiformat_loader.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/service/tee/loader/tee.hh \
unisim/service/tee/symbol_table_lookup/tee.hh \
unisim/service/tee/blob/tee.hh \
unisim/service/tee/stmt_lookup/tee.hh \
unisim/service/tee/backtrace/tee.hh \
unisim/service/telnet/telnet.hh \
unisim/service/os/linux_os/linux.hh \
unisim/service/os/linux_os/powerpc_linux32.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/processor/powerpc/cpu.hh \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/cpu.hh \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/mpu.hh \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/imem.hh \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/dmem.hh \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/l1i.hh \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/l1d.hh \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/floating.hh \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/genfflags.hh \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/cpu.hh \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/cpu.hh \
unisim/component/tlm2/processor/powerpc/e200/mpc57xx/cpu.hh \
unisim/component/tlm2/processor/powerpc/e200/mpc57xx/e200z710n3/cpu.hh \
unisim/component/tlm2/processor/powerpc/e200/mpc57xx/e200z425bn3/cpu.hh \
unisim/component/tlm2/memory/ram/memory.hh \
unisim/component/tlm2/interconnect/generic_router/router.hh \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.hh \
unisim/component/tlm2/interconnect/generic_router/config.hh \
unisim/component/tlm2/interrupt/freescale/mpc57xx/intc/intc.hh \
unisim/component/tlm2/timer/freescale/mpc57xx/stm/stm.hh \
unisim/component/tlm2/watchdog/freescale/mpc57xx/swt/swt.hh \
unisim/component/tlm2/timer/freescale/mpc57xx/pit/pit.hh \
unisim/component/tlm2/com/freescale/mpc57xx/linflexd/linflexd.hh \
unisim/component/tlm2/com/serial_terminal/serial_terminal.hh \
"

UNISIM_LIB_MPC5777M_TEMPLATE_FILES="\
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/profile.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/util/debug/stmt.tcc \
unisim/util/debug/data_object_initializer.tcc \
unisim/util/debug/dwarf/addr_range.tcc \
unisim/util/debug/dwarf/call_frame_prog.tcc \
unisim/util/debug/dwarf/cie.tcc \
unisim/util/debug/dwarf/die.tcc \
unisim/util/debug/dwarf/expr_vm.tcc \
unisim/util/debug/dwarf/loc.tcc \
unisim/util/debug/dwarf/pub.tcc \
unisim/util/debug/dwarf/stmt_prog.tcc \
unisim/util/debug/dwarf/attr.tcc \
unisim/util/debug/dwarf/call_frame_vm.tcc \
unisim/util/debug/dwarf/cu.tcc \
unisim/util/debug/dwarf/dwarf.tcc \
unisim/util/debug/dwarf/fde.tcc \
unisim/util/debug/dwarf/macinfo.tcc \
unisim/util/debug/dwarf/range.tcc \
unisim/util/debug/dwarf/stmt_vm.tcc \
unisim/util/debug/dwarf/frame.tcc \
unisim/util/debug/dwarf/data_object.tcc \
unisim/util/debug/dwarf/subprogram.tcc \
unisim/util/blob/blob.tcc \
unisim/util/blob/section.tcc \
unisim/util/blob/segment.tcc \
unisim/util/debug/elf_symtab/elf_symtab.tcc \
unisim/util/debug/coff_symtab/coff_symtab.tcc \
unisim/util/queue/queue.tcc \
unisim/util/simfloat/floating.tcc \
unisim/util/simfloat/integer.tcc \
unisim/util/simfloat/host_floating.tcc \
unisim/util/loader/elf_loader/elf_loader.tcc \
unisim/util/loader/coff_loader/coff_loader.tcc \
unisim/util/loader/coff_loader/ti/ti.tcc \
unisim/util/os/linux_os/calls.tcc \
unisim/util/os/linux_os/linux.tcc \
unisim/util/dictionary/dictionary.tcc \
unisim/util/lexer/lexer.tcc \
unisim/util/parser/parser.tcc \
unisim/util/reg/core/register.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/debug/debugger/debugger.tcc \
unisim/service/debug/profiler/profiler.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/loader/elf_loader/elf32_loader.tcc \
unisim/service/loader/elf_loader/elf64_loader.tcc \
unisim/service/loader/raw_loader/raw_loader.tcc \
unisim/service/loader/s19_loader/s19_loader.tcc \
unisim/service/loader/coff_loader/coff_loader.tcc \
unisim/service/loader/multiformat_loader/multiformat_loader.tcc \
unisim/service/tee/loader/tee.tcc \
unisim/service/tee/symbol_table_lookup/tee.tcc \
unisim/service/tee/blob/tee.tcc \
unisim/service/tee/stmt_lookup/tee.tcc \
unisim/service/tee/backtrace/tee.tcc \
unisim/service/os/linux_os/linux.tcc \
unisim/service/os/linux_os/powerpc_linux32.tcc \
unisim/component/cxx/processor/powerpc/cpu.tcc \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/cpu.tcc \
unisim/component/cxx/memory/ram/memory.tcc \
unisim/component/tlm2/processor/powerpc/e200/mpc57xx/cpu.tcc \
unisim/component/tlm2/memory/ram/memory.tcc \
unisim/component/tlm2/interconnect/generic_router/router.tcc \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.tcc \
unisim/component/tlm2/interrupt/freescale/mpc57xx/intc/intc.tcc \
unisim/component/tlm2/timer/freescale/mpc57xx/stm/stm.tcc \
unisim/component/tlm2/watchdog/freescale/mpc57xx/swt/swt.tcc \
unisim/component/tlm2/timer/freescale/mpc57xx/pit/pit.tcc \
unisim/component/tlm2/com/freescale/mpc57xx/linflexd/linflexd.tcc \
"

UNISIM_LIB_MPC5777M_M4_FILES="\
m4/times.m4 \
m4/endian.m4 \
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/systemc.m4 \
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4 \
m4/pthread.m4"

UNISIM_LIB_MPC5777M_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_powerpc_vle.xml \
unisim/util/debug/dwarf/powerpc_e500_dwarf_register_number_mapping.xml \
unisim/util/debug/dwarf/powerpc_e500_gcc_dwarf_register_number_mapping.xml"

MPC5777M_EXTERNAL_HEADERS="\
assert.h \
ctype.h \
cxxabi.h \
errno.h \
fcntl.h \
fenv.h \
float.h \
getopt.h \
inttypes.h \
limits.h \
math.h \
signal.h \
stdarg.h \
stdio.h \
stdlib.h \
string.h \
ctype.h \
sys/types.h \
unistd.h \
fstream \
cassert \
cmath \
cerrno \
cstddef \
cstdio \
cstdlib \
cstring \
iomanip \
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
set"

UNISIM_SIMULATORS_MPC5777M_SOURCE_FILES="\
simulator.cc \
debugger.cc \
xbar_0.cc \
xbar_1.cc \
pbridge_a.cc \
pbridge_b.cc \
intc_0.cc \
stm_0.cc \
stm_1.cc \
stm_2.cc \
swt_0.cc \
swt_1.cc \
swt_2.cc \
swt_3.cc \
pit_0.cc \
pit_1.cc \
linflexd_0.cc \
linflexd_1.cc \
linflexd_2.cc \
linflexd_14.cc \
linflexd_15.cc \
linflexd_16.cc \
"
UNISIM_SIMULATORS_MPC5777M_HEADER_FILES="\
simulator.hh \
"

UNISIM_SIMULATORS_MPC5777M_TEMPLATE_FILES="\
"

UNISIM_SIMULATORS_MPC5777M_EXTRA_FILES="\
trace32-core0.cmm.in \
trace32-core1.cmm.in \
trace32-core2.cmm.in \
trace32-multi.cmm.in \
sim_gtkwave.sh.in \
"

UNISIM_SIMULATORS_MPC5777M_TOP_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
"

UNISIM_SIMULATORS_MPC5777M_DATA_FILES="\
COPYING \
README \
INSTALL \
AUTHORS \
NEWS \
ChangeLog \
unisim.ico \
soft/Makefile \
soft/include/typedefs.h \
soft/include/MPC5777M.h \
soft/include/derivative.h \
soft/include/compiler_api.h \
soft/include/intc.h \
soft/include/stm.h \
soft/include/swt.h \
soft/include/pit.h \
soft/include/linflexd.h \
soft/src/Z4_2/57xx_ram.ld \
soft/src/Z4_2/startup.S \
soft/src/Z4_2/main_Z4_2.c \
soft/src/Z4_2/vector.S \
soft/src/Z4_2/57xx_flash.ld \
soft/src/Z4_2/intc_sw_handlers.S \
soft/src/Z4_2/flashrchw.c \
soft/src/Z7_0/57xx_ram.ld \
soft/src/Z7_0/startup.S \
soft/src/Z7_0/main_Z7_0.c \
soft/src/Z7_0/vector.S \
soft/src/Z7_0/57xx_flash.ld \
soft/src/Z7_0/intc_sw_handlers.S \
soft/src/Z7_1/57xx_ram.ld \
soft/src/Z7_1/startup.S \
soft/src/Z7_1/main_Z7_1.c \
soft/src/Z7_1/vector.S \
soft/src/Z7_1/57xx_flash.ld \
soft/src/Z7_1/intc_sw_handlers.S \
soft/src/drv/intc.c \
soft/src/drv/stm.c \
soft/src/drv/swt.c \
soft/src/drv/pit.c \
soft/src/drv/linflexd.c \
soft/bin/Z4_2/ram_boot.elf \
soft/bin/Z4_2/flash_boot.elf \
soft/bin/Z7_0/ram_boot.elf \
soft/bin/Z7_0/flash_boot.elf \
soft/bin/Z7_1/ram_boot.elf \
soft/bin/Z7_1/flash_boot.elf \
sim_config.xml \
gtkwave_init_script.tcl \
.gtkwaverc \
gtkwave.ini \
config.t32 \
baf.bin \
"

UNISIM_SIMULATORS_MPC5777M_TESTBENCH_FILES="\
main.cc \
"

has_to_build_configure=no
has_to_build_configure_cross=no
has_to_build_genisslib_configure=no
has_to_build_mpc5777m_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/mpc5777m

UNISIM_TOOLS_GENISSLIB_FILES="${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES} ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES} ${UNISIM_TOOLS_GENISSLIB_DATA_FILES}"

for file in ${UNISIM_TOOLS_GENISSLIB_FILES}; do
	mkdir -p "${DEST_DIR}/$(dirname ${file})"
	has_to_copy=no
	if [ -e "${DEST_DIR}/genisslib/${file}" ]; then
		if [ "${UNISIM_TOOLS_DIR}/genisslib/${file}" -nt "${DEST_DIR}/genisslib/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_TOOLS_DIR}/genisslib/${file} ==> ${DEST_DIR}/genisslib/${file}"
		${DISTCOPY} -f "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/genisslib/${file}" || exit
	fi
done

UNISIM_LIB_MPC5777M_FILES="${UNISIM_LIB_MPC5777M_SOURCE_FILES} ${UNISIM_LIB_MPC5777M_HEADER_FILES} ${UNISIM_LIB_MPC5777M_TEMPLATE_FILES} ${UNISIM_LIB_MPC5777M_DATA_FILES}"

for file in ${UNISIM_LIB_MPC5777M_FILES}; do
	mkdir -p "${DEST_DIR}/mpc5777m/$(dirname ${file})"
	has_to_copy=no
	if [ -e "${DEST_DIR}/mpc5777m/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/mpc5777m/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/mpc5777m/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/mpc5777m/${file}" || exit
	fi
done

UNISIM_SIMULATORS_MPC5777M_FILES="${UNISIM_SIMULATORS_MPC5777M_SOURCE_FILES} ${UNISIM_SIMULATORS_MPC5777M_HEADER_FILES} ${UNISIM_SIMULATORS_MPC5777M_TEMPLATE_FILES} ${UNISIM_SIMULATORS_MPC5777M_EXTRA_FILES} ${UNISIM_SIMULATORS_MPC5777M_DATA_FILES} ${UNISIM_SIMULATORS_MPC5777M_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATORS_MPC5777M_FILES}; do
	mkdir -p "${DEST_DIR}/mpc5777m/$(dirname ${file})"
	has_to_copy=no
	if [ -e "${DEST_DIR}/mpc5777m/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/mpc5777m/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/mpc5777m/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/mpc5777m/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_MPC5777M_TOP_DATA_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/${file}" || exit
	fi
done


mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/mpc5777m/config
mkdir -p ${DEST_DIR}/mpc5777m/m4
mkdir -p ${DEST_DIR}/genisslib/config
mkdir -p ${DEST_DIR}/genisslib/m4

for file in ${UNISIM_TOOLS_GENISSLIB_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/genisslib/${file}" ]; then
		if [ "${UNISIM_TOOLS_DIR}/${file}" -nt  "${DEST_DIR}/genisslib/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_TOOLS_DIR}/${file} ==> ${DEST_DIR}/genisslib/${file}"
		${DISTCOPY} -f "${UNISIM_TOOLS_DIR}/${file}" "${DEST_DIR}/genisslib/${file}" || exit
		has_to_build_genisslib_configure=yes
	fi
done

for file in ${UNISIM_LIB_MPC5777M_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/mpc5777m/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/mpc5777m/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/mpc5777m/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/mpc5777m/${file}" || exit
		has_to_build_mpc5777m_configure=yes
	fi
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Gilles Mouchard <gilles.mouchard@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - UNISIM GenISSLib: an instruction set simulator generator
  - UNISIM MPC5777M Simulator: a MPC5777M SoC simulator.
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
  - boost (http://www.boost.org) development package (libboost-devel for Redhat/Mandriva, libboost-graph-dev for Debian/Ubuntu)
  - libxml2 (http://xmlsoft.org/libxml2) development package (libxml2-devel for Redhat/Mandriva, libxml2-dev for Debian/Ubuntu)
  - zlib (http://www.zlib.net) development package (zlib1g-devel for Redhat/Mandriva, zlib1g-devel for Debian/Ubuntu)
  - libedit (http://www.thrysoee.dk/editline) development package (libedit-devel for Redhat/Mandriva, libedit-dev for Debian/Ubuntu)
  - Core SystemC Language >= 2.3 (http://www.systemc.org)

Building instructions:
  $ ./configure --with-systemc=<path-to-systemc-install-dir>
  $ make

Installing (optional):
  $ make install
EOF

CONFIGURE_AC="${DEST_DIR}/configure.ac"
MAKEFILE_AM="${DEST_DIR}/Makefile.am"
CONFIGURE_CROSS="${DEST_DIR}/configure.cross"

if [ ! -e "${CONFIGURE_AC}" ]; then
	has_to_build_configure=yes
else
	if [ "$0" -nt "${CONFIGURE_AC}" ]; then
		has_to_build_configure=yes
	fi
fi

if [ ! -e "${MAKEFILE_AM}" ]; then
	has_to_build_configure=yes
else
	if [ "$0" -nt "${MAKEFILE_AM}" ]; then
		has_to_build_configure=yes
	fi
fi

if [ ! -e "${CONFIGURE_CROSS}" ]; then
	has_to_build_configure_cross=yes
else
	if [ "$0" -nt "${CONFIGURE_CROSS}" ]; then
		has_to_build_configure_cross=yes
	fi
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Generating configure.ac"
	echo "AC_INIT([UNISIM MPC5777M Simulator Package], [${MPC5777M_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-mpc5777m])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([mpc5777m])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib mpc5777m" > "${MAKEFILE_AM}"
	echo "EXTRA_DIST = configure.cross" >> "${MAKEFILE_AM}"

	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

if [ "${has_to_build_configure_cross}" = "yes" ]; then
	echo "Building configure.cross"
	cat << EOF_CONFIGURE_CROSS > "${CONFIGURE_CROSS}"
#!/bin/bash
HERE=\$(pwd)
MY_DIR=\$(cd \$(dirname \$0); pwd)

# remove --host, --with-systemc, --with-zlib, --with-libxml2, --with-boost, --with-ncurses, --with-libedit from command line arguments
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
		--with-systemc=* | --with-zlib=* | --with-libxml2=* | --with-boost=* | --with-ncurses=* | --with-libedit=*)
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
\${MY_DIR}/genisslib/configure "\${args[@]}"
STATUS="\$?"
cd "\${HERE}"
if test \${STATUS} -ne 0; then
	exit \${STATUS}
fi

if test "\${help}" = "yes"; then
	echo "=== configure help for mpc5777m"
else
	echo "=== configuring in mpc5777m (\${HERE}/mpc5777m) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/mpc5777m/configure \$@"
fi

if test ! -d \${HERE}/mpc5777m; then
	mkdir \${HERE}/mpc5777m
fi
cd \${HERE}/mpc5777m
\${MY_DIR}/mpc5777m/configure "\$@"
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
all: mpc5777m-all
clean: genisslib-clean mpc5777m-clean
distclean: genisslib-distclean mpc5777m-distclean
	rm -f \${HERE}/Makefile.cross
install: mpc5777m-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
mpc5777m-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/mpc5777m all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
mpc5777m-clean:
	@\\\$(MAKE) -C \${HERE}/mpc5777m clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
mpc5777m-distclean:
	@\\\$(MAKE) -C \${HERE}/mpc5777m distclean
mpc5777m-install:
	@\\\$(MAKE) -C \${HERE}/mpc5777m install
EOF_MAKEFILE_CROSS

chmod +x Makefile.cross

echo "\$(basename \$0): run 'make -f \${HERE}/Makefile.cross' or '\${HERE}/Makefile.cross' to build for \${host} host system type"
EOF_CONFIGURE_CROSS
	chmod +x "${CONFIGURE_CROSS}"
fi  # has_to_build_configure_cross = "yes"

# GENISSLIB

GENISSLIB_CONFIGURE_AC="${DEST_DIR}/genisslib/configure.ac"
GENISSLIB_MAKEFILE_AM="${DEST_DIR}/genisslib/Makefile.am"


if [ ! -e "${GENISSLIB_CONFIGURE_AC}" ]; then
	has_to_build_genisslib_configure=yes
else
	if [ "$0" -nt "${GENISSLIB_CONFIGURE_AC}" ]; then
		has_to_build_genisslib_configure=yes
	fi
fi

if [ ! -e "${GENISSLIB_MAKEFILE_AM}" ]; then
	has_to_build_genisslib_configure=yes
else
	if [ "$0" -nt "${GENISSLIB_MAKEFILE_AM}" ]; then
		has_to_build_genisslib_configure=yes
	fi
fi

if [ "${has_to_build_genisslib_configure}" = "yes" ]; then
	echo "Generating GENISSLIB configure.ac"
	echo "AC_INIT([UNISIM GENISSLIB], [${GENISSLIB_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])" > "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${GENISSLIB_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LEXER_GENERATOR" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "UNISIM_CHECK_PARSER_GENERATOR" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${GENISSLIB_CONFIGURE_AC}"

	AM_GENISSLIB_VERSION=$(printf ${GENISSLIB_VERSION} | sed -e 's/\./_/g')
	echo "Generating GENISSLIB Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${GENISSLIB_MAKEFILE_AM}"
	echo "BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_YFLAGS = -d -p yy" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_LFLAGS = -l" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_PROGRAMS = genisslib" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_CPPFLAGS = -DGENISSLIB_VERSION=\\\"${GENISSLIB_VERSION}\\\"" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_TOOLS_GENISSLIB_M4_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
# The following lines are a workaround caused by a bugFix in AUTOMAKE 1.12
# Note that parser_tokens.hh has been added to BUILT_SOURCES above
# assumption: parser.cc and either parser.h or parser.hh are generated at the same time
    echo "\$(top_builddir)/parser_tokens.hh: \$(top_builddir)/parser.cc" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\tif test -f \"\$(top_builddir)/parser.h\"; then \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\t\tcp -f \"\$(top_builddir)/parser.h\" \"\$(top_builddir)/parser_tokens.hh\"; \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\telif test -f \"\$(top_builddir)/parser.hh\"; then \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\t\tcp -f \"\$(top_builddir)/parser.hh\" \"\$(top_builddir)/parser_tokens.hh\"; \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\tfi\n" >> "${GENISSLIB_MAKEFILE_AM}"

    echo "Building GENISSLIB configure"
	${SHELL} -c "cd ${DEST_DIR}/genisslib && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi


# mpc5777m

MPC5777M_CONFIGURE_AC="${DEST_DIR}/mpc5777m/configure.ac"
MPC5777M_MAKEFILE_AM="${DEST_DIR}/mpc5777m/Makefile.am"


if [ ! -e "${MPC5777M_CONFIGURE_AC}" ]; then
	has_to_build_mpc5777m_configure=yes
else
	if [ "$0" -nt "${MPC5777M_CONFIGURE_AC}" ]; then
		has_to_build_mpc5777m_configure=yes
	fi
fi

if [ ! -e "${MPC5777M_MAKEFILE_AM}" ]; then
	has_to_build_mpc5777m_configure=yes
else
	if [ "$0" -nt "${MPC5777M_MAKEFILE_AM}" ]; then
		has_to_build_mpc5777m_configure=yes
	fi
fi

if [ "${has_to_build_mpc5777m_configure}" = "yes" ]; then
	echo "Generating mpc5777m configure.ac"
	echo "AC_INIT([UNISIM MPC5777M Standalone simulator], [${MPC5777M_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>], [unisim-mpc5777m-core])" > "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${MPC5777M_CONFIGURE_AC}"
	echo "PKG_PROG_PKG_CONFIG([0.26])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${MPC5777M_CONFIGURE_AC}"
	echo "CPPFLAGS=\"${CPPFLAGS} -D_LARGEFILE64_SOURCE\"" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${MPC5777M_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${MPC5777M_CONFIGURE_AC}"
	echo "case \"\${host}\" in" >> "${MPC5777M_CONFIGURE_AC}"
	printf "\t*mingw*)\n" >> "${MPC5777M_CONFIGURE_AC}"
	printf "\tCPPFLAGS=\"-U__STRICT_ANSI__ \${CPPFLAGS}\"\n" >> "${MPC5777M_CONFIGURE_AC}"
	printf "\t;;\n" >> "${MPC5777M_CONFIGURE_AC}"
	printf "\t*)\n" >> "${MPC5777M_CONFIGURE_AC}"
	printf "\t;;\n" >> "${MPC5777M_CONFIGURE_AC}"
	echo "esac" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_PTHREAD(main)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TIMES(main)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ENDIAN(main)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES(main)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT(main)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS(main)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB(main)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2(main)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI(main)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH(main)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_REAL_PATH(main)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AX_BOOST_BASE([1.53.0], AC_MSG_NOTICE([boost >= 1.53.0 found.]), AC_MSG_ERROR([boost >= 1.53.0 not found.]))" >> "${MPC5777M_CONFIGURE_AC}"
	echo "CPPFLAGS=\"\${BOOST_CPPFLAGS} \${CPPFLAGS}\"" >> "${MPC5777M_CONFIGURE_AC}"
	echo "LDFLAGS=\"\${BOOST_LDFLAGS} \${LDFLAGS}\"" >> "${MPC5777M_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AX_CXXFLAGS_WARN_ALL" >> "${MPC5777M_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-mpc5777m-${MPC5777M_VERSION}\"], [path of shared data relative to bin directory])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([trace32-core0.cmm])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([trace32-core1.cmm])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([trace32-core2.cmm])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([trace32-multi.cmm])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([sim_gtkwave.sh], [chmod +x sim_gtkwave.sh])" >> "${MPC5777M_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${MPC5777M_CONFIGURE_AC}"

	AM_MPC5777M_VERSION=$(printf ${MPC5777M_VERSION} | sed -e 's/\./_/g')
	echo "Generating mpc5777m Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${MPC5777M_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${MPC5777M_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libunisim-mpc5777m-${MPC5777M_VERSION}.a" >> "${MPC5777M_MAKEFILE_AM}"
	echo "libunisim_mpc5777m_${AM_MPC5777M_VERSION}_a_SOURCES = ${UNISIM_LIB_MPC5777M_SOURCE_FILES}" >> "${MPC5777M_MAKEFILE_AM}"
	echo "nodist_libunisim_mpc5777m_${AM_MPC5777M_VERSION}_a_SOURCES = unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc" >> "${MPC5777M_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-mpc5777m-${MPC5777M_VERSION}" >> "${MPC5777M_MAKEFILE_AM}"
	echo "unisim_mpc5777m_${AM_MPC5777M_VERSION}_SOURCES = main.cc ${UNISIM_SIMULATORS_MPC5777M_SOURCE_FILES}" >> "${MPC5777M_MAKEFILE_AM}"
	echo "unisim_mpc5777m_${AM_MPC5777M_VERSION}_LDADD = libunisim-mpc5777m-${MPC5777M_VERSION}.a" >> "${MPC5777M_MAKEFILE_AM}"

	echo "noinst_HEADERS = ${UNISIM_LIB_MPC5777M_HEADER_FILES} ${UNISIM_LIB_MPC5777M_TEMPLATE_FILES} ${UNISIM_SIMULATORS_MPC5777M_HEADER_FILES} ${UNISIM_SIMULATORS_MPC5777M_TEMPLATE_FILES}" >> "${MPC5777M_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_MPC5777M_M4_FILES}" >> "${MPC5777M_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-mpc5777m-${MPC5777M_VERSION}" >> "${MPC5777M_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_MPC5777M_DATA_FILES} ${UNISIM_SIMULATORS_MPC5777M_TOP_DATA_FILES}" >> "${MPC5777M_MAKEFILE_AM}"
	echo "nobase_dist_share_DATA = ${UNISIM_SIMULATORS_MPC5777M_DATA_FILES} trace32-core0.cmm trace32-core1.cmm trace32-core2.cmm trace32-multi.cmm sim_gtkwave.sh" >> "${MPC5777M_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc" >> "${MPC5777M_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc" >> "${MPC5777M_MAKEFILE_AM}"
	
	echo "\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc: \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh" >> "${MPC5777M_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh: ${UNISIM_LIB_MPC5777M_ISA_FILES}" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\t" >> "${MPC5777M_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3 -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.isa" >> "${MPC5777M_MAKEFILE_AM}"
	
	echo "\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc: \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh" >> "${MPC5777M_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh: ${UNISIM_LIB_MPC5777M_ISA_FILES}" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\t" >> "${MPC5777M_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3 -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.isa" >> "${MPC5777M_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${MPC5777M_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${MPC5777M_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/bin/\$\$(basename \$\${PROGRAM})\"; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_builddir)/\$\${PROGRAM}\" \$(top_builddir)/bin/\$\$(basename \"\$\${PROGRAM}\"); \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tdone\n" >> "${MPC5777M_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tfi\n" >> "${MPC5777M_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-mpc5777m-${MPC5777M_VERSION}/\$\$(basename \$\${SHARED_DATA})\"; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-mpc5777m-${MPC5777M_VERSION}'; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_srcdir)/\$\${SHARED_DATA}\" \$(top_builddir)/share/unisim-mpc5777m-${MPC5777M_VERSION}/\$\$(basename \"\$\${SHARED_DATA}\"); \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tdone; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tNOBASE_SHARED_DATAS='\$(nobase_dist_share_DATA)'; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tfor NOBASE_SHARED_DATA in \$\${NOBASE_SHARED_DATAS}; do \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-mpc5777m-${MPC5777M_VERSION}/\$\${NOBASE_SHARED_DATA}\"; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tmkdir -p \"\$(top_builddir)/share/unisim-mpc5777m-${MPC5777M_VERSION}/\$\$(dirname \$\${NOBASE_SHARED_DATA})\"; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_srcdir)/\$\${NOBASE_SHARED_DATA}\" \"\$(top_builddir)/share/unisim-mpc5777m-${MPC5777M_VERSION}/\$\${NOBASE_SHARED_DATA}\"; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tdone\n" >> "${MPC5777M_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >> "${MPC5777M_MAKEFILE_AM}"
	printf "\tfi\n" >> "${MPC5777M_MAKEFILE_AM}"

	echo "Building powerpc configure"
	${SHELL} -c "cd ${DEST_DIR}/mpc5777m && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
