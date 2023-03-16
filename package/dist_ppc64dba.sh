#!/bin/bash

SIMPKG=ppc64dba
SIMPKG_SRCDIR=cxx/ppc64dba

source "$(dirname $0)/dist_common.sh"

import_genisslib || exit

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
import unisim/component/cxx/processor/powerpc/isa || exit
import unisim/util/symbolic/binsec || exit
import unisim/util/symbolic/vector || exit
import unisim/util/symbolic || exit
import unisim/util/arithmetic || exit
import unisim/util/likely || exit

import std/iomanip || exit
import std/iosfwd || exit
import std/iostream || exit
import std/set || exit

import m4/ax_cflags_warn_all || exit

copy source isa header template data
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_FILES="$(files isa)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
${UNISIM_LIB_SIMULATOR_ISA_FILES} \
$(files header) \
$(files template) \
"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

UNISIM_SIMULATOR_TOP_ISA="ppc64dec.isa"

UNISIM_SIMULATOR_SOURCE_FILES="\
arch.cc \
decoder.cc \
main.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
${UNISIM_SIMULATOR_TOP_ISA} \
arch.hh \
decoder.hh \
"

UNISIM_SIMULATOR_EXTRA_FILES="\
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
README \
INSTALL \
AUTHORS \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_EXTRA_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Simulator

output_simulator_configure_ac <(cat << EOF
AC_INIT([UNISIM Ppc64dba PPC64 to BINSEC-DBA translator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}-core])
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
nodist_libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ppc64dec.cc

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_SIMULATOR_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/ppc64dec.hh\
	\$(top_builddir)/ppc64dec.cc

CLEANFILES=\
	\$(top_builddir)/ppc64dec.hh\
	\$(top_builddir)/ppc64dec.cc

\$(top_builddir)/ppc64dec.cc: \$(top_builddir)/ppc64dec.hh
\$(top_builddir)/ppc64dec.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES} ${UNISIM_SIMULATOR_TOP_ISA}
	\$(PYTHON_BIN) \$(top_srcdir)/genisslib.py -o \$(top_builddir)/ppc64dec -w 8 -I \$(top_srcdir) \$(top_srcdir)/${UNISIM_SIMULATOR_TOP_ISA}
	 
EOF
)

build_simulator_configure

echo "Distribution is up-to-date"

