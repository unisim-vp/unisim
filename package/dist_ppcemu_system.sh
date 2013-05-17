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
UNISIM_SIMULATORS_DIR=${MY_DIR}/../unisim_simulators/tlm/ppcemu_system

PPCEMU_SYSTEM_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-ppcemu_system-${PPCEMU_SYSTEM_VERSION}

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

UNISIM_LIB_PPCEMU_SYSTEM_SOURCE_FILES="\
unisim/kernel/service/service.cc \
unisim/kernel/service/xml_helper.cc \
unisim/kernel/tlm/tlm.cc \
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
unisim/util/debug/dwarf/abbrev.cc \
unisim/util/debug/dwarf/attr.cc \
unisim/util/debug/dwarf/dwarf32.cc \
unisim/util/debug/dwarf/dwarf64.cc \
unisim/util/debug/dwarf/class.cc \
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
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/loader/elf_loader/elf64_loader.cc \
unisim/util/loader/coff_loader/coff_loader32.cc \
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
unisim/service/loader/pmac_linux_kernel_loader/pmac_linux_kernel_loader.cc \
unisim/service/loader/pmac_bootx/pmac_bootx.cc \
unisim/service/sdl/sdl.cc \
unisim/service/tee/memory_access_reporting/tee_32.cc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu.cc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu_debug.cc \
unisim/component/cxx/processor/powerpc/floating.cc \
unisim/component/cxx/processor/powerpc/config.cc \
unisim/component/cxx/processor/powerpc/mpc7447a/config.cc \
unisim/component/cxx/processor/powerpc/mpc7447a/vr_debug_if.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/pci/video/display_32.cc \
unisim/component/cxx/pci/macio/heathrow_32.cc \
unisim/component/cxx/bridge/pci_isa/bridge_32.cc \
unisim/component/cxx/isa/i8042/i8042.cc \
unisim/component/cxx/memory/flash/am29/am29.cc \
unisim/component/cxx/memory/flash/am29/am29lv800b.cc \
unisim/component/cxx/memory/flash/am29/am29lv800b_config.cc \
unisim/component/cxx/chipset/mpc107/address_map_entry.cc \
unisim/component/cxx/chipset/mpc107/config_regs.cc \
unisim/component/cxx/chipset/mpc107/address_maps.cc \
unisim/component/cxx/chipset/mpc107/address_maps_debug.cc \
unisim/component/cxx/chipset/mpc107/pci_controller.cc \
unisim/component/cxx/chipset/mpc107/pci_controller_debug.cc \
unisim/component/cxx/chipset/mpc107/atu/atu.cc \
unisim/component/cxx/chipset/mpc107/atu/atu_debug.cc \
unisim/component/cxx/chipset/mpc107/atu/register.cc \
unisim/component/cxx/chipset/mpc107/dma/buffer_32.cc \
unisim/component/cxx/chipset/mpc107/dma/dma_32_debug.cc \
unisim/component/cxx/chipset/mpc107/dma/register.cc \
unisim/component/cxx/chipset/mpc107/dma/dma_32.cc \
unisim/component/cxx/chipset/mpc107/dma/dma_32_debug.cc \
unisim/component/cxx/chipset/mpc107/epic/epic_32.cc \
unisim/component/cxx/chipset/mpc107/epic/epic_32_debug.cc \
unisim/component/cxx/chipset/mpc107/epic/inservice_reg.cc \
unisim/component/cxx/chipset/mpc107/epic/register.cc \
unisim/component/cxx/pci/ide/disk_image.cc \
unisim/component/cxx/pci/ide/ide_ctrl.cc \
unisim/component/cxx/pci/ide/ide_disk.cc \
unisim/component/cxx/pci/ide/pcidev.cc \
unisim/component/cxx/pci/types.cc \
unisim/component/tlm/isa/i8042/i8042.cc \
unisim/component/tlm/processor/powerpc/mpc7447a/cpu.cc \
unisim/component/tlm/processor/powerpc/mpc7447a/cpu_debug.cc \
unisim/component/tlm/bridge/pci_isa/bridge_32.cc \
unisim/component/tlm/pci/ide/pci_dev_32.cc \
unisim/component/tlm/pci/ide/pci_ide_module_32.cc \
unisim/component/tlm/pci/video/display_32.cc \
unisim/component/tlm/pci/macio/heathrow_32.cc \
unisim/component/tlm/message/interrupt.cc \
unisim/component/tlm/chipset/mpc107/epic/epic_32.cc \
unisim/component/tlm/chipset/mpc107/epic/epic_32_debug.cc \
unisim/component/tlm/chipset/mpc107/epic/timer.cc \
unisim/component/tlm/chipset/mpc107/mpc107_fsb32_pci32.cc \
unisim/component/tlm/chipset/mpc107/mpc107_fsb32_pci32_debug.cc \
unisim/component/tlm/fsb/snooping_bus/bus_addr32_size32_procs1.cc \
unisim/component/tlm/memory/ram/memory_32.cc \
unisim/component/tlm/memory/ram/memory_32_debug.cc \
unisim/component/tlm/memory/flash/am29/am29lv800b.cc"

