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
UNISIM_SIMULATORS_DIR=$4/cxx/tms320c3x

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
dlfcn.h \
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

UNISIM_LIB_TMS320C3X_SOURCE_FILES="\
unisim/kernel/service/service.cc \
unisim/kernel/service/xml_helper.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/kernel/debug/debug.cc \
unisim/util/xml/xml.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/symbol_table_64.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/watchpoint_registry_64.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/debug/breakpoint_registry_64.cc \
unisim/util/endian/endian.cc \
unisim/service/loader/coff_loader/coff_loader.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/inline_debugger/inline_debugger_64.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server_64.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/os/ti_c_io/ti_c_io_32.cc \
unisim/service/os/ti_c_io/ti_c_io_64.cc \
unisim/service/time/host_time/time.cc \
unisim/component/cxx/processor/tms320/cpu.cc \
unisim/component/cxx/processor/tms320/cpu_tms320vc33.cc \
unisim/component/cxx/processor/tms320/cpu_tms320vc33_debug.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc"

UNISIM_LIB_TMS320C3X_ISA_FILES="\
unisim/component/cxx/processor/tms320/isa/2op.isa \
unisim/component/cxx/processor/tms320/isa/3op.isa \
unisim/component/cxx/processor/tms320/isa/actions_dec.isa \
unisim/component/cxx/processor/tms320/isa/control.isa \
unisim/component/cxx/processor/tms320/isa/interlock.isa \
unisim/component/cxx/processor/tms320/isa/load_store.isa \
unisim/component/cxx/processor/tms320/isa/parallel.isa \
unisim/component/cxx/processor/tms320/isa/power.isa \
unisim/component/cxx/processor/tms320/isa/tms320.isa"

UNISIM_LIB_TMS320C3X_HEADER_FILES="${UNISIM_LIB_TMS320C3X_ISA_FILES} \
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/debug/debug.hh \
unisim/util/xml/xml.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/register.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/endian/endian.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/hash_table/hash_table.hh \
unisim/service/interfaces/debug_control.hh \
unisim/service/interfaces/memory_access_reporting.hh \
unisim/service/interfaces/ti_c_io.hh \
unisim/service/interfaces/disassembly.hh \
unisim/service/interfaces/loader.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/symbol_table_lookup.hh \
unisim/service/interfaces/time.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/loader/coff_loader/coff_loader.hh \
unisim/service/loader/coff_loader/ti/ti.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/os/ti_c_io/ti_c_io.hh \
unisim/service/time/host_time/time.hh \
unisim/component/cxx/processor/tms320/config.hh \
unisim/component/cxx/processor/tms320/cpu.hh \
unisim/component/cxx/processor/tms320/exception.hh \
unisim/component/cxx/memory/ram/memory.hh"


UNISIM_LIB_TMS320C3X_TEMPLATE_FILES="\
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/service/loader/coff_loader/coff_loader.tcc \
unisim/service/loader/coff_loader/ti/ti.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/os/ti_c_io/ti_c_io.tcc \
unisim/component/cxx/processor/tms320/cpu.tcc \
unisim/component/cxx/processor/tms320/exception.tcc \
unisim/component/cxx/memory/ram/memory.tcc"

UNISIM_LIB_TMS320C3X_M4_FILES="\
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/boost_graph.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/with_boost.m4"

TMS320C3X_EXTERNAL_HEADERS="\
assert.h \
endian.h \
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
sys/times.h \
sys/types.h \
time.h \
unistd.h \
vector \
"

UNISIM_SIMULATORS_TMS320C3X_SOURCE_FILES=main.cc
UNISIM_SIMULATORS_TMS320C3X_HEADER_FILES=
UNISIM_SIMULATORS_TMS320C3X_TEMPLATE_FILES=
UNISIM_SIMULATORS_TMS320C3X_DATA_FILES="COPYING INSTALL NEWS README AUTHORS ChangeLog"

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_tms320c3x_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/tms320c3x

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

UNISIM_LIB_TMS320C3X_FILES="${UNISIM_LIB_TMS320C3X_SOURCE_FILES} ${UNISIM_LIB_TMS320C3X_HEADER_FILES} ${UNISIM_LIB_TMS320C3X_TEMPLATE_FILES}"

