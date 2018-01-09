#!/bin/bash

SIMPKG=arm64emu

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
subdecoder.hh \
"

UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES="\
scanner.cc \
parser.cc \
parser_tokens.hh \
"

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
errtools.cc \
"

UNISIM_TOOLS_GENISSLIB_DATA_FILES="COPYING INSTALL NEWS README AUTHORS ChangeLog"

UNISIM_TOOLS_GENISSLIB_M4_FILES="\
m4/lexer.m4 \
m4/parser_gen.m4 \
"

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
vector \
"

UNISIM_LIB_SIMULATOR_SOURCE_FILES="\
unisim/util/backtrace/backtrace.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/kernel/api/api.cc \
unisim/kernel/service/endian.cc \
unisim/kernel/service/service.cc \
unisim/kernel/service/xml_helper.cc \
unisim/kernel/tlm2/tlm.cc \
unisim/api/debug/debug_api.cc \
unisim/service/debug/inline_debugger/inline_debugger_64.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/debug/gdb_server/gdb_server_64.cc \
unisim/service/debug/debugger/debugger64.cc \
unisim/service/debug/monitor/monitor.cc \
unisim/service/debug/monitor/monitor_64.cc \
unisim/service/profiling/addr_profiler/profiler64.cc \
unisim/service/os/linux_os/arm_linux64.cc \
unisim/service/trap_handler/trap_handler.cc \
unisim/service/trap_handler/trap_handler_identifier.cc \
unisim/service/time/host_time/time.cc \
unisim/service/time/sc_time/time.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/symbol_table_64.cc \
unisim/util/debug/dwarf/class.cc \
unisim/util/debug/dwarf/dwarf32.cc \
unisim/util/debug/dwarf/encoding.cc \
unisim/util/debug/dwarf/ml.cc \
unisim/util/debug/dwarf/attr.cc \
unisim/util/debug/dwarf/filename.cc \
unisim/util/debug/dwarf/leb128.cc \
unisim/util/debug/dwarf/abbrev.cc \
unisim/util/debug/dwarf/dwarf64.cc \
unisim/util/debug/dwarf/register_number_mapping.cc \
unisim/util/debug/dwarf/data_object.cc \
unisim/util/debug/dwarf/c_loc_expr_parser.cc \
unisim/util/debug/breakpoint_registry_64.cc \
unisim/util/blob/blob32.cc \
unisim/util/blob/section32.cc \
unisim/util/blob/section64.cc \
unisim/util/blob/segment32.cc \
unisim/util/blob/segment64.cc \
unisim/util/blob/blob64.cc \
unisim/util/debug/profile_64.cc \
unisim/util/debug/watchpoint_registry_64.cc \
unisim/util/debug/elf_symtab/elf_symtab32.cc \
unisim/util/debug/elf_symtab/elf_symtab64.cc \
unisim/util/debug/coff_symtab/coff_symtab64.cc \
unisim/util/debug/breakpoint_registry_64.cc \
unisim/util/debug/profile_64.cc \
unisim/util/debug/stmt_32.cc \
unisim/util/debug/stmt_64.cc \
unisim/util/debug/watchpoint_registry_64.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/type.cc \
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/loader/elf_loader/elf64_loader.cc \
unisim/util/loader/coff_loader/coff_loader64.cc \
unisim/util/os/linux_os/environment.cc \
unisim/util/os/linux_os/linux.cc \
unisim/util/lexer/lexer.cc \
unisim/util/ieee754/ieee754.cc \
unisim/util/xml/xml.cc \
unisim/util/garbage_collector/garbage_collector.cc \
unisim/util/random/random.cc \
unisim/util/queue/queue.cc \
unisim/component/tlm2/memory/ram/memory.cc \
unisim/component/tlm2/memory/ram/memory_debug.cc \
unisim/component/tlm2/processor/arm/cortex_a53/cpu.cc \
unisim/component/cxx/processor/arm/isa/arm64/disasm.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc \
"

