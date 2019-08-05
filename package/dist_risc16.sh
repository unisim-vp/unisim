#!/bin/bash
SIMPKG=risc16
SIMPKGDIR=tlm2/risc16
source "$(dirname $0)/dist_common.sh"

import_genisslib

import unisim/kernel/service || exit
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

SIMULATOR_EXTERNAL_HEADERS="\
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
unistd.h \
vector \
"

UNISIM_SIMULATOR_SOURCE_FILES="\
main.cc \
simulator.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
simulator.hh \
"

UNISIM_SIMULATOR_TEMPLATE_FILES=

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
${UNISIM_SIMULATOR_TEMPLATE_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
${UNISIM_SIMULATOR_CONFIG_FILES} \
"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

for file in ${UNISIM_SIMULATOR_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Top level

cat << EOF > "${DEST_DIR}/README"
This package contains GenISSLib, an instruction set simulator generator, and a risc16 instruction set simulator.
See INSTALL for installation instructions.
EOF

cat << EOF > "${DEST_DIR}/INSTALL"
INSTALLATION
------------

Requirements:
  - GNU bash (bash-3.0-19.3 on RHEL4)
  - GNU make (make-3.80-6.EL4 on RHEL4)
  - GNU autoconf (autoconf-2.59-5 on RHEL4)
  - GNU automake (automake-1.9.2-3 on RHEL4)
  - GNU flex (flex-2.5.4a-33 on RHEL4)
  - GNU bison (bison-1.875c-2 on RHEL4)
  - boost development package (boost-devel-1.32.0-1.rhel4 on RHEL4)
  - libxml2 development package (libxml2-devel-2.6.16-6 on RHEL4)
  - zlib development package (zlib-devel-1.2.1.2-1.2 on RHEL4)

Building instructions:
  $ ./configure
  $ make

Installing (optional):
  $ make install

EOF

output_top_configure_ac <(cat << EOF
AC_INIT([UNISIM risc16 Standalone simulator], [${SIMULATOR_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Réda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}])
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
)

output_top_makefile_am <(cat << EOF
SUBDIRS=genisslib ${SIMPKG}
EXTRA_DIST = configure.cross
EOF
)

build_top_configure
build_top_configure_cross

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
)

output_simulator_makefile_am <(cat << EOF
ACLOCAL_AMFLAGS=-I m4
AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)
noinst_LIBRARIES = librisc16-${SIMULATOR_VERSION}.a
nodist_librisc16_${AM_SIMULATOR_VERSION}_a_SOURCES = unisim/component/cxx/processor/risc16/isa.cc
librisc16_${AM_SIMULATOR_VERSION}_a_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES} 
bin_PROGRAMS = unisim-risc16-${SIMULATOR_VERSION}
unisim_risc16_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_SIMULATOR_SOURCE_FILES}
unisim_risc16_${AM_SIMULATOR_VERSION}_LDADD = librisc16-${SIMULATOR_VERSION}.a
noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_LIB_SIMULATOR_TEMPLATE_FILES} ${UNISIM_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_TEMPLATE_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-risc16-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_CONFIG_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/risc16/isa.hh\
	\$(top_builddir)/unisim/component/cxx/processor/risc16/isa.cc

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/risc16/isa.hh\
	\$(top_buildir)/unisim/component/cxx/processor/risc16/isa.cc

\$(top_builddir)/unisim/component/cxx/processor/risc16/isa.cc: \$(top_builddir)/unisim/component/cxx/processor/risc16/isa.hh
\$(top_builddir)/unisim/component/cxx/processor/risc16/isa.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/risc16/isa -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/risc16/risc16.isa
EOF
)

build_simulator_configure

echo "Distribution is up-to-date"
