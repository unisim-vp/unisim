#!/bin/bash
function Usage
{
	echo "Usage:"
	echo "  $0 <destination directory> <unisim_tools directory> <unisim_lib directory> <unisim_simulators directory>"
}

if test "x$1" = x || test "x$2" = x || test "x$3" = x || test "x$4" = x; then
	Usage
	exit
fi

HERE=`pwd`
DEST_DIR=$1
UNISIM_TOOLS_DIR=$2
UNISIM_LIB_DIR=$3
UNISIM_SIMULATORS_DIR=$4/tlm/ppcemu_system

UNISIM_TOOLS_GENISSLIB_HEADER_FILES="\
action.hh \
cli.hh \
errtools.hh \
isa.hh \
parser.hh \
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
parser.h"

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
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/kernel/debug/debug.cc \
unisim/util/xml/xml.cc \
unisim/util/debug/profile_32.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/endian/endian.cc \
unisim/util/queue/queue.cc \
unisim/util/garbage_collector/garbage_collector.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/logger/logger_server.cc \
unisim/service/logger/logger.cc \
unisim/service/time/host_time/time.cc \
unisim/service/time/sc_time/time.cc \
unisim/service/power/cache_power_estimator.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/loader/pmac_linux_kernel_loader/pmac_linux_kernel_loader.cc \
unisim/service/loader/pmac_bootx/pmac_bootx.cc \
unisim/service/sdl/sdl.cc \
unisim/component/cxx/processor/powerpc/cpu.cc \
unisim/component/cxx/processor/powerpc/exception.cc \
unisim/component/cxx/processor/powerpc/config.cc \
unisim/component/cxx/processor/powerpc/mpc7447a.cc \
unisim/component/cxx/processor/powerpc/mpc7447a_debug.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/pci/video/display_32.cc \
unisim/component/cxx/pci/macio/heathrow_32.cc \
unisim/component/cxx/bridge/pci_isa/bridge_32.cc \
unisim/component/cxx/isa/i8042/i8042.cc \
unisim/component/cxx/memory/flash/am29lv/am29lv.cc \
unisim/component/cxx/memory/flash/am29lv/config.cc \
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
unisim/component/cxx/pci/types.cc \
unisim/component/tlm/isa/i8042/i8042.cc \
unisim/component/tlm/processor/powerpc/mpc7447a.cc \
unisim/component/tlm/processor/powerpc/mpc7447a_debug.cc \
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
unisim/component/tlm/memory/flash/am29lv/am29lv.cc"

UNISIM_LIB_PPCEMU_SYSTEM_ISA_FILES="\
unisim/component/cxx/processor/powerpc/altivec.isa \
unisim/component/cxx/processor/powerpc/condition.isa \
unisim/component/cxx/processor/powerpc/integer.isa \
unisim/component/cxx/processor/powerpc/misc.isa \
unisim/component/cxx/processor/powerpc/ppc.isa \
unisim/component/cxx/processor/powerpc/branch.isa \
unisim/component/cxx/processor/powerpc/floating.isa \
unisim/component/cxx/processor/powerpc/loadstore.isa \
unisim/component/cxx/processor/powerpc/perf.isa \
unisim/component/cxx/processor/powerpc/sim_dependencies.isa \
unisim/component/cxx/processor/powerpc/sim_latencies.isa \
unisim/component/cxx/processor/powerpc/sim_load.isa \
unisim/component/cxx/processor/powerpc/sim_branch.isa \
unisim/component/cxx/processor/powerpc/sim_ppc.isa \
unisim/component/cxx/processor/powerpc/sim_latencies.isa"

