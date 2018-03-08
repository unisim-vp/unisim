#!/bin/bash

SIMPKG=intelemu

UNISIM_LIB_SIMULATOR_SOURCE_FILES="\
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/util/debug/symbol_table_64.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/dwarf/class.cc \
unisim/util/debug/dwarf/dwarf64.cc \
unisim/util/debug/dwarf/encoding.cc \
unisim/util/debug/dwarf/ml.cc \
unisim/util/debug/dwarf/filename.cc \
unisim/util/debug/dwarf/leb128.cc \
unisim/util/debug/dwarf/abbrev.cc \
unisim/util/debug/dwarf/dwarf32.cc \
unisim/util/debug/dwarf/register_number_mapping.cc \
unisim/util/debug/dwarf/data_object.cc \
unisim/util/debug/dwarf/c_loc_expr_parser.cc \
unisim/util/blob/section32.cc \
unisim/util/blob/blob32.cc \
unisim/util/blob/section64.cc \
unisim/util/blob/segment32.cc \
unisim/util/blob/segment64.cc \
unisim/util/blob/blob64.cc \
unisim/util/debug/stmt_32.cc \
unisim/util/debug/elf_symtab/elf_symtab32.cc \
unisim/util/debug/stmt_64.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/type.cc \
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/os/linux_os/environment.cc \
unisim/util/lexer/lexer.cc \
unisim/util/xml/xml.cc \
unisim/component/cxx/processor/intel/disasm.cc \
unisim/component/cxx/processor/intel/arch.cc \
unisim/component/cxx/processor/intel/math.cc \
"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
unisim/kernel/api/api.hh \
unisim/util/backtrace/backtrace.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/service/service.hh \
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/interfaces/linux_os.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/register.hh \
unisim/service/interfaces/stmt_lookup.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/blob/blob.hh \
unisim/util/blob/blob.tcc \
unisim/util/blob/section.hh \
unisim/util/blob/section.tcc \
unisim/util/blob/segment.hh \
unisim/util/blob/segment.tcc \
unisim/util/debug/data_object.hh \
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
unisim/util/debug/simple_register.hh \
unisim/util/debug/stmt.hh \
unisim/util/debug/stmt.tcc \
unisim/util/debug/subprogram.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/symbol.tcc \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/type.hh \
unisim/util/dictionary/dictionary.hh \
unisim/util/dictionary/dictionary.tcc \
unisim/util/endian/endian.hh \
unisim/util/inlining/inlining.hh \
unisim/util/lexer/lexer.hh \
unisim/util/lexer/lexer.tcc \
unisim/util/likely/likely.hh \
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
unisim/util/os/linux_os/linux.hh \
unisim/util/os/linux_os/linux.tcc \
unisim/util/os/linux_os/i386.hh \
unisim/util/parser/parser.hh \
unisim/util/parser/parser.tcc \
unisim/util/xml/xml.hh \
unisim/component/cxx/memory/sparse/memory.hh \
unisim/component/cxx/processor/intel/execute.hh \
unisim/component/cxx/processor/intel/modrm.hh \
unisim/component/cxx/processor/intel/arch.hh \
unisim/component/cxx/processor/intel/segments.hh \
unisim/component/cxx/processor/intel/vectorbank.hh \
unisim/component/cxx/processor/intel/math.hh \
unisim/component/cxx/processor/intel/isa/string.hh \
unisim/component/cxx/processor/intel/isa/special.hh \
unisim/component/cxx/processor/intel/isa/integer.hh \
unisim/component/cxx/processor/intel/isa/simd.hh \
unisim/component/cxx/processor/intel/isa/move.hh \
unisim/component/cxx/processor/intel/isa/branch.hh \
unisim/component/cxx/processor/intel/isa/intel.hh \
unisim/component/cxx/processor/intel/isa/intel.tcc \
unisim/component/cxx/processor/intel/isa/floatingpoint.hh \
unisim/component/cxx/processor/intel/disasm.hh \
unisim/component/cxx/processor/intel/types.hh \
unisim/component/cxx/processor/intel/tmp.hh \
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
m4/with_boost.m4 \
m4/cacti.m4 \
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4 \
m4/pthread.m4 \
"

UNISIM_LIB_SIMULATOR_DATA_FILES="\
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
linuxsystem.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
linuxsystem.hh \
"

UNISIM_SIMULATOR_TEMPLATE_FILES="\
"

UNISIM_SIMULATOR_EXTRA_FILES="\
"

UNISIM_SIMULATOR_TOP_DATA_FILES="\
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_TESTBENCH_FILES="\
"

UNISIM_DIR=$(cd $(dirname $(dirname $0)); pwd)
mkdir -p "$1"
DEST_DIR=$(cd "$1"; pwd)
UNISIM_TOOLS_DIR=${UNISIM_DIR}/unisim_tools
UNISIM_LIB_DIR=${UNISIM_DIR}/unisim_lib
UNISIM_SIMULATOR_DIR=${UNISIM_DIR}/unisim_simulators/cxx/${SIMPKG}

SIMULATOR_VERSION=$(cat ${UNISIM_SIMULATOR_DIR}/VERSION)

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

UNISIM_LIB_SIMULATOR_FILES="${UNISIM_LIB_SIMULATOR_SOURCE_FILES} ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_LIB_SIMULATOR_DATA_FILES}"

for file in ${UNISIM_LIB_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${file}"
done

UNISIM_SIMULATOR_FILES="${UNISIM_SIMULATOR_SOURCE_FILES} ${UNISIM_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_EXTRA_FILES} ${UNISIM_SIMULATOR_DATA_FILES}"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

mkdir -p ${DEST_DIR}/config

# Some imported files (m4 macros) impact configure generation
has_to_build_configure=no

for file in ${UNISIM_LIB_SIMULATOR_M4_FILES}; do
	dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${file}" && has_to_build_configure=yes
done

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

# Simulator

SIMULATOR_CONFIGURE_AC="${DEST_DIR}/configure.ac"
SIMULATOR_MAKEFILE_AM="${DEST_DIR}/Makefile.am"

if has_to_build "${SIMULATOR_CONFIGURE_AC}" "$0"; then
	echo "Generating ${SIMPKG} configure.ac"
	cat <<EOF > "${SIMULATOR_CONFIGURE_AC}"
AC_INIT([UNISIM INTELemu C++ simulator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-intelemu])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects tar-pax])
AC_PATH_PROGS(SH, sh)
CXXFLAGS="\$CXXFLAGS -std=c++11"
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
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/unisim-${SIMPKG}-${SIMULATOR_VERSION}"], [path of shared data relative to bin directory])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF
	has_to_build_configure=yes
fi

if has_to_build "${SIMULATOR_MAKEFILE_AM}" "$0"; then
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

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES}

EOF
	has_to_build_configure=yes
fi


if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Building ${SIMPKG} configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal -I m4 && libtoolize --force && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
