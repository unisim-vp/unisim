#!/bin/bash

SIMPKG=e5500_simtest

UNISIM_LIB_SIMULATOR_SOURCE_FILES="\
unisim/util/random/random.cc \
unisim/util/symbolic/symbolic.cc \
"

UNISIM_LIB_SIMULATOR_ISA_FILES="\
unisim/component/cxx/processor/powerpc/isa/book_i/branch/bcctr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/branch/bc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/branch/bclr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/branch/b.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crandc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crand.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/creqv.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crnand.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crnor.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crorc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/cror.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/crxor.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg/mcrf.isa \
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
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsmax.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsmin.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs/efsmsub.isa \
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
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/adde.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addic_.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addic.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/add.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addis.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addme.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/addze.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/andc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/andi_.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/and.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/andis_.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpiL.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmp.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpli.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpliL.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpl.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmpL.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cmplL.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cntlzd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/cntlzw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divdu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divw_sat.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divwu_sat.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/eqv.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/extsb.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/extsh.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/extsw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/integer.hh \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/isel.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lbz.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lbzu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lbzux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lbzx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/ld.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/ldu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/ldux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/ldx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lha.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhau.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhaux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhax.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhz.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhzu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhzux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lhzx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lmw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lswi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lswx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwa.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwaux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwax.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwz.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwzu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwzux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/lwzx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mcrxr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mfcr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mtcrf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mulhd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mulhdu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mulhw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mulhwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mulld.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mulli.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/mullw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/nand.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/neg.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/nor.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/orc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/ori.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/or.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/oris.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rldcl.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rldcr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rldic.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rldicl.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rldicr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rldimi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rlwimi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rlwinm.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/rlwnm.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sld.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/slw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sradi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/srad.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/srawi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sraw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/srd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/srw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stb.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stbu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stbux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stbx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/std.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stdu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stdux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stdx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sthbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sth.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sthu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sthux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/sthx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stmw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stswi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stswx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stwbrx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stwu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stwux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/stwx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfc.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfe.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfic.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfme.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/subfze.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/twi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/tw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/xori.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/xor.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/xoris.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fabs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fadd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fadds.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fcfid.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fcmpo.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fcmpu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fctid.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fctidz.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fctiw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fctiwz.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fdiv.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fdivs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmadd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmadds.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmr.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmsub.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmsubs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmul.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fmuls.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fnabs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fneg.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fnmadd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fnmadds.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fnmsub.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fnmsubs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fres.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/frsp.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/frsqrte.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fsel.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fsqrt.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fsqrts.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fsub.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/fsubs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfdu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfdux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfdx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfsu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfsux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/lfsx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mcrfs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mffs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mtfsb0.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mtfsb1.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mtfsfi.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/mtfsf.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfd.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfdu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfdux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfdx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfiwx.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfs.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfsu.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfsux.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/stfsx.isa \
unisim/component/cxx/processor/powerpc/isa/book_vle/vle.hh \
"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
${UNISIM_LIB_SIMULATOR_ISA_FILES} \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/integer.hh \
unisim/component/cxx/processor/powerpc/isa/book_i/floating_point/floating.hh \
unisim/component/cxx/processor/powerpc/isa/book_vle/vle.hh \
unisim/util/endian/endian.hh \
unisim/util/inlining/inlining.hh \
unisim/util/likely/likely.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/random/random.hh \
unisim/util/symbolic/identifier.hh \
unisim/util/symbolic/symbolic.hh \
"

UNISIM_LIB_SIMULATOR_M4_FILES="\
"

UNISIM_LIB_SIMULATOR_DATA_FILES="\
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

UNISIM_SIMULATOR_ISA_FILES="\
top_ppc64.isa \
"

UNISIM_SIMULATOR_SOURCE_FILES="\
main.cc \
arch.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
${UNISIM_SIMULATOR_ISA_FILES} \
arch.hh \
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

Usage()
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

UNISIM_LIB_DIR=${UNISIM_DIR}/unisim_lib
UNISIM_SIMULATOR_DIR=${UNISIM_DIR}/unisim_simulators/cxx/${SIMPKG}

SIMULATOR_VERSION=$(cat ${UNISIM_SIMULATOR_DIR}/VERSION)

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

GILINSTALL=noinst ${UNISIM_DIR}/package/dist_genisslib.sh ${DEST_DIR}/genisslib

mkdir -p ${DEST_DIR}/${SIMPKG}

UNISIM_LIB_SIMULATOR_FILES="${UNISIM_LIB_SIMULATOR_SOURCE_FILES} ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_LIB_SIMULATOR_DATA_FILES}"

for file in ${UNISIM_LIB_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

UNISIM_SIMULATOR_FILES="${UNISIM_SIMULATOR_SOURCE_FILES} ${UNISIM_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_DATA_FILES}"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

for file in ${UNISIM_SIMULATOR_TOP_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/${SIMPKG}/config
mkdir -p ${DEST_DIR}/${SIMPKG}/m4

# Some imported files (m4 macros) impact configure generation
has_to_build_simulator_configure=no

for file in ${UNISIM_LIB_SIMULATOR_M4_FILES}; do
	dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}" && has_to_build_simulator_configure=yes
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Yves Lhuillier <yves.lhuillier@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - e5500_simtest: a QorIQ e5500 (PowerPC64) test generator
  - UNISIM GenISSLib (will not be installed): an instruction set simulator generator

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

has_to_build_configure=no

if has_to_build "${CONFIGURE_AC}" "$0"; then
	echo "Generating configure.ac"
	cat <<EOF > "${CONFIGURE_AC}"
AC_INIT([UNISIM e5500 test generation package], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}])
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
fi  # has to build configure.cross

# Simulator

SIMULATOR_CONFIGURE_AC="${DEST_DIR}/${SIMPKG}/configure.ac"
SIMULATOR_MAKEFILE_AM="${DEST_DIR}/${SIMPKG}/Makefile.am"

if has_to_build "${SIMULATOR_CONFIGURE_AC}" "$0"; then
	echo "Generating ${SIMPKG} configure.ac"
	cat <<EOF > "${SIMULATOR_CONFIGURE_AC}"
AC_INIT([UNISIM e5500 test generator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}-core])
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
\$(top_builddir)/top_ppc64.hh: ${UNISIM_SIMULATOR_ISA_FILES} ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(GENISSLIB_PATH) \$(GILFLAGS) -o \$(top_builddir)/top_ppc64 -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_ppc64.isa

EOF
	has_to_build_simulator_configure=yes
fi

if [ "${has_to_build_simulator_configure}" = "yes" ]; then
	echo "Building ${SIMPKG} configure"
	${SHELL} -c "cd ${DEST_DIR}/${SIMPKG} && aclocal -I m4 && libtoolize --force && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"

