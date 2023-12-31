#!/bin/bash
SIMPKG=ppcemu
SIMPKG_SRCDIR=tlm2/ppcemu
source "$(dirname $0)/dist_common.sh"

import_genisslib || exit

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
import unisim/service/os/linux_os || exit
import unisim/service/time/sc_time || exit
import unisim/service/time/host_time || exit
import unisim/component/tlm2/processor/powerpc/e600/mpc7447a || exit
import unisim/component/tlm2/memory/ram || exit

import std/stdexcept || exit
import std/iostream || exit
import std/string || exit

import m4/ax_cflags_warn_all || exit

copy source isa header template data
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_FILES="$(files isa)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
${UNISIM_LIB_SIMULATOR_ISA_FILES} \
$(files header) \
$(files template)"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

UNISIM_SIMULATOR_SOURCE_FILES="\
main.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
template_default_config.xml \
unisim.ico \
"

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

output_simulator_configure_ac <(cat << EOF
AC_INIT([UNISIM PowerMac G4 PCI Standalone simulator], [${SIMULATOR_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>], [unisim-${SIMPKG}])
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
nodist_libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = unisim/component/cxx/processor/powerpc/e600/mpc7447a/isa/mpc7447a.cc

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_SIMULATOR_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e600/mpc7447a/isa/mpc7447a.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e600/mpc7447a/isa/mpc7447a.cc

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e600/mpc7447a/isa/mpc7447a.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e600/mpc7447a/isa/mpc7447a.cc

\$(top_builddir)/unisim/component/cxx/processor/powerpc/e600/mpc7447a/isa/mpc7447a.cc: \$(top_builddir)/unisim/component/cxx/processor/powerpc/e600/mpc7447a/isa/mpc7447a.hh
\$(top_builddir)/unisim/component/cxx/processor/powerpc/e600/mpc7447a/isa/mpc7447a.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(PYTHON_BIN) \$(top_srcdir)/genisslib.py -o \$(top_builddir)/unisim/component/cxx/processor/powerpc/e600/mpc7447a/isa/mpc7447a -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/powerpc/e600/mpc7447a/isa/mpc7447a.isa

EOF
)

build_simulator_configure

echo "Distribution is up-to-date"

