#!/bin/bash

SIMPKG=risc16
SIMPKG_SRCDIR=tlm2/risc16

source "$(dirname $0)/dist_common.sh"

import_genisslib || exit

import unisim/kernel || exit
import unisim/kernel/config/xml || exit
import unisim/kernel/config/ini || exit
import unisim/kernel/config/json || exit
import unisim/kernel/logger/console || exit
import unisim/kernel/logger/text_file || exit
import unisim/kernel/logger/xml_file || exit
import unisim/util/debug || exit
import unisim/util/blob || exit
import unisim/util/loader/elf_loader || exit
import unisim/service/debug/debugger || exit
import unisim/service/debug/inline_debugger || exit
import unisim/service/loader/raw_loader || exit
import unisim/service/time/sc_time || exit
import unisim/service/time/host_time || exit
import unisim/component/tlm2/processor/risc16 || exit
import unisim/component/tlm2/memory/ram || exit

import libc/stdint || exit
import std/iostream || exit
import std/stdexcept || exit

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

UNISIM_SIMULATOR_SOURCE_FILES="\
main.cc \
simulator.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
simulator.hh \
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
unisim.ico \
"

UNISIM_SIMULATOR_CONFIG_FILES=""

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
${UNISIM_SIMULATOR_CONFIG_FILES} \
"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Simulator

output_simulator_configure_ac <(cat << EOF
AC_INIT([UNISIM risc16 C++ simulator], [${SIMULATOR_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Réda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}-core])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects tar-pax])
AC_PATH_PROGS(SH, sh)
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL
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
noinst_LIBRARIES = lib${SIMPKG}-${SIMULATOR_VERSION}.a
nodist_lib${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_a_SOURCES = unisim/component/cxx/processor/risc16/isa.cc
lib${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_a_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES} 
bin_PROGRAMS = unisim-${SIMPKG}-${SIMULATOR_VERSION}
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_SIMULATOR_SOURCE_FILES}
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = lib${SIMPKG}-${SIMULATOR_VERSION}.a
noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_CONFIG_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/risc16/isa.hh\
	\$(top_builddir)/unisim/component/cxx/processor/risc16/isa.cc

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/risc16/isa.hh\
	\$(top_buildir)/unisim/component/cxx/processor/risc16/isa.cc

\$(top_builddir)/unisim/component/cxx/processor/risc16/isa.cc: \$(top_builddir)/unisim/component/cxx/processor/risc16/isa.hh
\$(top_builddir)/unisim/component/cxx/processor/risc16/isa.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(PYTHON_BIN) \$(top_srcdir)/genisslib.py -o \$(top_builddir)/unisim/component/cxx/processor/risc16/isa -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/risc16/risc16.isa
EOF
)

build_simulator_configure

echo "Distribution is up-to-date"

