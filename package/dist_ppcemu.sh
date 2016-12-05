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
UNISIM_SIMULATORS_DIR=${MY_DIR}/../unisim_simulators/tlm2/ppcemu

PPCEMU_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-ppcemu-${PPCEMU_VERSION}

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

UNISIM_LIB_PPCEMU_SOURCE_FILES="\
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
unisim/util/ieee754/ieee754.cc \
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
unisim/service/os/linux_os/powerpc_linux32.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/tee/memory_access_reporting/tee_32.cc \
unisim/component/cxx/processor/powerpc/config.cc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu.cc \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu_debug.cc \
unisim/component/cxx/processor/powerpc/floating.cc \
unisim/component/cxx/processor/powerpc/mpc7447a/config.cc \
unisim/component/cxx/processor/powerpc/mpc7447a/vr_debug_if.cc \
unisim/component/cxx/processor/powerpc/mpc7447a/tb_debug_if.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc \
unisim/component/tlm2/processor/powerpc/mpc7447a/cpu.cc \
unisim/component/tlm2/processor/powerpc/mpc7447a/cpu_debug.cc \
unisim/component/tlm2/memory/ram/memory.cc \
unisim/component/tlm2/memory/ram/memory_debug.cc"

UNISIM_LIB_PPCEMU_ISA_FILES="\
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
unisim/component/cxx/processor/powerpc/mpc7447a/isa/misc.isa"

UNISIM_LIB_PPCEMU_HEADER_FILES="${UNISIM_LIB_PPCEMU_ISA_FILES} \
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/tlm2/tlm.hh \
unisim/kernel/debug/debug.hh \
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
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/interfaces/subprogram_lookup.hh \
unisim/service/interfaces/register.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/debug/debugger/debugger.hh \
unisim/service/tee/memory_access_reporting/tee.hh \
unisim/service/profiling/addr_profiler/profiler.hh \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/service/power/cache_power_estimator.hh \
unisim/service/power/cache_profile.hh \
unisim/service/power/cache_dynamic_energy.hh \
unisim/service/power/cache_dynamic_power.hh \
unisim/service/power/cache_leakage_power.hh \
unisim/service/os/linux_os/linux.hh \
unisim/service/os/linux_os/powerpc_linux32.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/processor/powerpc/floating.hh \
unisim/component/cxx/processor/powerpc/config.hh \
unisim/component/cxx/processor/powerpc/mpc7447a/cpu.hh \
unisim/component/cxx/processor/powerpc/mpc7447a/config.hh \
unisim/component/cxx/cache/cache.hh \
unisim/component/cxx/tlb/tlb.hh \
unisim/component/tlm2/interrupt/types.hh \
unisim/component/tlm2/processor/powerpc/mpc7447a/cpu.hh \
unisim/component/tlm2/memory/ram/memory.hh"

UNISIM_LIB_PPCEMU_TEMPLATE_FILES="\
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
unisim/service/tee/memory_access_reporting/tee.tcc \
unisim/service/os/linux_os/linux.tcc \
unisim/service/os/linux_os/powerpc_linux32.tcc \
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
unisim/component/tlm2/processor/powerpc/mpc7447a/cpu.tcc \
unisim/component/tlm2/memory/ram/memory.tcc"

UNISIM_LIB_PPCEMU_M4_FILES="\
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
m4/cacti.m4 \
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4 \
m4/pthread.m4"

UNISIM_LIB_PPCEMU_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_powerpc_32.xml \
unisim/util/debug/dwarf/powerpc_eabi_dwarf_register_number_mapping.xml \
unisim/util/debug/dwarf/powerpc_eabi_gcc_dwarf_register_number_mapping.xml"

PPCEMU_EXTERNAL_HEADERS="\
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

UNISIM_SIMULATORS_PPCEMU_SOURCE_FILES="\
main.cc \
"
UNISIM_SIMULATORS_PPCEMU_HEADER_FILES="\
"

UNISIM_SIMULATORS_PPCEMU_TEMPLATE_FILES=
UNISIM_SIMULATORS_PPCEMU_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
template_default_config.xml \
unisim.ico"

UNISIM_SIMULATORS_PPCEMU_TESTBENCH_FILES=""

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_ppcemu_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/ppcemu

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
		cp -f "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/genisslib/${file}" || exit
	fi
done

UNISIM_LIB_PPCEMU_FILES="${UNISIM_LIB_PPCEMU_SOURCE_FILES} ${UNISIM_LIB_PPCEMU_HEADER_FILES} ${UNISIM_LIB_PPCEMU_TEMPLATE_FILES} ${UNISIM_LIB_PPCEMU_DATA_FILES}"

