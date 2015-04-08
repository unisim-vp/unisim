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
UNISIM_SIMULATORS_DIR=${UNISIM_DIR}/unisim_simulators/tlm2/arm926ejs

ARM926EJS_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-arm926ejs-${ARM926EJS_VERSION}

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

UNISIM_LIB_ARM926EJS_SOURCE_FILES="\
unisim/kernel/debug/debug.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/kernel/api/api.cc \
unisim/kernel/tlm/tlm.cc \
unisim/kernel/service/service.cc \
unisim/kernel/service/xml_helper.cc \
unisim/kernel/tlm2/tlm.cc \
unisim/api/debug/debug_api.cc \
unisim/service/tee/loader/tee.cc \
unisim/service/tee/symbol_table_lookup/tee_32.cc \
unisim/service/tee/symbol_table_lookup/tee_64.cc \
unisim/service/tee/blob/tee_32.cc \
unisim/service/tee/blob/tee_64.cc \
unisim/service/tee/stmt_lookup/tee_32.cc \
unisim/service/tee/stmt_lookup/tee_64.cc \
unisim/service/tee/backtrace/tee_32.cc \
unisim/service/tee/backtrace/tee_64.cc \
unisim/service/tee/memory_access_reporting/tee_32.cc \
unisim/service/tee/memory_access_reporting/tee_64.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/inline_debugger/inline_debugger_64.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/sim_debugger/sim_debugger_64.cc \
unisim/service/debug/sim_debugger/sim_debugger_32.cc \
unisim/service/debug/sim_debugger/sim_debugger.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server_64.cc \
unisim/service/debug/debugger/debugger32.cc \
unisim/service/debug/debugger/debugger64.cc \
unisim/service/profiling/addr_profiler/profiler32.cc \
unisim/service/profiling/addr_profiler/profiler64.cc \
unisim/service/power/cache_profile.cc \
unisim/service/power/cache_dynamic_power.cc \
unisim/service/power/cache_leakage_power.cc \
unisim/service/power/cache_power_estimator.cc \
unisim/service/power/cache_dynamic_energy.cc \
unisim/service/os/linux_os/linux.cc \
unisim/service/loader/raw_loader/raw_loader32.cc \
unisim/service/loader/raw_loader/raw_loader64.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/loader/elf_loader/elf64_loader.cc \
unisim/service/loader/coff_loader/coff_loader32.cc \
unisim/service/loader/coff_loader/coff_loader64.cc \
unisim/service/loader/s19_loader/s19_loader.cc \
unisim/service/loader/multiformat_loader/multiformat_loader32.cc \
unisim/service/loader/multiformat_loader/multiformat_loader64.cc \
unisim/service/trap_handler/trap_handler.cc \
unisim/service/trap_handler/trap_handler_identifier.cc \
unisim/service/time/host_time/time.cc \
unisim/service/time/sc_time/time.cc \
unisim/util/debug/symbol_table_64.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/dwarf/class.cc \
unisim/util/debug/dwarf/dwarf64.cc \
unisim/util/debug/dwarf/encoding.cc \
unisim/util/debug/dwarf/ml.cc \
unisim/util/debug/dwarf/attr.cc \
unisim/util/debug/dwarf/filename.cc \
unisim/util/debug/dwarf/leb128.cc \
unisim/util/debug/dwarf/abbrev.cc \
unisim/util/debug/dwarf/dwarf32.cc \
unisim/util/debug/dwarf/register_number_mapping.cc \
unisim/util/debug/dwarf/data_object.cc \
unisim/util/debug/dwarf/c_loc_expr_parser.cc \
unisim/util/debug/breakpoint_registry_64.cc \
unisim/util/debug/blob/blob32.cc \
unisim/util/debug/blob/blob64.cc \
unisim/util/debug/blob/section32.cc \
unisim/util/debug/blob/section64.cc \
unisim/util/debug/blob/segment32.cc \
unisim/util/debug/blob/segment64.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/watchpoint_registry_64.cc \
unisim/util/debug/stmt_32.cc \
unisim/util/debug/stmt_64.cc \
unisim/util/debug/elf_symtab/elf_symtab32.cc \
unisim/util/debug/elf_symtab/elf_symtab64.cc \
unisim/util/debug/coff_symtab/coff_symtab32.cc \
unisim/util/debug/coff_symtab/coff_symtab64.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/debug/breakpoint_registry_64.cc \
unisim/util/debug/profile_32.cc \
unisim/util/debug/profile_64.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/type.cc \
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/loader/elf_loader/elf64_loader.cc \
unisim/util/loader/coff_loader/coff_loader32.cc \
unisim/util/loader/coff_loader/coff_loader64.cc \
unisim/util/os/linux_os/environment.cc \
unisim/util/os/linux_os/linux.cc \
unisim/util/lexer/lexer.cc \
unisim/util/ieee754/ieee754.cc \
unisim/util/xml/xml.cc \
unisim/util/endian/endian.cc \
unisim/util/garbage_collector/garbage_collector.cc \
unisim/util/random/random.cc \
unisim/util/queue/queue.cc \
unisim/util/generic_peripheral_register/generic_peripheral_register.cc \
unisim/component/tlm2/processor/arm/arm926ejs/arm926ejs.cc \
unisim/component/tlm2/memory/ram/memory.cc \
unisim/component/tlm2/memory/ram/memory_debug.cc \
unisim/component/tlm2/chipset/arm926ejs_pxp/pxp.cc \
unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_stubs.cc \
unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic.cc \
unisim/component/tlm2/chipset/arm926ejs_pxp/util/time_signal_splitter.cc \
unisim/component/tlm2/chipset/arm926ejs_pxp/watchdog/watchdog.cc \
unisim/component/tlm2/chipset/arm926ejs_pxp/dma/pl080/pl080.cc \
unisim/component/tlm2/chipset/arm926ejs_pxp/uart/pl011/pl011.cc \
unisim/component/tlm2/chipset/arm926ejs_pxp/sic/sic.cc \
unisim/component/tlm2/chipset/arm926ejs_pxp/dual_timer/dt.cc \
unisim/component/tlm2/chipset/arm926ejs_pxp/system_controller/sc.cc \
unisim/component/tlm2/chipset/arm926ejs_pxp/ethernet/smsc_lan91c111.cc \
unisim/component/cxx/processor/arm/disasm.cc \
unisim/component/cxx/processor/arm/arm926ejs/cpu.cc \
unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.cc \
unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.cc \
unisim/component/cxx/processor/arm/arm926ejs/cache.cc \
unisim/component/cxx/processor/arm/arm926ejs/lockdown_tlb.cc \
unisim/component/cxx/processor/arm/arm926ejs/tlb.cc \
unisim/component/cxx/processor/arm/arm926ejs/cp15.cc \
unisim/component/cxx/processor/arm/memory_op.cc \
unisim/component/cxx/processor/arm/cpu.cc \
unisim/component/cxx/memory/ram/memory_64.cc \
unisim/component/cxx/memory/ram/memory_32.cc"

