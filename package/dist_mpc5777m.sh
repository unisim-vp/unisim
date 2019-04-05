#!/bin/bash

SIMPKG=mpc5777m

UNISIM_LIB_SIMULATOR_SOURCE_FILES="\
unisim/kernel/service/service.cc \
unisim/kernel/config/xml_config_file_helper.cc \
unisim/kernel/config/ini_config_file_helper.cc \
unisim/service/http_server/http_server.cc \
unisim/kernel/tlm2/tlm.cc \
unisim/kernel/tlm2/simulator.cc \
unisim/kernel/tlm2/clock.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/util/backtrace/backtrace.cc \
unisim/util/xml/xml.cc \
unisim/util/debug/simple_register_registry.cc \
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
unisim/util/hypapp/hypapp.cc \
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
unisim/service/netstreamer/netstreamer.cc \
unisim/service/os/linux_os/powerpc_linux32.cc \
unisim/service/instrumenter/instrumenter.cc \
unisim/service/web_terminal/web_terminal.cc \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/floating.cc \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/cpu.cc \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/cpu.cc \
unisim/component/cxx/processor/powerpc/isa/disasm.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc \
unisim/component/tlm2/memory/ram/memory.cc \
unisim/component/tlm2/memory/ram/memory_debug.cc \
unisim/component/tlm2/interconnect/generic_router/variable_mapping.cc \
unisim/component/tlm2/interconnect/generic_router/config.cc \
unisim/component/tlm2/interconnect/freescale/mpc57xx/pbridge/acr_mapping.cc \
unisim/component/tlm2/com/serial_terminal/serial_terminal.cc \
"

UNISIM_LIB_SIMULATOR_ISA_FILES="\
unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.isa \
unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.isa \
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
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divw.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divw_sat.isa \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/divwu.isa \
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
unisim/component/cxx/processor/powerpc/isa/disasm.hh \
unisim/component/cxx/processor/powerpc/isa/book_vle/vle.hh \
unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/integer.hh \
unisim/component/cxx/processor/powerpc/isa/lsp/lsp.hh \
unisim/kernel/service/service.hh \
unisim/kernel/config/xml_config_file_helper.hh \
unisim/kernel/config/ini_config_file_helper.hh \
unisim/service/http_server/http_server.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/tlm2/tlm.hh \
unisim/kernel/tlm2/simulator.hh \
unisim/kernel/tlm2/clock.hh \
unisim/kernel/tlm2/trans_attr.hh \
unisim/kernel/tlm2/tlm_serial.hh \
unisim/kernel/tlm2/tlm_can.hh \
unisim/util/backtrace/backtrace.hh \
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
unisim/util/debug/simple_register_registry.hh \
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
unisim/util/hypapp/hypapp.hh \
unisim/util/nat_sort/nat_sort.hh \
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
unisim/service/interfaces/field.hh \
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
unisim/service/interfaces/http_server.hh \
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
unisim/service/tee/char_io/tee.hh \
unisim/service/netstreamer/netstreamer.hh \
unisim/service/os/linux_os/linux.hh \
unisim/service/os/linux_os/powerpc_linux32.hh \
unisim/service/instrumenter/instrumenter.hh \
unisim/service/web_terminal/web_terminal.hh \
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
unisim/component/tlm2/interconnect/generic_router/mapping.hh \
unisim/component/tlm2/interconnect/generic_router/protection.hh \
unisim/component/tlm2/interconnect/programmable_router/router.hh \
unisim/component/tlm2/interconnect/programmable_router/config.hh \
unisim/component/tlm2/interconnect/freescale/mpc57xx/xbar/xbar.hh \
unisim/component/tlm2/interconnect/freescale/mpc57xx/xbar/config.hh \
unisim/component/tlm2/interconnect/freescale/mpc57xx/xbar/smpu/smpu.hh \
unisim/component/tlm2/interconnect/freescale/mpc57xx/pbridge/pbridge.hh \
unisim/component/tlm2/interconnect/freescale/mpc57xx/ebi/ebi.hh \
unisim/component/tlm2/interconnect/freescale/mpc57xx/ebi/config.hh \
unisim/component/tlm2/interrupt/freescale/mpc57xx/intc/intc.hh \
unisim/component/tlm2/timer/freescale/mpc57xx/stm/stm.hh \
unisim/component/tlm2/watchdog/freescale/mpc57xx/swt/swt.hh \
unisim/component/tlm2/timer/freescale/mpc57xx/pit/pit.hh \
unisim/component/tlm2/com/freescale/mpc57xx/linflexd/linflexd.hh \
unisim/component/tlm2/com/serial_terminal/serial_terminal.hh \
unisim/component/tlm2/dma/freescale/mpc57xx/dmamux/dmamux.hh \
unisim/component/tlm2/dma/freescale/mpc57xx/edma/edma.hh \
unisim/component/tlm2/operators/associative_operator.hh \
unisim/component/tlm2/com/freescale/mpc57xx/dspi/dspi.hh \
unisim/component/tlm2/com/freescale/mpc57xx/siul2/siul2.hh \
unisim/component/tlm2/com/freescale/mpc57xx/siul2/defs.hh \
unisim/component/tlm2/com/bosch/m_can/m_can.hh \
unisim/component/tlm2/memory/semaphore/freescale/mpc57xx/sema42/sema42.hh \
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
unisim/component/tlm2/interconnect/programmable_router/router.tcc \
unisim/component/tlm2/interconnect/freescale/mpc57xx/xbar/xbar.tcc \
unisim/component/tlm2/interconnect/freescale/mpc57xx/xbar/smpu/smpu.tcc \
unisim/component/tlm2/interconnect/freescale/mpc57xx/pbridge/pbridge.tcc \
unisim/component/tlm2/interconnect/freescale/mpc57xx/ebi/ebi.tcc \
unisim/component/tlm2/interrupt/freescale/mpc57xx/intc/intc.tcc \
unisim/component/tlm2/timer/freescale/mpc57xx/stm/stm.tcc \
unisim/component/tlm2/watchdog/freescale/mpc57xx/swt/swt.tcc \
unisim/component/tlm2/timer/freescale/mpc57xx/pit/pit.tcc \
unisim/component/tlm2/com/freescale/mpc57xx/linflexd/linflexd.tcc \
unisim/component/tlm2/dma/freescale/mpc57xx/dmamux/dmamux.tcc \
unisim/component/tlm2/dma/freescale/mpc57xx/edma/edma.tcc \
unisim/component/tlm2/com/freescale/mpc57xx/dspi/dspi.tcc \
unisim/component/tlm2/com/freescale/mpc57xx/siul2/siul2.tcc \
unisim/component/tlm2/com/bosch/m_can/m_can.tcc \
unisim/component/tlm2/memory/semaphore/freescale/mpc57xx/sema42/sema42.tcc \
"

