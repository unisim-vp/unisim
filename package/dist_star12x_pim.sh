#!/bin/bash
SIMPKG=s12x
SIMPKGDIR=tlm2/s12x
source "$(dirname $0)/dist_common.sh"

import_genisslib

import unisim/service/debug/debugger
import unisim/service/debug/gdb_server
import unisim/service/loader/multiformat_loader
import unisim/service/tee/registers
import unisim/service/tee/memory_import_export
import unisim/service/telnet
import unisim/service/time/sc_time
import unisim/service/time/host_time
import unisim/component/tlm2/processor/hcs12x
import unisim/component/tlm2/memory/ram
import unisim/service/pim
import unisim/service/monitor
import unisim/service/debug/inline_debugger
import m4/rtbcob

copy source isa isa_s12xgate isa_xb header template data
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_FILES="$(files isa)"

UNISIM_LIB_SIMULATOR_ISA_XB_FILES="$(files isa_xb)"

UNISIM_LIB_SIMULATOR_ISA_S12XGATE_FILES="$(files isa_s12xgate)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="${UNISIM_LIB_SIMULATOR_ISA_FILES} ${UNISIM_LIB_SIMULATOR_ISA_XB_FILES} ${UNISIM_LIB_SIMULATOR_ISA_S12XGATE_FILES} $(files header) $(files template)"

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
simulator.cc \
atd_pwm_stub.cc \
xml_atd_pwm_stub.cc \
rtb_unisim.cc \
can_stub.cc \
tle8264_2e.cc \
simulator_if.cc \
"

UNISIM_SIMULATOR_TEMPLATE_FILES=

UNISIM_SIMULATOR_HEADER_FILES="${UNISIM_SIMULATOR_TEMPLATE_FILES} \
simulator.hh \
atd_pwm_stub.hh \
xml_atd_pwm_stub.hh \
rtb_unisim.hh \
can_stub.hh \
tle8264_2e.hh \
simulator_if.h \
"

UNISIM_SIMULATOR_MAIN_SOURCE_FILES="\
main_pim.cc \
"

UNISIM_SIMULATOR_PKG_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
unisim.ico \
default_config.xml \
ATD.xml \
ATD.xls \
pwm.xls \
pim.xml \
"

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_MAIN_SOURCE_FILES} \
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

for file in ${UNISIM_SIMULATOR_PKG_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Reda Nouacer <reda.nouacer@cea.fr>
Gilles Mouchard <gilles.mouchard@cea.fr>
Yves Lhuillier <yves.lhuillier@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains GenISSLib, an instruction set simulator generator, and a star12xe instruction set simulator.
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
AC_INIT([UNISIM star12xe Standalone simulator], [${SIMULATOR_VERSION}], [reda.nouacer@cea.fr], [unisim-${SIMPKG}])
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
EOF
)

build_top_configure
build_top_configure_cross

# simulator

output_simulator_configure_ac <(cat << EOF
AC_INIT([UNISIM star12x C++ simulator], [${SIMULATOR_VERSION}], [reda.nouacer@cea.fr], [unisim-${SIMPKG}-core])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_SYSTEM
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([subdir-objects tar-pax])

AC_PATH_PROGS(SH, sh)
AC_PROG_CXX
AC_PROG_INSTALL

LT_INIT([win32-dll], [dlopen])

AC_SUBST(LIBTOOL_DEPS)
AC_LTDL_DLLIB

AC_PROG_LN_S
AC_LANG([C++])
AM_PROG_CC_C_O
AC_CHECK_HEADERS([${SIMULATOR_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))
$(lines ac)
UNISIM_CHECK_RTBCOB

GENISSLIB_PATH=\`pwd\`/../genisslib/genisslib
AC_SUBST(GENISSLIB_PATH)
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

# -------------------------
bin_PROGRAMS = unisim-${SIMPKG}-${SIMULATOR_VERSION}
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_SIMULATOR_MAIN_SOURCE_FILES}
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la libunisim-${SIMPKG}-simulator-${SIMULATOR_VERSION}.la
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_LDFLAGS = -static

# -------------------------
lib_LTLIBRARIES = libunisim-${SIMPKG}-simulator-${SIMULATOR_VERSION}.la
libunisim_${SIMPKG}_simulator_${AM_SIMULATOR_VERSION}_la_CXXFLAGS =
libunisim_${SIMPKG}_simulator_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -no-undefined
libunisim_${SIMPKG}_simulator_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_SIMULATOR_SOURCE_FILES}
libunisim_${SIMPKG}_simulator_${AM_SIMULATOR_VERSION}_la_LIBADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la

# -------------------------
noinst_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_CXXFLAGS =
libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -no-undefined
nodist_libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = unisim/component/cxx/processor/hcs12x/xb.cc unisim/component/cxx/processor/hcs12x/hcs12x.cc unisim/component/cxx/processor/hcs12x/s12xgate.cc

# -------------------------
libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES}

# -------------------------
noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}


includedir = \$(prefix)/include
include_HEADERS = simulator_if.h

sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_LIB_STAR12X_DATA_FILES} ${UNISIM_SIMULATORS_STAR12X_DATA_FILES} ${UNISIM_SIMULATORS_STAR12X_CONFIG_FILES}

EXTRA_DIST = ${UNISIM_LIB_STAR12X_M4_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/hcs12x.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/s12xgate.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/s12xgate.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/hcs12x.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/s12xgate.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/s12xgate.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa
	
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb.cc: \$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb.hh
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa: \$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb.hh
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb.hh: ${UNISIM_LIB_SIMULATOR_ISA_XB_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb -w 32 -I \$(top_srcdir)/unisim/component/cxx/processor/hcs12x -I \$(top_builddir)/unisim/component/cxx/processor/hcs12x \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.isa
	 
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/hcs12x.cc: \$(top_builddir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES} \$(top_builddir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa
	 \$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/hcs12x/hcs12x -w 32 -I \$(top_srcdir)/unisim/component/cxx/processor/hcs12x \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.isa
	 
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/s12xgate.cc: \$(top_builddir)/unisim/component/cxx/processor/hcs12x/s12xgate.hh
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/s12xgate.hh: ${UNISIM_LIB_SIMULATOR_ISA_S12XGATE_FILES}
	 \$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/hcs12x/s12xgate -w 32 -I \$(top_srcdir)/unisim/component/cxx/processor/hcs12x \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/s12xgate.isa

EOF
)

build_simulator_configure

echo "Distribution is up-to-date"