UNISIM_LIB_ARM926EJS_ISA_THUMB_FILES="\
unisim/component/cxx/processor/arm/isa/thumb/thumb.isa \
unisim/component/cxx/processor/arm/isa/thumb/exception.isa \
unisim/component/cxx/processor/arm/isa/thumb/load_store.isa \
unisim/component/cxx/processor/arm/isa/thumb/multiply.isa \
unisim/component/cxx/processor/arm/isa/thumb/misc_arithmetic.isa \
unisim/component/cxx/processor/arm/isa/thumb/branch.isa \
unisim/component/cxx/processor/arm/isa/thumb/branch_T1.isa \
unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa \
unisim/component/cxx/processor/arm/isa/thumb/ordering.isa \
unisim/component/cxx/processor/arm/isa/thumb/profiling.isa \
"

UNISIM_LIB_ARM926EJS_ISA_ARM32_FILES="\
unisim/component/cxx/processor/arm/isa/arm32/specialization.isa \
unisim/component/cxx/processor/arm/isa/arm32/coprocessor.isa \
unisim/component/cxx/processor/arm/isa/arm32/exception.isa \
unisim/component/cxx/processor/arm/isa/arm32/execution_latency.isa \
unisim/component/cxx/processor/arm/isa/arm32/branch.isa \
unisim/component/cxx/processor/arm/isa/arm32/data_processing.isa \
unisim/component/cxx/processor/arm/isa/arm32/dependency.isa \
unisim/component/cxx/processor/arm/isa/arm32/misc_arithmetic.isa \
unisim/component/cxx/processor/arm/isa/arm32/hints.isa \
unisim/component/cxx/processor/arm/isa/arm32/multiply.isa \
unisim/component/cxx/processor/arm/isa/arm32/status_register_access.isa \
unisim/component/cxx/processor/arm/isa/arm32/ordering.isa \
unisim/component/cxx/processor/arm/isa/arm32/profiling.isa \
unisim/component/cxx/processor/arm/isa/arm32/load_store.isa \
unisim/component/cxx/processor/arm/isa/arm32/arm32.isa"

