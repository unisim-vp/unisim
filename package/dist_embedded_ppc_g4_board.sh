#!/bin/bash
function Usage
{
	echo "Usage:"
	echo "  $0 <destination directory> <unisim repository>"
}

if [ -z "$1" ] || [ -z "$2" ]; then
	Usage
	exit -1
fi

HERE=`pwd`
DEST_DIR=$1
UNISIM_TOOLS_DIR=$2/unisim_tools
UNISIM_LIB_DIR=$2/unisim_lib
UNISIM_SIMULATORS_DIR=$2/unisim_simulators/tlm/embedded_ppc_g4_board

EMBEDDED_PPC_G4_BOARD_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}

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

UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_SOURCE_FILES="\
unisim/kernel/service/service.cc \
unisim/kernel/service/xml_helper.cc \
unisim/kernel/tlm/tlm.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/kernel/debug/debug.cc \
unisim/util/xml/xml.cc \
unisim/util/debug/profile_32.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/debug/netstub.cc \
unisim/util/endian/endian.cc \
unisim/util/queue/queue.cc \
unisim/util/garbage_collector/garbage_collector.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/time/host_time/time.cc \
unisim/service/time/sc_time/time.cc \
unisim/service/power/cache_power_estimator.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/tee/memory_access_reporting/tee_32.cc \
unisim/service/tee/symbol_table_lookup/tee_32.cc \
unisim/component/cxx/processor/powerpc/cpu.cc \
unisim/component/cxx/processor/powerpc/exception.cc \
unisim/component/cxx/processor/powerpc/config.cc \
unisim/component/cxx/processor/powerpc/mpc7447a.cc \
unisim/component/cxx/processor/powerpc/mpc7447a_debug.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
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
unisim/component/cxx/pci/types.cc \
unisim/component/cxx/pci/debug/pci_stub.cc \
unisim/component/tlm/pci/debug/pci_stub_32.cc \
unisim/component/tlm/processor/powerpc/mpc7447a.cc \
unisim/component/tlm/processor/powerpc/mpc7447a_debug.cc \
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

UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_ISA_FILES="\
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

UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_HEADER_FILES="${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_ISA_FILES} \
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
unisim/util/debug/netstub.hh \
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
unisim/service/tee/memory_access_reporting/tee.hh \
unisim/service/tee/symbol_table_lookup/tee.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/loader/elf_loader/elf_common.h \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32.h \
unisim/service/loader/elf_loader/elf64.h \
unisim/service/loader/elf_loader/elf32_loader.hh \
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
unisim/component/tlm/message/pci.hh \
unisim/component/cxx/pci/debug/pci_stub.hh \
unisim/component/tlm/pci/debug/pci_stub.hh"

UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_TEMPLATE_FILES="\
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/profile.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/util/debug/netstub.tcc \
unisim/util/queue/queue.tcc \
unisim/util/simfloat/floating.tcc \
unisim/util/simfloat/integer.tcc \
unisim/util/simfloat/host_floating.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/tee/memory_access_reporting/tee.tcc \
unisim/service/tee/symbol_table_lookup/tee.tcc \
unisim/component/cxx/processor/powerpc/cpu.tcc \
unisim/component/cxx/processor/powerpc/exception.tcc \
unisim/component/cxx/memory/ram/memory.tcc \
unisim/component/cxx/cache/cache.tcc \
unisim/component/cxx/tlb/tlb.tcc \
unisim/component/tlm/processor/powerpc/powerpc.tcc \
unisim/component/tlm/memory/ram/memory.tcc \
unisim/component/cxx/memory/flash/am29lv/am29lv.tcc \
unisim/component/cxx/chipset/mpc107/address_maps.tcc \
unisim/component/cxx/chipset/mpc107/pci_controller.tcc \
unisim/component/cxx/chipset/mpc107/atu/atu.tcc \
unisim/component/cxx/chipset/mpc107/dma/buffer.tcc \
unisim/component/cxx/chipset/mpc107/dma/dma.tcc \
unisim/component/cxx/chipset/mpc107/epic/epic.tcc \
unisim/component/tlm/chipset/mpc107/epic/epic.tcc \
unisim/component/tlm/chipset/mpc107/mpc107.tcc \
unisim/component/tlm/fsb/snooping_bus/bus.tcc \
unisim/component/tlm/memory/flash/am29lv/am29lv.tcc \
unisim/component/cxx/pci/debug/pci_stub.tcc \
unisim/component/tlm/pci/debug/pci_stub.tcc"

UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_M4_FILES="\
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
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4"

UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_powerpc.xml"

EMBEDDED_PPC_G4_BOARD_EXTERNAL_HEADERS="\
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

UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_SOURCE_FILES="\
main.cc \
"
UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_HEADER_FILES="\
"

UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_TEMPLATE_FILES=
UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
template_default_config.xml \
unisim.ico"

UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_TESTBENCH_FILES=""

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_embedded_ppc_g4_board_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/embedded_ppc_g4_board

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

UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_FILES="${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_SOURCE_FILES} ${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_HEADER_FILES} ${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_TEMPLATE_FILES} ${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_DATA_FILES}"

for file in ${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_FILES}; do
	mkdir -p "${DEST_DIR}/embedded_ppc_g4_board/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/embedded_ppc_g4_board/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/embedded_ppc_g4_board/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/embedded_ppc_g4_board/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/embedded_ppc_g4_board/${file}" || exit
	fi
done

UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_FILES="${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_SOURCE_FILES} ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_HEADER_FILES} ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_TEMPLATE_FILES} ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_DATA_FILES} ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/embedded_ppc_g4_board/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/embedded_ppc_g4_board/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/embedded_ppc_g4_board/${file}"
		cp -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/embedded_ppc_g4_board/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_DATA_FILES}; do
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
mkdir -p ${DEST_DIR}/embedded_ppc_g4_board/config
mkdir -p ${DEST_DIR}/embedded_ppc_g4_board/m4
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

