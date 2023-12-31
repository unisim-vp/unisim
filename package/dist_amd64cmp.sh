#!/bin/bash

SIMPKG=amd64cmp
SIMPKG_SRCDIR=cxx/amd64cmp
source "$(dirname $0)/dist_common.sh"

import unisim/service/interfaces/linux_os || exit
import unisim/service/interfaces/memory_injection || exit
import unisim/service/interfaces/memory || exit
import unisim/service/interfaces/registers || exit
import unisim/kernel/config/json || exit
import unisim/kernel/logger/console || exit
import unisim/component/cxx/processor/intel || exit
import unisim/component/cxx/processor/intel/isa || exit
import unisim/component/cxx/memory/sparse || exit
import unisim/component/cxx/vector || exit
import unisim/util/os/linux_os || exit
import unisim/util/identifier || exit
import unisim/util/loader/elf_loader || exit

import libc/inttypes || exit
import std/algorithm || exit
import std/cctype || exit
import std/cmath || exit
import std/fstream || exit
import std/iomanip || exit
import std/iostream || exit
import std/sstream || exit
import std/string || exit
import std/map || exit
import std/vector || exit

import m4/pthread || exit
import m4/endian || exit
import m4/ax_cflags_warn_all || exit

copy source header template data
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="$(files header) $(files template)"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

UNISIM_SIMULATOR_SOURCE_FILES="\
arch.cc \
isa.cc \
linuxsystem.cc \
tracee.cc \
main.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
arch.hh \
linuxsystem.hh \
tracee.hh \
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Yves Lhuillier <yves.lhuillier@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - amd64emu: an amd64/intel64 application level simulator (with LinuxOS emulation)

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

output_simulator_configure_ac <(cat <<EOF
AC_INIT([UNISIM AMD64CMP C++ simulator validator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}])
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
case "\${host}" in
	*mingw*)
		CPPFLAGS="-U__STRICT_ANSI__ \${CPPFLAGS}"
		;;
	*)
		;;
esac
$(lines ac)
AX_CXXFLAGS_WARN_ALL
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/unisim-${SIMPKG}-${SIMULATOR_VERSION}"], [path of shared data relative to bin directory])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF
)

output_simulator_makefile_am <(cat << EOF
ACLOCAL_AMFLAGS=-I m4
AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: \$(LIBTOOL_DEPS)
	\$(SHELL) ./config.status libtool

# Program
bin_PROGRAMS = unisim-${SIMPKG}-${SIMULATOR_VERSION}
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_SIMULATOR_SOURCE_FILES}
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDFLAGS = -static-libtool-libs
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la

# Static Library
noinst_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES}
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -static

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_SIMULATOR_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES}

EOF
)

build_simulator_configure

echo "Distribution is up-to-date"

