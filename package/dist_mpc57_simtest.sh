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

MY_DIR=$(cd $(dirname $0); pwd)
DEST_DIR=$1
mkdir -p ${DEST_DIR}
DEST_DIR=$(cd $DEST_DIR; pwd)
UNISIM_DIR=$(cd ${MY_DIR}/..; pwd)
UNISIM_TOOLS_DIR=${UNISIM_DIR}/unisim_tools
UNISIM_LIB_DIR=${UNISIM_DIR}/unisim_lib
UNISIM_SIMULATOR_DIR=${UNISIM_DIR}/unisim_simulators/cxx/mpc57_simtest

MPC57_SIMTEST_VERSION=$(cat ${UNISIM_SIMULATOR_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-mpc57_simtest-${MPC57_SIMTEST_VERSION}

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
subdecoder.hh \
"

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

UNISIM_LIB_MPC57_SIMTEST_SOURCE_FILES="\
unisim/util/symbolic/symbolic.cc \
unisim/util/random/random.cc \
"

UNISIM_LIB_MPC57_SIMTEST_ISA_FILES="\
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

UNISIM_LIB_MPC57_SIMTEST_HEADER_FILES="\
${UNISIM_LIB_MPC57_SIMTEST_ISA_FILES} \
unisim/util/truth_table/truth_table.hh \
unisim/util/endian/endian.hh \
unisim/util/inlining/inlining.hh \
unisim/util/likely/likely.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/symbolic/symbolic.hh \
unisim/util/random/random.hh \
"

UNISIM_LIB_MPC57_SIMTEST_TEMPLATE_FILES="\
"

UNISIM_LIB_MPC57_SIMTEST_M4_FILES="\
"

UNISIM_LIB_MPC57_SIMTEST_DATA_FILES="\
"

MPC57_SIMTEST_EXTERNAL_HEADERS="\
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
string"

UNISIM_SIMULATOR_MPC57_SIMTEST_ISA_FILES="\
top_mpc57.isa \
"

UNISIM_SIMULATOR_MPC57_SIMTEST_SOURCE_FILES="\
main.cc \
arch.cc \
"
UNISIM_SIMULATOR_MPC57_SIMTEST_HEADER_FILES="\
${UNISIM_SIMULATOR_MPC57_SIMTEST_ISA_FILES} \
arch.hh \
testutils.hh \
"

UNISIM_SIMULATOR_MPC57_SIMTEST_EXTRA_FILES="\
config.h.in \
"

UNISIM_SIMULATOR_MPC57_SIMTEST_TEMPLATE_FILES=
UNISIM_SIMULATOR_MPC57_SIMTEST_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_MPC57_SIMTEST_TESTBENCH_FILES=""

UNISIM_MPC57_SIMTEST_ISA_FILES="${UNISIM_LIB_MPC57_SIMTEST_ISA_FILES} ${UNISIM_SIMULATOR_MPC57_SIMTEST_ISA_FILES}"

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_mpc57_simtest_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/mpc57_simtest

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

UNISIM_LIB_MPC57_SIMTEST_FILES="${UNISIM_LIB_MPC57_SIMTEST_SOURCE_FILES} ${UNISIM_LIB_MPC57_SIMTEST_HEADER_FILES} ${UNISIM_LIB_MPC57_SIMTEST_TEMPLATE_FILES} ${UNISIM_LIB_MPC57_SIMTEST_DATA_FILES}"

for file in ${UNISIM_LIB_MPC57_SIMTEST_FILES}; do
	mkdir -p "${DEST_DIR}/mpc57_simtest/$(dirname ${file})"
	has_to_copy=no
	if [ -e "${DEST_DIR}/mpc57_simtest/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/mpc57_simtest/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/mpc57_simtest/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/mpc57_simtest/${file}" || exit
	fi
done

UNISIM_SIMULATOR_MPC57_SIMTEST_FILES="${UNISIM_SIMULATOR_MPC57_SIMTEST_SOURCE_FILES} ${UNISIM_SIMULATOR_MPC57_SIMTEST_HEADER_FILES} ${UNISIM_SIMULATOR_MPC57_SIMTEST_EXTRA_FILES} ${UNISIM_SIMULATOR_MPC57_SIMTEST_TEMPLATE_FILES} ${UNISIM_SIMULATOR_MPC57_SIMTEST_DATA_FILES} ${UNISIM_SIMULATOR_MPC57_SIMTEST_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATOR_MPC57_SIMTEST_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/mpc57_simtest/${file}" ]; then
		if [ "${UNISIM_SIMULATOR_DIR}/${file}" -nt "${DEST_DIR}/mpc57_simtest/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATOR_DIR}/${file} ==> ${DEST_DIR}/mpc57_simtest/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/mpc57_simtest/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATOR_MPC57_SIMTEST_DATA_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/${file}" ]; then
		if [ "${UNISIM_SIMULATOR_DIR}/${file}" -nt "${DEST_DIR}/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATOR_DIR}/${file} ==> ${DEST_DIR}/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}" || exit
	fi
done


mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/mpc57_simtest/config
mkdir -p ${DEST_DIR}/mpc57_simtest/m4
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

for file in ${UNISIM_LIB_MPC57_SIMTEST_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/mpc57_simtest/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/mpc57_simtest/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/mpc57_simtest/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/mpc57_simtest/${file}" || exit
		has_to_build_mpc57_simtest_configure=yes
	fi
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


Building instructions:
  $ ./configure
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
	echo "AC_INIT([UNISIM Mpc57_Simtest Standalone simulator], [${MPC57_SIMTEST_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-mpc57_simtest])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([mpc57_simtest])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib mpc57_simtest" > "${MAKEFILE_AM}"
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
\${MY_DIR}/genisslib/configure "\${args[@]}"
STATUS="\$?"
cd "\${HERE}"
if test \${STATUS} -ne 0; then
	exit \${STATUS}
fi

if test "\${help}" = "yes"; then
	echo "=== configure help for mpc57_simtest"
else
	echo "=== configuring in mpc57_simtest (\${HERE}/mpc57_simtest) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/mpc57_simtest/configure \$@"
fi

if test ! -d \${HERE}/mpc57_simtest; then
	mkdir \${HERE}/mpc57_simtest
fi
cd \${HERE}/mpc57_simtest
\${MY_DIR}/mpc57_simtest/configure "\$@"
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
all: mpc57_simtest-all
clean: genisslib-clean mpc57_simtest-clean
distclean: genisslib-distclean mpc57_simtest-distclean
	rm -f \${HERE}/Makefile.cross
install: mpc57_simtest-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
mpc57_simtest-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/mpc57_simtest all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
mpc57_simtest-clean:
	@\\\$(MAKE) -C \${HERE}/mpc57_simtest clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
mpc57_simtest-distclean:
	@\\\$(MAKE) -C \${HERE}/mpc57_simtest distclean
mpc57_simtest-install:
	@\\\$(MAKE) -C \${HERE}/mpc57_simtest install
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
	echo "AC_INIT([UNISIM GENISSLIB], [${GENISSLIB_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])" > "${GENISSLIB_CONFIGURE_AC}"
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


# mpc57_simtest

MPC57_SIMTEST_CONFIGURE_AC="${DEST_DIR}/mpc57_simtest/configure.ac"
MPC57_SIMTEST_MAKEFILE_AM="${DEST_DIR}/mpc57_simtest/Makefile.am"


if [ ! -e "${MPC57_SIMTEST_CONFIGURE_AC}" ]; then
	has_to_build_mpc57_simtest_configure=yes
else
	if [ "$0" -nt "${MPC57_SIMTEST_CONFIGURE_AC}" ]; then
		has_to_build_mpc57_simtest_configure=yes
	fi
fi

if [ ! -e "${MPC57_SIMTEST_MAKEFILE_AM}" ]; then
	has_to_build_mpc57_simtest_configure=yes
else
	if [ "$0" -nt "${MPC57_SIMTEST_MAKEFILE_AM}" ]; then
		has_to_build_mpc57_simtest_configure=yes
	fi
fi

if [ "${has_to_build_mpc57_simtest_configure}" = "yes" ]; then
	echo "Generating mpc57_simtest configure.ac"
	echo "AC_INIT([UNISIM Mpc57_Simtest C++ simulator], [${MPC57_SIMTEST_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-mpc57_simtest-core])" > "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
        echo "CXXFLAGS=\"\$CXXFLAGS -std=c++11\"" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${MPC57_SIMTEST_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-mpc57_simtest-${MPC57_SIMTEST_VERSION}\"], [path of shared data relative to bin directory])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	SIM_VERSION_MAJOR=$(printf "${MPC57_SIMTEST_VERSION}" | cut -f 1 -d .)
	SIM_VERSION_MINOR=$(printf "${MPC57_SIMTEST_VERSION}" | cut -f 2 -d .)
	SIM_VERSION_PATCH=$(printf "${MPC57_SIMTEST_VERSION}" | cut -f 3 -d .)
	SIM_VERSION="${MPC57_SIMTEST_VERSION}"
	SIM_VERSION_CODENAME="Triumphalis Tarraco"
	SIM_AUTHOR="Yves Lhuillier (yves.lhuillier@cea.fr)"
	SIM_PROGRAM_NAME="UNISIM Mpc57_Simtest"
	SIM_LICENSE="BSD (See file COPYING)"
	SIM_COPYRIGHT="Copyright (C) 2007-2017, Commissariat a l'Energie Atomique"
	SIM_DESCRIPTION="UNISIM MPC57 SELF SIMULATOR TEST GENERATION"
	SIM_SCHEMATIC="mpc57_simtest/fig_schematic.pdf"
	echo "AC_DEFINE([SIM_VERSION_MAJOR], [${SIM_VERSION_MAJOR}], [Version major number])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION_MINOR], [${SIM_VERSION_MINOR}], [Version minor number])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION_PATCH], [${SIM_VERSION_PATCH}], [Version patch number])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION], [\"${SIM_VERSION}\"], [Version])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION_CODENAME], [\"${SIM_VERSION_CODENAME}\"], [Version code name])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_AUTHOR], [\"${SIM_AUTHOR}\"], [Author])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_PROGRAM_NAME], [\"${SIM_PROGRAM_NAME}\"], [Program name])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_COPYRIGHT], [\"${SIM_COPYRIGHT}\"], [Copyright])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_LICENSE], [\"${SIM_LICENSE}\"], [License])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_DESCRIPTION], [\"${SIM_DESCRIPTION}\"], [Description])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_SCHEMATIC], [\"${SIM_SCHEMATIC}\"], [Schematic])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${MPC57_SIMTEST_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${MPC57_SIMTEST_CONFIGURE_AC}"

	AM_MPC57_SIMTEST_VERSION=$(printf ${MPC57_SIMTEST_VERSION} | sed -e 's/\./_/g')
	echo "Generating mpc57_simtest Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libmpc57_simtest-${MPC57_SIMTEST_VERSION}.a" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "libmpc57_simtest_${AM_MPC57_SIMTEST_VERSION}_a_SOURCES = ${UNISIM_LIB_MPC57_SIMTEST_SOURCE_FILES}" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-mpc57_simtest-${MPC57_SIMTEST_VERSION}" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "unisim_mpc57_simtest_${AM_MPC57_SIMTEST_VERSION}_SOURCES = ${UNISIM_SIMULATOR_MPC57_SIMTEST_SOURCE_FILES} top_mpc57.cc" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "unisim_mpc57_simtest_${AM_MPC57_SIMTEST_VERSION}_CPPFLAGS = -DSIM_EXECUTABLE" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "unisim_mpc57_simtest_${AM_MPC57_SIMTEST_VERSION}_LDADD = libmpc57_simtest-${MPC57_SIMTEST_VERSION}.a" >> "${MPC57_SIMTEST_MAKEFILE_AM}"

	echo "noinst_HEADERS = ${UNISIM_LIB_MPC57_SIMTEST_HEADER_FILES} ${UNISIM_LIB_MPC57_SIMTEST_TEMPLATE_FILES} ${UNISIM_SIMULATOR_MPC57_SIMTEST_HEADER_FILES} ${UNISIM_SIMULATOR_MPC57_SIMTEST_TEMPLATE_FILES}" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_MPC57_SIMTEST_M4_FILES}" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-mpc57_simtest-${MPC57_SIMTEST_VERSION}" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_MPC57_SIMTEST_DATA_FILES} ${UNISIM_SIMULATOR_MPC57_SIMTEST_DATA_FILES}" >> "${MPC57_SIMTEST_MAKEFILE_AM}"

	echo -n "BUILT_SOURCES=" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo -n "top_mpc57.hh " >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo -n "top_mpc57.cc " >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	
	echo -n "CLEANFILES=" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo -n "top_mpc57.hh " >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo -n "top_mpc57.cc " >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	
	echo "\$(top_builddir)/top_mpc57.cc: \$(top_builddir)/top_mpc57.hh" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "\$(top_builddir)/top_mpc57.hh: ${UNISIM_MPC57_SIMTEST_ISA_FILES}" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\t" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/top_mpc57 -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_mpc57.isa" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\t" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "\$(top_srcdir)/gilfix.py \$(top_builddir)/top_mpc57.cc" >> "${MPC57_SIMTEST_MAKEFILE_AM}" 

	echo "all-local: all-local-bin all-local-share" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/bin/\$\$(basename \$\${PROGRAM})\"; \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_builddir)/\$\${PROGRAM}\" \$(top_builddir)/bin/\$\$(basename \"\$\${PROGRAM}\"); \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\tdone\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\tfi\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-mpc57_simtest-${MPC57_SIMTEST_VERSION}/\$\$(basename \$\${SHARED_DATA})\"; \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-mpc57_simtest-${MPC57_SIMTEST_VERSION}'; \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_srcdir)/\$\${SHARED_DATA}\" \$(top_builddir)/share/unisim-mpc57_simtest-${MPC57_SIMTEST_VERSION}/\$\$(basename \"\$\${SHARED_DATA}\"); \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\tdone\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"
	printf "\tfi\n" >> "${MPC57_SIMTEST_MAKEFILE_AM}"

	${DISTCOPY} ${DEST_DIR}/INSTALL ${DEST_DIR}/mpc57_simtest
	${DISTCOPY} ${DEST_DIR}/README ${DEST_DIR}/mpc57_simtest
	${DISTCOPY} ${DEST_DIR}/AUTHORS ${DEST_DIR}/mpc57_simtest
	
	echo "Building mpc57_simtest configure"
	${SHELL} -c "cd ${DEST_DIR}/mpc57_simtest && aclocal -I m4 && autoconf --force && automake -ac"
fi

echo "Distribution is up-to-date"