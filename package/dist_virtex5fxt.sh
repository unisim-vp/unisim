#!/bin/bash
source "$(dirname $0)/dist_common.sh"

import unisim/component/tlm2/processor/powerpc/ppc440 || exit
import unisim/component/cxx/processor/powerpc/ppc440/isa || exit
import unisim/component/tlm2/memory/ram || exit
import unisim/component/tlm2/interrupt/xilinx/xps_intc || exit
import unisim/component/tlm2/interrupt/xilinx/xps_intc || exit
import unisim/component/tlm2/timer/xilinx/xps_timer || exit
import unisim/component/tlm2/timer/xilinx/xps_timer || exit
import unisim/component/tlm2/interconnect/generic_router || exit
import unisim/component/tlm2/memory/flash/am29 || exit
import unisim/component/tlm2/interconnect/xilinx/dcr_controller || exit
import unisim/component/tlm2/interconnect/xilinx/crossbar || exit
import unisim/component/tlm2/interconnect/xilinx/mci || exit
import unisim/component/tlm2/com/xilinx/xps_uart_lite || exit
import unisim/component/tlm2/com/xilinx/xps_gpio || exit
import unisim/component/tlm2/com/xilinx/xps_gpio || exit
import unisim/component/tlm2/com/xilinx/xps_gpio || exit
import unisim/kernel/service || exit
import unisim/service/debug/debugger || exit
import unisim/service/debug/gdb_server || exit
import unisim/service/debug/inline_debugger || exit
import unisim/service/profiling/addr_profiler || exit
import unisim/service/loader/multiformat_loader || exit
import unisim/service/power || exit
import unisim/service/time/sc_time || exit
import unisim/service/time/host_time || exit
import unisim/service/translator/memory_address/memory || exit
import unisim/service/telnet || exit
import unisim/service/os/linux_os || exit
import unisim/kernel/logger || exit
import unisim/kernel/tlm2 || exit

function Usage
{
	echo "Usage:"
	echo "  $0 <destination directory>"
}

if [ -z "$1" ]; then
	Usage
	exit -1
fi

HERE=$(pwd)
MY_DIR=$(cd $(dirname $0); pwd)
DEST_DIR=$1
UNISIM_TOOLS_DIR=${MY_DIR}/../unisim_tools
UNISIM_LIB_DIR=${MY_DIR}/../unisim_lib
UNISIM_SIMULATORS_DIR=${MY_DIR}/../unisim_simulators/tlm2/virtex5fxt
LIGHT_DIST=no #yes

VIRTEX5FXT_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-virtex5fxt-${VIRTEX5FXT_VERSION}

if test -z "${DISTCOPY}"; then
    DISTCOPY=cp
fi

UNISIM_TOOLS_GENISSLIB_HEADER_FILES="\
action.hh \
cli.hh \
errtools.hh \
isa.hh \
parser_defs.hh \
riscgenerator.hh \
specialization.hh \
variable.hh \
bitfield.hh \
comment.hh \
fwd.hh \
main.hh \
product.hh \
scanner.hh \
strtools.hh \
vect.hh \
ciscgenerator.hh \
conststr.hh \
generator.hh \
operation.hh \
referencecounting.hh \
sourcecode.hh \
subdecoder.hh"

UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES="\
scanner.cc \
parser.cc \
parser_tokens.hh"

UNISIM_TOOLS_GENISSLIB_SOURCE_FILES="\
parser.yy \
scanner.ll \
action.cc \
bitfield.cc \
cli.cc \
comment.cc \
conststr.cc \
isa.cc \
main.cc \
operation.cc \
product.cc \
referencecounting.cc \
sourcecode.cc \
strtools.cc \
variable.cc \
generator.cc \
riscgenerator.cc \
ciscgenerator.cc \
subdecoder.cc \
specialization.cc \
errtools.cc"

UNISIM_TOOLS_GENISSLIB_DATA_FILES="COPYING INSTALL NEWS README AUTHORS ChangeLog"