UNISIM_LIB_SIMULATOR_M4_FILES="\
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
m4/pthread.m4 \
m4/tvs.m4 \
"

UNISIM_LIB_SIMULATOR_TOP_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_powerpc_vle.xml \
unisim/util/debug/dwarf/powerpc_e500_dwarf_register_number_mapping.xml \
unisim/util/debug/dwarf/powerpc_e500_gcc_dwarf_register_number_mapping.xml \
unisim/service/http_server/favicon.ico \
"

UNISIM_LIB_SIMULATOR_DATA_FILES="\
unisim/kernel/logger/style.css \
unisim/kernel/logger/icon.svg \
unisim/service/instrumenter/style.css \
unisim/service/instrumenter/status_style.css \
unisim/service/instrumenter/script.js \
unisim/service/instrumenter/favicon.ico \
unisim/service/instrumenter/icon.svg \
unisim/service/instrumenter/icon_cont.svg \
unisim/service/instrumenter/icon_intr.svg \
unisim/service/instrumenter/icon_halt.svg \
unisim/service/http_server/style.css \
unisim/service/http_server/close-tab-hover.svg \
unisim/service/http_server/close-tab.svg \
unisim/service/http_server/history-shortcut.svg \
unisim/service/http_server/left-arrow.svg \
unisim/service/http_server/right-arrow.svg \
unisim/service/http_server/close-tab-hover.png \
unisim/service/http_server/close-tab.png \
unisim/service/http_server/history-shortcut.png \
unisim/service/http_server/left-arrow.png \
unisim/service/http_server/right-arrow.png \
unisim/service/http_server/var_style.css \
unisim/service/http_server/var_script.js \
unisim/service/http_server/script.js \
unisim/service/http_server/reg_style.css \
unisim/service/http_server/reg_script.js \
unisim/service/http_server/embedded_script.js \
unisim/service/http_server/embedded_style.css \
unisim/service/http_server/uri.js \
unisim/service/http_server/icon_export_config_xml.svg \
unisim/service/http_server/icon_export_config_ini.svg \
unisim/service/http_server/icon_import_config_xml.svg \
unisim/service/http_server/icon_import_config_ini.svg \
unisim/service/debug/profiler/by_address.css \
unisim/service/debug/profiler/by_function_histogram.css \
unisim/service/debug/profiler/by_function_table.css \
unisim/service/debug/profiler/disassembly.css \
unisim/service/debug/profiler/index.css \
unisim/service/debug/profiler/source_code.css \
unisim/service/debug/profiler/icon_profile_cpu0.svg \
unisim/service/debug/profiler/icon_profile_cpu1.svg \
unisim/service/debug/profiler/icon_profile_cpu2.svg \
unisim/service/debug/profiler/icon_histogram.svg \
unisim/service/debug/profiler/icon_svg_histogram.svg \
unisim/service/debug/profiler/icon_table.svg \
unisim/service/debug/profiler/icon_csv_table.svg \
unisim/service/debug/profiler/icon_pc_table.svg \
unisim/service/debug/profiler/icon_index.svg \
unisim/service/debug/profiler/icon_to_table.svg \
unisim/service/debug/profiler/icon_to_histogram.svg \
unisim/service/debug/profiler/script.js \
unisim/service/debug/profiler/by_function_histogram.js \
unisim/service/debug/profiler/by_function_table.js \
unisim/service/debug/profiler/index.js \
unisim/service/web_terminal/style.css \
unisim/service/web_terminal/script.js \
unisim/service/web_terminal/icon_term0.svg \
unisim/service/web_terminal/icon_term1.svg \
unisim/service/web_terminal/icon_term2.svg \
unisim/service/web_terminal/icon_term14.svg \
unisim/service/web_terminal/icon_term15.svg \
unisim/service/web_terminal/icon_term16.svg \
"