UNISIM_LIB_SIMULATOR_ISA_FILES="\
unisim/component/cxx/processor/arm/isa/arm64/base.isa \
unisim/component/cxx/processor/arm/isa/arm64/simd_fp.isa \
unisim/component/cxx/processor/arm/isa/arm64/arm64.isa \
"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
${UNISIM_LIB_SIMULATOR_ISA_FILES} \
unisim/util/backtrace/backtrace.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/api/api.hh \
unisim/kernel/tlm/tlm.hh \
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/tlm2/tlm.hh \
unisim/api/debug/debug_api.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/debug/debugger/debugger.hh \
unisim/service/debug/monitor/monitor.hh \
unisim/service/profiling/addr_profiler/profiler.hh \
unisim/service/os/linux_os/linux.hh \
unisim/service/os/linux_os/arm_linux64.hh \
unisim/service/trap_handler/trap_handler.hh \
unisim/service/trap_handler/trap_handler_identifier.hh \
unisim/service/trap_handler/trap_handler_identifier_interface.hh \
unisim/service/interfaces/debug_selecting.hh \
unisim/service/interfaces/debug_yielding.hh \
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
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/os.hh \
unisim/service/interfaces/linux_os.hh \
unisim/service/interfaces/stmt_lookup.hh \
unisim/service/interfaces/loader.hh \
unisim/service/interfaces/register.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/symbol_table_lookup.hh \
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/interfaces/subprogram_lookup.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/util/likely/likely.hh \
unisim/util/debug/data_object.hh \
unisim/util/debug/commit_insn_event.hh \
unisim/util/debug/fetch_insn_event.hh \
unisim/util/debug/memory_access_type.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/trap_event.hh \
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
unisim/util/blob/segment.hh \
unisim/util/blob/blob.hh \
unisim/util/blob/section.hh \
unisim/util/debug/stmt.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/elf_symtab/elf_symtab.hh \
unisim/util/debug/coff_symtab/coff_symtab.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/event.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/profile.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/type.hh \
unisim/util/debug/data_object_initializer.hh \
unisim/util/debug/subprogram.hh \
unisim/util/loader/elf_loader/elf64_loader.hh \
unisim/util/loader/elf_loader/elf_loader.hh \
unisim/util/loader/elf_loader/elf64_loader.hh \
unisim/util/loader/elf_loader/elf_common.h \
unisim/util/loader/elf_loader/elf64.h \
unisim/util/loader/coff_loader/coff_loader.hh \
unisim/util/loader/coff_loader/ti/ti.hh \
unisim/util/os/linux_os/aarch64.hh \
unisim/util/os/linux_os/aux_table.hh \
unisim/util/os/linux_os/environment.hh \
unisim/util/os/linux_os/files_flags.hh \
unisim/util/os/linux_os/linux.hh \
unisim/util/os/linux_os/errno.hh \
unisim/util/dictionary/dictionary.hh \
unisim/util/lexer/lexer.hh \
unisim/util/parser/parser.hh \
unisim/util/xml/xml.hh \
unisim/util/endian/endian.hh \
unisim/util/garbage_collector/garbage_collector.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/truth_table/truth_table.hh \
unisim/util/random/random.hh \
unisim/util/hash_table/hash_table.hh \
unisim/util/queue/queue.hh \
unisim/util/simfloat/floating.hh \
unisim/util/simfloat/integer.hh \
unisim/util/simfloat/host_floating.hh \
unisim/util/ieee754/ieee754.hh \
unisim/util/inlining/inlining.hh \
unisim/component/tlm2/memory/ram/memory.hh \
unisim/component/tlm2/processor/arm/cortex_a53/cpu.hh \
unisim/component/cxx/processor/arm/vmsav8/cpu.hh \
unisim/component/cxx/processor/arm/isa/arm64/execute.hh \
unisim/component/cxx/processor/arm/isa/arm64/decode.hh \
unisim/component/cxx/processor/arm/isa/arm64/disasm.hh \
unisim/component/cxx/memory/ram/memory.hh \
"

UNISIM_LIB_SIMULATOR_TEMPLATE_FILES="\
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/debug/debugger/debugger.tcc \
unisim/service/debug/monitor/monitor.tcc \
unisim/service/profiling/addr_profiler/profiler.tcc \
unisim/service/os/linux_os/linux.tcc \
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
unisim/util/blob/section.tcc \
unisim/util/blob/blob.tcc \
unisim/util/blob/segment.tcc \
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
unisim/component/cxx/processor/arm/vmsav8/cpu.tcc \
unisim/component/tlm2/memory/ram/memory.tcc \
unisim/component/cxx/memory/ram/memory.tcc \
"

UNISIM_LIB_SIMULATOR_M4_FILES="\
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
m4/pthread.m4 \
"