for file in ${UNISIM_LIB_PPCEMU_FILES}; do
	mkdir -p "${DEST_DIR}/ppcemu/$(dirname ${file})"
	has_to_copy=no
	if [ -e "${DEST_DIR}/ppcemu/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/ppcemu/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/ppcemu/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/ppcemu/${file}" || exit
	fi
done

UNISIM_SIMULATORS_PPCEMU_FILES="${UNISIM_SIMULATORS_PPCEMU_SOURCE_FILES} ${UNISIM_SIMULATORS_PPCEMU_HEADER_FILES} ${UNISIM_SIMULATORS_PPCEMU_TEMPLATE_FILES} ${UNISIM_SIMULATORS_PPCEMU_DATA_FILES} ${UNISIM_SIMULATORS_PPCEMU_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATORS_PPCEMU_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/ppcemu/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/ppcemu/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/ppcemu/${file}"
		cp -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/ppcemu/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_PPCEMU_DATA_FILES}; do
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
mkdir -p ${DEST_DIR}/ppcemu/config
mkdir -p ${DEST_DIR}/ppcemu/m4
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

for file in ${UNISIM_LIB_PPCEMU_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/ppcemu/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/ppcemu/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/ppcemu/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/ppcemu/${file}" || exit
		has_to_build_ppcemu_configure=yes
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
  - UNISIM ppcemu: An user level PowerPC simulator with support of ELF32 binaries and Linux system call translation.
                   This simulator can run simple PowerPC Linux applications.

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
  - Core SystemC Language >= 2.3.0 (http://www.systemc.org)


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
	echo "AC_INIT([UNISIM PowerMAC G4 PCI Standalone simulator], [${PPCEMU_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>], [unisim-ppcemu])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([ppcemu])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib ppcemu" > "${MAKEFILE_AM}"
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
	echo "=== configure help for ppcemu"
else
	echo "=== configuring in ppcemu (\${HERE}/ppcemu) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/ppcemu/configure \$@"
fi

if test ! -d \${HERE}/ppcemu; then
	mkdir \${HERE}/ppcemu
fi
cd \${HERE}/ppcemu
\${MY_DIR}/ppcemu/configure "\$@"
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
all: ppcemu-all
clean: genisslib-clean ppcemu-clean
distclean: genisslib-distclean ppcemu-distclean
	rm -f \${HERE}/Makefile.cross
install: ppcemu-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
ppcemu-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/ppcemu all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
ppcemu-clean:
	@\\\$(MAKE) -C \${HERE}/ppcemu clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
ppcemu-distclean:
	@\\\$(MAKE) -C \${HERE}/ppcemu distclean
ppcemu-install:
	@\\\$(MAKE) -C \${HERE}/ppcemu install
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


# ppcemu

PPCEMU_CONFIGURE_AC="${DEST_DIR}/ppcemu/configure.ac"
PPCEMU_MAKEFILE_AM="${DEST_DIR}/ppcemu/Makefile.am"


if [ ! -e "${PPCEMU_CONFIGURE_AC}" ]; then
	has_to_build_ppcemu_configure=yes
else
	if [ "$0" -nt "${PPCEMU_CONFIGURE_AC}" ]; then
		has_to_build_ppcemu_configure=yes
	fi
fi

if [ ! -e "${PPCEMU_MAKEFILE_AM}" ]; then
	has_to_build_ppcemu_configure=yes
else
	if [ "$0" -nt "${PPCEMU_MAKEFILE_AM}" ]; then
		has_to_build_ppcemu_configure=yes
	fi
fi

if [ "${has_to_build_ppcemu_configure}" = "yes" ]; then
	echo "Generating ppcemu configure.ac"
	echo "AC_INIT([UNISIM PowerMac G4 PCI C++ simulator], [${PPCEMU_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>], [unisim-ppcemu-core])" > "${PPCEMU_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${PPCEMU_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${PPCEMU_CONFIGURE_AC}"
	echo "case \"\${host}\" in" >> "${PPCEMU_CONFIGURE_AC}"
	printf "\t*mingw*)\n" >> "${PPCEMU_CONFIGURE_AC}"
	printf "\t;;\n" >> "${PPCEMU_CONFIGURE_AC}"
	printf "\t*)\n" >> "${PPCEMU_CONFIGURE_AC}"
	printf "\tUNISIM_CHECK_PTHREAD(main)\n" >> "${PPCEMU_CONFIGURE_AC}"
	printf "\t;;\n" >> "${PPCEMU_CONFIGURE_AC}"
	echo "esac" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TIMES(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ENDIAN(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CACTI(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_REAL_PATH(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_WITH_BOOST(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_GRAPH(main)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${PPCEMU_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-ppcemu-${PPCEMU_VERSION}\"], [path of shared data relative to bin directory])" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${PPCEMU_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${PPCEMU_CONFIGURE_AC}"

	AM_PPCEMU_VERSION=$(printf ${PPCEMU_VERSION} | sed -e 's/\./_/g')
	echo "Generating ppcemu Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${PPCEMU_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${PPCEMU_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libppcemu-${PPCEMU_VERSION}.a" >> "${PPCEMU_MAKEFILE_AM}"
	echo "libppcemu_${AM_PPCEMU_VERSION}_a_SOURCES = ${UNISIM_LIB_PPCEMU_SOURCE_FILES}" >> "${PPCEMU_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-ppcemu-${PPCEMU_VERSION} unisim-ppcemu-debug-${PPCEMU_VERSION}" >> "${PPCEMU_MAKEFILE_AM}"
	echo "unisim_ppcemu_${AM_PPCEMU_VERSION}_SOURCES = ${UNISIM_SIMULATORS_PPCEMU_SOURCE_FILES}" >> "${PPCEMU_MAKEFILE_AM}"
	echo "unisim_ppcemu_debug_${AM_PPCEMU_VERSION}_SOURCES = ${UNISIM_SIMULATORS_PPCEMU_SOURCE_FILES}" >> "${PPCEMU_MAKEFILE_AM}"
	echo "unisim_ppcemu_debug_${AM_PPCEMU_VERSION}_CPPFLAGS = -DDEBUG_PPCEMU" >> "${PPCEMU_MAKEFILE_AM}"
	echo "unisim_ppcemu_${AM_PPCEMU_VERSION}_LDADD = libppcemu-${PPCEMU_VERSION}.a" >> "${PPCEMU_MAKEFILE_AM}"
	echo "unisim_ppcemu_debug_${AM_PPCEMU_VERSION}_LDADD = libppcemu-${PPCEMU_VERSION}.a" >> "${PPCEMU_MAKEFILE_AM}"

	echo "noinst_HEADERS = ${UNISIM_LIB_PPCEMU_HEADER_FILES} ${UNISIM_LIB_PPCEMU_TEMPLATE_FILES} ${UNISIM_SIMULATORS_PPCEMU_HEADER_FILES} ${UNISIM_SIMULATORS_PPCEMU_TEMPLATE_FILES}" >> "${PPCEMU_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_PPCEMU_M4_FILES}" >> "${PPCEMU_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-ppcemu-${PPCEMU_VERSION}" >> "${PPCEMU_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_PPCEMU_DATA_FILES} ${UNISIM_SIMULATORS_PPCEMU_DATA_FILES}" >> "${PPCEMU_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.hh \$(top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.tcc" >> "${PPCEMU_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.hh \$(top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.tcc" >> "${PPCEMU_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.tcc: \$(top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.hh" >> "${PPCEMU_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa.hh: ${UNISIM_LIB_PPCEMU_ISA_FILES}" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\t" >> "${PPCEMU_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/powerpc/mpc7447a/isa/mpc7447a.isa" >> "${PPCEMU_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${PPCEMU_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${PPCEMU_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/bin/\$\$(basename \$\${PROGRAM})\"; \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_builddir)/\$\${PROGRAM}\" \$(top_builddir)/bin/\$\$(basename \"\$\${PROGRAM}\"); \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\tdone\n" >> "${PPCEMU_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\tfi\n" >> "${PPCEMU_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-ppcemu-${PPCEMU_VERSION}/\$\$(basename \$\${SHARED_DATA})\"; \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-ppcemu-${PPCEMU_VERSION}'; \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_srcdir)/\$\${SHARED_DATA}\" \$(top_builddir)/share/unisim-ppcemu-${PPCEMU_VERSION}/\$\$(basename \"\$\${SHARED_DATA}\"); \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\tdone\n" >> "${PPCEMU_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >> "${PPCEMU_MAKEFILE_AM}"
	printf "\tfi\n" >> "${PPCEMU_MAKEFILE_AM}"
	
	echo "Building powerpc configure"
	${SHELL} -c "cd ${DEST_DIR}/ppcemu && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