UNISIM_LIB_ARM926EJS_ISA_FILES="${UNISIM_LIB_ARM926EJS_ISA_THUMB_FILES} ${UNISIM_LIB_ARM926EJS_ISA_ARM32_FILES}"

UNISIM_LIB_ARM926EJS_HEADER_FILES="${UNISIM_LIB_ARM926EJS_ISA_FILES} \
unisim/kernel/debug/debug.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/api/api.hh \
unisim/kernel/tlm/tlm.hh \
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/tlm2/tlm.hh \
unisim/api/debug/debug_api.hh \
unisim/service/tee/loader/tee.hh \
unisim/service/tee/symbol_table_lookup/tee.hh \
unisim/service/tee/blob/tee.hh \
unisim/service/tee/stmt_lookup/tee.hh \
unisim/service/tee/backtrace/tee.hh \
unisim/service/tee/memory_access_reporting/tee.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/sim_debugger/sim_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/debug/debugger/debugger.hh \
unisim/service/profiling/addr_profiler/profiler.hh \
unisim/service/power/cache_power_estimator.hh \
unisim/service/power/cache_profile.hh \
unisim/service/power/cache_dynamic_power.hh \
unisim/service/power/cache_leakage_power.hh \
unisim/service/power/cache_dynamic_energy.hh \
unisim/service/os/linux_os/linux.hh \
unisim/service/loader/raw_loader/raw_loader.hh \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/elf_loader/elf64_loader.hh \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/coff_loader/coff_loader.hh \
unisim/service/loader/s19_loader/s19_loader.hh \
unisim/service/loader/multiformat_loader/multiformat_loader.hh \
unisim/service/trap_handler/trap_handler.hh \
unisim/service/trap_handler/trap_handler_identifier.hh \
unisim/service/trap_handler/trap_handler_identifier_interface.hh \
unisim/service/interfaces/debug_control.hh \
unisim/service/interfaces/debug_event.hh \
unisim/service/interfaces/debug_info_loading.hh \
unisim/service/interfaces/profiling.hh \
unisim/service/interfaces/blob.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/interfaces/power_mode.hh \
unisim/service/interfaces/memory_access_reporting.hh \
unisim/service/interfaces/time.hh \
unisim/service/interfaces/backtrace.hh \
unisim/service/interfaces/disassembly.hh \
unisim/service/interfaces/cache_power_estimator.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/debug_control.hh \
unisim/service/interfaces/os.hh \
unisim/service/interfaces/linux_os.hh \
unisim/service/interfaces/stmt_lookup.hh \
unisim/service/interfaces/loader.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/symbol_table_lookup.hh \
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/interfaces/subprogram_lookup.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/util/likely/likely.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/data_object.hh \
unisim/util/debug/dwarf/fwd.hh \
unisim/util/debug/dwarf/addr_range.hh \
unisim/util/debug/dwarf/fmt.hh \
unisim/util/debug/dwarf/range.hh \
unisim/util/debug/dwarf/call_frame_vm.hh \
unisim/util/debug/dwarf/encoding.hh \
unisim/util/debug/dwarf/stmt_vm.hh \
unisim/util/debug/dwarf/call_frame_prog.hh \
unisim/util/debug/dwarf/expr_vm.hh \
unisim/util/debug/dwarf/filename.hh \
unisim/util/debug/dwarf/fde.hh \
unisim/util/debug/dwarf/cu.hh \
unisim/util/debug/dwarf/stmt_prog.hh \
unisim/util/debug/dwarf/abbrev.hh \
unisim/util/debug/dwarf/leb128.hh \
unisim/util/debug/dwarf/attr.hh \
unisim/util/debug/dwarf/die.hh \
unisim/util/debug/dwarf/macinfo.hh \
unisim/util/debug/dwarf/pub.hh \
unisim/util/debug/dwarf/ml.hh \
unisim/util/debug/dwarf/cie.hh \
unisim/util/debug/dwarf/dwarf.hh \
unisim/util/debug/dwarf/loc.hh \
unisim/util/debug/dwarf/class.hh \
unisim/util/debug/dwarf/register_number_mapping.hh \
unisim/util/debug/dwarf/frame.hh \
unisim/util/debug/dwarf/util.hh \
unisim/util/debug/dwarf/version.hh \
unisim/util/debug/dwarf/option.hh \
unisim/util/debug/dwarf/cfa.hh \
unisim/util/debug/dwarf/data_object.hh \
unisim/util/debug/dwarf/subprogram.hh \
unisim/util/debug/dwarf/c_loc_expr_parser.hh \
unisim/util/debug/memory_access_type.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/blob/segment.hh \
unisim/util/debug/blob/blob.hh \
unisim/util/debug/blob/section.hh \
unisim/util/debug/stmt.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/register.hh \
unisim/util/debug/elf_symtab/elf_symtab.hh \
unisim/util/debug/coff_symtab/coff_symtab.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/event.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/profile.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/data_object.hh \
unisim/util/debug/type.hh \
unisim/util/debug/data_object_initializer.hh \
unisim/util/debug/subprogram.hh \
unisim/util/loader/elf_loader/elf32_loader.hh \
unisim/util/loader/elf_loader/elf_loader.hh \
unisim/util/loader/elf_loader/elf64_loader.hh \
unisim/util/loader/elf_loader/elf_common.h \
unisim/util/loader/elf_loader/elf32.h \
unisim/util/loader/elf_loader/elf64.h \
unisim/util/loader/coff_loader/coff_loader.hh \
unisim/util/loader/coff_loader/ti/ti.hh \
unisim/util/os/linux_os/arm.hh \
unisim/util/os/linux_os/aux_table.hh \
unisim/util/os/linux_os/environment.hh \
unisim/util/os/linux_os/files_flags.hh \
unisim/util/os/linux_os/linux.hh \
unisim/util/os/linux_os/ppc.hh \
unisim/util/os/linux_os/errno.hh \
unisim/util/dictionary/dictionary.hh \
unisim/util/lexer/lexer.hh \
unisim/util/parser/parser.hh \
unisim/util/xml/xml.hh \
unisim/util/endian/endian.hh \
unisim/util/garbage_collector/garbage_collector.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/random/random.hh \
unisim/util/hash_table/hash_table.hh \
unisim/util/queue/queue.hh \
unisim/util/simfloat/floating.hh \
unisim/util/simfloat/integer.hh \
unisim/util/simfloat/host_floating.hh \
unisim/util/ieee754/ieee754.hh \
unisim/util/inlining/inlining.hh \
unisim/util/generic_peripheral_register/generic_peripheral_register.hh \
unisim/component/tlm2/processor/arm/arm926ejs/arm926ejs.hh \
unisim/component/tlm2/memory/ram/memory.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_stubs.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_int_source_identifier.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/util/time_signal_splitter.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/pxp.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/watchdog/watchdog.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/dma/pl080/pl080.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/uart/pl011/pl011.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/sic/sic.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/dual_timer/dt.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/system_controller/sc.hh \
unisim/component/tlm2/chipset/arm926ejs_pxp/ethernet/smsc_lan91c111.hh \
unisim/component/cxx/processor/arm/psr.hh \
unisim/component/cxx/processor/arm/cpu.hh \
unisim/component/cxx/processor/arm/coprocessor_interface.hh \
unisim/component/cxx/processor/arm/arm926ejs/cpu.hh \
unisim/component/cxx/processor/arm/arm926ejs/lockdown_tlb.hh \
unisim/component/cxx/processor/arm/arm926ejs/cp15.hh \
unisim/component/cxx/processor/arm/arm926ejs/cp15interface.hh \
unisim/component/cxx/processor/arm/arm926ejs/cache.hh \
unisim/component/cxx/processor/arm/arm926ejs/tlb.hh \
unisim/component/cxx/processor/arm/memory_op.hh \
unisim/component/cxx/processor/arm/exception.hh \
unisim/component/cxx/processor/arm/execute.hh \
unisim/component/cxx/processor/arm/models.hh \
unisim/component/cxx/processor/arm/disasm.hh \
unisim/component/cxx/memory/ram/memory.hh"