UNISIM_LIB_SIMULATOR_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_arm_with_fpa.xml \
unisim/service/debug/gdb_server/gdb_arm_with_neon.xml \
unisim/util/debug/dwarf/aarch64_eabi_dwarf_register_number_mapping.xml \
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

UNISIM_SIMULATOR_SOURCE_FILES="\
main.cc \
simulator.cc \
"
UNISIM_SIMULATOR_HEADER_FILES="\
simulator.hh \
"

UNISIM_SIMULATOR_TEMPLATE_FILES="\
"

UNISIM_SIMULATOR_EXTRA_FILES="\
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

UNISIM_SIMULATOR_TESTBENCH_FILES="\
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
DEST_DIR=$(cd $1; pwd)
mkdir -p ${DEST_DIR}
UNISIM_TOOLS_DIR=${UNISIM_DIR}/unisim_tools
UNISIM_LIB_DIR=${UNISIM_DIR}/unisim_lib
UNISIM_SIMULATOR_DIR=${UNISIM_DIR}/unisim_simulators/tlm2/${SIMPKG}

SIMULATOR_VERSION=$(cat ${UNISIM_SIMULATOR_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-${SIMPKG}-${SIMULATOR_VERSION}

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

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/${SIMPKG}

UNISIM_TOOLS_GENISSLIB_FILES="${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES} ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES} ${UNISIM_TOOLS_GENISSLIB_DATA_FILES}"

for file in ${UNISIM_TOOLS_GENISSLIB_FILES}; do
	dist_copy "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/genisslib/${file}"
done

UNISIM_LIB_SIMULATOR_FILES="${UNISIM_LIB_SIMULATOR_SOURCE_FILES} ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_LIB_SIMULATOR_TEMPLATE_FILES} ${UNISIM_LIB_SIMULATOR_DATA_FILES}"

for file in ${UNISIM_LIB_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_EXTRA_FILES} \
${UNISIM_SIMULATOR_TEMPLATE_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
${UNISIM_SIMULATOR_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

for file in ${UNISIM_SIMULATOR_TOP_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/${SIMPKG}/config
mkdir -p ${DEST_DIR}/${SIMPKG}/m4
mkdir -p ${DEST_DIR}/genisslib/config
mkdir -p ${DEST_DIR}/genisslib/m4

has_to_build_genisslib_configure=no
for file in ${UNISIM_TOOLS_GENISSLIB_M4_FILES}; do
	if dist_copy "${UNISIM_TOOLS_DIR}/${file}" "${DEST_DIR}/genisslib/${file}"; then
		has_to_build_genisslib_configure=yes
	fi
done

has_to_build_simulator_configure=no
for file in ${UNISIM_LIB_SIMULATOR_M4_FILES}; do
	if dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"; then
		has_to_build_simulator_configure=yes
	fi
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Yves Lhuillier <yves.lhuillier@cea.fr>
Gilles Mouchard <gilles.mouchard@cea.fr>
Reda Nouacer <reda.nouacer@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - ARMemu: an ARMv7 application level simulator (LinuxOS emulation)
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

if has_to_build "${CONFIGURE_AC}" "$0" || has_to_build "${MAKEFILE_AM}" "$0"; then
	echo "Generating configure.ac"
	cat <<EOF > "${CONFIGURE_AC}"
AC_INIT([UniSIM ARMemu Standalone simulator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}])
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

	echo "Generating Makefile.am"
	cat <<EOF > "${MAKEFILE_AM}"
SUBDIRS=genisslib ${SIMPKG}
EXTRA_DIST = configure.cross
EOF

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

# GENISSLIB

GENISSLIB_CONFIGURE_AC="${DEST_DIR}/genisslib/configure.ac"
GENISSLIB_MAKEFILE_AM="${DEST_DIR}/genisslib/Makefile.am"

if has_to_build "${GENISSLIB_CONFIGURE_AC}" "$0" || has_to_build "${GENISSLIB_MAKEFILE_AM}" "$0"; then
	has_to_build_genisslib_configure=yes
fi

if [ "${has_to_build_genisslib_configure}" = "yes" ]; then
	echo "Generating GENISSLIB configure.ac"
	cat <<EOF > "${GENISSLIB_CONFIGURE_AC}"
AC_INIT([UNISIM GENISSLIB], [${GENISSLIB_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])
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
AC_PROG_LN_S
AC_LANG([C++])
AC_CHECK_HEADERS([${GENISSLIB_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))
UNISIM_CHECK_LEXER_GENERATOR
UNISIM_CHECK_PARSER_GENERATOR
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF

	AM_GENISSLIB_VERSION=$(printf ${GENISSLIB_VERSION} | sed -e 's/\./_/g')
	echo "Generating GENISSLIB Makefile.am"
	cat <<EOF > "${GENISSLIB_MAKEFILE_AM}"
ACLOCAL_AMFLAGS=-I m4
BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}
CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}
AM_YFLAGS = -d -p yy
AM_LFLAGS = -l
AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)
noinst_PROGRAMS = genisslib
genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}
genisslib_CPPFLAGS = -DGENISSLIB_VERSION=\"${GENISSLIB_VERSION}\"
genisslib_CXXFLAGS = -O1 -Wno-error
noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}
EXTRA_DIST = ${UNISIM_TOOLS_GENISSLIB_M4_FILES}
# The following lines are a workaround caused by a bugFix in AUTOMAKE 1.12
# Note that parser_tokens.hh has been added to BUILT_SOURCES above
# assumption: parser.cc and either parser.h or parser.hh are generated at the same time
\$(top_builddir)/parser_tokens.hh: \$(top_builddir)/parser.cc
	if test -f "\$(top_builddir)/parser.h"; then\
		cp -f "\$(top_builddir)/parser.h" "\$(top_builddir)/parser_tokens.hh";\
	elif test -f "\$(top_builddir)/parser.hh"; then\
		cp -f "\$(top_builddir)/parser.hh" "\$(top_builddir)/parser_tokens.hh";\
	fi
# The following line disable some C++ flags that prevent the flex generated file to compile properly
\$(top_builddir)/genisslib-scanner.o: CXXFLAGS += -O1 -Wno-error
EOF

	echo "Building GENISSLIB configure"
	${SHELL} -c "cd ${DEST_DIR}/genisslib && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

# Simulator

SIMULATOR_CONFIGURE_AC="${DEST_DIR}/${SIMPKG}/configure.ac"
SIMULATOR_MAKEFILE_AM="${DEST_DIR}/${SIMPKG}/Makefile.am"

if has_to_build "${SIMULATOR_CONFIGURE_AC}" "$0" || has_to_build "${SIMULATOR_MAKEFILE_AM}" "$0"; then
	has_to_build_simulator_configure=yes
fi

if [ "${has_to_build_simulator_configure}" = "yes" ]; then
	echo "Generating ${SIMPKG} configure.ac"
	cat <<EOF > "${SIMULATOR_CONFIGURE_AC}"
AC_INIT([UNISIM ARMemu C++ simulator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}-core])
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
UNISIM_CHECK_PTHREAD(main)
UNISIM_CHECK_TIMES(main)
UNISIM_CHECK_ENDIAN(main)
UNISIM_CHECK_CURSES(main)
UNISIM_CHECK_LIBEDIT(main)
UNISIM_CHECK_BSD_SOCKETS(main)
UNISIM_CHECK_ZLIB(main)
UNISIM_CHECK_LIBXML2(main)
UNISIM_CHECK_CXXABI(main)
UNISIM_WITH_BOOST(main)
UNISIM_CHECK_BOOST_GRAPH(main)
UNISIM_CHECK_CACTI(main)
UNISIM_CHECK_GET_EXECUTABLE_PATH(main)
UNISIM_CHECK_REAL_PATH(main)
UNISIM_CHECK_SYSTEMC
GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib
AC_SUBST(GENISSLIB_PATH)
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/unisim-${SIMPKG}-${SIMULATOR_VERSION}"], [path of shared data relative to bin directory])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF

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

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_LIB_SIMULATOR_TEMPLATE_FILES} ${UNISIM_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_TEMPLATE_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm64.hh\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm64.tcc

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm64.hh\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm64.tcc

\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm64.tcc: \$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm64.hh
\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm64.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm64 -w 8 -I \$(top_srcdir) -I \$(top_srcdir)/unisim/component/cxx/processor/arm/isa/arm64 \$(top_srcdir)/unisim/component/cxx/processor/arm/isa/arm64/arm64.isa
EOF

	echo "Building ${SIMPKG} configure"
	${SHELL} -c "cd ${DEST_DIR}/${SIMPKG} && aclocal -I m4 && libtoolize --force && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"

