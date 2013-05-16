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
UNISIM_SIMULATORS_DIR=${MY_DIR}/../unisim_simulators/tlm2/virtex5fxt

VIRTEX5FXT_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-virtex5fxt-${VIRTEX5FXT_VERSION}

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

UNISIM_LIB_VIRTEX5FXT_SOURCE_FILES="\
unisim/kernel/service/service.cc \
unisim/kernel/service/xml_helper.cc \
unisim/kernel/tlm2/tlm.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/kernel/debug/debug.cc \
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
unisim/util/debug/data_object.cc \
unisim/util/debug/stmt_32.cc \
unisim/util/debug/stmt_64.cc \
unisim/util/debug/data_object.cc \
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
unisim/util/debug/blob/blob32.cc \
unisim/util/debug/blob/blob64.cc \
unisim/util/debug/blob/section32.cc \
unisim/util/debug/blob/section64.cc \
unisim/util/debug/blob/segment32.cc \
unisim/util/debug/blob/segment64.cc \
unisim/util/debug/elf_symtab/elf_symtab32.cc \
unisim/util/debug/elf_symtab/elf_symtab64.cc \
unisim/util/debug/coff_symtab/coff_symtab32.cc \
unisim/util/endian/endian.cc \
unisim/util/queue/queue.cc \
unisim/util/garbage_collector/garbage_collector.cc \
unisim/util/random/random.cc \
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/loader/elf_loader/elf64_loader.cc \
unisim/util/loader/coff_loader/coff_loader32.cc \
unisim/util/os/linux_os/environment.cc \
unisim/util/os/linux_os/linux.cc \
unisim/util/lexer/lexer.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/debug/debugger/debugger32.cc \
unisim/service/profiling/addr_profiler/profiler32.cc \
unisim/service/time/host_time/time.cc \
unisim/service/time/sc_time/time.cc \
unisim/service/power/cache_dynamic_energy.cc \
unisim/service/power/cache_dynamic_power.cc \
unisim/service/power/cache_leakage_power.cc \
unisim/service/power/cache_power_estimator.cc \
unisim/service/power/cache_profile.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/loader/elf_loader/elf64_loader.cc \
unisim/service/loader/raw_loader/raw_loader32.cc \
unisim/service/loader/s19_loader/s19_loader.cc \
unisim/service/loader/coff_loader/coff_loader32.cc \
unisim/service/loader/multiformat_loader/multiformat_loader.cc \
unisim/service/loader/multiformat_loader/multiformat_loader32.cc \
unisim/service/translator/memory_address/memory/translator.cc \
unisim/service/tee/loader/tee.cc \
unisim/service/tee/symbol_table_lookup/tee_32.cc \
unisim/service/tee/blob/tee_32.cc \
unisim/service/tee/stmt_lookup/tee_32.cc \
unisim/service/tee/backtrace/tee_32.cc \
unisim/service/tee/memory_access_reporting/tee_32.cc \
unisim/service/telnet/telnet.cc \
unisim/service/os/linux_os/linux.cc \
unisim/component/cxx/processor/powerpc/ppc440/cpu.cc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_debug.cc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_wommu.cc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_debug_wommu.cc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_wfpu.cc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_debug_wfpu.cc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_wommu_wfpu.cc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_debug_wommu_wfpu.cc \
unisim/component/cxx/processor/powerpc/floating.cc \
unisim/component/cxx/processor/powerpc/config.cc \
unisim/component/cxx/processor/powerpc/ppc440/config.cc \
unisim/component/cxx/processor/powerpc/ppc440/tb_debug_if.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc \
unisim/component/cxx/timer/xilinx/xps_timer/config.cc \
unisim/component/cxx/memory/flash/am29/am29.cc \
unisim/component/cxx/memory/flash/am29/s29gl256p.cc \
unisim/component/cxx/memory/flash/am29/s29gl256p_config.cc \
unisim/component/cxx/interconnect/xilinx/mci/mci.cc \
unisim/component/cxx/com/xilinx/xps_gpio/config.cc \
unisim/component/tlm2/timer/xilinx/xps_timer/capture_trigger_stub.cc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu.cc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu_debug.cc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu_wommu.cc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu_debug_wommu.cc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu_wfpu.cc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu_debug_wfpu.cc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu_wommu_wfpu.cc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu_debug_wommu_wfpu.cc \
unisim/component/tlm2/memory/ram/memory.cc \
unisim/component/tlm2/memory/ram/memory_debug.cc \
unisim/component/tlm2/interconnect/generic_router/variable_mapping.cc \
unisim/component/tlm2/memory/flash/am29/s29gl256p.cc \
unisim/component/tlm2/interconnect/xilinx/mci/mci.cc \
"