for file in ${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/embedded_ppc_g4_board/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/embedded_ppc_g4_board/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/embedded_ppc_g4_board/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/embedded_ppc_g4_board/${file}" || exit
		has_to_build_embedded_ppc_g4_board_configure=yes
	fi
done

# Top level

echo "This package contains:" > "${DEST_DIR}/README"
echo "  - UNISIM GenISSLib: an instruction set simulator generator" >> "${DEST_DIR}/README"
echo "  - UNISIM embedded-ppc-g4-board: a MPC7447A/MPC107 board SystemC TLM simulator." >> "${DEST_DIR}/README"
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
	echo "AC_INIT([UNISIM PowerMAC G4 PCI Standalone simulator], [${EMBEDDED_PPC_G4_BOARD_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-embedded-ppc-g4-board])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([embedded_ppc_g4_board])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib embedded_ppc_g4_board" > "${MAKEFILE_AM}"

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
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${GENISSLIB_MAKEFILE_AM}"
	echo "BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_YFLAGS = -d -p yy" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_LFLAGS = -l" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_PROGRAMS = genisslib" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_CPPFLAGS = -DGENISSLIB_VERSION=\\\"${GENISSLIB_VERSION}\\\"" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_TOOLS_GENISSLIB_M4_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"

	echo "Building GENISSLIB configure"
	${SHELL} -c "cd ${DEST_DIR}/genisslib && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi


# embedded_ppc_g4_board

EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC="${DEST_DIR}/embedded_ppc_g4_board/configure.ac"
EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM="${DEST_DIR}/embedded_ppc_g4_board/Makefile.am"


if [ ! -e "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}" ]; then
	has_to_build_embedded_ppc_g4_board_configure=yes
else
	if [ "$0" -nt "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}" ]; then
		has_to_build_embedded_ppc_g4_board_configure=yes
	fi
fi

if [ ! -e "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}" ]; then
	has_to_build_embedded_ppc_g4_board_configure=yes
else
	if [ "$0" -nt "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}" ]; then
		has_to_build_embedded_ppc_g4_board_configure=yes
	fi
fi

if [ "${has_to_build_embedded_ppc_g4_board_configure}" = "yes" ]; then
	echo "Generating embedded_ppc_g4_board configure.ac"
	echo "AC_INIT([UNISIM PowerMac G4 PCI C++ simulator], [${EMBEDDED_PPC_G4_BOARD_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-embedded-ppc-g4-board-core])" > "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${EMBEDDED_PPC_G4_BOARD_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TIMES" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ENDIAN" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_WITH_BOOST" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_GRAPH" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "UNISIM_CHECK_REAL_PATH" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\`pwd\`/../genisslib/genisslib" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}\"], [path of shared data relative to bin directory])" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${EMBEDDED_PPC_G4_BOARD_CONFIGURE_AC}"

	AM_EMBEDDED_PPC_G4_BOARD_VERSION=`printf ${EMBEDDED_PPC_G4_BOARD_VERSION} | sed -e 's/\./_/g'`
	echo "Generating embedded_ppc_g4_board Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libembedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}.a" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "libembedded_ppc_g4_board_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_a_SOURCES = ${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_SOURCE_FILES}" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION} unisim-embedded-ppc-g4-board-debug-${EMBEDDED_PPC_G4_BOARD_VERSION} unisim-embedded-ppc-g4-board-no-pci-stub-${EMBEDDED_PPC_G4_BOARD_VERSION} unisim-embedded-ppc-g4-board-no-pci-stub-debug-${EMBEDDED_PPC_G4_BOARD_VERSION}" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_SOURCES = ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_SOURCE_FILES}" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_debug_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_SOURCES = ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_SOURCE_FILES}" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_no_pci_stub_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_SOURCES = ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_SOURCE_FILES}" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_no_pci_stub_debug_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_SOURCES = ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_SOURCE_FILES}" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_CPPFLAGS = -DWITH_PCI_STUB" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_debug_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_CPPFLAGS = -DWITH_PCI_STUB -DDEBUG_EMBEDDED_PPC_G4_BOARD" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_no_pci_stub_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_CPPFLAGS = " >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_no_pci_stub_debug_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_CPPFLAGS = -DDEBUG_EMBEDDED_PPC_G4_BOARD" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_LDADD = libembedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}.a" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_debug_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_LDADD = libembedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}.a" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_no_pci_stub_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_LDADD = libembedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}.a" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "unisim_embedded_ppc_g4_board_no_pci_stub_debug_${AM_EMBEDDED_PPC_G4_BOARD_VERSION}_LDADD = libembedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}.a" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "noinst_HEADERS = ${UNISIM_TOOLS_EMBEDDED_PPC_G4_BOARD_HEADER_FILES} ${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_HEADER_FILES} ${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_TEMPLATE_FILES} ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_HEADER_FILES} ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_TEMPLATE_FILES}" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_M4_FILES}" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_DATA_FILES} ${UNISIM_SIMULATORS_EMBEDDED_PPC_G4_BOARD_DATA_FILES}" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh \$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.tcc" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh \$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.tcc" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.tcc: \$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh: ${UNISIM_LIB_EMBEDDED_PPC_G4_BOARD_ISA_FILES}" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\t" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "cd \$(top_srcdir)/unisim/component/cxx/processor/powerpc; \$(GENISSLIB_PATH) -o powerpc -w 32 -I . ppc.isa" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/bin/\`basename \$\${PROGRAM}\`\"; \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\t(cd '\$(top_builddir)/bin' && cp -f \"\$(abs_top_builddir)/\$\${PROGRAM}\" \`basename \"\$\${PROGRAM}\"\`); \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\tdone\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\tfi\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}/\`basename \$\${SHARED_DATA}\`\"; \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}'; \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\t(cd '\$(top_builddir)/share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}' && cp -f \"\$(abs_top_builddir)/\$\${SHARED_DATA}\" \`basename \"\$\${SHARED_DATA}\"\`); \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\tdone\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"
	printf "\tfi\n" >> "${EMBEDDED_PPC_G4_BOARD_MAKEFILE_AM}"

	echo "Building powerpc configure"
	${SHELL} -c "cd ${DEST_DIR}/embedded_ppc_g4_board && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"