UNISIM_LIB_PPCEMU_SYSTEM_ISA_FILES="\
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
unisim/component/cxx/processor/powerpc/mpc7447a/isa/mpc7447a.isa \
unisim/component/cxx/processor/powerpc/mpc7447a/isa/synchronization.isa \
unisim/component/cxx/processor/powerpc/mpc7447a/isa/tlb_management.isa \
unisim/component/cxx/processor/powerpc/mpc7447a/isa/misc.isa \
unisim/component/cxx/processor/powerpc/mpc7447a/isa/perf.isa"

UNISIM_LIB_PPCEMU_SYSTEM_HEADER_FILES="${UNISIM_LIB_PPCEMU_SYSTEM_ISA_FILES} \
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/tlm/tlm.hh \
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
unisim/util/debug/dwarf/encoding.hh \
unisim/util/debug/dwarf/class.hh \
unisim/util/debug/dwarf/cu.hh \
unisim/util/debug/dwarf/dwarf.hh \
unisim/util/debug/dwarf/fde.hh \
unisim/util/debug/dwarf/fmt.hh \
unisim/util/debug/dwarf/version.hh \
unisim/util/debug/dwarf/option.hh \
unisim/util/debug/dwarf/filename.hh \
unisim/util/debug/dwarf/fmt.hh \
unisim/util/debug/dwarf/leb128.hh \
unisim/util/debug/dwarf/loc.hh \
unisim/util/debug/dwarf/ml.hh \
unisim/util/debug/dwarf/pub.hh \
unisim/util/debug/dwarf/stmt_prog.hh \
unisim/util/debug/dwarf/addr_range.hh \
unisim/util/debug/dwarf/call_frame_prog.hh \
unisim/util/debug/dwarf/cie.hh \
unisim/util/debug/dwarf/die.hh \
unisim/util/debug/dwarf/expr_vm.hh \
unisim/util/debug/dwarf/fwd.hh \
unisim/util/debug/dwarf/macinfo.hh \
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
unisim/util/loader/elf_loader/elf_common.h \
unisim/util/loader/elf_loader/elf_loader.hh \
unisim/util/loader/elf_loader/elf32.h \
unisim/util/loader/elf_loader/elf64.h \
unisim/util/loader/elf_loader/elf32_loader.hh \
unisim/util/loader/elf_loader/elf64_loader.hh \
unisim/util/loader/coff_loader/coff_loader.hh \
unisim/util/loader/coff_loader/ti/ti.hh \
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
unisim/service/interfaces/os.hh \
unisim/service/interfaces/cache_power_estimator.hh \
unisim/service/interfaces/power_mode.hh \
unisim/service/interfaces/synchronizable.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/interfaces/blob.hh \
unisim/service/interfaces/backtrace.hh \
unisim/service/interfaces/video.hh \
unisim/service/interfaces/keyboard.hh \
unisim/service/interfaces/mouse.hh \
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/debug/debugger/debugger.hh \
unisim/service/tee/memory_access_reporting/tee.hh \
unisim/service/profiling/addr_profiler/profiler.hh \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/pmac_linux_kernel_loader/pmac_linux_kernel_loader.hh \
unisim/service/loader/pmac_bootx/pmac_bootx.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/service/power/cache_power_estimator.hh \
unisim/service/power/cache_profile.hh \
unisim/service/power/cache_dynamic_energy.hh \
unisim/service/power/cache_dynamic_power.hh \
unisim/service/power/cache_leakage_power.hh \
unisim/service/sdl/sdl.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/processor/powerpc/exception.hh \
unisim/component/cxx/processor/powerpc/floating.hh \
unisim/component/cxx/processor/powerpc/config.hh \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu.hh \
unisim/component/cxx/processor/powerpc/mpc7447a/exception.hh \
unisim/component/cxx/processor/powerpc/mpc7447a/config.hh \
unisim/component/cxx/cache/cache.hh \
unisim/component/cxx/tlb/tlb.hh \
unisim/component/cxx/memory/flash/am29/am29.hh \
unisim/component/cxx/memory/flash/am29/am29lv800b_config.hh \
unisim/component/cxx/memory/flash/am29/types.hh \
unisim/component/cxx/chipset/mpc107/address_maps.hh \
unisim/component/cxx/chipset/mpc107/address_map_entry.hh \
unisim/component/cxx/chipset/mpc107/config_regs.hh \
unisim/component/cxx/chipset/mpc107/pci_controller.hh \
unisim/component/cxx/chipset/mpc107/dma/dma.hh \
unisim/component/cxx/chipset/mpc107/dma/dma_client_interface.hh \
unisim/component/cxx/chipset/mpc107/atu/atu.hh \
unisim/component/cxx/chipset/mpc107/epic/register.hh \
unisim/component/cxx/chipset/mpc107/epic/inservice_reg.hh \
unisim/component/cxx/chipset/mpc107/atu/register.hh \
unisim/component/cxx/chipset/mpc107/dma/buffer.hh \
unisim/component/cxx/chipset/mpc107/dma/register.hh \
unisim/component/cxx/chipset/mpc107/epic/epic.hh \
unisim/component/cxx/pci/types.hh \
unisim/component/cxx/pci/video/display.hh \
unisim/component/cxx/pci/ide/disk_image.hh \
unisim/component/cxx/pci/ide/ide_disk.hh \
unisim/component/cxx/pci/ide/ide_ctrl.hh \
unisim/component/cxx/pci/ide/ide_atareg.hh \
unisim/component/cxx/pci/ide/pcidev.hh \
unisim/component/cxx/pci/ide/pcireg.hh \
unisim/component/cxx/pci/ide/pci_master.hh \
unisim/component/cxx/pci/ide/ide_wdcreg.hh \
unisim/component/cxx/pci/ide/event.hh \
unisim/component/cxx/pci/ide/event_manager.hh \
unisim/component/cxx/pci/macio/heathrow.hh \
unisim/component/cxx/bridge/pci_isa/bridge.hh \
unisim/component/cxx/isa/types.hh \
unisim/component/cxx/isa/i8042/i8042.hh \
unisim/component/tlm/processor/powerpc/mpc7447a/cpu.hh \
unisim/component/tlm/message/snooping_fsb.hh \
unisim/component/tlm/message/interrupt.hh \
unisim/component/tlm/debug/transaction_spy.hh \
unisim/component/tlm/message/memory.hh \
unisim/component/tlm/memory/ram/memory.hh \
unisim/component/tlm/memory/flash/am29/am29.hh \
unisim/component/tlm/fsb/snooping_bus/bus.hh \
unisim/component/tlm/chipset/mpc107/mpc107.hh \
unisim/component/tlm/chipset/mpc107/epic/epic.hh \
unisim/component/tlm/chipset/mpc107/epic/timer.hh \
unisim/component/tlm/pci/bus/bus.hh \
unisim/component/tlm/pci/video/display.hh \
unisim/component/tlm/pci/ide/pci_ide_module.hh \
unisim/component/tlm/pci/ide/pci_dev.hh \
unisim/component/tlm/pci/macio/heathrow.hh \
unisim/component/tlm/bridge/pci_isa/bridge.hh \
unisim/component/tlm/isa/i8042/i8042.hh \
unisim/component/tlm/message/pci.hh \
unisim/component/tlm/message/isa.hh"