UNISIM_LIB_PPCEMU_SYSTEM_HEADER_FILES="${UNISIM_LIB_PPCEMU_SYSTEM_ISA_FILES} \
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/tlm/tlm.hh \
unisim/kernel/debug/debug.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/profile.hh \
unisim/util/debug/register.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/endian/endian.hh \
unisim/util/garbage_collector/garbage_collector.hh \
unisim/util/hash_table/hash_table.hh \
unisim/util/xml/xml.hh \
unisim/util/queue/queue.hh \
unisim/util/simfloat/floating.hh \
unisim/util/simfloat/integer.hh \
unisim/util/simfloat/host_floating.hh \
unisim/util/device/register.hh \
unisim/service/interfaces/debug_control.hh \
unisim/service/interfaces/memory_access_reporting.hh \
unisim/service/interfaces/disassembly.hh \
unisim/service/interfaces/loader.hh \
unisim/service/interfaces/logger.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/symbol_table_lookup.hh \
unisim/service/interfaces/time.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/cpu_linux_os.hh \
unisim/service/interfaces/linux_os.hh \
unisim/service/interfaces/os.hh \
unisim/service/interfaces/cpu_os.hh \
unisim/service/interfaces/cache_power_estimator.hh \
unisim/service/interfaces/power_mode.hh \
unisim/service/interfaces/synchronizable.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/interfaces/video.hh \
unisim/service/interfaces/keyboard.hh \
unisim/service/interfaces/mouse.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/logger/logger_server.hh \
unisim/service/logger/logger.hh \
unisim/service/logger/logger_server_interface.hh \
unisim/service/loader/elf_loader/elf_common.h \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32.h \
unisim/service/loader/elf_loader/elf64.h \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/pmac_linux_kernel_loader/pmac_linux_kernel_loader.hh \
unisim/service/loader/pmac_bootx/pmac_bootx.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/service/power/cache_power_estimator.hh \
unisim/service/sdl/sdl.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/processor/powerpc/cpu.hh \
unisim/component/cxx/processor/powerpc/exception.hh \
unisim/component/cxx/processor/powerpc/floating.hh \
unisim/component/cxx/processor/powerpc/config.hh \
unisim/component/cxx/cache/cache.hh \
unisim/component/cxx/tlb/tlb.hh \
unisim/component/cxx/memory/flash/am29lv/am29lv.hh \
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
unisim/component/cxx/memory/flash/am29lv/types.hh \
unisim/component/cxx/memory/flash/am29lv/config.hh \
unisim/component/tlm/processor/powerpc/powerpc.hh \
unisim/component/tlm/message/snooping_fsb.hh \
unisim/component/tlm/message/interrupt.hh \
unisim/component/tlm/debug/transaction_spy.hh \
unisim/component/tlm/message/memory.hh \
unisim/component/tlm/memory/ram/memory.hh \
unisim/component/tlm/memory/flash/am29lv/am29lv.hh \
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
unisim/util/queue/queue.tcc \
unisim/util/simfloat/floating.tcc \
unisim/util/simfloat/integer.tcc \
unisim/util/simfloat/host_floating.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/sdl/sdl.tcc \
unisim/component/cxx/processor/powerpc/cpu.tcc \
unisim/component/cxx/processor/powerpc/exception.tcc \
unisim/component/cxx/memory/ram/memory.tcc \
unisim/component/cxx/cache/cache.tcc \
unisim/component/cxx/tlb/tlb.tcc \
unisim/component/tlm/processor/powerpc/powerpc.tcc \
unisim/component/tlm/memory/ram/memory.tcc \
unisim/component/tlm/bridge/pci_isa/bridge.tcc \
unisim/component/tlm/isa/i8042/i8042.tcc \
unisim/component/cxx/pci/video/display.tcc \
unisim/component/tlm/pci/ide/pci_dev.tcc \
unisim/component/tlm/pci/ide/pci_ide_module.tcc \
unisim/component/cxx/pci/macio/heathrow.tcc \
unisim/component/cxx/bridge/pci_isa/bridge.tcc \
unisim/component/cxx/memory/flash/am29lv/am29lv.tcc \
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
unisim/component/tlm/memory/flash/am29lv/am29lv.tcc"

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
m4/check_lib.m4"

UNISIM_LIB_PPCEMU_SYSTEM_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_powerpc.xml \
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
ChangeLog"

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
		cp -f "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/genisslib/${file}" || exit
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
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/ppcemu_system/${file}" || exit
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
		cp -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/ppcemu_system/${file}" || exit
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
		cp -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/${file}" || exit
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
		cp -f "${UNISIM_TOOLS_DIR}/${file}" "${DEST_DIR}/genisslib/${file}" || exit
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
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/ppcemu_system/${file}" || exit
		has_to_build_ppcemu_system_configure=yes
	fi
done

# Top level

echo "This package contains:" > "${DEST_DIR}/README"
echo "  - GenISSLib: an instruction set simulator generator" >> "${DEST_DIR}/README"
echo "  - ppcemu-system: a PowerMac G4 PCI SystemC TLM simulator." >> "${DEST_DIR}/README"
echo "See INSTALL for installation instructions." >> "${DEST_DIR}/README"