UNISIM_LIB_VIRTEX5FXT_ISA_FILES="\
unisim/component/cxx/processor/powerpc/isa/altivec.isa \
unisim/component/cxx/processor/powerpc/isa/condition.isa \
unisim/component/cxx/processor/powerpc/isa/integer.isa \
unisim/component/cxx/processor/powerpc/isa/misc.isa \
unisim/component/cxx/processor/powerpc/isa/ppc.isa \
unisim/component/cxx/processor/powerpc/isa/branch.isa \
unisim/component/cxx/processor/powerpc/isa/floating.isa \
unisim/component/cxx/processor/powerpc/isa/optional_floating.isa \
unisim/component/cxx/processor/powerpc/isa/int_load_store.isa \
unisim/component/cxx/processor/powerpc/isa/byte_reverse_load_store.isa \
unisim/component/cxx/processor/powerpc/isa/string_load_store.isa \
unisim/component/cxx/processor/powerpc/isa/multiple_load_store.isa \
unisim/component/cxx/processor/powerpc/isa/fp_load_store.isa \
unisim/component/cxx/processor/powerpc/isa/external_control.isa \
unisim/component/cxx/processor/powerpc/isa/synchronization.isa \
unisim/component/cxx/processor/powerpc/isa/cache_management.isa \
unisim/component/cxx/processor/powerpc/isa/tlb_management.isa \
unisim/component/cxx/processor/powerpc/isa/specialization.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/ppc440.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/cache_management.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/tlb_management.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/synchronization.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/integer.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/misc.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/esr.isa"