UNISIM_LIB_PPCEMU_SYSTEM_TEMPLATE_FILES="\
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
unisim/util/dictionary/dictionary.tcc \
unisim/util/lexer/lexer.tcc \
unisim/util/parser/parser.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/debug/debugger/debugger.tcc \
unisim/service/profiling/addr_profiler/profiler.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/loader/elf_loader/elf32_loader.tcc \
unisim/service/sdl/sdl.tcc \
unisim/service/tee/memory_access_reporting/tee.tcc \
unisim/component/cxx/processor/powerpc/exception.tcc \
unisim/component/cxx/processor/powerpc/mpc7447a/exception.tcc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu.tcc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu_cache.tcc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu_debugging.tcc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu_exception_handling.tcc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu_fetch.tcc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu_load_store.tcc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu_mmu.tcc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu_perf_model.tcc \
unisim/component/cxx/memory/ram/memory.tcc \
unisim/component/cxx/cache/cache.tcc \
unisim/component/cxx/tlb/tlb.tcc \
unisim/component/tlm/processor/powerpc/mpc7447a/cpu.tcc \
unisim/component/tlm/memory/ram/memory.tcc \
unisim/component/tlm/bridge/pci_isa/bridge.tcc \
unisim/component/tlm/isa/i8042/i8042.tcc \
unisim/component/cxx/pci/video/display.tcc \
unisim/component/tlm/pci/ide/pci_dev.tcc \
unisim/component/tlm/pci/ide/pci_ide_module.tcc \
unisim/component/cxx/pci/macio/heathrow.tcc \
unisim/component/cxx/bridge/pci_isa/bridge.tcc \
unisim/component/cxx/memory/flash/am29/am29.tcc \
unisim/component/cxx/chipset/mpc107/address_maps.tcc \
unisim/component/cxx/chipset/mpc107/pci_controller.tcc \
unisim/component/cxx/chipset/mpc107/atu/atu.tcc \
unisim/component/cxx/chipset/mpc107/dma/buffer.tcc \
unisim/component/cxx/chipset/mpc107/dma/dma.tcc \
unisim/component/cxx/pci/ide/ide_ctrl.tcc \
unisim/component/cxx/pci/ide/ide_disk.tcc \
unisim/component/cxx/pci/ide/pcidev.tcc \
unisim/component/cxx/chipset/mpc107/epic/epic.tcc \
unisim/component/tlm/chipset/mpc107/epic/epic.tcc \
unisim/component/tlm/pci/video/display.tcc \
unisim/component/tlm/chipset/mpc107/mpc107.tcc \
unisim/component/tlm/fsb/snooping_bus/bus.tcc \
unisim/component/tlm/pci/macio/heathrow.tcc \
unisim/component/tlm/memory/flash/am29/am29.tcc"