UNISIM_LIB_ARM926EJS_TEMPLATE_FILES="\
unisim/service/tee/loader/tee.tcc \
unisim/service/tee/symbol_table_lookup/tee.tcc \
unisim/service/tee/blob/tee.tcc \
unisim/service/tee/stmt_lookup/tee.tcc \
unisim/service/tee/backtrace/tee.tcc \
unisim/service/tee/memory_access_reporting/tee.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/sim_debugger/sim_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/debug/debugger/debugger.tcc \
unisim/service/profiling/addr_profiler/profiler.tcc \
unisim/service/os/linux_os/linux.tcc \
unisim/service/loader/raw_loader/raw_loader.tcc \
unisim/service/loader/elf_loader/elf64_loader.tcc \
unisim/service/loader/elf_loader/elf32_loader.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/loader/coff_loader/coff_loader.tcc \
unisim/service/loader/s19_loader/s19_loader.tcc \
unisim/service/loader/multiformat_loader/multiformat_loader.tcc \
unisim/util/debug/profile.tcc \
unisim/util/debug/data_object_initializer.tcc \
unisim/util/debug/dwarf/die.tcc \
unisim/util/debug/dwarf/range.tcc \
unisim/util/debug/dwarf/addr_range.tcc \
unisim/util/debug/dwarf/call_frame_vm.tcc \
unisim/util/debug/dwarf/fde.tcc \
unisim/util/debug/dwarf/attr.tcc \
unisim/util/debug/dwarf/pub.tcc \
unisim/util/debug/dwarf/cu.tcc \
unisim/util/debug/dwarf/stmt_vm.tcc \
unisim/util/debug/dwarf/call_frame_prog.tcc \
unisim/util/debug/dwarf/expr_vm.tcc \
unisim/util/debug/dwarf/cie.tcc \
unisim/util/debug/dwarf/stmt_prog.tcc \
unisim/util/debug/dwarf/macinfo.tcc \
unisim/util/debug/dwarf/loc.tcc \
unisim/util/debug/dwarf/dwarf.tcc \
unisim/util/debug/dwarf/frame.tcc \
unisim/util/debug/dwarf/data_object.tcc \
unisim/util/debug/dwarf/subprogram.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/blob/section.tcc \
unisim/util/debug/blob/blob.tcc \
unisim/util/debug/blob/segment.tcc \
unisim/util/debug/symbol.tcc \
unisim/util/debug/elf_symtab/elf_symtab.tcc \
unisim/util/debug/coff_symtab/coff_symtab.tcc \
unisim/util/debug/stmt.tcc \
unisim/util/loader/elf_loader/elf_loader.tcc \
unisim/util/loader/coff_loader/coff_loader.tcc \
unisim/util/loader/coff_loader/ti/ti.tcc \
unisim/util/simfloat/floating.tcc \
unisim/util/simfloat/integer.tcc \
unisim/util/simfloat/host_floating.tcc \
unisim/util/os/linux_os/calls.tcc \
unisim/util/os/linux_os/linux.tcc \
unisim/util/dictionary/dictionary.tcc \
unisim/util/lexer/lexer.tcc \
unisim/util/parser/parser.tcc \
unisim/util/queue/queue.tcc \
unisim/component/tlm2/memory/ram/memory.tcc \
unisim/component/cxx/memory/ram/memory.tcc"