UNISIM_LIB_VIRTEX5FXT_HEADER_FILES="${UNISIM_LIB_VIRTEX5FXT_ISA_FILES} \
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/tlm2/tlm.hh \
unisim/kernel/debug/debug.hh \
unisim/kernel/api/api.hh \
unisim/util/likely/likely.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/debug/memory_access_type.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/event.hh \
unisim/util/debug/profile.hh \
unisim/util/debug/register.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/stmt.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/data_object.hh \
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
unisim/util/debug/dwarf/c_loc_expr_parser.hh \
unisim/util/debug/blob/blob.hh \
unisim/util/debug/blob/section.hh \
unisim/util/debug/blob/segment.hh \
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
unisim/util/os/linux_os/arm.hh \
unisim/util/os/linux_os/aux_table.hh \
unisim/util/os/linux_os/environment.hh \
unisim/util/os/linux_os/files_flags.hh \
unisim/util/os/linux_os/linux.hh \
unisim/util/os/linux_os/ppc.hh \
unisim/util/dictionary/dictionary.hh \
unisim/util/lexer/lexer.hh \
unisim/util/parser/parser.hh \
unisim/service/interfaces/debug_control.hh \
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
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/linux_os.hh \
unisim/service/interfaces/cache_power_estimator.hh \
unisim/service/interfaces/power_mode.hh \
unisim/service/interfaces/synchronizable.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/interfaces/blob.hh \
unisim/service/interfaces/char_io.hh \
unisim/service/interfaces/keyboard.hh \
unisim/service/interfaces/led_board.hh \
unisim/service/interfaces/backtrace.hh \
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/debug/debugger/debugger.hh \
unisim/service/profiling/addr_profiler/profiler.hh \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/elf_loader/elf64_loader.hh \
unisim/service/loader/raw_loader/raw_loader.hh \
unisim/service/loader/s19_loader/s19_loader.hh \
unisim/service/loader/coff_loader/coff_loader.hh \
unisim/service/loader/multiformat_loader/multiformat_loader.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/service/power/cache_power_estimator.hh \
unisim/service/power/cache_profile.hh \
unisim/service/power/cache_dynamic_energy.hh \
unisim/service/power/cache_dynamic_power.hh \
unisim/service/power/cache_leakage_power.hh \
unisim/service/translator/memory_address/memory/translator.hh \
unisim/service/tee/loader/tee.hh \
unisim/service/tee/symbol_table_lookup/tee.hh \
unisim/service/tee/blob/tee.hh \
unisim/service/tee/stmt_lookup/tee.hh \
unisim/service/tee/backtrace/tee.hh \
unisim/service/tee/memory_access_reporting/tee.hh \
unisim/service/telnet/telnet.hh \
unisim/service/os/linux_os/linux.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/processor/powerpc/exception.hh \
unisim/component/cxx/processor/powerpc/floating.hh \
unisim/component/cxx/processor/powerpc/config.hh \
unisim/component/cxx/processor/powerpc/ppc440/cpu.hh \
unisim/component/cxx/processor/powerpc/ppc440/exception.hh \
unisim/component/cxx/processor/powerpc/ppc440/config.hh \
unisim/component/cxx/cache/cache.hh \
unisim/component/cxx/tlb/tlb.hh \
unisim/component/cxx/interrupt/xilinx/xps_intc/xps_intc.hh \
unisim/component/cxx/interrupt/xilinx/xps_intc/config.hh \
unisim/component/cxx/timer/xilinx/xps_timer/xps_timer.hh \
unisim/component/cxx/timer/xilinx/xps_timer/config.hh \
unisim/component/cxx/memory/flash/am29/types.hh \
unisim/component/cxx/memory/flash/am29/am29.hh \
unisim/component/cxx/memory/flash/am29/s29gl256p_config.hh \
unisim/component/cxx/interconnect/xilinx/dcr_controller/dcr_controller.hh \
unisim/component/cxx/interconnect/xilinx/dcr_controller/config.hh \
unisim/component/cxx/interconnect/xilinx/crossbar/crossbar.hh \
unisim/component/cxx/interconnect/xilinx/crossbar/config.hh \
unisim/component/cxx/interconnect/xilinx/mci/mci.hh \
unisim/component/cxx/interconnect/xilinx/mci/config.hh \
unisim/component/cxx/com/xilinx/xps_uart_lite/xps_uart_lite.hh \
unisim/component/cxx/com/xilinx/xps_uart_lite/config.hh \
unisim/component/cxx/com/xilinx/xps_gpio/xps_gpio.hh \
unisim/component/cxx/com/xilinx/xps_gpio/config.hh \
unisim/component/tlm2/interrupt/types.hh \
unisim/component/tlm2/processor/powerpc/ppc440/cpu.hh \
unisim/component/tlm2/memory/ram/memory.hh \
unisim/component/tlm2/interrupt/xilinx/xps_intc/xps_intc.hh \
unisim/component/tlm2/timer/xilinx/xps_timer/xps_timer.hh \
unisim/component/tlm2/timer/xilinx/xps_timer/capture_trigger_stub.hh \
unisim/component/tlm2/interconnect/generic_router/router.hh \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.hh \
unisim/component/tlm2/interconnect/generic_router/config.hh \
unisim/component/tlm2/memory/flash/am29/am29.hh \
unisim/component/tlm2/interconnect/xilinx/dcr_controller/dcr_controller.hh \
unisim/component/tlm2/interconnect/xilinx/crossbar/crossbar.hh \
unisim/component/tlm2/interconnect/xilinx/mci/mci.hh \
unisim/component/tlm2/com/xilinx/xps_uart_lite/xps_uart_lite.hh \
unisim/component/tlm2/com/xilinx/xps_gpio/xps_gpio.hh \
unisim/component/tlm2/com/xilinx/xps_gpio/gpio_leds.hh \
unisim/component/tlm2/com/xilinx/xps_gpio/gpio_switches.hh \
"