UNISIM_LIB_PPCEMU_SYSTEM_M4_FILES="\
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
m4/with_boost.m4 \
m4/sdl.m4 \
m4/cacti.m4 \
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4"

UNISIM_LIB_PPCEMU_SYSTEM_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_powerpc.xml \
unisim/util/debug/dwarf/powerpc_eabi_dwarf_register_number_mapping.xml \
unisim/util/debug/dwarf/powerpc_eabi_gcc_dwarf_register_number_mapping.xml \
unisim/service/loader/pmac_bootx/device_tree_pmac_g4.xml \
unisim/service/sdl/pc_linux_fr_keymap.xml \
unisim/service/sdl/pc_linux_us_keymap.xml \
unisim/service/sdl/pc_windows_keymap.xml \
"

PPCEMU_SYSTEM_EXTERNAL_HEADERS="\
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

UNISIM_SIMULATORS_PPCEMU_SYSTEM_SOURCE_FILES="\
main.cc \
"
UNISIM_SIMULATORS_PPCEMU_SYSTEM_HEADER_FILES="\
"

UNISIM_SIMULATORS_PPCEMU_SYSTEM_TEMPLATE_FILES=
UNISIM_SIMULATORS_PPCEMU_SYSTEM_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
unisim.ico \
template_default_config.xml \
"