UNISIM_LIB_ARM926EJS_M4_FILES="\
m4/times.m4 \
m4/endian.m4 \
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/boost_graph.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/systemc.m4 \
m4/tlm20.m4 \
m4/with_boost.m4 \
m4/cacti.m4 \
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4 \
m4/pthread.m4"

UNISIM_LIB_ARM926EJS_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_armv5l.xml \
unisim/service/debug/gdb_server/gdb_armv4l.xml \
unisim/service/debug/gdb_server/gdb_armv5b.xml \
unisim/util/debug/dwarf/arm_eabi_dwarf_register_number_mapping.xml \
"

ARM926EJS_EXTERNAL_HEADERS="\
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

UNISIM_SIMULATORS_ARM926EJS_SOURCE_FILES="\
main.cc \
simulator.cc \
"
UNISIM_SIMULATORS_ARM926EJS_HEADER_FILES="\
simulator.hh \
"

UNISIM_SIMULATORS_ARM926EJS_EXTRA_FILES="\
config.h.in \
"

UNISIM_SIMULATORS_ARM926EJS_TEMPLATE_FILES=
UNISIM_SIMULATORS_ARM926EJS_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
boot-loader.bin \
"

UNISIM_SIMULATORS_ARM926EJS_TESTBENCH_FILES=""

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_arm926ejs_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/arm926ejs

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

