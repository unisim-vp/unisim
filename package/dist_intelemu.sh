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
UNISIM_SIMULATORS_DIR=${UNISIM_DIR}/unisim_simulators/cxx/intelemu

INTELEMU_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)

if test -z "${DISTCOPY}"; then
    DISTCOPY=cp
fi

UNISIM_LIB_INTELEMU_SOURCE_FILES="\
unisim/kernel/debug/debug.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/kernel/api/api.cc \
unisim/kernel/tlm/tlm.cc \
unisim/kernel/tlm2/tlm.cc \
unisim/kernel/service/service.cc \
unisim/kernel/service/xml_helper.cc \
unisim/api/debug/debug_api.cc \
unisim/service/tee/memory_access_reporting/tee_32.cc \
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
unisim/service/profiling/addr_profiler/profiler32.cc \
unisim/service/power/cache_profile.cc \
unisim/service/power/cache_dynamic_power.cc \
unisim/service/power/cache_leakage_power.cc \
unisim/service/power/cache_power_estimator.cc \
unisim/service/power/cache_dynamic_energy.cc \
unisim/service/os/linux_os/linux.cc \
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
unisim/util/debug/blob/section32.cc \
unisim/util/debug/blob/blob32.cc \
unisim/util/debug/blob/section64.cc \
unisim/util/debug/blob/segment32.cc \
unisim/util/debug/blob/segment64.cc \
unisim/util/debug/blob/blob64.cc \
unisim/util/debug/profile_64.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/stmt_32.cc \
unisim/util/debug/elf_symtab/elf_symtab32.cc \
unisim/util/debug/elf_symtab/elf_symtab64.cc \
unisim/util/debug/coff_symtab/coff_symtab32.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/debug/profile_32.cc \
unisim/util/debug/stmt_64.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/watchpoint_registry_64.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/type.cc \
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/loader/elf_loader/elf64_loader.cc \
unisim/util/loader/coff_loader/coff_loader32.cc \
unisim/util/os/linux_os/environment.cc \
unisim/util/os/linux_os/linux.cc \
unisim/util/lexer/lexer.cc \
unisim/util/ieee754/ieee754.cc \
unisim/util/xml/xml.cc \
unisim/util/endian/endian.cc \
unisim/util/garbage_collector/garbage_collector.cc \
unisim/util/random/random.cc \
unisim/util/queue/queue.cc \
unisim/component/cxx/processor/intel/disasm.cc \
unisim/component/cxx/processor/intel/arch.cc \
unisim/component/cxx/processor/intel/memory_op.cc \
unisim/component/cxx/memory/ram/memory_64.cc \
unisim/component/cxx/memory/ram/memory_32.cc"