UNISIM_SIMULATORS_PPCEMU_SYSTEM_TESTBENCH_FILES=""

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_ppcemu_system_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/ppcemu_system

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
		${DISTCOPY} "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/genisslib/${file}" || exit
	fi
done

UNISIM_LIB_PPCEMU_SYSTEM_FILES="${UNISIM_LIB_PPCEMU_SYSTEM_SOURCE_FILES} ${UNISIM_LIB_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_LIB_PPCEMU_SYSTEM_TEMPLATE_FILES} ${UNISIM_LIB_PPCEMU_SYSTEM_DATA_FILES}"

for file in ${UNISIM_LIB_PPCEMU_SYSTEM_FILES}; do
	mkdir -p "${DEST_DIR}/ppcemu_system/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/ppcemu_system/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/ppcemu_system/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/ppcemu_system/${file}"
		${DISTCOPY} "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/ppcemu_system/${file}" || exit
	fi
done

UNISIM_SIMULATORS_PPCEMU_SYSTEM_FILES="${UNISIM_SIMULATORS_PPCEMU_SYSTEM_SOURCE_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_TEMPLATE_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_DATA_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/ppcemu_system/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/ppcemu_system/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/ppcemu_system/${file}"
		${DISTCOPY} "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/ppcemu_system/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_DATA_FILES}; do
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
		${DISTCOPY} "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/${file}" || exit
	fi
done


mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/ppcemu_system/config
mkdir -p ${DEST_DIR}/ppcemu_system/m4
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
		${DISTCOPY} "${UNISIM_TOOLS_DIR}/${file}" "${DEST_DIR}/genisslib/${file}" || exit
		has_to_build_genisslib_configure=yes
	fi
done