UNISIM_TOOLS_GENISSLIB_M4_FILES="\
m4/lexer.m4 \
m4/parser_gen.m4"

GENISSLIB_EXTERNAL_HEADERS="\
cassert \
cctype \
cerrno \
cstdarg \
cstdio \
cstdlib \
cstring \
fstream \
inttypes.h \
iosfwd \
iostream \
limits \
map \
memory \
ostream \
unistd.h \
vector"

UNISIM_LIB_VIRTEX5FXT_SOURCE_FILES="$(files source)"

UNISIM_LIB_VIRTEX5FXT_ISA_FILES="$(files isa)"

UNISIM_LIB_VIRTEX5FXT_HEADER_FILES="\
${UNISIM_LIB_VIRTEX5FXT_ISA_FILES} \
$(files header) \
"

UNISIM_LIB_VIRTEX5FXT_TEMPLATE_FILES="\
$(files template) \
"

UNISIM_LIB_VIRTEX5FXT_M4_FILES="\
m4/times.m4 \
m4/endian.m4 \
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/systemc.m4 \
m4/cacti.m4 \
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4 \
m4/pthread.m4"

UNISIM_LIB_VIRTEX5FXT_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_powerpc_32.xml \
unisim/util/debug/dwarf/powerpc_eabi_dwarf_register_number_mapping.xml \
unisim/util/debug/dwarf/powerpc_eabi_gcc_dwarf_register_number_mapping.xml"

VIRTEX5FXT_EXTERNAL_HEADERS="\
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
set"

UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES="\
config.cc \
mplb.cc \
mplb_debug.cc \
intc.cc \
timer.cc \
dcr_controller.cc \
crossbar.cc \
uart_lite.cc \
gpio_dip_switches_8bit.cc \
gpio_leds_8bit.cc \
gpio_5_leds_positions.cc \
gpio_push_buttons_5bit.cc \
dip_switches_8bit.cc \
leds_8bit.cc \
5_leds_positions.cc \
push_buttons_5bit.cc \
memory_router.cc \
memory_router_debug.cc \
"

UNISIM_SIMULATORS_VIRTEX5FXT_HEADER_FILES="\
simulator.hh \
config.hh \
"

UNISIM_SIMULATORS_VIRTEX5FXT_TEMPLATE_FILES="\
simulator.tcc \
"

UNISIM_SIMULATORS_VIRTEX5FXT_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
template_default_config.xml \
unisim.ico"

UNISIM_SIMULATORS_VIRTEX5FXT_TESTBENCH_FILES="\
main.cc \
main_wocache.cc \
main_wfpu.cc \
main_wfpu_wocache.cc \
main_debug.cc \
main_wocache_debug.cc \
main_wfpu_debug.cc \
main_wfpu_wocache_debug.cc \
"

has_to_build_configure=no
has_to_build_configure_cross=no
has_to_build_genisslib_configure=no
has_to_build_virtex5fxt_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/virtex5fxt

UNISIM_TOOLS_GENISSLIB_FILES="${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES} ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES} ${UNISIM_TOOLS_GENISSLIB_DATA_FILES}"