for file in ${UNISIM_LIB_TMS320C3X_FILES}; do
	mkdir -p "${DEST_DIR}/tms320c3x/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/tms320c3x/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/tms320c3x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/tms320c3x/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/tms320c3x/${file}" || exit
	fi
done

UNISIM_SIMULATORS_TMS320C3X_FILES="${UNISIM_SIMULATORS_TMS320C3X_SOURCE_FILES} ${UNISIM_SIMULATORS_TMS320C3X_HEADER_FILES} ${UNISIM_SIMULATORS_TMS320C3X_TEMPLATE_FILES} ${UNISIM_SIMULATORS_TMS320C3X_DATA_FILES}"

for file in ${UNISIM_SIMULATORS_TMS320C3X_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/tms320c3x/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/tms320c3x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/tms320c3x/${file}"
		cp -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/tms320c3x/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_TMS320C3X_DATA_FILES}; do
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
mkdir -p ${DEST_DIR}/tms320c3x/config
mkdir -p ${DEST_DIR}/tms320c3x/m4
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

for file in ${UNISIM_LIB_TMS320C3X_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/tms320c3x/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/tms320c3x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/tms320c3x/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/tms320c3x/${file}" || exit
		has_to_build_tms320c3x_configure=yes
	fi
done

# Top level

echo "This package contains GenISSLib, an instruction set simulator generator, and a TMS320C3X instruction set simulator." > "${DEST_DIR}/README"
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
	echo "AC_INIT([UNISIM TMS320C3X Standalone simulator], [0.1], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>], [tms320c3x])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([tms320c3x])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib tms320c3x" > "${MAKEFILE_AM}"

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


# TMS320C3X

TMS320C3X_CONFIGURE_AC="${DEST_DIR}/tms320c3x/configure.ac"
TMS320C3X_MAKEFILE_AM="${DEST_DIR}/tms320c3x/Makefile.am"


if [ ! -e "${TMS320C3X_CONFIGURE_AC}" ]; then
	has_to_build_tms320c3x_configure=yes
fi

if [ ! -e "${TMS320C3X_MAKEFILE_AM}" ]; then
	has_to_build_tms320c3x_configure=yes
fi

if [ "${has_to_build_tms320c3x_configure}" = "yes" ]; then
	echo "Generating TMS320C3X configure.ac"
	echo "AC_INIT([UNISIM TMS320C3X C++ simulator], [0.1], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>], [tms320c3x_cxx])" > "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${TMS320C3X_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\`pwd\`/../genisslib/genisslib" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${TMS320C3X_CONFIGURE_AC}"

	echo "Generating TMS320C3X Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${TMS320C3X_MAKEFILE_AM}"
	echo "tms320c3x_INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "bin_PROGRAMS = tms320c3x" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "tms320c3x_SOURCES = ${UNISIM_LIB_TMS320C3X_SOURCE_FILES} ${UNISIM_SIMULATORS_TMS320C3X_SOURCE_FILES}" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "noinst_HEADERS = ${UNISIM_TOOLS_TMS320C3X_HEADER_FILES} ${UNISIM_LIB_TMS320C3X_HEADER_FILES} ${UNISIM_LIB_TMS320C3X_TEMPLATE_FILES} ${UNISIM_SIMULATORS_TMS320C3X_HEADER_FILES} ${UNISIM_SIMULATORS_TMS320C3X_TEMPLATE_FILES}" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_TMS320C3X_M4_FILES}" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "BUILT_SOURCES=\$(top_srcdir)/unisim/component/cxx/processor/tms320/isa_tms320.hh \$(top_srcdir)/unisim/component/cxx/processor/tms320/isa_tms320.tcc" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_srcdir)/unisim/component/cxx/processor/tms320/isa_tms320.hh \$(top_srcdir)/unisim/component/cxx/processor/tms320/isa_tms320.tcc" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/tms320/isa_tms320.tcc: \$(top_srcdir)/unisim/component/cxx/processor/tms320/isa_tms320.hh" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/tms320/isa_tms320.hh: ${UNISIM_LIB_TMS320C3X_ISA_FILES}" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\t" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "cd \$(top_srcdir)/unisim/component/cxx/processor/tms320; \$(GENISSLIB_PATH) -o isa_tms320 -w 32 -I isa isa/tms320.isa" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "Building TMS320C3X configure"
	${SHELL} -c "cd ${DEST_DIR}/tms320c3x && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
