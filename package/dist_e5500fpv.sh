#!/bin/bash

SIMPKG=e5500fpv
SIMPKG_SRCDIR=cxx/e5500fpv

source "$(dirname $0)/dist_common.sh"

import unisim/kernel/logger/console || exit
import unisim/component/cxx/processor/powerpc || exit
import unisim/component/cxx/processor/opcache || exit
import unisim/component/cxx/memory/sparse || exit
import unisim/util/reg/core || exit
import unisim/util/random || exit
import unisim/util/os/linux_os || exit
import unisim/service/interfaces/memory || exit
import unisim/service/interfaces/memory_access_reporting || exit
import unisim/service/interfaces/memory_injection || exit
import unisim/service/interfaces/registers || exit
import unisim/service/interfaces/linux_os || exit
import unisim/service/interfaces/debug_yielding || exit
import unisim/service/interfaces/disassembly || exit
import unisim/service/interfaces/trap_reporting || exit
import unisim/service/interfaces/registers || exit
import unisim/service/interfaces/blob || exit
import unisim/service/debug/debugger || exit
import unisim/service/debug/inline_debugger || exit
import unisim/service/debug/gdb_server || exit

import unisim/component/cxx/processor/powerpc/isa/book_i/branch || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/floating_point || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/legacy_move_assist || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/syscall || exit
import unisim/component/cxx/processor/powerpc/isa/book_ii || exit
import unisim/component/cxx/processor/powerpc/isa/book_iii_e || exit
import unisim/component/cxx/processor/powerpc/isa/book_e || exit

import libc/inttypes || exit
import std/iostream || exit
import std/sstream || exit
import std/string || exit
import std/vector || exit
import std/map || exit
import std/stdexcept || exit

import m4/ax_cflags_warn_all || exit

copy source isa header template data
dist_copy "${UNISIM_TOOLS_DIR}/genisslib/genisslib.py" "${DEST_DIR}/genisslib.py"
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_FILES="$(files isa)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="${UNISIM_LIB_SIMULATOR_ISA_FILES} $(files header) $(files template)"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

UNISIM_SIMULATOR_TOP_ISA="top_ppc64.isa"

UNISIM_SIMULATOR_SOURCE_FILES="\
main.cc \
linuxsystem.cc \
debugger.cc \
core.cc \
arch.cc \
simfloat.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
${UNISIM_SIMULATOR_TOP_ISA} \
linuxsystem.hh \
debugger.hh \
core.hh \
arch.hh \
simfloat.hh \
types.hh \
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
"

UNISIM_SIMULATOR_FILES=" \
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Simulator

output_simulator_configure_ac <(cat << EOF
AC_INIT([UNISIM FPV e5500 Floating-Point verification], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}-core])
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
nodist_libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = top_ppc64.cc

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_SIMULATOR_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/top_ppc64.hh\
	\$(top_builddir)/top_ppc64.cc\

CLEANFILES=\
	\$(top_builddir)/top_ppc64.hh\
	\$(top_builddir)/top_ppc64.cc\

\$(top_builddir)/top_ppc64.cc: \$(top_builddir)/top_ppc64.hh
\$(top_builddir)/top_ppc64.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES} ${UNISIM_SIMULATOR_TOP_ISA}
	\$(top_srcdir)/genisslib.py -o \$(top_builddir)/top_ppc64 -w 8 -I \$(top_srcdir) \$(top_srcdir)/${UNISIM_SIMULATOR_TOP_ISA}

EOF
)

build_simulator_configure

echo "Distribution is up-to-date"