UNISIM_LIB_VIRTEX5FXT_TEMPLATE_FILES="\
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/profile.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/util/debug/stmt.tcc \
unisim/util/debug/data_object.tcc \
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
unisim/util/debug/blob/blob.tcc \
unisim/util/debug/blob/section.tcc \
unisim/util/debug/blob/segment.tcc \
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
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/debug/debugger/debugger.tcc \
unisim/service/profiling/addr_profiler/profiler.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/loader/elf_loader/elf32_loader.tcc \
unisim/service/loader/elf_loader/elf64_loader.tcc \
unisim/service/loader/raw_loader/raw_loader.tcc \
unisim/service/loader/s19_loader/s19_loader.tcc \
unisim/service/loader/coff_loader/coff_loader.tcc \
unisim/service/loader/multiformat_loader/multiformat_loader.tcc \
unisim/service/translator/memory_address/memory/translator.tcc \
unisim/service/tee/loader/tee.tcc \
unisim/service/tee/symbol_table_lookup/tee.tcc \
unisim/service/tee/blob/tee.tcc \
unisim/service/tee/stmt_lookup/tee.tcc \
unisim/service/tee/backtrace/tee.tcc \
unisim/service/tee/memory_access_reporting/tee.tcc \
unisim/service/os/linux_os/linux.tcc \
unisim/component/cxx/processor/powerpc/exception.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu.tcc \
unisim/component/cxx/processor/powerpc/ppc440/exception.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_cache.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_debugging.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_exception_handling.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_fetch.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_load_store.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_mmu.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_dcr.tcc \
unisim/component/cxx/memory/ram/memory.tcc \
unisim/component/cxx/cache/cache.tcc \
unisim/component/cxx/tlb/tlb.tcc \
unisim/component/cxx/interrupt/xilinx/xps_intc/xps_intc.tcc \
unisim/component/cxx/timer/xilinx/xps_timer/xps_timer.tcc \
unisim/component/cxx/memory/flash/am29/am29.tcc \
unisim/component/cxx/interconnect/xilinx/dcr_controller/dcr_controller.tcc \
unisim/component/cxx/interconnect/xilinx/crossbar/crossbar.tcc \
unisim/component/cxx/interconnect/xilinx/mci/mci.tcc \
unisim/component/cxx/com/xilinx/xps_uart_lite/xps_uart_lite.tcc \
unisim/component/cxx/com/xilinx/xps_gpio/xps_gpio.tcc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu.tcc \
unisim/component/tlm2/memory/ram/memory.tcc \
unisim/component/tlm2/interrupt/xilinx/xps_intc/xps_intc.tcc \
unisim/component/tlm2/timer/xilinx/xps_timer/xps_timer.tcc \
unisim/component/tlm2/interconnect/generic_router/router.tcc \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.tcc \
unisim/component/tlm2/memory/flash/am29/am29.tcc \
unisim/component/tlm2/interconnect/xilinx/dcr_controller/dcr_controller.tcc \
unisim/component/tlm2/interconnect/xilinx/crossbar/crossbar.tcc \
unisim/component/tlm2/interconnect/xilinx/mci/mci.tcc \
unisim/component/tlm2/com/xilinx/xps_uart_lite/xps_uart_lite.tcc \
unisim/component/tlm2/com/xilinx/xps_gpio/xps_gpio.tcc \
unisim/component/tlm2/com/xilinx/xps_gpio/gpio_leds.tcc \
unisim/component/tlm2/com/xilinx/xps_gpio/gpio_switches.tcc \
"

UNISIM_LIB_VIRTEX5FXT_M4_FILES="\
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
m4/real_path.m4"

UNISIM_LIB_VIRTEX5FXT_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_ppc440.xml \
unisim/service/debug/gdb_server/gdb_ppc440fp.xml \
unisim/util/debug/dwarf/powerpc_eabi_dwarf_register_number_mapping.xml \
unisim/util/debug/dwarf/powerpc_eabi_gcc_dwarf_register_number_mapping.xml"

VIRTEX5FXT_EXTERNAL_HEADERS="\
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

UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES="\
config.cc \
mplb.cc \
mplb_debug.cc \
intc.cc \
timer.cc \
dcr_controller.cc \
crossbar.cc \
uart_lite.cc \
gpio_dip_switches_8bit.cc \
gpio_leds_8bit.cc \
gpio_5_leds_positions.cc \
gpio_push_buttons_5bit.cc \
dip_switches_8bit.cc \
leds_8bit.cc \
5_leds_positions.cc \
push_buttons_5bit.cc \
memory_router.cc \
memory_router_debug.cc \
"
UNISIM_SIMULATORS_VIRTEX5FXT_HEADER_FILES="\
simulator.hh \
config.hh \
"

UNISIM_SIMULATORS_VIRTEX5FXT_TEMPLATE_FILES="\
simulator.tcc \
"

UNISIM_SIMULATORS_VIRTEX5FXT_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
template_default_config.xml \
unisim.ico"

UNISIM_SIMULATORS_VIRTEX5FXT_TESTBENCH_FILES="\
main.cc \
main_wfpu.cc \
main_debug.cc \
main_wfpu_debug.cc \
"

has_to_build_configure=no
has_to_build_configure_cross=no
has_to_build_genisslib_configure=no
has_to_build_virtex5fxt_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/virtex5fxt

UNISIM_TOOLS_GENISSLIB_FILES="${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES} ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES} ${UNISIM_TOOLS_GENISSLIB_DATA_FILES}"