echo "INSTALLATION" > "${DEST_DIR}/INSTALL"
echo "------------" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Requirements:" >> "${DEST_DIR}/INSTALL"
echo "  - GNU bash" >> "${DEST_DIR}/INSTALL"
echo "  - GNU make" >> "${DEST_DIR}/INSTALL"
echo "  - GNU autoconf" >> "${DEST_DIR}/INSTALL"
echo "  - GNU automake" >> "${DEST_DIR}/INSTALL"
echo "  - GNU flex" >> "${DEST_DIR}/INSTALL"
echo "  - GNU bison" >> "${DEST_DIR}/INSTALL"
echo "  - boost (http://www.boost.org) development package (libboost-devel for Redhat/Mandriva, libboost-graph-dev for Debian/Ubuntu)" >> "${DEST_DIR}/INSTALL"
echo "  - libxml2 (http://xmlsoft.org/libxml2) development package (libxml2-devel for Redhat/Mandriva, libxml2-dev for Debian/Ubuntu)" >> "${DEST_DIR}/INSTALL"
echo "  - zlib (http://www.zlib.net) development package (zlib1g-devel for Redhat/Mandriva, zlib1g-devel for Debian/Ubuntu)" >> "${DEST_DIR}/INSTALL"
echo "  - SDL (http://www.libsdl.org) development package (libsdl-devel for Redhat/Mandriva, libsdl12-dev for Debian/Ubuntu)" >> "${DEST_DIR}/INSTALL"
echo "  - libedit (http://www.thrysoee.dk/editline) development package (libedit-devel for Redhat/Mandriva, libedit-dev for Debian/Ubuntu)" >> "${DEST_DIR}/INSTALL"
echo "  - Core SystemC Language >= 2.1 (http://www.systemc.org)" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Building instructions:" >> "${DEST_DIR}/INSTALL"
echo "  $ ./configure --with-systemc=<path-to-systemc-install-dir>" >> "${DEST_DIR}/INSTALL"
echo "  $ make" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Installing (optional):" >> "${DEST_DIR}/INSTALL"
echo "  $ make install" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"

CONFIGURE_AC="${DEST_DIR}/configure.ac"
MAKEFILE_AM="${DEST_DIR}/Makefile.am"


if [ ! -e "${CONFIGURE_AC}" ]; then
	has_to_build_configure=yes
fi

if [ ! -e "${MAKEFILE_AM}" ]; then
	has_to_build_configure=yes
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Generating configure.ac"
	echo "AC_INIT([UNISIM PowerMAC G4 PCI Standalone simulator], [0.1], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [ppcemu_system])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE" >> "${CONFIGURE_AC}"
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
fi

if [ ! -e "${GENISSLIB_MAKEFILE_AM}" ]; then
	has_to_build_genisslib_configure=yes
fi

if [ "${has_to_build_genisslib_configure}" = "yes" ]; then
	echo "Generating GENISSLIB configure.ac"
	echo "AC_INIT([UNISIM GENISSLIB], [0.1], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])" > "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE" >> "${GENISSLIB_CONFIGURE_AC}"
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

	echo "Generating GENISSLIB Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${GENISSLIB_MAKEFILE_AM}"
	echo "BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_YFLAGS = -d -p yy" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_LFLAGS = -l" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "bin_PROGRAMS = genisslib" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_TOOLS_GENISSLIB_M4_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"

	echo "Building GENISSLIB configure"
	${SHELL} -c "cd ${DEST_DIR}/genisslib && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi


# ppcemu_system

PPCEMU_SYSTEM_CONFIGURE_AC="${DEST_DIR}/ppcemu_system/configure.ac"
PPCEMU_SYSTEM_MAKEFILE_AM="${DEST_DIR}/ppcemu_system/Makefile.am"


if [ ! -e "${PPCEMU_SYSTEM_CONFIGURE_AC}" ]; then
	has_to_build_ppcemu_system_configure=yes
fi

if [ ! -e "${PPCEMU_SYSTEM_MAKEFILE_AM}" ]; then
	has_to_build_ppcemu_system_configure=yes
fi

if [ "${has_to_build_ppcemu_system_configure}" = "yes" ]; then
	echo "Generating ppcemu_system configure.ac"
	echo "AC_INIT([UNISIM PowerMac G4 PCI C++ simulator], [0.1], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [ppcemu_system])" > "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${PPCEMU_SYSTEM_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TIMES" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ENDIAN" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SDL" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_WITH_BOOST" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_GRAPH" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\`pwd\`/../genisslib/genisslib" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"

	echo "Generating ppcemu_system Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "PPCEMU_SYSTEM_INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "PPCEMU_SYSTEM_DEBUG_INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "bin_PROGRAMS = ppcemu-system ppcemu-system-debug" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "ppcemu_system_SOURCES = ${UNISIM_LIB_PPCEMU_SYSTEM_SOURCE_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_SOURCE_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "ppcemu_system_debug_SOURCES = ${UNISIM_LIB_PPCEMU_SYSTEM_SOURCE_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_SOURCE_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "ppcemu_system_debug_CPPFLAGS = -DDEBUG_PPCEMU_SYSTEM" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "noinst_HEADERS = ${UNISIM_TOOLS_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_LIB_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_LIB_PPCEMU_SYSTEM_TEMPLATE_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_TEMPLATE_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_PPCEMU_SYSTEM_M4_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh \$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.tcc" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh \$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.tcc" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.tcc: \$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh: ${UNISIM_LIB_PPCEMU_SYSTEM_ISA_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\t" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "cd \$(top_srcdir)/unisim/component/cxx/processor/powerpc; \$(GENISSLIB_PATH) -o powerpc -w 32 -I . ppc.isa" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"

	echo "Building powerpc configure"
	${SHELL} -c "cd ${DEST_DIR}/ppcemu_system && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"