UNISIM_LIB_INTELEMU_HEADER_FILES="
unisim/api/debug/debug_api.hh \
unisim/kernel/api/api.hh \
unisim/kernel/debug/debug.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/tlm/tlm.hh \
unisim/kernel/tlm2/tlm.hh \
unisim/service/debug/debugger/debugger.hh \
unisim/service/debug/debugger/debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/sim_debugger/sim_debugger.hh \
unisim/service/debug/sim_debugger/sim_debugger.tcc \
unisim/service/interfaces/backtrace.hh \
unisim/service/interfaces/blob.hh \
unisim/service/interfaces/cache_power_estimator.hh \
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/interfaces/debug_control.hh \
unisim/service/interfaces/debug_event.hh \
unisim/service/interfaces/debug_info_loading.hh \
unisim/service/interfaces/disassembly.hh \
unisim/service/interfaces/linux_os.hh \
unisim/service/interfaces/loader.hh \
unisim/service/interfaces/memory_access_reporting.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/os.hh \
unisim/service/interfaces/power_mode.hh \
unisim/service/interfaces/profiling.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/stmt_lookup.hh \
unisim/service/interfaces/subprogram_lookup.hh \
unisim/service/interfaces/symbol_table_lookup.hh \
unisim/service/interfaces/time.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/os/linux_os/linux.hh \
unisim/service/os/linux_os/linux.tcc \
unisim/service/power/cache_dynamic_energy.hh \
unisim/service/power/cache_dynamic_power.hh \
unisim/service/power/cache_leakage_power.hh \
unisim/service/power/cache_power_estimator.hh \
unisim/service/power/cache_profile.hh \
unisim/service/profiling/addr_profiler/profiler.hh \
unisim/service/profiling/addr_profiler/profiler.tcc \
unisim/service/tee/memory_access_reporting/tee.hh \
unisim/service/tee/memory_access_reporting/tee.tcc \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/service/trap_handler/trap_handler.hh \
unisim/service/trap_handler/trap_handler_identifier.hh \
unisim/service/trap_handler/trap_handler_identifier_interface.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/debug/blob/blob.hh \
unisim/util/debug/blob/blob.tcc \
unisim/util/debug/blob/section.hh \
unisim/util/debug/blob/section.tcc \
unisim/util/debug/blob/segment.hh \
unisim/util/debug/blob/segment.tcc \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/coff_symtab/coff_symtab.hh \
unisim/util/debug/coff_symtab/coff_symtab.tcc \
unisim/util/debug/data_object.hh \
unisim/util/debug/data_object_initializer.hh \
unisim/util/debug/data_object_initializer.tcc \
unisim/util/debug/dwarf/abbrev.hh \
unisim/util/debug/dwarf/addr_range.hh \
unisim/util/debug/dwarf/addr_range.tcc \
unisim/util/debug/dwarf/attr.hh \
unisim/util/debug/dwarf/attr.tcc \
unisim/util/debug/dwarf/call_frame_prog.hh \
unisim/util/debug/dwarf/call_frame_prog.tcc \
unisim/util/debug/dwarf/call_frame_vm.hh \
unisim/util/debug/dwarf/call_frame_vm.tcc \
unisim/util/debug/dwarf/cfa.hh \
unisim/util/debug/dwarf/cie.hh \
unisim/util/debug/dwarf/cie.tcc \
unisim/util/debug/dwarf/class.hh \
unisim/util/debug/dwarf/c_loc_expr_parser.hh \
unisim/util/debug/dwarf/cu.hh \
unisim/util/debug/dwarf/cu.tcc \
unisim/util/debug/dwarf/data_object.hh \
unisim/util/debug/dwarf/data_object.tcc \
unisim/util/debug/dwarf/die.hh \
unisim/util/debug/dwarf/die.tcc \
unisim/util/debug/dwarf/dwarf.hh \
unisim/util/debug/dwarf/dwarf.tcc \
unisim/util/debug/dwarf/encoding.hh \
unisim/util/debug/dwarf/expr_vm.hh \
unisim/util/debug/dwarf/expr_vm.tcc \
unisim/util/debug/dwarf/fde.hh \
unisim/util/debug/dwarf/fde.tcc \
unisim/util/debug/dwarf/filename.hh \
unisim/util/debug/dwarf/fmt.hh \
unisim/util/debug/dwarf/frame.hh \
unisim/util/debug/dwarf/frame.tcc \
unisim/util/debug/dwarf/fwd.hh \
unisim/util/debug/dwarf/leb128.hh \
unisim/util/debug/dwarf/loc.hh \
unisim/util/debug/dwarf/loc.tcc \
unisim/util/debug/dwarf/macinfo.hh \
unisim/util/debug/dwarf/macinfo.tcc \
unisim/util/debug/dwarf/ml.hh \
unisim/util/debug/dwarf/option.hh \
unisim/util/debug/dwarf/pub.hh \
unisim/util/debug/dwarf/pub.tcc \
unisim/util/debug/dwarf/range.hh \
unisim/util/debug/dwarf/range.tcc \
unisim/util/debug/dwarf/register_number_mapping.hh \
unisim/util/debug/dwarf/stmt_prog.hh \
unisim/util/debug/dwarf/stmt_prog.tcc \
unisim/util/debug/dwarf/stmt_vm.hh \
unisim/util/debug/dwarf/stmt_vm.tcc \
unisim/util/debug/dwarf/subprogram.hh \
unisim/util/debug/dwarf/subprogram.tcc \
unisim/util/debug/dwarf/util.hh \
unisim/util/debug/dwarf/version.hh \
unisim/util/debug/elf_symtab/elf_symtab.hh \
unisim/util/debug/elf_symtab/elf_symtab.tcc \
unisim/util/debug/event.hh \
unisim/util/debug/memory_access_type.hh \
unisim/util/debug/profile.hh \
unisim/util/debug/profile.tcc \
unisim/util/debug/register.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/stmt.hh \
unisim/util/debug/stmt.tcc \
unisim/util/debug/subprogram.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/util/debug/type.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/dictionary/dictionary.hh \
unisim/util/dictionary/dictionary.tcc \
unisim/util/endian/endian.hh \
unisim/util/garbage_collector/garbage_collector.hh \
unisim/util/hash_table/hash_table.hh \
unisim/util/ieee754/ieee754.hh \
unisim/util/inlining/inlining.hh \
unisim/util/lexer/lexer.hh \
unisim/util/lexer/lexer.tcc \
unisim/util/likely/likely.hh \
unisim/util/loader/coff_loader/coff_loader.hh \
unisim/util/loader/coff_loader/coff_loader.tcc \
unisim/util/loader/coff_loader/ti/ti.hh \
unisim/util/loader/coff_loader/ti/ti.tcc \
unisim/util/loader/elf_loader/elf32.h \
unisim/util/loader/elf_loader/elf32_loader.hh \
unisim/util/loader/elf_loader/elf64.h \
unisim/util/loader/elf_loader/elf64_loader.hh \
unisim/util/loader/elf_loader/elf_common.h \
unisim/util/loader/elf_loader/elf_loader.hh \
unisim/util/loader/elf_loader/elf_loader.tcc \
unisim/util/os/linux_os/aux_table.hh \
unisim/util/os/linux_os/calls.tcc \
unisim/util/os/linux_os/environment.hh \
unisim/util/os/linux_os/errno.hh \
unisim/util/os/linux_os/files_flags.hh \
unisim/util/os/linux_os/intel.hh \
unisim/util/os/linux_os/linux.hh \
unisim/util/os/linux_os/linux.tcc \
unisim/util/os/linux_os/ppc.hh \
unisim/util/parser/parser.hh \
unisim/util/parser/parser.tcc \
unisim/util/queue/queue.hh \
unisim/util/queue/queue.tcc \
unisim/util/random/random.hh \
unisim/util/simfloat/floating.hh \
unisim/util/simfloat/floating.tcc \
unisim/util/simfloat/host_floating.hh \
unisim/util/simfloat/host_floating.tcc \
unisim/util/simfloat/integer.hh \
unisim/util/simfloat/integer.tcc \
unisim/util/xml/xml.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/memory/ram/memory.tcc \
unisim/component/cxx/processor/intel/arch.hh \
unisim/component/cxx/processor/intel/disasm.hh \
unisim/component/cxx/processor/intel/exception.hh \
unisim/component/cxx/processor/intel/execute.hh \
unisim/component/cxx/processor/intel/extregbank.hh \
unisim/component/cxx/processor/intel/hostfloat.hh \
unisim/component/cxx/processor/intel/models.hh \
unisim/component/cxx/processor/intel/psr.hh \
unisim/component/cxx/processor/intel/register_field.hh \
"