for file in ${UNISIM_TOOLS_GENISSLIB_FILES}; do
	mkdir -p "${DEST_DIR}/`dirname ${file}`"
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

UNISIM_LIB_VIRTEX5FXT_FILES="${UNISIM_LIB_VIRTEX5FXT_SOURCE_FILES} ${UNISIM_LIB_VIRTEX5FXT_HEADER_FILES} ${UNISIM_LIB_VIRTEX5FXT_TEMPLATE_FILES} ${UNISIM_LIB_VIRTEX5FXT_DATA_FILES}"

for file in ${UNISIM_LIB_VIRTEX5FXT_FILES}; do
	mkdir -p "${DEST_DIR}/virtex5fxt/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/virtex5fxt/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/virtex5fxt/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/virtex5fxt/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/virtex5fxt/${file}" || exit
	fi
done

UNISIM_SIMULATORS_VIRTEX5FXT_FILES="${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_HEADER_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_TEMPLATE_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_DATA_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATORS_VIRTEX5FXT_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/virtex5fxt/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/virtex5fxt/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/virtex5fxt/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/virtex5fxt/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_VIRTEX5FXT_DATA_FILES}; do
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
mkdir -p ${DEST_DIR}/virtex5fxt/config
mkdir -p ${DEST_DIR}/virtex5fxt/m4
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