for file in ${UNISIM_LIB_PPCEMU_SYSTEM_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/ppcemu_system/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/ppcemu_system/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/ppcemu_system/${file}"
		${DISTCOPY} "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/ppcemu_system/${file}" || exit
		has_to_build_ppcemu_system_configure=yes
	fi
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Gilles Mouchard <gilles.mouchard@cea.fr>
Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - UNISIM GenISSLib: an instruction set simulator generator
  - UNISIM ppcemu-system Simulator: A full system simulator of a "PowerMac G4 PCI" like machine (MPC7447A/MPC107) with Linux boot support.
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
  - libSDL (http://www.libsdl.org) development package (libsdl-devel for Redhat/Mandriva, libSDL-1.2-dev for Debian/Ubuntu)
  - Core SystemC Language >= 2.1 (http://www.systemc.org)

Building instructions:
  $ ./configure --with-systemc=<path-to-systemc-install-dir>
  $ make

Installing (optional):
  $ make install
EOF

CONFIGURE_AC="${DEST_DIR}/configure.ac"
MAKEFILE_AM="${DEST_DIR}/Makefile.am"


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

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Generating configure.ac"
	echo "AC_INIT([UNISIM PowerMAC G4 PCI Standalone simulator], [${PPCEMU_SYSTEM_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-ppcemu-system])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([ppcemu_system])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib ppcemu_system" > "${MAKEFILE_AM}"

	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

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
	echo "ACLOCAL_AMFLAGS=-I \$(abs_top_srcdir)/m4" > "${GENISSLIB_MAKEFILE_AM}"
	echo "BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_YFLAGS = -d -p yy" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_LFLAGS = -l" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(abs_top_srcdir) -I\$(abs_top_builddir)" >> "${GENISSLIB_MAKEFILE_AM}"
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


# ppcemu_system

PPCEMU_SYSTEM_CONFIGURE_AC="${DEST_DIR}/ppcemu_system/configure.ac"
PPCEMU_SYSTEM_MAKEFILE_AM="${DEST_DIR}/ppcemu_system/Makefile.am"


if [ ! -e "${PPCEMU_SYSTEM_CONFIGURE_AC}" ]; then
	has_to_build_ppcemu_system_configure=yes
else
	if [ "$0" -nt "${PPCEMU_SYSTEM_CONFIGURE_AC}" ]; then
		has_to_build_ppcemu_system_configure=yes
	fi
fi

if [ ! -e "${PPCEMU_SYSTEM_MAKEFILE_AM}" ]; then
	has_to_build_ppcemu_system_configure=yes
else
	if [ "$0" -nt "${PPCEMU_SYSTEM_MAKEFILE_AM}" ]; then
		has_to_build_ppcemu_system_configure=yes
	fi
fi

if [ "${has_to_build_ppcemu_system_configure}" = "yes" ]; then
	echo "Generating ppcemu_system configure.ac"
	echo "AC_INIT([UNISIM PowerMac G4 PCI C++ simulator], [${PPCEMU_SYSTEM_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-ppcemu-system-core])" > "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${PPCEMU_SYSTEM_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TIMES(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ENDIAN(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SDL(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CACTI(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_REAL_PATH(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_WITH_BOOST(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_GRAPH(main)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\`pwd\`/../genisslib/genisslib" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}\"], [path of shared data relative to bin directory])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"

	AM_PPCEMU_SYSTEM_VERSION=`printf ${PPCEMU_SYSTEM_VERSION} | sed -e 's/\./_/g'`
	echo "Generating ppcemu_system Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(abs_top_srcdir)/m4" > "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(abs_top_srcdir) -I\$(abs_top_builddir)" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libppcemu-system-${PPCEMU_SYSTEM_VERSION}.a" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "libppcemu_system_${AM_PPCEMU_SYSTEM_VERSION}_a_SOURCES = ${UNISIM_LIB_PPCEMU_SYSTEM_SOURCE_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION} unisim-ppcemu-system-debug-${PPCEMU_SYSTEM_VERSION}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "unisim_ppcemu_system_${AM_PPCEMU_SYSTEM_VERSION}_SOURCES = ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_SOURCE_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "unisim_ppcemu_system_debug_${AM_PPCEMU_SYSTEM_VERSION}_SOURCES = ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_SOURCE_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "unisim_ppcemu_system_debug_${AM_PPCEMU_SYSTEM_VERSION}_CPPFLAGS = -DDEBUG_PPCEMU_SYSTEM" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "unisim_ppcemu_system_${AM_PPCEMU_SYSTEM_VERSION}_LDADD = libppcemu-system-${PPCEMU_SYSTEM_VERSION}.a" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "unisim_ppcemu_system_debug_${AM_PPCEMU_SYSTEM_VERSION}_LDADD = libppcemu-system-${PPCEMU_SYSTEM_VERSION}.a" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "noinst_HEADERS = ${UNISIM_TOOLS_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_LIB_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_LIB_PPCEMU_SYSTEM_TEMPLATE_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_TEMPLATE_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_PPCEMU_SYSTEM_M4_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_PPCEMU_SYSTEM_DATA_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_DATA_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.hh \$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.tcc" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "CLEANFILES=\$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.hh \$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.tcc" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "\$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.tcc: \$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.hh" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "\$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.hh: ${UNISIM_LIB_PPCEMU_SYSTEM_ISA_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\t" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa -w 8 -I \$(abs_top_srcdir) \$(abs_top_srcdir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa/mpc7447a.isa" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\trm -f \"\$(abs_top_builddir)/bin/\`basename \$\${PROGRAM}\`\"; \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(abs_top_builddir)/bin'; \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\t(cd '\$(abs_top_builddir)/bin' && cp -f \"\$(abs_top_builddir)/\$\${PROGRAM}\" \`basename \"\$\${PROGRAM}\"\`); \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\tdone\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\trm -rf '\$(abs_top_builddir)/bin'; \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\tfi\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\trm -f \"\$(abs_top_builddir)/share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/\`basename \$\${SHARED_DATA}\`\"; \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(abs_top_builddir)/share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}'; \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\t(cd '\$(abs_top_builddir)/share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}' && cp -f \"\$(abs_top_srcdir)/\$\${SHARED_DATA}\" \`basename \"\$\${SHARED_DATA}\"\`); \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\tdone\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\trm -rf '\$(abs_top_builddir)/share'; \\\\\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\tfi\n" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"

	echo "Building powerpc configure"
	${SHELL} -c "cd ${DEST_DIR}/ppcemu_system && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
