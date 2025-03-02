#!/bin/bash

SIMPKG=s12x
SIMPKG_SRCDIR=tlm2/s12x

source "$(dirname $0)/dist_common.sh"

import_genisslib || exit

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
import unisim/service/loader/multiformat_loader || exit
import unisim/service/tee/registers || exit
import unisim/service/tee/memory_import_export || exit
import unisim/service/netstreamer || exit
import unisim/service/debug/profiler || exit
import unisim/service/instrumenter || exit
import unisim/service/http_server || exit
import unisim/service/tee/char_io || exit
import unisim/service/web_terminal || exit
import unisim/service/time/sc_time || exit
import unisim/service/time/host_time || exit
import unisim/component/tlm2/processor/hcs12x || exit
import unisim/component/tlm2/memory/ram || exit
import unisim/component/cxx/processor/opcache || exit
import unisim/service/pim || exit
import unisim/service/monitor || exit
import unisim/service/debug/inline_debugger || exit
import m4/rtbcob || exit

import libc/inttypes || exit
import libc/signal || exit
import libc/stdint || exit
import libc/stdio || exit
import libc/stdlib || exit
import libc/string || exit
import std/iostream || exit
import std/stdexcept || exit

import m4/ax_cflags_warn_all || exit

copy source isa isa_s12xgate isa_xb header template data
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_FILES="$(files isa:unisim/component/cxx/processor/hcs12x/isa/hcs12x)"

UNISIM_LIB_SIMULATOR_ISA_XB_FILES="$(files isa:unisim/component/cxx/processor/hcs12x/isa/xb)"

UNISIM_LIB_SIMULATOR_ISA_S12XGATE_FILES="$(files isa:unisim/component/cxx/processor/hcs12x/isa/s12xgate)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
${UNISIM_LIB_SIMULATOR_ISA_FILES} \
${UNISIM_LIB_SIMULATOR_ISA_XB_FILES} \
${UNISIM_LIB_SIMULATOR_ISA_S12XGATE_FILES} \
$(files header) \
$(files template) \
"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

UNISIM_SIMULATOR_SOURCE_FILES="\
simulator.cc \
atd_pwm_stub.cc \
xml_atd_pwm_stub.cc \
rtb_unisim.cc \
can_stub.cc \
tle8264_2e.cc \
simulator_if.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
simulator.hh \
atd_pwm_stub.hh \
xml_atd_pwm_stub.hh \
rtb_unisim.hh \
can_stub.hh \
tle8264_2e.hh \
simulator_if.h \
"

UNISIM_SIMULATOR_MAIN_SOURCE_FILES="\
main.cc \
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
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Simulator

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

# -------------------------
bin_PROGRAMS = unisim-${SIMPKG}-${SIMULATOR_VERSION}
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_SIMULATOR_MAIN_SOURCE_FILES}
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la libunisim-${SIMPKG}-simulator-${SIMULATOR_VERSION}.la
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDFLAGS = -static

# -------------------------
lib_LTLIBRARIES = libunisim-${SIMPKG}-simulator-${SIMULATOR_VERSION}.la
libunisim_${AM_SIMPKG}_simulator_${AM_SIMULATOR_VERSION}_la_CXXFLAGS =
libunisim_${AM_SIMPKG}_simulator_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -no-undefined
libunisim_${AM_SIMPKG}_simulator_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_SIMULATOR_SOURCE_FILES}
libunisim_${AM_SIMPKG}_simulator_${AM_SIMULATOR_VERSION}_la_LIBADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la

# -------------------------
noinst_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_CXXFLAGS =
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -no-undefined
nodist_libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = unisim/component/cxx/processor/hcs12x/isa/xb/xb.cc unisim/component/cxx/processor/hcs12x/isa/hcs12x/hcs12x.cc unisim/component/cxx/processor/hcs12x/isa/s12xgate/s12xgate.cc

# -------------------------
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES}

# -------------------------
noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}


includedir = \$(prefix)/include
include_HEADERS = simulator_if.h

sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES}

EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/xb/xb.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/xb/xb.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/hcs12x.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/hcs12x.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/s12xgate/s12xgate.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/s12xgate/s12xgate.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/isa/xb/xb_sub.isa

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/xb/xb.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/xb/xb.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/hcs12x.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/hcs12x.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/s12xgate/s12xgate.hh\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/s12xgate/s12xgate.cc\
	\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/isa/xb/xb_sub.isa
	
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/xb/xb.cc: \$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/xb/xb.hh
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/isa/xb/xb_sub.isa: \$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/xb/xb.hh
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/xb/xb.hh: ${UNISIM_LIB_SIMULATOR_ISA_XB_FILES}
	\$(mkdir_p) \$(@D)
	\$(PYTHON_BIN) \$(top_srcdir)/genisslib.py -o \$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/xb/xb -w 32 -I \$(top_srcdir)/unisim/component/cxx/processor/hcs12x -I \$(top_builddir)/unisim/component/cxx/processor/hcs12x \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/isa/xb/xb.isa
	 
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/hcs12x.cc: \$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/hcs12x.hh
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/hcs12x.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES} \$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/xb/xb_sub.isa
	\$(mkdir_p) \$(@D)
	\$(PYTHON_BIN) \$(top_srcdir)/genisslib.py -o \$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/hcs12x -w 32 -I \$(top_srcdir)/unisim/component/cxx/processor/hcs12x -I \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/isa/hcs12x/hcs12x.isa
	 
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/s12xgate/s12xgate.cc: \$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/s12xgate/s12xgate.hh
\$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/s12xgate/s12xgate.hh: ${UNISIM_LIB_SIMULATOR_ISA_S12XGATE_FILES}
	\$(mkdir_p) \$(@D)
	\$(PYTHON_BIN) \$(top_srcdir)/genisslib.py -o \$(top_builddir)/unisim/component/cxx/processor/hcs12x/isa/s12xgate/s12xgate -w 32 -I \$(top_srcdir)/unisim/component/cxx/processor/hcs12x \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/isa/s12xgate/s12xgate.isa

EOF
)

build_simulator_configure

echo "Distribution is up-to-date"