for file in ${UNISIM_LIB_VIRTEX5FXT_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/virtex5fxt/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/virtex5fxt/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/virtex5fxt/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/virtex5fxt/${file}" || exit
		has_to_build_virtex5fxt_configure=yes
	fi
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Gilles Mouchard <gilles.mouchard@cea.fr>
Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>
Réda Nouacer <reda.nouacer@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - UNISIM GenISSLib: an instruction set simulator generator
  - UNISIM Virtex 5 FXT Simulator: a full system Virtex-5-FXT-like simulator including a PPC440x5 and some Xilinx Virtex 5 FXT IPs.
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
	echo "AC_INIT([UNISIM Virtex 5 FXT Simulator Package], [${VIRTEX5FXT_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-virtex5fxt])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([virtex5fxt])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib virtex5fxt" > "${MAKEFILE_AM}"
	echo "EXTRA_DIST = configure.cross" >> "${MAKEFILE_AM}"

	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

if [ "${has_to_build_configure_cross}" = "yes" ]; then
	echo "Building configure.cross"
	cat << EOF_CONFIGURE_CROSS > "${CONFIGURE_CROSS}"
#!/bin/bash
HERE=\$(pwd)
MY_DIR=\$(dirname \$0)
if test \${MY_DIR} = "."; then
	MY_DIR=\${HERE}
elif test \${MY_DIR} = ".."; then
	MY_DIR=\${HERE}/..
fi

# remove --host, --with-systemc, --with-tlm20, --with-zlib, --with-libxml2, --with-boost, --with-ncurses, --with-libedit from command line arguments
host=""
help=""
old_args=$@
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
	echo "=== configure help for virtex5fxt"
else
	echo "=== configuring in virtex5fxt (\${HERE}/virtex5fxt) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/virtex5fxt/configure \$@"
fi

if test ! -d \${HERE}/virtex5fxt; then
	mkdir \${HERE}/virtex5fxt
fi
cd \${HERE}/virtex5fxt
\${MY_DIR}/virtex5fxt/configure "\$@"
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
all: virtex5fxt-all
clean: genisslib-clean virtex5fxt-clean
distclean: genisslib-distclean virtex5fxt-distclean
	rm -f \${HERE}/Makefile.cross
install: virtex5fxt-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
virtex5fxt-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/virtex5fxt all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
virtex5fxt-clean:
	@\\\$(MAKE) -C \${HERE}/virtex5fxt clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
virtex5fxt-distclean:
	@\\\$(MAKE) -C \${HERE}/virtex5fxt distclean
virtex5fxt-install:
	@\\\$(MAKE) -C \${HERE}/virtex5fxt install
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

	AM_GENISSLIB_VERSION=`printf ${GENISSLIB_VERSION} | sed -e 's/\./_/g'`
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


# virtex5fxt

VIRTEX5FXT_CONFIGURE_AC="${DEST_DIR}/virtex5fxt/configure.ac"
VIRTEX5FXT_MAKEFILE_AM="${DEST_DIR}/virtex5fxt/Makefile.am"


if [ ! -e "${VIRTEX5FXT_CONFIGURE_AC}" ]; then
	has_to_build_virtex5fxt_configure=yes
else
	if [ "$0" -nt "${VIRTEX5FXT_CONFIGURE_AC}" ]; then
		has_to_build_virtex5fxt_configure=yes
	fi
fi

if [ ! -e "${VIRTEX5FXT_MAKEFILE_AM}" ]; then
	has_to_build_virtex5fxt_configure=yes
else
	if [ "$0" -nt "${VIRTEX5FXT_MAKEFILE_AM}" ]; then
		has_to_build_virtex5fxt_configure=yes
	fi
fi

if [ "${has_to_build_virtex5fxt_configure}" = "yes" ]; then
	echo "Generating virtex5fxt configure.ac"
	echo "AC_INIT([UNISIM Virtex 5 FXT Standalone simulator], [${VIRTEX5FXT_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-virtex5fxt-core])" > "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "CPPFLAGS=\"${CPPFLAGS} -D_LARGEFILE64_SOURCE\"" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${VIRTEX5FXT_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TIMES(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ENDIAN(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CACTI(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_REAL_PATH(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_WITH_BOOST(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_GRAPH(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TLM20" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-virtex5fxt-${VIRTEX5FXT_VERSION}\"], [path of shared data relative to bin directory])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${VIRTEX5FXT_CONFIGURE_AC}"

	AM_VIRTEX5FXT_VERSION=`printf ${VIRTEX5FXT_VERSION} | sed -e 's/\./_/g'`
	echo "Generating virtex5fxt Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libvirtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "libvirtex5fxt_${AM_VIRTEX5FXT_VERSION}_a_SOURCES = ${UNISIM_LIB_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-virtex5fxt-${VIRTEX5FXT_VERSION} unisim-virtex5fxt-debug-${VIRTEX5FXT_VERSION} unisim-virtex5fxt-wfpu-${VIRTEX5FXT_VERSION} unisim-virtex5fxt-wfpu-debug-${VIRTEX5FXT_VERSION}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "unisim_virtex5fxt_${AM_VIRTEX5FXT_VERSION}_SOURCES = main.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "unisim_virtex5fxt_wfpu_${AM_VIRTEX5FXT_VERSION}_SOURCES = main_wfpu.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "unisim_virtex5fxt_debug_${AM_VIRTEX5FXT_VERSION}_SOURCES = main_debug.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "unisim_virtex5fxt_wfpu_debug_${AM_VIRTEX5FXT_VERSION}_SOURCES = main_wfpu_debug.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "unisim_virtex5fxt_${AM_VIRTEX5FXT_VERSION}_LDADD = libvirtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "unisim_virtex5fxt_wfpu_${AM_VIRTEX5FXT_VERSION}_LDADD = libvirtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "unisim_virtex5fxt_debug_${AM_VIRTEX5FXT_VERSION}_LDADD = libvirtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "unisim_virtex5fxt_wfpu_debug_${AM_VIRTEX5FXT_VERSION}_LDADD = libvirtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"

	echo "noinst_HEADERS = ${UNISIM_LIB_VIRTEX5FXT_HEADER_FILES} ${UNISIM_LIB_VIRTEX5FXT_TEMPLATE_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_HEADER_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_TEMPLATE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_VIRTEX5FXT_M4_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-virtex5fxt-${VIRTEX5FXT_VERSION}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_VIRTEX5FXT_DATA_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_DATA_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh \$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.tcc" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh \$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.tcc" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.tcc: \$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh: ${UNISIM_LIB_VIRTEX5FXT_ISA_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\t" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/powerpc/ppc440/isa/ppc440.isa" >> "${VIRTEX5FXT_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/bin/\$\$(basename \$\${PROGRAM})\"; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_builddir)/\$\${PROGRAM}\" \$(top_builddir)/bin/\$\$(basename \"\$\${PROGRAM}\"); \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tdone\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tfi\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-virtex5fxt-${VIRTEX5FXT_VERSION}/\$\$(basename \$\${SHARED_DATA})\"; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-virtex5fxt-${VIRTEX5FXT_VERSION}'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_srcdir)/\$\${SHARED_DATA}\" \$(top_builddir)/share/unisim-virtex5fxt-${VIRTEX5FXT_VERSION}/\$\$(basename \"\$\${SHARED_DATA}\"); \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tdone\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tfi\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"

	echo "Building powerpc configure"
	${SHELL} -c "cd ${DEST_DIR}/virtex5fxt && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
