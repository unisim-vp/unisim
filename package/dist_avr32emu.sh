#!/bin/bash

SIMPKG=avr32emu
SIMPKG_SRCDIR=tlm2/avr32emu

source "$(dirname $0)/dist_common.sh"

import_genisslib || exit

import unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa || exit
import unisim/component/tlm2/processor/avr32/avr32uc || exit
import unisim/component/tlm2/memory/ram || exit
import unisim/component/tlm2/interconnect/generic_router || exit
import unisim/kernel || exit
import unisim/kernel/config/xml || exit
import unisim/kernel/config/ini || exit
import unisim/kernel/config/json || exit
import unisim/kernel/logger/console || exit
import unisim/kernel/logger/text_file || exit
import unisim/kernel/logger/http || exit
import unisim/kernel/logger/xml_file || exit
import unisim/kernel/logger/netstream || exit
import unisim/service/debug/debugger || exit
import unisim/service/debug/gdb_server || exit
import unisim/service/debug/inline_debugger || exit
import unisim/service/loader/multiformat_loader || exit
import unisim/service/os/avr32_t2h_syscalls || exit
import unisim/service/time/sc_time || exit
import unisim/service/time/host_time || exit
import unisim/service/instrumenter || exit
import unisim/service/debug/profiler || exit
import unisim/service/http_server || exit
import unisim/kernel/logger || exit
import unisim/kernel/tlm2 || exit

import std/iostream || exit
import std/sstream || exit
import std/string || exit
import std/stdexcept || exit

import m4/ax_cflags_warn_all || exit

copy source isa header template data
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_FILES="$(files isa)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="${UNISIM_LIB_SIMULATOR_ISA_FILES} $(files header) $(files template)"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

UNISIM_SIMULATOR_SOURCE_FILES="\
config.cc \
memory_router.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
simulator.tcc \
simulator.hh \
config.hh \
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
unisim.ico"

UNISIM_SIMULATOR_TESTBENCH_FILES="\
main.cc \
"

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
${UNISIM_SIMULATOR_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Simulator

output_simulator_configure_ac <(cat << EOF
AC_INIT([UNISIM AVR32EMU Standalone simulator], [${SIMULATOR_VERSION}], [Julien Lisita <julien.lisita@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>], [unisim-${SIMPKG}-core])
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
PKG_PROG_PKG_CONFIG([0.26])
AC_LANG([C++])
AM_PROG_CC_C_O
CPPFLAGS="\${CPPFLAGS} -D_LARGEFILE64_SOURCE"
case "\${host}" in
	*mingw*)
		CPPFLAGS="-U__STRICT_ANSI__ \${CPPFLAGS}"
		;;
	*)
		;;
esac
$(lines ac)
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/unisim-avr32emu-${SIMULATOR_VERSION}"], [path of shared data relative to bin directory])
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
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_SIMULATOR_TESTBENCH_FILES} ${UNISIM_SIMULATOR_SOURCE_FILES}
#unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDFLAGS = -static-libtool-libs
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la

# Static Library
noinst_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES}
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -static

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_SIMULATOR_PKG_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.hh\
	\$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.tcc

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.hh\
	\$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.tcc
	
\$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.tcc: \$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.hh
\$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(PYTHON_BIN) \$(top_srcdir)/genisslib.py -o \$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa/avr32uc.isa
EOF
)

build_simulator_configure

echo "Distribution is up-to-date"

