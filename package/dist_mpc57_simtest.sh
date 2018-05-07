#!/bin/bash

SIMPKG=mpc57_simtest

UNISIM_LIB_SIMULATOR_SOURCE_FILES="\
unisim/component/cxx/processor/powerpc/isa/book_i/disasm.cc \
unisim/util/symbolic/symbolic.cc \
unisim/util/random/random.cc \
"

UNISIM_LIB_SIMULATOR_ISA_FILES="\
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

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
${UNISIM_LIB_SIMULATOR_ISA_FILES} \
unisim/component/cxx/processor/powerpc/isa/book_i/disasm.hh \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/integer.hh \
unisim/component/cxx/processor/powerpc/isa/book_vle/vle.hh \
unisim/component/cxx/processor/powerpc/isa/lsp/lsp.hh \
unisim/util/truth_table/truth_table.hh \
unisim/util/endian/endian.hh \
unisim/util/inlining/inlining.hh \
unisim/util/likely/likely.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/symbolic/symbolic.hh \
unisim/util/symbolic/identifier.hh \
unisim/util/random/random.hh \
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
top_mpc57.isa \
"

UNISIM_SIMULATOR_SOURCE_FILES="\
main.cc \
arch.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
${UNISIM_SIMULATOR_ISA_FILES} \
arch.hh \
testutils.hh \
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
  - mpc57_simtest: an MPC57 V5 user level simulator
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
AC_INIT([UNISIM mpc57xx simulator validation tests generator package], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}])
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
AC_INIT([UNISIM mpc57xx simulator validation tests generator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}-core])
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
nodist_libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = top_mpc57.cc

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/top_mpc57.hh\
	\$(top_builddir)/top_mpc57.cc\

CLEANFILES=\
	\$(top_builddir)/top_mpc57.hh\
	\$(top_builddir)/top_mpc57.cc\

\$(top_builddir)/top_mpc57.cc: \$(top_builddir)/top_mpc57.hh
\$(top_builddir)/top_mpc57.hh: ${UNISIM_SIMULATOR_ISA_FILES} ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(GENISSLIB_PATH) \$(GILFLAGS) -o \$(top_builddir)/top_mpc57 -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_mpc57.isa

EOF
	has_to_build_simulator_configure=yes
fi

if [ "${has_to_build_simulator_configure}" = "yes" ]; then
	echo "Building ${SIMPKG} configure"
	${SHELL} -c "cd ${DEST_DIR}/${SIMPKG} && aclocal -I m4 && libtoolize --force && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