SIMULATOR_EXTERNAL_HEADERS="\
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
set \
"

UNISIM_SIMULATOR_SOURCE_FILES="\
simulator.cc \
config.cc \
debugger.cc \
xbar_0.cc \
xbar_1.cc \
pbridge_a.cc \
pbridge_b.cc \
ebi.cc \
xbar_1_m1_concentrator.cc \
iahbg_0.cc \
iahbg_1.cc \
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
dmamux_0.cc \
dmamux_1.cc \
dmamux_2.cc \
dmamux_3.cc \
dmamux_4.cc \
dmamux_5.cc \
dmamux_6.cc \
dmamux_7.cc \
dmamux_8.cc \
dmamux_9.cc \
edma_0.cc \
edma_1.cc \
dspi_0.cc \
dspi_1.cc \
dspi_2.cc \
dspi_3.cc \
dspi_4.cc \
dspi_5.cc \
dspi_6.cc \
dspi_12.cc \
siul2.cc \
m_can_1.cc \
m_can_2.cc \
m_can_3.cc \
m_can_4.cc \
shared_can_message_ram_router.cc \
sema4.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
simulator.hh \
config.hh \
"

UNISIM_SIMULATOR_EXTRA_FILES="\
trace32-core0.cmm.in \
trace32-core1.cmm.in \
trace32-core2.cmm.in \
trace32-multi.cmm.in \
sim_gtkwave.sh.in \
"

