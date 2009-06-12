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
UNISIM_SIMULATORS_DIR=$4/tlm2/s12x

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

UNISIM_LIB_HCS12X_SOURCE_FILES="\
unisim/kernel/service/service.cc \
unisim/kernel/service/xml_helper.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/kernel/debug/debug.cc \
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
unisim/util/endian/endian.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/inline_debugger/inline_debugger_64.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server_64.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/logger/logger_server.cc \
unisim/service/logger/logger.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/loader/elf_loader/elf64_loader.cc \
unisim/service/loader/s19_loader/s19_loader.cc \
unisim/service/time/host_time/time.cc \
unisim/service/time/sc_time/time.cc \
unisim/component/cxx/processor/hcs12x/ccr.cc \
unisim/component/cxx/processor/hcs12x/cpu.cc \
unisim/component/cxx/processor/hcs12x/exception.cc \
unisim/component/cxx/processor/hcs12x/mmc.cc \
unisim/component/tlm2/processor/hcs12x/hcs12x.cc \
unisim/component/tlm2/processor/hcs12x/xint.cc \
unisim/component/tlm2/processor/hcs12x/s12xmmc.cc \
unisim/component/tlm2/processor/hcs12x/pwm.cc \
unisim/component/tlm2/processor/hcs12x/atd10b.cc \
unisim/component/tlm2/processor/hcs12x/crg.cc \
unisim/component/tlm2/processor/hcs12x/ect.cc \
unisim/component/tlm2/processor/hcs12x/tlm_types.cc \
unisim/component/tlm2/interconnect/generic_router/router.cc \
unisim/component/tlm2/interconnect/generic_router/variable_mapping.cc \
unisim/component/tlm2/memory/ram/memory.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc"


UNISIM_LIB_HCS12X_ISA_FILES="\
unisim/component/cxx/processor/hcs12x/addition_substraction.isa \
unisim/component/cxx/processor/hcs12x/bit.isa \
unisim/component/cxx/processor/hcs12x/boolean-logic.isa \
unisim/component/cxx/processor/hcs12x/branch.isa \
unisim/component/cxx/processor/hcs12x/compare_test.isa \
unisim/component/cxx/processor/hcs12x/decrement_increment.isa \
unisim/component/cxx/processor/hcs12x/fuzzy-logic.isa \
unisim/component/cxx/processor/hcs12x/hcs12x.isa \
unisim/component/cxx/processor/hcs12x/load_store.isa \
unisim/component/cxx/processor/hcs12x/move.isa \
unisim/component/cxx/processor/hcs12x/multiplication_division.isa \
unisim/component/cxx/processor/hcs12x/others.isa \
unisim/component/cxx/processor/hcs12x/stacking.isa \
unisim/component/cxx/processor/hcs12x/transfer_exchange.isa"

UNISIM_LIB_XB_ISA_FILES="\
unisim/component/cxx/processor/hcs12x/xb.isa"


UNISIM_LIB_HCS12X_HEADER_FILES="${UNISIM_LIB_HCS12X_ISA_FILES} ${UNISIM_LIB_XB_ISA_FILES} \
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/debug/debug.hh \
unisim/kernel/tlm2/tlm.hh \
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
unisim/service/interfaces/debug_control.hh \
unisim/service/interfaces/memory_access_reporting.hh \
unisim/service/interfaces/ti_c_io.hh \
unisim/service/interfaces/disassembly.hh \
unisim/service/interfaces/loader.hh \
unisim/service/interfaces/logger.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/symbol_table_lookup.hh \
unisim/service/interfaces/time.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/logger/logger_server.hh \
unisim/service/logger/logger.hh \
unisim/service/logger/logger_server_interface.hh \
unisim/service/loader/elf_loader/elf_common.h \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32.h \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/elf_loader/elf64.h \
unisim/service/loader/elf_loader/elf64_loader.hh \
unisim/service/loader/s19_loader/s19_loader.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/processor/hcs12x/cpu.hh \
unisim/component/cxx/processor/hcs12x/mmc.hh \
unisim/component/cxx/processor/hcs12x/ccr.hh \
unisim/component/cxx/processor/hcs12x/exception.hh \
unisim/component/cxx/processor/hcs12x/types.hh \
unisim/component/cxx/processor/hcs12x/concatenated_register.hh \
unisim/component/cxx/processor/hcs12x/config.hh \
unisim/component/tlm2/memory/ram/memory.hh \
unisim/component/tlm2/interconnect/generic_router/config.hh \
unisim/component/tlm2/interconnect/generic_router/router.hh \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.hh \
unisim/component/tlm2/processor/hcs12x/hcs12x.hh \
unisim/component/tlm2/processor/hcs12x/xint.hh \
unisim/component/tlm2/processor/hcs12x/tlm_types.hh \
unisim/component/tlm2/processor/hcs12x/s12xmmc.hh \
unisim/component/tlm2/processor/hcs12x/pwm.hh \
unisim/component/tlm2/processor/hcs12x/atd10b.hh \
unisim/component/tlm2/processor/hcs12x/crg.hh \
unisim/component/tlm2/processor/hcs12x/ect.hh \
"