UNISIM_LIB_ARM926EJS_FILES="${UNISIM_LIB_ARM926EJS_SOURCE_FILES} ${UNISIM_LIB_ARM926EJS_HEADER_FILES} ${UNISIM_LIB_ARM926EJS_TEMPLATE_FILES} ${UNISIM_LIB_ARM926EJS_DATA_FILES}"

for file in ${UNISIM_LIB_ARM926EJS_FILES}; do
	mkdir -p "${DEST_DIR}/arm926ejs/$(dirname ${file})"
	has_to_copy=no
	if [ -e "${DEST_DIR}/arm926ejs/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/arm926ejs/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/arm926ejs/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/arm926ejs/${file}" || exit
	fi
done

UNISIM_SIMULATORS_ARM926EJS_FILES="${UNISIM_SIMULATORS_ARM926EJS_SOURCE_FILES} ${UNISIM_SIMULATORS_ARM926EJS_HEADER_FILES} ${UNISIM_SIMULATORS_ARM926EJS_EXTRA_FILES} ${UNISIM_SIMULATORS_ARM926EJS_TEMPLATE_FILES} ${UNISIM_SIMULATORS_ARM926EJS_DATA_FILES} ${UNISIM_SIMULATORS_ARM926EJS_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATORS_ARM926EJS_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/arm926ejs/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/arm926ejs/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/arm926ejs/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/arm926ejs/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_ARM926EJS_DATA_FILES}; do
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
mkdir -p ${DEST_DIR}/arm926ejs/config
mkdir -p ${DEST_DIR}/arm926ejs/m4
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

for file in ${UNISIM_LIB_ARM926EJS_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/arm926ejs/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/arm926ejs/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/arm926ejs/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/arm926ejs/${file}" || exit
		has_to_build_arm926ejs_configure=yes
	fi
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>
Gilles Mouchard <gilles.mouchard@cea.fr>
Réda Nouacer <reda.nouacer@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - arm926ejs: an ARM V5 user level simulator
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
  - boost (http://www.boost.org) development package (libboost-devel for Redhat/Mandriva, libboost-graph-dev for Debian/Ubuntu)
  - libxml2 (http://xmlsoft.org/libxml2) development package (libxml2-devel for Redhat/Mandriva, libxml2-dev for Debian/Ubuntu)
  - zlib (http://www.zlib.net) development package (zlib1g-devel for Redhat/Mandriva, zlib1g-devel for Debian/Ubuntu)
  - libedit (http://www.thrysoee.dk/editline) development package (libedit-devel for Redhat/Mandriva, libedit-dev for Debian/Ubuntu)
  - Core SystemC Language >= 2.1 (http://www.systemc.org)
  - TLM Transaction Level Modeling Library, Release >= 2.0 (http://www.systemc.org)

Building instructions:
  $ ./configure --with-systemc=<path-to-systemc-install-dir> --with-tlm20=<path-to-TLM-library-install-dir>
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
	echo "AC_INIT([UNISIM ARMemu Standalone simulator], [${ARM926EJS_VERSION}], [Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>, Réda Nouacer <reda.nouacer@cea.fr>], [unisim-arm926ejs])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([arm926ejs])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib arm926ejs" > "${MAKEFILE_AM}"
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

# remove --host, --with-systemc, --with-tlm20, --with-zlib, --with-libxml2, --with-boost, --with-ncurses, --with-libedit from command line arguments
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
		--with-systemc=* | --with-tlm20=* | --with-zlib=* | --with-libxml2=* | --with-boost=* | --with-ncurses=* | --with-libedit=*)
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
	echo "=== configure help for arm926ejs"
else
	echo "=== configuring in arm926ejs (\${HERE}/arm926ejs) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/arm926ejs/configure \$@"
fi

if test ! -d \${HERE}/arm926ejs; then
	mkdir \${HERE}/arm926ejs
fi
cd \${HERE}/arm926ejs
\${MY_DIR}/arm926ejs/configure "\$@"
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
all: arm926ejs-all
clean: genisslib-clean arm926ejs-clean
distclean: genisslib-distclean arm926ejs-distclean
	rm -f \${HERE}/Makefile.cross
install: arm926ejs-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
arm926ejs-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/arm926ejs all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
arm926ejs-clean:
	@\\\$(MAKE) -C \${HERE}/arm926ejs clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
arm926ejs-distclean:
	@\\\$(MAKE) -C \${HERE}/arm926ejs distclean
arm926ejs-install:
	@\\\$(MAKE) -C \${HERE}/arm926ejs install
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


# arm926ejs

ARM926EJS_CONFIGURE_AC="${DEST_DIR}/arm926ejs/configure.ac"
ARM926EJS_MAKEFILE_AM="${DEST_DIR}/arm926ejs/Makefile.am"


if [ ! -e "${ARM926EJS_CONFIGURE_AC}" ]; then
	has_to_build_arm926ejs_configure=yes
else
	if [ "$0" -nt "${ARM926EJS_CONFIGURE_AC}" ]; then
		has_to_build_arm926ejs_configure=yes
	fi
fi

if [ ! -e "${ARM926EJS_MAKEFILE_AM}" ]; then
	has_to_build_arm926ejs_configure=yes
else
	if [ "$0" -nt "${ARM926EJS_MAKEFILE_AM}" ]; then
		has_to_build_arm926ejs_configure=yes
	fi
fi

if [ "${has_to_build_arm926ejs_configure}" = "yes" ]; then
	echo "Generating arm926ejs configure.ac"
	echo "AC_INIT([UNISIM ARMemu C++ simulator], [${ARM926EJS_VERSION}], [Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>, Réda Nouacer <reda.nouacer@cea.fr>], [unisim-arm926ejs-core])" > "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${ARM926EJS_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_PTHREAD(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TIMES(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ENDIAN(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_WITH_BOOST(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_GRAPH(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CACTI(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_REAL_PATH(main)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TLM20" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-arm926ejs-${ARM926EJS_VERSION}\"], [path of shared data relative to bin directory])" >> "${ARM926EJS_CONFIGURE_AC}"
	SIM_VERSION_MAJOR=$(printf "${ARM926EJS_VERSION}" | cut -f 1 -d .)
	SIM_VERSION_MINOR=$(printf "${ARM926EJS_VERSION}" | cut -f 2 -d .)
	SIM_VERSION_PATCH=$(printf "${ARM926EJS_VERSION}" | cut -f 3 -d .)
	SIM_VERSION="${ARM926EJS_VERSION}"
	SIM_VERSION_CODENAME="Triumphalis Tarraco"
	SIM_AUTHOR="Daniel Gracia Perez (daniel.gracia-perez@cea.fr)"
	SIM_PROGRAM_NAME="UNISIM ARMEmu"
	SIM_LICENSE="BSD (See file COPYING)"
	SIM_COPYRIGHT="Copyright (C) 2007-2010, Commissariat a l'Energie Atomique"
	SIM_DESCRIPTION="UNISIM ARMv5 User Level Simulator"
	SIM_SCHEMATIC="arm926ejs/fig_schematic.pdf"
	echo "AC_DEFINE([SIM_VERSION_MAJOR], [${SIM_VERSION_MAJOR}], [Version major number])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION_MINOR], [${SIM_VERSION_MINOR}], [Version minor number])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION_PATCH], [${SIM_VERSION_PATCH}], [Version patch number])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION], [\"${SIM_VERSION}\"], [Version])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION_CODENAME], [\"${SIM_VERSION_CODENAME}\"], [Version code name])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_AUTHOR], [\"${SIM_AUTHOR}\"], [Author])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_PROGRAM_NAME], [\"${SIM_PROGRAM_NAME}\"], [Program name])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_COPYRIGHT], [\"${SIM_COPYRIGHT}\"], [Copyright])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_LICENSE], [\"${SIM_LICENSE}\"], [License])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_DESCRIPTION], [\"${SIM_DESCRIPTION}\"], [Description])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_SCHEMATIC], [\"${SIM_SCHEMATIC}\"], [Schematic])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${ARM926EJS_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${ARM926EJS_CONFIGURE_AC}"

	AM_ARM926EJS_VERSION=$(printf ${ARM926EJS_VERSION} | sed -e 's/\./_/g')
	echo "Generating arm926ejs Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${ARM926EJS_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libarm926ejs-${ARM926EJS_VERSION}.a" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "libarm926ejs_${AM_ARM926EJS_VERSION}_a_SOURCES = ${UNISIM_LIB_ARM926EJS_SOURCE_FILES}" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-arm926ejs-${ARM926EJS_VERSION}" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "unisim_arm926ejs_${AM_ARM926EJS_VERSION}_SOURCES = ${UNISIM_SIMULATORS_ARM926EJS_SOURCE_FILES}" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "unisim_arm926ejs_${AM_ARM926EJS_VERSION}_CPPFLAGS = -DSIM_EXECUTABLE -DSIM_GDB_SERVER_SUPPORT -DSIM_INLINE_DEBUGGER_SUPPORT" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "unisim_arm926ejs_${AM_ARM926EJS_VERSION}_LDADD = libarm926ejs-${ARM926EJS_VERSION}.a" >> "${ARM926EJS_MAKEFILE_AM}"

	echo "noinst_HEADERS = ${UNISIM_LIB_ARM926EJS_HEADER_FILES} ${UNISIM_LIB_ARM926EJS_TEMPLATE_FILES} ${UNISIM_SIMULATORS_ARM926EJS_HEADER_FILES} ${UNISIM_SIMULATORS_ARM926EJS_TEMPLATE_FILES}" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_ARM926EJS_M4_FILES}" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-arm926ejs-${ARM926EJS_VERSION}" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_ARM926EJS_DATA_FILES} ${UNISIM_SIMULATORS_ARM926EJS_DATA_FILES}" >> "${ARM926EJS_MAKEFILE_AM}"

	echo -n "BUILT_SOURCES=" >> "${ARM926EJS_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.hh " >> "${ARM926EJS_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.tcc " >> "${ARM926EJS_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.hh " >> "${ARM926EJS_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.tcc " >> "${ARM926EJS_MAKEFILE_AM}"
	echo >> "${ARM926EJS_MAKEFILE_AM}"
	
	echo -n "CLEANFILES=" >> "${ARM926EJS_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.hh " >> "${ARM926EJS_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.tcc " >> "${ARM926EJS_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.hh " >> "${ARM926EJS_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.tcc " >> "${ARM926EJS_MAKEFILE_AM}"
	echo >> "${ARM926EJS_MAKEFILE_AM}"
	
	echo "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.tcc: \$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.hh" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.hh: ${UNISIM_LIB_ARM926EJS_ISA_ARM32_FILES}" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\t" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32 -w 8 -I \$(top_srcdir) -I \$(top_srcdir)/unisim/component/cxx/processor/arm/isa/arm32 \$(top_srcdir)/unisim/component/cxx/processor/arm/isa/arm32/arm32.isa" >> "${ARM926EJS_MAKEFILE_AM}"

	echo "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.tcc: \$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.hh" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.hh: ${UNISIM_LIB_ARM926EJS_ISA_THUMB_FILES}" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\t" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb -w 8 -I \$(top_srcdir) -I \$(top_srcdir)/unisim/component/cxx/processor/arm/isa/thumb \$(top_srcdir)/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa" >> "${ARM926EJS_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/bin/\$\$(basename \$\${PROGRAM})\"; \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_builddir)/\$\${PROGRAM}\" \$(top_builddir)/bin/\$\$(basename \"\$\${PROGRAM}\"); \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\tdone\n" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\tfi\n" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-arm926ejs-${ARM926EJS_VERSION}/\$\$(basename \$\${SHARED_DATA})\"; \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-arm926ejs-${ARM926EJS_VERSION}'; \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_srcdir)/\$\${SHARED_DATA}\" \$(top_builddir)/share/unisim-arm926ejs-${ARM926EJS_VERSION}/\$\$(basename \"\$\${SHARED_DATA}\"); \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\tdone\n" >> "${ARM926EJS_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >> "${ARM926EJS_MAKEFILE_AM}"
	printf "\tfi\n" >> "${ARM926EJS_MAKEFILE_AM}"

	${DISTCOPY} ${DEST_DIR}/INSTALL ${DEST_DIR}/arm926ejs
	${DISTCOPY} ${DEST_DIR}/README ${DEST_DIR}/arm926ejs
	${DISTCOPY} ${DEST_DIR}/AUTHORS ${DEST_DIR}/arm926ejs
	
	echo "Building arm926ejs configure"
	${SHELL} -c "cd ${DEST_DIR}/arm926ejs && aclocal -I m4 && autoconf --force && automake -ac"
fi

echo "Distribution is up-to-date"
