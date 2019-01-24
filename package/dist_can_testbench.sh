#!/bin/bash

SIMPKG=can_testbench

UNISIM_LIB_SIMULATOR_SOURCE_FILES="\
unisim/kernel/service/service.cc \
unisim/kernel/config/xml_config_file_helper.cc \
unisim/kernel/config/ini_config_file_helper.cc \
unisim/kernel/tlm2/tlm.cc \
unisim/kernel/tlm2/clock.cc \
unisim/kernel/tlm2/simulator.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/backtrace/backtrace.cc \
unisim/kernel/api/api.cc \
unisim/util/random/random.cc \
unisim/service/time/host_time/time.cc \
unisim/service/time/sc_time/time.cc \
"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
unisim/kernel/service/service.hh \
unisim/kernel/config/xml_config_file_helper.hh \
unisim/kernel/config/ini_config_file_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/tlm2/tlm.hh \
unisim/kernel/tlm2/simulator.hh \
unisim/kernel/tlm2/clock.hh \
unisim/kernel/tlm2/tlm_serial.hh \
unisim/kernel/tlm2/tlm_can.hh \
unisim/util/backtrace/backtrace.hh \
unisim/kernel/api/api.hh \
unisim/util/likely/likely.hh \
unisim/util/inlining/inlining.hh \
unisim/util/random/random.hh \
unisim/util/hash_table/hash_table.hh \
unisim/service/interfaces/time.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
"

UNISIM_LIB_SIMULATOR_TEMPLATE_FILES="\
"

UNISIM_LIB_SIMULATOR_M4_FILES="\
m4/times.m4 \
m4/endian.m4 \
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/bsd_sockets.m4 \
m4/systemc.m4 \
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4 \
m4/pthread.m4 \
"

UNISIM_LIB_SIMULATOR_DATA_FILES="\
unisim/kernel/tlm2/style.css \
unisim/kernel/tlm2/script.js \
unisim/kernel/tlm2/favicon.ico \
"

SIMULATOR_EXTERNAL_HEADERS="\
assert.h \
ctype.h \
cxxabi.h \
errno.h \
fcntl.h \
fenv.h \
float.h \
getopt.h \
inttypes.h \
limits.h \
math.h \
signal.h \
stdarg.h \
stdio.h \
stdlib.h \
string.h \
ctype.h \
sys/types.h \
unistd.h \
fstream \
cassert \
cmath \
cerrno \
cstddef \
cstdio \
cstdlib \
cstring \
iomanip \
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
set \
"

UNISIM_SIMULATOR_SOURCE_FILES="\
"
UNISIM_SIMULATOR_HEADER_FILES="\
"

UNISIM_SIMULATOR_TEMPLATE_FILES="\
"

UNISIM_SIMULATOR_EXTRA_FILES="\
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
unisim.ico \
"

UNISIM_SIMULATOR_TESTBENCH_FILES="\
main.cc \
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
DEST_DIR=$(cd $1; pwd)
mkdir -p ${DEST_DIR}
UNISIM_LIB_DIR=${UNISIM_DIR}/unisim_lib
UNISIM_SIMULATOR_DIR=${UNISIM_DIR}/unisim_simulators/tlm2/${SIMPKG}

SIMULATOR_VERSION=$(cat ${UNISIM_SIMULATOR_DIR}/VERSION)

if [ -z "${DISTCOPY}" ]; then
	DISTCOPY=cp
fi

has_to_build() {
	if [ ! -e "$1" -o "$2" -nt "$1" ]; then
		return 0;
	else
		return 1;
	fi
}

dist_copy() {
	if has_to_build "$2" "$1"; then
		echo "$1 ==> $2"
		mkdir -p "$(dirname $2)"
		${DISTCOPY} -f "$1" "$2" || exit
		return 0
	fi
	return 1
}

mkdir -p ${DEST_DIR}/${SIMPKG}

UNISIM_LIB_SIMULATOR_FILES="${UNISIM_LIB_SIMULATOR_SOURCE_FILES} ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_LIB_SIMULATOR_TEMPLATE_FILES} ${UNISIM_LIB_SIMULATOR_DATA_FILES}"

for file in ${UNISIM_LIB_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_EXTRA_FILES} \
${UNISIM_SIMULATOR_TEMPLATE_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
${UNISIM_SIMULATOR_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"
done

for file in ${UNISIM_SIMULATOR_PKG_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/${SIMPKG}/config
mkdir -p ${DEST_DIR}/${SIMPKG}/m4

has_to_build_simulator_configure=no
for file in ${UNISIM_LIB_SIMULATOR_M4_FILES}; do
	if dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${SIMPKG}/${file}"; then
		has_to_build_simulator_configure=yes
	fi
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Gilles Mouchard <gilles.mouchard@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - UNISIM GenISSLib: an instruction set simulator generator
  - UNISIM CAN_TESTBENCH Simulator: a CAN_TESTBENCH SoC simulator.
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

if has_to_build "${CONFIGURE_AC}" "$0" || has_to_build "${MAKEFILE_AM}" "$0"; then
	echo "Generating configure.ac"
	cat <<EOF > "${CONFIGURE_AC}"
AC_INIT([UNISIM CAN_TESTBENCH Simulator Package], [${SIMULATOR_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves Lhuillier <yves.lhuillier@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}])
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects tar-pax])
AC_PATH_PROGS(SH, sh)
AC_PROG_INSTALL
AC_PROG_LN_S
AC_CONFIG_SUBDIRS([${SIMPKG}]) 
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF

	echo "Generating Makefile.am"
	cat <<EOF > "${MAKEFILE_AM}"
SUBDIRS=${SIMPKG}
EXTRA_DIST = configure.cross
EOF

	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

if has_to_build "${CONFIGURE_CROSS}" "$0"; then
	echo "Building configure.cross"
	cat << EOF_CONFIGURE_CROSS > "${CONFIGURE_CROSS}"
#!/bin/bash
HERE=\$(pwd)
MY_DIR=\$(cd \$(dirname \$0); pwd)

# remove --host from command line arguments
host=""
help=""
i=0
j=0
for arg in "\$@"
do
	case "\${arg}" in
		--host=*)
			host=\$(printf "%s" "\${arg}" | cut -f 2- -d '=')
			;;
		--help=* | --help)
			help="yes"
			args[\${j}]=\${arg}
			j=\$((\${j}+1))
			;;
		*)
			args[\${j}]=\${arg}
			j=\$((\${j}+1))
			;;
	esac
	i=\$((\${i}+1))