for file in ${UNISIM_TOOLS_GENISSLIB_FILES}; do
	mkdir -p "${DEST_DIR}/$(dirname ${file})"
	has_to_copy=no
	if [ -e "${DEST_DIR}/genisslib/${file}" ]; then
		if [ "${UNISIM_TOOLS_DIR}/genisslib/${file}" -nt "${DEST_DIR}/genisslib/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_TOOLS_DIR}/genisslib/${file} ==> ${DEST_DIR}/genisslib/${file}"
		${DISTCOPY} -f "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/genisslib/${file}" || exit
	fi
done

UNISIM_LIB_VIRTEX5FXT_FILES="${UNISIM_LIB_VIRTEX5FXT_SOURCE_FILES} ${UNISIM_LIB_VIRTEX5FXT_HEADER_FILES} ${UNISIM_LIB_VIRTEX5FXT_TEMPLATE_FILES} ${UNISIM_LIB_VIRTEX5FXT_DATA_FILES}"

for file in ${UNISIM_LIB_VIRTEX5FXT_FILES}; do
	mkdir -p "${DEST_DIR}/virtex5fxt/$(dirname ${file})"
	has_to_copy=no
	if [ -e "${DEST_DIR}/virtex5fxt/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/virtex5fxt/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/virtex5fxt/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/virtex5fxt/${file}" || exit
	fi
done

UNISIM_SIMULATORS_VIRTEX5FXT_FILES="${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_HEADER_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_TEMPLATE_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_DATA_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATORS_VIRTEX5FXT_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/virtex5fxt/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/virtex5fxt/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/virtex5fxt/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/virtex5fxt/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_VIRTEX5FXT_DATA_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/${file}" || exit
	fi
done


mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/virtex5fxt/config
mkdir -p ${DEST_DIR}/virtex5fxt/m4
mkdir -p ${DEST_DIR}/genisslib/config
mkdir -p ${DEST_DIR}/genisslib/m4

for file in ${UNISIM_TOOLS_GENISSLIB_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/genisslib/${file}" ]; then
		if [ "${UNISIM_TOOLS_DIR}/${file}" -nt  "${DEST_DIR}/genisslib/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_TOOLS_DIR}/${file} ==> ${DEST_DIR}/genisslib/${file}"
		${DISTCOPY} -f "${UNISIM_TOOLS_DIR}/${file}" "${DEST_DIR}/genisslib/${file}" || exit
		has_to_build_genisslib_configure=yes
	fi
done

for file in ${UNISIM_LIB_VIRTEX5FXT_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/virtex5fxt/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/virtex5fxt/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/virtex5fxt/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/virtex5fxt/${file}" || exit
		has_to_build_virtex5fxt_configure=yes
	fi
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Gilles Mouchard <gilles.mouchard@cea.fr>
Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>
Réda Nouacer <reda.nouacer@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - UNISIM GenISSLib: an instruction set simulator generator
  - UNISIM Virtex 5 FXT Simulator: a full system Virtex-5-FXT-like simulator including a PPC440x5 and some Xilinx Virtex 5 FXT IPs.
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

if [ ! -e "${CONFIGURE_AC}" ]; then
	has_to_build_configure=yes
else
	if [ "$0" -nt "${CONFIGURE_AC}" ]; then
		has_to_build_configure=yes
	fi
fi

if [ ! -e "${MAKEFILE_AM}" ]; then
	has_to_build_configure=yes
else
	if [ "$0" -nt "${MAKEFILE_AM}" ]; then
		has_to_build_configure=yes
	fi
fi

if [ ! -e "${CONFIGURE_CROSS}" ]; then
	has_to_build_configure_cross=yes
else
	if [ "$0" -nt "${CONFIGURE_CROSS}" ]; then
		has_to_build_configure_cross=yes
	fi
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Generating configure.ac"
	echo "AC_INIT([UNISIM Virtex 5 FXT Simulator Package], [${VIRTEX5FXT_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-virtex5fxt])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([virtex5fxt])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib virtex5fxt" > "${MAKEFILE_AM}"
	echo "EXTRA_DIST = configure.cross" >> "${MAKEFILE_AM}"

	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

if [ "${has_to_build_configure_cross}" = "yes" ]; then
	echo "Building configure.cross"
	cat << EOF_CONFIGURE_CROSS > "${CONFIGURE_CROSS}"
#!/bin/bash
HERE=\$(pwd)
MY_DIR=\$(cd \$(dirname \$0); pwd)

# remove --host, --with-systemc, --with-zlib, --with-libxml2, --with-boost, --with-ncurses, --with-libedit from command line arguments
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
		--with-systemc=* | --with-zlib=* | --with-libxml2=* | --with-boost=* | --with-ncurses=* | --with-libedit=*)
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
	echo "=== configure help for genisslib"
else
	echo "=== configuring in genisslib (\${HERE}/genisslib)"
	echo "\$(basename \$0): running \${MY_DIR}/genisslib/configure \${args[@]}"
fi
if test ! -d \${HERE}/genisslib; then
	mkdir "\${HERE}/genisslib"
fi
cd "\${HERE}/genisslib"
\${MY_DIR}/genisslib/configure "\${args[@]}"
STATUS="\$?"
cd "\${HERE}"
if test \${STATUS} -ne 0; then
	exit \${STATUS}
fi

if test "\${help}" = "yes"; then
	echo "=== configure help for virtex5fxt"
else
	echo "=== configuring in virtex5fxt (\${HERE}/virtex5fxt) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/virtex5fxt/configure \$@"
fi

if test ! -d \${HERE}/virtex5fxt; then
	mkdir \${HERE}/virtex5fxt
fi
cd \${HERE}/virtex5fxt
\${MY_DIR}/virtex5fxt/configure "\$@"
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
all: virtex5fxt-all
clean: genisslib-clean virtex5fxt-clean
distclean: genisslib-distclean virtex5fxt-distclean
	rm -f \${HERE}/Makefile.cross
install: virtex5fxt-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
virtex5fxt-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/virtex5fxt all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
virtex5fxt-clean:
	@\\\$(MAKE) -C \${HERE}/virtex5fxt clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
virtex5fxt-distclean:
	@\\\$(MAKE) -C \${HERE}/virtex5fxt distclean
virtex5fxt-install:
	@\\\$(MAKE) -C \${HERE}/virtex5fxt install
EOF_MAKEFILE_CROSS

chmod +x Makefile.cross

echo "\$(basename \$0): run 'make -f \${HERE}/Makefile.cross' or '\${HERE}/Makefile.cross' to build for \${host} host system type"
EOF_CONFIGURE_CROSS
	chmod +x "${CONFIGURE_CROSS}"
fi  # has_to_build_configure_cross = "yes"

# GENISSLIB

GENISSLIB_CONFIGURE_AC="${DEST_DIR}/genisslib/configure.ac"
GENISSLIB_MAKEFILE_AM="${DEST_DIR}/genisslib/Makefile.am"


if [ ! -e "${GENISSLIB_CONFIGURE_AC}" ]; then
	has_to_build_genisslib_configure=yes
else
	if [ "$0" -nt "${GENISSLIB_CONFIGURE_AC}" ]; then
		has_to_build_genisslib_configure=yes
	fi
fi

if [ ! -e "${GENISSLIB_MAKEFILE_AM}" ]; then
	has_to_build_genisslib_configure=yes
else
	if [ "$0" -nt "${GENISSLIB_MAKEFILE_AM}" ]; then
		has_to_build_genisslib_configure=yes
	fi
fi

if [ "${has_to_build_genisslib_configure}" = "yes" ]; then
	echo "Generating GENISSLIB configure.ac"
	echo "AC_INIT([UNISIM GENISSLIB], [${GENISSLIB_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])" > "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${GENISSLIB_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LEXER_GENERATOR" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "UNISIM_CHECK_PARSER_GENERATOR" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${GENISSLIB_CONFIGURE_AC}"

	AM_GENISSLIB_VERSION=$(printf ${GENISSLIB_VERSION} | sed -e 's/\./_/g')
	echo "Generating GENISSLIB Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I m4" > "${GENISSLIB_MAKEFILE_AM}"
	echo "BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_YFLAGS = -d -p yy" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_LFLAGS = -l" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_PROGRAMS = genisslib" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_CPPFLAGS = -DGENISSLIB_VERSION=\\\"${GENISSLIB_VERSION}\\\"" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_TOOLS_GENISSLIB_M4_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
# The following lines are a workaround caused by a bugFix in AUTOMAKE 1.12
# Note that parser_tokens.hh has been added to BUILT_SOURCES above
# assumption: parser.cc and either parser.h or parser.hh are generated at the same time
    echo "\$(top_builddir)/parser_tokens.hh: \$(top_builddir)/parser.cc" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\tif test -f \"\$(top_builddir)/parser.h\"; then \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\t\tcp -f \"\$(top_builddir)/parser.h\" \"\$(top_builddir)/parser_tokens.hh\"; \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\telif test -f \"\$(top_builddir)/parser.hh\"; then \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\t\tcp -f \"\$(top_builddir)/parser.hh\" \"\$(top_builddir)/parser_tokens.hh\"; \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\tfi\n" >> "${GENISSLIB_MAKEFILE_AM}"

    echo "Building GENISSLIB configure"
	${SHELL} -c "cd ${DEST_DIR}/genisslib && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi


# virtex5fxt

VIRTEX5FXT_CONFIGURE_AC="${DEST_DIR}/virtex5fxt/configure.ac"
VIRTEX5FXT_MAKEFILE_AM="${DEST_DIR}/virtex5fxt/Makefile.am"


if [ ! -e "${VIRTEX5FXT_CONFIGURE_AC}" ]; then
	has_to_build_virtex5fxt_configure=yes
else
	if [ "$0" -nt "${VIRTEX5FXT_CONFIGURE_AC}" ]; then
		has_to_build_virtex5fxt_configure=yes
	fi
fi

if [ ! -e "${VIRTEX5FXT_MAKEFILE_AM}" ]; then
	has_to_build_virtex5fxt_configure=yes
else
	if [ "$0" -nt "${VIRTEX5FXT_MAKEFILE_AM}" ]; then
		has_to_build_virtex5fxt_configure=yes
	fi
fi

if [ "${has_to_build_virtex5fxt_configure}" = "yes" ]; then
	echo "Generating virtex5fxt configure.ac"
	echo "AC_INIT([UNISIM Virtex 5 FXT Standalone simulator], [${VIRTEX5FXT_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-virtex5fxt-core])" > "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "PKG_PROG_PKG_CONFIG([0.26])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "CPPFLAGS=\"${CPPFLAGS} -D_LARGEFILE64_SOURCE\"" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${VIRTEX5FXT_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "case \"\${host}\" in" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	printf "\t*mingw*)\n" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	printf "\tCPPFLAGS=\"-U__STRICT_ANSI__ \${CPPFLAGS}\"\n" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	printf "\t;;\n" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	printf "\t*)\n" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	printf "\tUNISIM_CHECK_PTHREAD(main)\n" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	printf "\t;;\n" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "esac" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TIMES(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ENDIAN(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CACTI(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_REAL_PATH(main)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AX_BOOST_BASE([1.53.0], AC_MSG_NOTICE([boost >= 1.53.0 found.]), AC_MSG_ERROR([boost >= 1.53.0 not found.]))" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "CPPFLAGS=\"\${BOOST_CPPFLAGS} \${CPPFLAGS}\"" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "LDFLAGS=\"\${BOOST_LDFLAGS} \${LDFLAGS}\"" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-virtex5fxt-${VIRTEX5FXT_VERSION}\"], [path of shared data relative to bin directory])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${VIRTEX5FXT_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${VIRTEX5FXT_CONFIGURE_AC}"

	AM_VIRTEX5FXT_VERSION=$(printf ${VIRTEX5FXT_VERSION} | sed -e 's/\./_/g')
	echo "Generating virtex5fxt Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I m4" > "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libunisim-virtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "libunisim_virtex5fxt_${AM_VIRTEX5FXT_VERSION}_a_SOURCES = ${UNISIM_LIB_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "bin_PROGRAMS = unisim-virtex5fxt-wfpu-${VIRTEX5FXT_VERSION}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	if [ "${LIGHT_DIST}" != "yes" ]; then
		echo " unisim-virtex5fxt-${VIRTEX5FXT_VERSION} unisim-virtex5fxt-wocache-${VIRTEX5FXT_VERSION} unisim-virtex5fxt-debug-${VIRTEX5FXT_VERSION} unisim-virtex5fxt-wocache-debug-${VIRTEX5FXT_VERSION} unisim-virtex5fxt-wfpu-wocache-${VIRTEX5FXT_VERSION} unisim-virtex5fxt-wfpu-debug-${VIRTEX5FXT_VERSION} unisim-virtex5fxt-wfpu-wocache-debug-${VIRTEX5FXT_VERSION}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	else
		echo "" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	fi
	echo "unisim_virtex5fxt_wfpu_${AM_VIRTEX5FXT_VERSION}_SOURCES = main_wfpu.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "unisim_virtex5fxt_wfpu_${AM_VIRTEX5FXT_VERSION}_LDADD = libunisim-virtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	if [ "${LIGHT_DIST}" != "yes" ]; then
		echo "unisim_virtex5fxt_${AM_VIRTEX5FXT_VERSION}_SOURCES = main.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_wocache_${AM_VIRTEX5FXT_VERSION}_SOURCES = main_wocache.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_wfpu_wocache_${AM_VIRTEX5FXT_VERSION}_SOURCES = main_wfpu_wocache.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_debug_${AM_VIRTEX5FXT_VERSION}_SOURCES = main_debug.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_wocache_debug_${AM_VIRTEX5FXT_VERSION}_SOURCES = main_wocache_debug.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_wfpu_debug_${AM_VIRTEX5FXT_VERSION}_SOURCES = main_wfpu_debug.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_wfpu_wocache_debug_${AM_VIRTEX5FXT_VERSION}_SOURCES = main_wfpu_wocache_debug.cc ${UNISIM_SIMULATORS_VIRTEX5FXT_SOURCE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_${AM_VIRTEX5FXT_VERSION}_LDADD = libunisim-virtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_wocache_${AM_VIRTEX5FXT_VERSION}_LDADD = libunisim-virtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_wfpu_wocache_${AM_VIRTEX5FXT_VERSION}_LDADD = libunisim-virtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_debug_${AM_VIRTEX5FXT_VERSION}_LDADD = libunisim-virtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_wocache_debug_${AM_VIRTEX5FXT_VERSION}_LDADD = libunisim-virtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_wfpu_debug_${AM_VIRTEX5FXT_VERSION}_LDADD = libunisim-virtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
		echo "unisim_virtex5fxt_wfpu_wocache_debug_${AM_VIRTEX5FXT_VERSION}_LDADD = libunisim-virtex5fxt-${VIRTEX5FXT_VERSION}.a" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	fi

	echo "noinst_HEADERS = ${UNISIM_LIB_VIRTEX5FXT_HEADER_FILES} ${UNISIM_LIB_VIRTEX5FXT_TEMPLATE_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_HEADER_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_TEMPLATE_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_VIRTEX5FXT_M4_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-virtex5fxt-${VIRTEX5FXT_VERSION}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_VIRTEX5FXT_DATA_FILES} ${UNISIM_SIMULATORS_VIRTEX5FXT_DATA_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh \$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.tcc" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh \$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.tcc" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.tcc: \$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh: ${UNISIM_LIB_VIRTEX5FXT_ISA_FILES}" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\t" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/powerpc/ppc440/isa/ppc440.isa" >> "${VIRTEX5FXT_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/bin/\$\$(basename \$\${PROGRAM})\"; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_builddir)/\$\${PROGRAM}\" \$(top_builddir)/bin/\$\$(basename \"\$\${PROGRAM}\"); \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tdone\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tfi\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-virtex5fxt-${VIRTEX5FXT_VERSION}/\$\$(basename \$\${SHARED_DATA})\"; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-virtex5fxt-${VIRTEX5FXT_VERSION}'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_srcdir)/\$\${SHARED_DATA}\" \$(top_builddir)/share/unisim-virtex5fxt-${VIRTEX5FXT_VERSION}/\$\$(basename \"\$\${SHARED_DATA}\"); \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tdone\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"
	printf "\tfi\n" >> "${VIRTEX5FXT_MAKEFILE_AM}"

	echo "Building powerpc configure"
	${SHELL} -c "cd ${DEST_DIR}/virtex5fxt && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