UNISIM_LIB_INTELEMU_M4_FILES="\
m4/times.m4 \
m4/endian.m4 \
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/boost_graph.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/with_boost.m4 \
m4/cacti.m4 \
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4 \
m4/pthread.m4"

UNISIM_LIB_INTELEMU_DATA_FILES="\
"

INTELEMU_EXTERNAL_HEADERS="\
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

UNISIM_SIMULATORS_INTELEMU_SOURCE_FILES="\
main.cc \
simulator.cc \
"
UNISIM_SIMULATORS_INTELEMU_HEADER_FILES="\
simulator.hh \
"

UNISIM_SIMULATORS_INTELEMU_EXTRA_FILES="\
config.h.in \
"

UNISIM_SIMULATORS_INTELEMU_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
"

has_to_build_configure=no

install_src_file () {
    has_new_src_file=no
    src_path="$1"
    dst_path="$2"
    mkdir -p "$(dirname ${dst_path})"
    has_to_copy=no
    if [ ! -e "${dst_path}" ] || [ "${src_path}" -nt "${dst_path}" ]; then
	echo "${src_path} ==> ${dst_path}"
	${DISTCOPY} -f "${src_path}" "${dst_path}" || exit
        has_new_src_file=yes
    fi
}

UNISIM_LIB_INTELEMU_FILES="${UNISIM_LIB_INTELEMU_SOURCE_FILES} ${UNISIM_LIB_INTELEMU_HEADER_FILES} ${UNISIM_LIB_INTELEMU_DATA_FILES}"