done

if test "\${help}" != "yes"; then
	if test -z "\${host}"; then
		echo "ERROR: No canonical name for the host system type was specified. Use --host=<canonical name> to specify a host system type (e.g. --host=i586-pc-mingw32)"
		exit -1
	fi
fi

if test "\${help}" = "yes"; then
	echo "=== configure help for ${SIMPKG}"
else
	echo "=== configuring in ${SIMPKG} (\${HERE}/${SIMPKG}) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/${SIMPKG}/configure \$@"
fi

if test ! -d \${HERE}/${SIMPKG}; then
	mkdir \${HERE}/${SIMPKG}
fi
cd \${HERE}/${SIMPKG}
\${MY_DIR}/${SIMPKG}/configure "\$@"
STATUS="\$?"
cd "\${HERE}"
if test \${STATUS} -ne 0; then
	exit \${STATUS}
fi

if test "\${help}" = "yes"; then
	exit 0
fi

echo "\$(basename \$0): creating Makefile.cross"
cat << EOF_MAKEFILE_CROSS > Makefile.cross
#!/usr/bin/make -f
all: ${SIMPKG}-all
clean: ${SIMPKG}-clean
distclean: ${SIMPKG}-distclean
	rm -f \${HERE}/Makefile.cross
install: ${SIMPKG}-install

${SIMPKG}-all:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} all
${SIMPKG}-clean:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} clean
${SIMPKG}-distclean:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} distclean
${SIMPKG}-install:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} install
EOF_MAKEFILE_CROSS

chmod +x Makefile.cross

echo "\$(basename \$0): run 'make -f \${HERE}/Makefile.cross' or '\${HERE}/Makefile.cross' to build for \${host} host system type"
EOF_CONFIGURE_CROSS
	chmod +x "${CONFIGURE_CROSS}"
fi  # has to build configure cross

# Simulator

SIMULATOR_CONFIGURE_AC="${DEST_DIR}/${SIMPKG}/configure.ac"
SIMULATOR_MAKEFILE_AM="${DEST_DIR}/${SIMPKG}/Makefile.am"

if has_to_build "${SIMULATOR_CONFIGURE_AC}" "$0" || has_to_build "${SIMULATOR_MAKEFILE_AM}" "$0"; then
	has_to_build_simulator_configure=yes
fi

if [ "${has_to_build_simulator_configure}" = "yes" ]; then
	echo "Generating ${SIMPKG} configure.ac"
	cat <<EOF > "${SIMULATOR_CONFIGURE_AC}"
AC_INIT([UNISIM CAN_TESTBENCH Standalone simulator], [${SIMULATOR_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>], [unisim-${SIMPKG}-core])
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
AC_CHECK_HEADERS([${SIMULATOR_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))
case "\${host}" in
	*mingw*)
		CPPFLAGS="-U__STRICT_ANSI__ \${CPPFLAGS}"
		;;
	*)
		;;
esac
UNISIM_CHECK_PTHREAD(main)
UNISIM_CHECK_TIMES(main)
UNISIM_CHECK_ENDIAN(main)
UNISIM_CHECK_BSD_SOCKETS(main)
UNISIM_CHECK_ZLIB(main)
UNISIM_CHECK_LIBXML2(main)
UNISIM_CHECK_CXXABI(main)
UNISIM_CHECK_GET_EXECUTABLE_PATH(main)
UNISIM_CHECK_REAL_PATH(main)
AX_BOOST_BASE([1.53.0], AC_MSG_NOTICE([boost >= 1.53.0 found.]), AC_MSG_ERROR([boost >= 1.53.0 not found.]))
CPPFLAGS="\${BOOST_CPPFLAGS} \${CPPFLAGS}"
LDFLAGS="\${BOOST_LDFLAGS} \${LDFLAGS}"
UNISIM_CHECK_SYSTEMC
AX_CXXFLAGS_WARN_ALL
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/unisim-${SIMPKG}-${SIMULATOR_VERSION}"], [path of shared data relative to bin directory])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF

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
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_SIMULATOR_TESTBENCH_FILES} ${UNISIM_SIMULATOR_SOURCE_FILES}
#unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_LDFLAGS = -static-libtool-libs
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la

# Static Library
noinst_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES}
libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -static

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_LIB_SIMULATOR_TEMPLATE_FILES} ${UNISIM_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_TEMPLATE_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_PKG_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_SIMULATOR_DATA_FILES}

EOF

	echo "Building ${SIMPKG} configure"
	${SHELL} -c "cd ${DEST_DIR}/${SIMPKG} && aclocal -I m4 && libtoolize --force && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