UNISIM_SIMULATOR_PKG_DATA_FILES="\
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
unisim.ico \
soft/Makefile \
soft/app/Makefile \
soft/app/Z4_2/main.c \
soft/app/Z7_0/main.c \
soft/app/Z7_1/main.c \
soft/app/Z4_2/boot.elf \
soft/app/Z7_0/boot.elf \
soft/app/Z7_1/boot.elf \
soft/libsys/Makefile \
soft/libsys/bin/common/gcc-wrapper \
soft/libsys/bin/Z4_2/bin/powerpc-eabivle-ranlib \
soft/libsys/bin/Z4_2/bin/powerpc-eabivle-gcc \
soft/libsys/bin/Z4_2/bin/powerpc-eabivle-ld \
soft/libsys/bin/Z4_2/bin/powerpc-eabivle-strip \
soft/libsys/bin/Z4_2/bin/powerpc-eabivle-ar \
soft/libsys/bin/Z7_0/bin/powerpc-eabivle-ranlib \
soft/libsys/bin/Z7_0/bin/powerpc-eabivle-gcc \
soft/libsys/bin/Z7_0/bin/powerpc-eabivle-ld \
soft/libsys/bin/Z7_0/bin/powerpc-eabivle-strip \
soft/libsys/bin/Z7_0/bin/powerpc-eabivle-ar \
soft/libsys/bin/Z7_1/bin/powerpc-eabivle-ranlib \
soft/libsys/bin/Z7_1/bin/powerpc-eabivle-gcc \
soft/libsys/bin/Z7_1/bin/powerpc-eabivle-ld \
soft/libsys/bin/Z7_1/bin/powerpc-eabivle-strip \
soft/libsys/bin/Z7_1/bin/powerpc-eabivle-ar \
soft/libsys/lib/libsys.a \
soft/libsys/lib/ldscripts/common/mpc57xx.ld \
soft/libsys/lib/ldscripts/Z4_2/mpc57xx.ld \
soft/libsys/lib/ldscripts/Z7_0/mpc57xx.ld \
soft/libsys/lib/ldscripts/Z7_1/mpc57xx.ld \
soft/libsys/include/typedefs.h \
soft/libsys/include/MPC5777M.h \
soft/libsys/include/derivative.h \
soft/libsys/include/compiler_api.h \
soft/libsys/include/intc.h \
soft/libsys/include/stm.h \
soft/libsys/include/swt.h \
soft/libsys/include/pit.h \
soft/libsys/include/linflexd.h \
soft/libsys/include/dmamux.h \
soft/libsys/include/edma.h \
soft/libsys/include/dspi.h \
soft/libsys/include/siul2.h \
soft/libsys/include/ebi.h \
soft/libsys/include/pbridge.h \
soft/libsys/include/xbar.h \
soft/libsys/include/smpu.h \
soft/libsys/include/m_can.h \
soft/libsys/include/sema4.h \
soft/libsys/include/console.h \
soft/libsys/include/ramdisk.h \
soft/libsys/include/lfs.h \
soft/libsys/include/lfs_util.h \
soft/libsys/src/core/intc_sw_handlers.S \
soft/libsys/src/core/startup.S \
soft/libsys/src/core/vector.S \
soft/libsys/src/core/cache.c \
soft/libsys/src/core/mpu.c \
soft/libsys/src/sys/sys.c \
soft/libsys/src/drv/intc.c \
soft/libsys/src/drv/stm.c \
soft/libsys/src/drv/swt.c \
soft/libsys/src/drv/pit.c \
soft/libsys/src/drv/linflexd.c \
soft/libsys/src/drv/dmamux.c \
soft/libsys/src/drv/edma.c \
soft/libsys/src/drv/dspi.c \
soft/libsys/src/drv/siul2.c \
soft/libsys/src/drv/ebi.c \
soft/libsys/src/drv/pbridge.c \
soft/libsys/src/drv/xbar.c \
soft/libsys/src/drv/smpu.c \
soft/libsys/src/drv/m_can.c \
soft/libsys/src/drv/sema4.c \
soft/libsys/src/drv/console.c \
soft/libsys/src/drv/ramdisk.c \
soft/libsys/src/fs/lfs.c \
soft/libsys/src/fs/lfs_util.c \
sim_config.xml \
gtkwave_init_script.tcl \
bandwidth_gtkwave_init_script.tcl \
.gtkwaverc \
gtkwave.ini \
config.t32 \
baf.bin \
"

UNISIM_SIMULATOR_TESTBENCH_FILES="\
main.cc \
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
UNISIM_SIMULATOR_DIR=${UNISIM_DIR}/unisim_simulators/tlm2/${SIMPKG}

SIMULATOR_VERSION=$(cat ${UNISIM_SIMULATOR_DIR}/VERSION)

if [ -z "${DISTCOPY}" ]; then
	DISTCOPY=cp
fi

has_to_build() {
	if [ ! -e "$1" -o "$2" -nt "$1" ]; then
		return 0;
	else
		return 1;
	fi
}

dist_copy() {
	if has_to_build "$2" "$1"; then
		echo "$1 ==> $2"
		mkdir -p "$(dirname $2)"
		${DISTCOPY} -f "$1" "$2" || exit
		return 0
	fi
	return 1
}

GILINSTALL=noinst ${UNISIM_DIR}/package/dist_genisslib.sh ${DEST_DIR}/genisslib

mkdir -p ${DEST_DIR}/${SIMPKG}

UNISIM_LIB_SIMULATOR_FILES="${UNISIM_LIB_SIMULATOR_SOURCE_FILES} ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_LIB_SIMULATOR_TOP_DATA_FILES}"