UNISIM_LIB_HCS12X_TEMPLATE_FILES="\
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/profile.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/loader/s19_loader/s19_loader.tcc \
unisim/component/tlm2/interconnect/generic_router/router.tcc \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.tcc \
unisim/component/cxx/memory/ram/memory.tcc \
unisim/component/tlm2/memory/ram/memory.tcc \
unisim/component/tlm2/processor/hcs12x/pwm.tcc \
unisim/component/tlm2/processor/hcs12x/atd10b.tcc"

UNISIM_LIB_HCS12X_M4_FILES="\
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/boost_graph.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/systemc.m4 \
m4/tlm20.m4 \
m4/with_boost.m4"

HCS12X_EXTERNAL_HEADERS="\
assert.h \
errno.h \
fcntl.h \
fstream \
getopt.h \
inttypes.h \
iosfwd \
iostream \
list \
map \
ostream \
queue \
signal.h \
sstream \
stdarg.h \
stdexcept \
stdio.h \
stdlib.h \
string \
string.h \
sys/stat.h \
sys/types.h \
time.h \
unistd.h \
vector \
"

UNISIM_SIMULATORS_HCS12X_SOURCE_FILES="\
main.cc \
rtb_unisim.cc \
"
UNISIM_SIMULATORS_HCS12X_HEADER_FILES="\
rtb_unisim.hh \
"

UNISIM_SIMULATORS_HCS12X_TEMPLATE_FILES=
UNISIM_SIMULATORS_HCS12X_DATA_FILES="COPYING INSTALL NEWS README AUTHORS ChangeLog"

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_hcs12x_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/hcs12x

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

UNISIM_LIB_HCS12X_FILES="${UNISIM_LIB_HCS12X_SOURCE_FILES} ${UNISIM_LIB_HCS12X_HEADER_FILES} ${UNISIM_LIB_HCS12X_TEMPLATE_FILES}"

for file in ${UNISIM_LIB_HCS12X_FILES}; do
	mkdir -p "${DEST_DIR}/hcs12x/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/hcs12x/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/hcs12x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/hcs12x/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/hcs12x/${file}" || exit
	fi
done

UNISIM_SIMULATORS_HCS12X_FILES="${UNISIM_SIMULATORS_HCS12X_SOURCE_FILES} ${UNISIM_SIMULATORS_HCS12X_HEADER_FILES} ${UNISIM_SIMULATORS_HCS12X_TEMPLATE_FILES} ${UNISIM_SIMULATORS_HCS12X_DATA_FILES}"

for file in ${UNISIM_SIMULATORS_HCS12X_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/hcs12x/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/hcs12x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/hcs12x/${file}"
		cp -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/hcs12x/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_HCS12X_DATA_FILES}; do
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
mkdir -p ${DEST_DIR}/hcs12x/config
mkdir -p ${DEST_DIR}/hcs12x/m4
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

