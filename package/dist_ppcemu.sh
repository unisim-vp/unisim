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
UNISIM_SIMULATORS_DIR=$4/tlm/ppcemu

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
unisim/service/os/linux_os/linux_os_32.cc \
unisim/service/os/linux_os/linux_os_exception.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/loader/linux_loader/linux32_loader.cc \
unisim/component/cxx/processor/powerpc/cpu.cc \
unisim/component/cxx/processor/powerpc/exception.cc \
unisim/component/cxx/processor/powerpc/config.cc \
unisim/component/cxx/processor/powerpc/mpc7447a.cc \
unisim/component/cxx/processor/powerpc/mpc7447a_debug.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/tlm/message/interrupt.cc \
unisim/component/tlm/processor/powerpc/mpc7447a.cc \
unisim/component/tlm/processor/powerpc/mpc7447a_debug.cc \
unisim/component/tlm/fsb/snooping_bus/bus_addr32_size32_procs1.cc \
unisim/component/tlm/memory/ram/memory_32.cc \
unisim/component/tlm/memory/ram/memory_32_debug.cc \
unisim/component/tlm/bridge/snooping_fsb_to_mem/addr32_burstsize32.cc \
unisim/component/tlm/bridge/snooping_fsb_to_mem/addr32_burstsize32_debug.cc"

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
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/loader/elf_loader/elf_common.h \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32.h \
unisim/service/loader/elf_loader/elf64.h \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/linux_loader/linux_loader.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/service/power/cache_power_estimator.hh \
unisim/service/os/linux_os/linux_os.hh \
unisim/service/os/linux_os/linux_os_exception.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/processor/powerpc/cpu.hh \
unisim/component/cxx/processor/powerpc/exception.hh \
unisim/component/cxx/processor/powerpc/floating.hh \
unisim/component/cxx/processor/powerpc/config.hh \
unisim/component/cxx/cache/cache.hh \
unisim/component/cxx/tlb/tlb.hh \
unisim/component/tlm/message/interrupt.hh \
unisim/component/tlm/processor/powerpc/powerpc.hh \
unisim/component/tlm/message/snooping_fsb.hh \
unisim/component/tlm/debug/transaction_spy.hh \
unisim/component/tlm/message/memory.hh \
unisim/component/tlm/memory/ram/memory.hh \
unisim/component/tlm/fsb/snooping_bus/bus.hh \
unisim/component/tlm/bridge/snooping_fsb_to_mem/bridge.hh \
unisim/component/tlm/bridge/snooping_fsb_to_mem/config.hh"

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
unisim/service/os/linux_os/linux_os.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/loader/linux_loader/linux_loader.tcc \
unisim/component/cxx/processor/powerpc/cpu.tcc \
unisim/component/cxx/processor/powerpc/exception.tcc \
unisim/component/cxx/memory/ram/memory.tcc \
unisim/component/cxx/cache/cache.tcc \
unisim/component/cxx/tlb/tlb.tcc \
unisim/component/tlm/processor/powerpc/powerpc.tcc \
unisim/component/tlm/memory/ram/memory.tcc \
unisim/component/tlm/fsb/snooping_bus/bus.tcc \
unisim/component/tlm/bridge/snooping_fsb_to_mem/bridge.tcc"

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
m4/with_boost.m4"

UNISIM_LIB_PPCEMU_SYSTEM_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_powerpc.xml"

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
has_to_build_ppcemu_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/ppcemu

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
	mkdir -p "${DEST_DIR}/ppcemu/`dirname ${file}`"
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

UNISIM_SIMULATORS_PPCEMU_SYSTEM_FILES="${UNISIM_SIMULATORS_PPCEMU_SYSTEM_SOURCE_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_TEMPLATE_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_DATA_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_FILES}; do
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

for file in ${UNISIM_LIB_PPCEMU_SYSTEM_M4_FILES}; do
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

echo "This package contains:" > "${DEST_DIR}/README"
echo "  - GenISSLib: an instruction set simulator generator" >> "${DEST_DIR}/README"
echo "  - ppcemu: a user level PowerPC SystemC TLM simulator." >> "${DEST_DIR}/README"
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
fi

if [ ! -e "${MAKEFILE_AM}" ]; then
	has_to_build_configure=yes
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Generating configure.ac"
	echo "AC_INIT([UNISIM PowerMAC G4 PCI Standalone simulator], [0.1], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [ppcemu])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([ppcemu])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib ppcemu" > "${MAKEFILE_AM}"

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


# ppcemu

PPCEMU_SYSTEM_CONFIGURE_AC="${DEST_DIR}/ppcemu/configure.ac"
PPCEMU_SYSTEM_MAKEFILE_AM="${DEST_DIR}/ppcemu/Makefile.am"


if [ ! -e "${PPCEMU_SYSTEM_CONFIGURE_AC}" ]; then
	has_to_build_ppcemu_configure=yes
fi

if [ ! -e "${PPCEMU_SYSTEM_MAKEFILE_AM}" ]; then
	has_to_build_ppcemu_configure=yes
fi

if [ "${has_to_build_ppcemu_configure}" = "yes" ]; then
	echo "Generating ppcemu configure.ac"
	echo "AC_INIT([UNISIM PowerMac G4 PCI C++ simulator], [0.1], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [ppcemu])" > "${PPCEMU_SYSTEM_CONFIGURE_AC}"
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
	echo "UNISIM_CHECK_SYSTEMC" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_WITH_BOOST" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_GRAPH" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\`pwd\`/../genisslib/genisslib" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${PPCEMU_SYSTEM_CONFIGURE_AC}"

	echo "Generating ppcemu Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "PPCEMU_SYSTEM_INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "PPCEMU_SYSTEM_DEBUG_INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "bin_PROGRAMS = ppcemu ppcemu-debug" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "ppcemu_SOURCES = ${UNISIM_LIB_PPCEMU_SYSTEM_SOURCE_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_SOURCE_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "ppcemu_debug_SOURCES = ${UNISIM_LIB_PPCEMU_SYSTEM_SOURCE_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_SOURCE_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "ppcemu_debug_CPPFLAGS = -DDEBUG_PPCEMU_SYSTEM" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "noinst_HEADERS = ${UNISIM_TOOLS_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_LIB_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_LIB_PPCEMU_SYSTEM_TEMPLATE_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_HEADER_FILES} ${UNISIM_SIMULATORS_PPCEMU_SYSTEM_TEMPLATE_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_PPCEMU_SYSTEM_M4_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh \$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.tcc" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh \$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.tcc" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.tcc: \$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/powerpc/powerpc.hh: ${UNISIM_LIB_PPCEMU_SYSTEM_ISA_FILES}" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	printf "\t" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"
	echo "cd \$(top_srcdir)/unisim/component/cxx/processor/powerpc; \$(GENISSLIB_PATH) -o powerpc -w 32 -I . ppc.isa" >> "${PPCEMU_SYSTEM_MAKEFILE_AM}"

	echo "Building powerpc configure"
	${SHELL} -c "cd ${DEST_DIR}/ppcemu && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
