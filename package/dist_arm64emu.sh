#!/bin/bash
source "$(dirname $0)/dist_common.sh"

SIMPKG=arm64emu

import unisim/component/tlm2/processor/arm/cortex_a53 || exit
import unisim/component/tlm2/memory/ram || exit
import unisim/util/likely || exit
import unisim/service/time/sc_time || exit
import unisim/service/time/host_time || exit
import unisim/service/os/linux_os || exit
import unisim/service/trap_handler || exit
import unisim/service/debug/gdb_server || exit
import unisim/service/debug/inline_debugger || exit
import unisim/service/debug/debugger || exit
import unisim/service/debug/profiler || exit
import unisim/service/http_server || exit
import unisim/service/instrumenter || exit

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_FILES="$(files isa)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
${UNISIM_LIB_SIMULATOR_ISA_FILES} \
$(files header) \
$(files template)"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

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

UNISIM_SIMULATOR_PKG_DATA_FILES="\
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
mkdir -p "$1"
DEST_DIR=$(cd "$1"; pwd)

UNISIM_LIB_DIR=${UNISIM_DIR}/unisim_lib
UNISIM_SIMULATOR_DIR=${UNISIM_DIR}/unisim_simulators/tlm2/${SIMPKG}

SIMULATOR_VERSION=$(cat ${UNISIM_SIMULATOR_DIR}/VERSION)

GILINSTALL=noinst ${UNISIM_DIR}/package/dist_genisslib.sh ${DEST_DIR}/genisslib

mkdir -p ${DEST_DIR}/${SIMPKG}

UNISIM_LIB_SIMULATOR_FILES="${UNISIM_LIB_SIMULATOR_SOURCE_FILES} ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_LIB_SIMULATOR_DATA_FILES}"

for file in ${UNISIM_LIB_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

UNISIM_SIMULATOR_FILES="${UNISIM_SIMULATOR_SOURCE_FILES} ${UNISIM_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_DATA_FILES}"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

for file in ${UNISIM_SIMULATOR_PKG_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/${SIMPKG}/config
mkdir -p ${DEST_DIR}/${SIMPKG}/m4

# Some imported files (m4 macros) impact configure generation
has_to_build_simulator_configure=no
pkg_deps_changed "${CONFIGURE_AC}" && pkg_deps_changed "${MAKEFILE_AM}" && has_to_build_configure=yes

for file in ${UNISIM_LIB_SIMULATOR_M4_FILES}; do
	dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}" && has_to_build_simulator_configure=yes
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Yves Lhuillier <yves.lhuillier@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - ARM64emu: an ARMv8 application level simulator (LinuxOS emulation)
  - UniSIM GenISSLib (will not be installed): an instruction set simulator generator

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

has_to_build_configure=no

if has_to_build "${CONFIGURE_AC}" "$0"; then
	echo "Generating configure.ac"
	cat <<EOF > "${CONFIGURE_AC}"
AC_INIT([UniSIM ARM64emu Standalone simulator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}])
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
	has_to_build_configure=yes
fi

if has_to_build "${MAKEFILE_AM}" "$0"; then
	echo "Generating Makefile.am"
	cat <<EOF > "${MAKEFILE_AM}"
SUBDIRS=genisslib ${SIMPKG}
EXTRA_DIST = configure.cross
EOF
	has_to_build_configure=yes
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Building top configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

if has_to_build "${CONFIGURE_CROSS}" "$0"; then
	echo "Building configure.cross"
	make_pkg_configure_cross "${SIMPKG}" "${CONFIGURE_CROSS}"
fi  # has to build configure cross

# Simulator

SIMULATOR_CONFIGURE_AC="${DEST_DIR}/${SIMPKG}/configure.ac"
SIMULATOR_MAKEFILE_AM="${DEST_DIR}/${SIMPKG}/Makefile.am"

if has_to_build "${SIMULATOR_CONFIGURE_AC}" "$0"; then
	echo "Generating ${SIMPKG} configure.ac"
	cat <<EOF > "${SIMULATOR_CONFIGURE_AC}"
AC_INIT([UNISIM ARM64emu C++ simulator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}-core])
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
$(lines ac)
GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib
AC_SUBST(GENISSLIB_PATH)
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/unisim-${SIMPKG}-${SIMULATOR_VERSION}"], [path of shared data relative to bin directory])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF
	has_to_build_simulator_configure=yes
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
dist_share_DATA = ${UNISIM_SIMULATOR_PKG_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES}

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
	has_to_build_simulator_configure=yes
fi

if [ "${has_to_build_simulator_configure}" = "yes" ]; then
	echo "Building ${SIMPKG} configure"
	${SHELL} -c "cd ${DEST_DIR}/${SIMPKG} && aclocal -I m4 && libtoolize --force && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"