for file in ${UNISIM_LIB_INTELEMU_FILES}; do
    install_src_file "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${file}"
done

UNISIM_SIMULATORS_INTELEMU_FILES="${UNISIM_SIMULATORS_INTELEMU_SOURCE_FILES} ${UNISIM_SIMULATORS_INTELEMU_HEADER_FILES} ${UNISIM_SIMULATORS_INTELEMU_EXTRA_FILES} ${UNISIM_SIMULATORS_INTELEMU_DATA_FILES}"

for file in ${UNISIM_SIMULATORS_INTELEMU_FILES}; do
    install_src_file "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/${file}"
done

mkdir -p ${DEST_DIR}/config

for file in ${UNISIM_LIB_INTELEMU_M4_FILES}; do
    install_src_file "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${file}"
done

if [ "${has_new_src_file}" = "yes" ]; then
    has_to_build_configure=yes
if 

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Yves Lhuillier <yves.lhuillier@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - intelemu: an INTEL user level simulator
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

Building instructions:
  $ ./configure 
  $ make

Installing (optional):
  $ make install
EOF

# intelemu

INTELEMU_CONFIGURE_AC="${DEST_DIR}/configure.ac"
INTELEMU_MAKEFILE_AM="${DEST_DIR}/Makefile.am"

if [ ! -e "${INTELEMU_CONFIGURE_AC}" ] || [ "$0" -nt "${INTELEMU_CONFIGURE_AC}" ] || [ ! -e "${INTELEMU_MAKEFILE_AM}" ] || [ "$0" -nt "${INTELEMU_MAKEFILE_AM}" ]; then
    has_to_build_configure=yes
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Generating intelemu configure.ac"
        exec 5> "${INTELEMU_CONFIGURE_AC}"
	echo "AC_INIT([UNISIM INTELemu C++ simulator], [${INTELEMU_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-intelemu])" >&5
	echo "AC_CONFIG_MACRO_DIR([m4])" >&5
	echo "AC_CONFIG_AUX_DIR(config)" >&5
	echo "AC_CONFIG_HEADERS([config.h])" >&5
	echo "AC_CANONICAL_BUILD" >&5
	echo "AC_CANONICAL_HOST" >&5
	echo "AC_CANONICAL_TARGET" >&5
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >&5
	echo "AC_PATH_PROGS(SH, sh)" >&5
	echo "AC_PROG_CXX" >&5
	echo "AC_PROG_RANLIB" >&5
	echo "AC_PROG_INSTALL" >&5
	echo "AC_PROG_LN_S" >&5
	echo "AC_LANG([C++])" >&5
	echo "AM_PROG_CC_C_O" >&5
	echo "AC_CHECK_HEADERS([${INTELEMU_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >&5
	echo "UNISIM_CHECK_PTHREAD(main)" >&5
	echo "UNISIM_CHECK_TIMES(main)" >&5
	echo "UNISIM_CHECK_ENDIAN(main)" >&5
	echo "UNISIM_CHECK_CURSES(main)" >&5
	echo "UNISIM_CHECK_LIBEDIT(main)" >&5
	echo "UNISIM_CHECK_BSD_SOCKETS(main)" >&5
	echo "UNISIM_CHECK_ZLIB(main)" >&5
	echo "UNISIM_CHECK_LIBXML2(main)" >&5
	echo "UNISIM_CHECK_CXXABI(main)" >&5
	echo "UNISIM_WITH_BOOST(main)" >&5
	echo "UNISIM_CHECK_BOOST_GRAPH(main)" >&5
	echo "UNISIM_CHECK_CACTI(main)" >&5
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH(main)" >&5
	echo "UNISIM_CHECK_REAL_PATH(main)" >&5
	echo "GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib" >&5
	echo "AC_SUBST(GENISSLIB_PATH)" >&5
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-intelemu-${INTELEMU_VERSION}\"], [path of shared data relative to bin directory])" >&5
	SIM_VERSION_MAJOR=$(printf "${INTELEMU_VERSION}" | cut -f 1 -d .)
	SIM_VERSION_MINOR=$(printf "${INTELEMU_VERSION}" | cut -f 2 -d .)
	SIM_VERSION_PATCH=$(printf "${INTELEMU_VERSION}" | cut -f 3 -d .)
	SIM_VERSION="${INTELEMU_VERSION}"
	SIM_VERSION_CODENAME="Triumphalis Tarraco"
	SIM_AUTHOR="Daniel Gracia Perez (daniel.gracia-perez@cea.fr)"
	SIM_PROGRAM_NAME="UNISIM INTELEmu"
	SIM_LICENSE="BSD (See file COPYING)"
	SIM_COPYRIGHT="Copyright (C) 2007-2010, Commissariat a l'Energie Atomique"
	SIM_DESCRIPTION="UNISIM INTELv5 User Level Simulator"
	SIM_SCHEMATIC="fig_schematic.pdf"
	echo "AC_DEFINE([SIM_VERSION_MAJOR], [${SIM_VERSION_MAJOR}], [Version major number])" >&5
	echo "AC_DEFINE([SIM_VERSION_MINOR], [${SIM_VERSION_MINOR}], [Version minor number])" >&5
	echo "AC_DEFINE([SIM_VERSION_PATCH], [${SIM_VERSION_PATCH}], [Version patch number])" >&5
	echo "AC_DEFINE([SIM_VERSION], [\"${SIM_VERSION}\"], [Version])" >&5
	echo "AC_DEFINE([SIM_VERSION_CODENAME], [\"${SIM_VERSION_CODENAME}\"], [Version code name])" >&5
	echo "AC_DEFINE([SIM_AUTHOR], [\"${SIM_AUTHOR}\"], [Author])" >&5
	echo "AC_DEFINE([SIM_PROGRAM_NAME], [\"${SIM_PROGRAM_NAME}\"], [Program name])" >&5
	echo "AC_DEFINE([SIM_COPYRIGHT], [\"${SIM_COPYRIGHT}\"], [Copyright])" >&5
	echo "AC_DEFINE([SIM_LICENSE], [\"${SIM_LICENSE}\"], [License])" >&5
	echo "AC_DEFINE([SIM_DESCRIPTION], [\"${SIM_DESCRIPTION}\"], [Description])" >&5
	echo "AC_DEFINE([SIM_SCHEMATIC], [\"${SIM_SCHEMATIC}\"], [Schematic])" >&5
	echo "AC_CONFIG_FILES([Makefile])" >&5
	echo "AC_OUTPUT" >&5
        exec 5>&-

	AM_INTELEMU_VERSION=$(printf ${INTELEMU_VERSION} | sed -e 's/\./_/g')
	echo "Generating intelemu Makefile.am"
        exec 5> "${INTELEMU_MAKEFILE_AM}"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" >&5
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >&5
	echo "noinst_LIBRARIES = libintelemu-${INTELEMU_VERSION}.a" >&5
	echo "libintelemu_${AM_INTELEMU_VERSION}_a_SOURCES = ${UNISIM_LIB_INTELEMU_SOURCE_FILES}" >&5
	echo "bin_PROGRAMS = unisim-intelemu-${INTELEMU_VERSION}" >&5
	echo "unisim_intelemu_${AM_INTELEMU_VERSION}_SOURCES = ${UNISIM_SIMULATORS_INTELEMU_SOURCE_FILES}" >&5
	echo "unisim_intelemu_${AM_INTELEMU_VERSION}_CPPFLAGS = -DSIM_EXECUTABLE" >&5
	echo "unisim_intelemu_${AM_INTELEMU_VERSION}_LDADD = libintelemu-${INTELEMU_VERSION}.a" >&5

	echo "noinst_HEADERS = ${UNISIM_LIB_INTELEMU_HEADER_FILES} ${UNISIM_SIMULATORS_INTELEMU_HEADER_FILES}" >&5
	echo "EXTRA_DIST = ${UNISIM_LIB_INTELEMU_M4_FILES}" >&5
	echo "sharedir = \$(prefix)/share/unisim-intelemu-${INTELEMU_VERSION}" >&5
	echo "dist_share_DATA = ${UNISIM_LIB_INTELEMU_DATA_FILES} ${UNISIM_SIMULATORS_INTELEMU_DATA_FILES}" >&5

	echo -n "BUILT_SOURCES=" >&5
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_intel32.hh " >&5
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_intel32.tcc " >&5
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_thumb.hh " >&5
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_thumb.tcc " >&5
	echo >&5
	
	echo -n "CLEANFILES=" >&5
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_intel32.hh " >&5
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_intel32.tcc " >&5
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_thumb.hh " >&5
	echo -n "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_thumb.tcc " >&5
	echo >&5
	
	echo "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_intel32.tcc: \$(top_builddir)/unisim/component/cxx/processor/intel/isa_intel32.hh" >&5
	echo "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_intel32.hh: ${UNISIM_LIB_INTELEMU_ISA_INTEL32_FILES}" >&5
	printf "\t" >&5
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/intel/isa_intel32 -w 8 -I \$(top_srcdir) -I \$(top_srcdir)/unisim/component/cxx/processor/intel/isa/intel32 \$(top_srcdir)/unisim/component/cxx/processor/intel/isa/intel32/intel32.isa" >&5

	echo "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_thumb.tcc: \$(top_builddir)/unisim/component/cxx/processor/intel/isa_thumb.hh" >&5
	echo "\$(top_builddir)/unisim/component/cxx/processor/intel/isa_thumb.hh: ${UNISIM_LIB_INTELEMU_ISA_THUMB_FILES}" >&5
	printf "\t" >&5
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/intel/isa_thumb -w 8 -I \$(top_srcdir) -I \$(top_srcdir)/unisim/component/cxx/processor/intel/isa/thumb2 \$(top_srcdir)/unisim/component/cxx/processor/intel/isa/thumb2/thumb.isa" >&5

	echo "all-local: all-local-bin all-local-share" >&5
	echo "clean-local: clean-local-bin clean-local-share" >&5
	echo "all-local-bin: \$(bin_PROGRAMS)" >&5
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >&5
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >&5
	printf "\trm -f \"\$(top_builddir)/bin/\$\$(basename \$\${PROGRAM})\"; \\\\\n" >&5
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >&5
	printf "\tcp -f \"\$(top_builddir)/\$\${PROGRAM}\" \$(top_builddir)/bin/\$\$(basename \"\$\${PROGRAM}\"); \\\\\n" >&5
	printf "\tdone\n" >&5
	echo "clean-local-bin:" >&5
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >&5
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >&5
	printf "\tfi\n" >&5
	echo "all-local-share: \$(dist_share_DATA)" >&5
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >&5
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >&5
	printf "\trm -f \"\$(top_builddir)/share/unisim-intelemu-${INTELEMU_VERSION}/\$\$(basename \$\${SHARED_DATA})\"; \\\\\n" >&5
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-intelemu-${INTELEMU_VERSION}'; \\\\\n" >&5
	printf "\tcp -f \"\$(top_srcdir)/\$\${SHARED_DATA}\" \$(top_builddir)/share/unisim-intelemu-${INTELEMU_VERSION}/\$\$(basename \"\$\${SHARED_DATA}\"); \\\\\n" >&5
	printf "\tdone\n" >&5
	echo "clean-local-share:" >&5
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >&5
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >&5
	printf "\tfi\n" >&5
        exec 5>&-
        
	echo "Building intelemu configure"
	${SHELL} -c "cd ${DEST_DIR}/intelemu && aclocal -I m4 && autoconf --force && automake -ac"
fi

echo "Distribution is up-to-date"