for file in ${UNISIM_LIB_HCS12X_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/hcs12x/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/hcs12x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/hcs12x/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/hcs12x/${file}" || exit
		has_to_build_hcs12x_configure=yes
	fi
done

# Top level

echo "This package contains GenISSLib, an instruction set simulator generator, and a hcs12x instruction set simulator." > "${DEST_DIR}/README"
echo "See INSTALL for installation instructions." >> "${DEST_DIR}/README"

echo "INSTALLATION" > "${DEST_DIR}/INSTALL"
echo "------------" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Requirements:" >> "${DEST_DIR}/INSTALL"
echo "  - GNU bash (bash-3.0-19.3 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU make (make-3.80-6.EL4 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU autoconf (autoconf-2.59-5 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU automake (automake-1.9.2-3 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU flex (flex-2.5.4a-33 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU bison (bison-1.875c-2 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - boost development package (boost-devel-1.32.0-1.rhel4 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - libxml2 development package (libxml2-devel-2.6.16-6 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - zlib development package (zlib-devel-1.2.1.2-1.2 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Building instructions:" >> "${DEST_DIR}/INSTALL"
echo "  $ ./configure" >> "${DEST_DIR}/INSTALL"
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
	echo "AC_INIT([UNISIM hcs12x Standalone simulator], [0.1], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [hcs12x])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([hcs12x])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib hcs12x" > "${MAKEFILE_AM}"

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


# hcs12x

HCS12X_CONFIGURE_AC="${DEST_DIR}/hcs12x/configure.ac"
HCS12X_MAKEFILE_AM="${DEST_DIR}/hcs12x/Makefile.am"


if [ ! -e "${HCS12X_CONFIGURE_AC}" ]; then
	has_to_build_hcs12x_configure=yes
fi

if [ ! -e "${HCS12X_MAKEFILE_AM}" ]; then
	has_to_build_hcs12x_configure=yes
fi

if [ "${has_to_build_hcs12x_configure}" = "yes" ]; then
	echo "Generating hcs12x configure.ac"
	echo "AC_INIT([UNISIM hcs12x C++ simulator], [0.1], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [hcs12x_cxx])" > "${HCS12X_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${HCS12X_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects])" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${HCS12X_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${HCS12X_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${HCS12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES" >> "${HCS12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT" >> "${HCS12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS" >> "${HCS12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB" >> "${HCS12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2" >> "${HCS12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI" >> "${HCS12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${HCS12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TLM20" >> "${HCS12X_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\`pwd\`/../genisslib/genisslib" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${HCS12X_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${HCS12X_CONFIGURE_AC}"

	echo "Generating hcs12x Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${HCS12X_MAKEFILE_AM}"
	echo "HCS12X_INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${HCS12X_MAKEFILE_AM}"
	echo "bin_PROGRAMS = hcs12x" >> "${HCS12X_MAKEFILE_AM}"
	echo "hcs12x_SOURCES = ${UNISIM_LIB_HCS12X_SOURCE_FILES} ${UNISIM_SIMULATORS_HCS12X_SOURCE_FILES} unisim/component/cxx/processor/hcs12x/xb.cc unisim/component/cxx/processor/hcs12x/hcs12x.cc" >> "${HCS12X_MAKEFILE_AM}"
#	echo "hcs12x_SOURCES = ${UNISIM_LIB_HCS12X_SOURCE_FILES} ${UNISIM_SIMULATORS_HCS12X_SOURCE_FILES}" >> "${HCS12X_MAKEFILE_AM}"
	echo "noinst_HEADERS = ${UNISIM_TOOLS_HCS12X_HEADER_FILES} ${UNISIM_LIB_HCS12X_HEADER_FILES} ${UNISIM_LIB_HCS12X_TEMPLATE_FILES} ${UNISIM_SIMULATORS_HCS12X_HEADER_FILES} ${UNISIM_SIMULATORS_HCS12X_TEMPLATE_FILES}" >> "${HCS12X_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_HCS12X_M4_FILES}" >> "${HCS12X_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.cc \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.hh \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.cc \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa" >> "${HCS12X_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.cc \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.hh \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.cc \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa" >> "${HCS12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.cc: \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.hh" >> "${HCS12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa: \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.hh" >> "${HCS12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.hh: \${UNISIM_LIB_XB_ISA_FILES}" >> "${HCS12X_MAKEFILE_AM}"
	printf "\t" >> "${HCS12X_MAKEFILE_AM}"
	echo "cd \$(top_srcdir)/unisim/component/cxx/processor/hcs12x; \$(GENISSLIB_PATH) -o xb -w 32 -I . xb.isa" >> "${HCS12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.cc: \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh" >> "${HCS12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh: \${UNISIM_LIB_HCS12X_ISA_FILES} \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa" >> "${HCS12X_MAKEFILE_AM}"
	printf "\t" >> "${HCS12X_MAKEFILE_AM}"
	echo "cd \$(top_srcdir)/unisim/component/cxx/processor/hcs12x; \$(GENISSLIB_PATH) -o hcs12x -w 32 -I . hcs12x.isa" >> "${HCS12X_MAKEFILE_AM}"

	echo "Building hcs12x configure"
	${SHELL} -c "cd ${DEST_DIR}/hcs12x && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