for file in ${UNISIM_LIB_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_EXTRA_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
${UNISIM_SIMULATOR_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

for file in ${UNISIM_SIMULATOR_PKG_DATA_FILES}; do
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
Gilles Mouchard <gilles.mouchard@cea.fr>
Yves Lhuillier <yves.lhuillier@cea.fr>
Franck Vedrine <franck.vedrine@cea.fr>
Réda Nouacer <reda.nouacer@cea.fr>
Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>
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

has_to_build_configure=no

if has_to_build "${CONFIGURE_AC}" "$0"; then
	echo "Generating configure.ac"
	cat <<EOF > "${CONFIGURE_AC}"
AC_INIT([UNISIM MPC5777M Simulator Package], [${SIMULATOR_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves Lhuillier <yves.lhuillier@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}])
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

# Simulator

SIMULATOR_CONFIGURE_AC="${DEST_DIR}/${SIMPKG}/configure.ac"
SIMULATOR_MAKEFILE_AM="${DEST_DIR}/${SIMPKG}/Makefile.am"

if has_to_build "${SIMULATOR_CONFIGURE_AC}" "$0"; then
	echo "Generating ${SIMPKG} configure.ac"
	cat <<EOF > "${SIMULATOR_CONFIGURE_AC}"
AC_INIT([UNISIM MPC5777M Standalone simulator], [${SIMULATOR_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>], [unisim-${SIMPKG}-core])
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
LT_INIT
AC_SUBST(LIBTOOL_DEPS)
AC_PROG_LN_S
PKG_PROG_PKG_CONFIG([0.26])
AC_LANG([C++])
AM_PROG_CC_C_O
CPPFLAGS="\${CPPFLAGS} -D_LARGEFILE64_SOURCE"
AC_CHECK_HEADERS([${SIMULATOR_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))
case "\${host}" in
	*mingw*)
		CPPFLAGS="-U__STRICT_ANSI__ \${CPPFLAGS}"
		;;
	*)
		;;
esac
UNISIM_CHECK_PTHREAD(main)
UNISIM_CHECK_TIMES(main)
UNISIM_CHECK_ENDIAN(main)
UNISIM_CHECK_CURSES(main)
UNISIM_CHECK_LIBEDIT(main)
UNISIM_CHECK_BSD_SOCKETS(main)
UNISIM_CHECK_ZLIB(main)
UNISIM_CHECK_LIBXML2(main)
UNISIM_CHECK_CXXABI(main)
UNISIM_CHECK_GET_EXECUTABLE_PATH(main)
UNISIM_CHECK_REAL_PATH(main)
AX_BOOST_BASE([1.53.0], AC_MSG_NOTICE([boost >= 1.53.0 found.]), AC_MSG_ERROR([boost >= 1.53.0 not found.]))
CPPFLAGS="\${BOOST_CPPFLAGS} \${CPPFLAGS}"
LDFLAGS="\${BOOST_LDFLAGS} \${LDFLAGS}"
UNISIM_CHECK_SYSTEMC
UNISIM_CHECK_TVS
AX_CXXFLAGS_WARN_ALL
GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib
AC_SUBST(GENISSLIB_PATH)
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/unisim-${SIMPKG}-${SIMULATOR_VERSION}"], [path of shared data relative to bin directory])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([trace32-core0.cmm])
AC_CONFIG_FILES([trace32-core1.cmm])
AC_CONFIG_FILES([trace32-core2.cmm])
AC_CONFIG_FILES([trace32-multi.cmm])
AC_CONFIG_FILES([sim_gtkwave.sh], [chmod +x sim_gtkwave.sh])
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
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_SIMULATOR_TESTBENCH_FILES} ${UNISIM_SIMULATOR_SOURCE_FILES}
#unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_LDFLAGS = -static-libtool-libs
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la

# Static Library
noinst_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES}
libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -static
nodist_libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_LIB_SIMULATOR_TOP_DATA_FILES} ${UNISIM_SIMULATOR_PKG_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES} trace32-core0.cmm trace32-core1.cmm trace32-core2.cmm trace32-multi.cmm sim_gtkwave.sh

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc

\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc: \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh
\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3 -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.isa

\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc: \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh
\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3 -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.isa
EOF
	has_to_build_simulator_configure=yes
fi

if [ "${has_to_build_simulator_configure}" = "yes" ]; then
	echo "Building ${SIMPKG} configure"
	${SHELL} -c "cd ${DEST_DIR}/${SIMPKG} && aclocal -I m4 && libtoolize --force && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
