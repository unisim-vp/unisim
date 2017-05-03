#!/bin/bash
function Usage
{
	echo "Usage:"
	echo "  $0 <destination directory>"
}

if [ -z "$1" ]; then
	Usage
	exit -1
fi

MY_DIR=$(cd $(dirname $0); pwd)
DEST_DIR=$1
mkdir -p ${DEST_DIR}
DEST_DIR=$(cd $DEST_DIR; pwd)
UNISIM_DIR=$(cd ${MY_DIR}/..; pwd)
UNISIM_TOOLS_DIR=${UNISIM_DIR}/unisim_tools
UNISIM_LIB_DIR=${UNISIM_DIR}/unisim_lib
UNISIM_SIMULATOR_DIR=${UNISIM_DIR}/unisim_simulators/cxx/arm_simtest

ARM_SIMTEST_VERSION=$(cat ${UNISIM_SIMULATOR_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-arm_simtest-${ARM_SIMTEST_VERSION}

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

UNISIM_LIB_ARM_SIMTEST_SOURCE_FILES="\
unisim/component/cxx/processor/arm/disasm.cc \
unisim/util/symbolic/symbolic.cc \
unisim/util/random/random.cc \
"

UNISIM_LIB_ARM_SIMTEST_ISA_THUMB_FILES="\
unisim/component/cxx/processor/arm/isa/thumb/exception.isa \
unisim/component/cxx/processor/arm/isa/thumb/load_store.isa \
unisim/component/cxx/processor/arm/isa/thumb/multiply.isa \
unisim/component/cxx/processor/arm/isa/thumb/misc_arithmetic.isa \
unisim/component/cxx/processor/arm/isa/thumb/branch.isa \
unisim/component/cxx/processor/arm/isa/thumb/branch_T1.isa \
unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa \
unisim/component/cxx/processor/arm/isa/thumb/ordering.isa \
unisim/component/cxx/processor/arm/isa/thumb/profiling.isa \
unisim/component/cxx/processor/arm/isa/thumb2/branch.isa \
unisim/component/cxx/processor/arm/isa/thumb2/coprocessor.isa \
unisim/component/cxx/processor/arm/isa/thumb2/data_processing.isa \
unisim/component/cxx/processor/arm/isa/thumb2/exception.isa \
unisim/component/cxx/processor/arm/isa/thumb2/hints.isa \
unisim/component/cxx/processor/arm/isa/thumb2/load_store.isa \
unisim/component/cxx/processor/arm/isa/thumb2/misc_arithmetic.isa \
unisim/component/cxx/processor/arm/isa/thumb2/multiply.isa \
unisim/component/cxx/processor/arm/isa/thumb2/neon.isa \
unisim/component/cxx/processor/arm/isa/thumb2/ordering.isa \
unisim/component/cxx/processor/arm/isa/thumb2/status_register_access.isa \
unisim/component/cxx/processor/arm/isa/thumb2/vfp.isa \
unisim/component/cxx/processor/arm/isa/thumb2/xscale.isa"

UNISIM_LIB_ARM_SIMTEST_ISA_ARM32_FILES="\
unisim/component/cxx/processor/arm/isa/arm32/branch.isa \
unisim/component/cxx/processor/arm/isa/arm32/coprocessor.isa \
unisim/component/cxx/processor/arm/isa/arm32/data_processing.isa \
unisim/component/cxx/processor/arm/isa/arm32/dependency.isa \
unisim/component/cxx/processor/arm/isa/arm32/exception.isa \
unisim/component/cxx/processor/arm/isa/arm32/execution_latency.isa \
unisim/component/cxx/processor/arm/isa/arm32/hints.isa \
unisim/component/cxx/processor/arm/isa/arm32/load_store.isa \
unisim/component/cxx/processor/arm/isa/arm32/misc_arithmetic.isa \
unisim/component/cxx/processor/arm/isa/arm32/multiply.isa \
unisim/component/cxx/processor/arm/isa/arm32/neon.isa \
unisim/component/cxx/processor/arm/isa/arm32/ordering.isa \
unisim/component/cxx/processor/arm/isa/arm32/profiling.isa \
unisim/component/cxx/processor/arm/isa/arm32/specialization.isa \
unisim/component/cxx/processor/arm/isa/arm32/status_register_access.isa \
unisim/component/cxx/processor/arm/isa/arm32/vfp.isa \
unisim/component/cxx/processor/arm/isa/arm32/xscale.isa \
"

UNISIM_LIB_ARM_SIMTEST_ISA_FILES="${UNISIM_LIB_ARM_SIMTEST_ISA_THUMB_FILES} ${UNISIM_LIB_ARM_SIMTEST_ISA_ARM32_FILES}"

UNISIM_LIB_ARM_SIMTEST_HEADER_FILES="${UNISIM_LIB_ARM_SIMTEST_ISA_FILES} \
unisim/util/truth_table/truth_table.hh \
unisim/util/endian/endian.hh \
unisim/util/inlining/inlining.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/symbolic/symbolic.hh \
unisim/util/random/random.hh \
unisim/component/cxx/processor/arm/disasm.hh \
unisim/component/cxx/processor/arm/exception.hh \
unisim/component/cxx/processor/arm/execute.hh \
unisim/component/cxx/processor/arm/models.hh \
unisim/component/cxx/processor/arm/psr.hh \
unisim/component/cxx/processor/arm/register_field.hh \
"

UNISIM_LIB_ARM_SIMTEST_TEMPLATE_FILES="\
"

UNISIM_LIB_ARM_SIMTEST_M4_FILES="\
"

UNISIM_LIB_ARM_SIMTEST_DATA_FILES="\
"

ARM_SIMTEST_EXTERNAL_HEADERS="\
assert.h \
ctype.h \
cxxabi.h \
errno.h \
fcntl.h \
fenv.h \
float.h \
fstream \
getopt.h \
inttypes.h \
limits.h \
math.h \
signal.h \
stdarg.h \
stdio.h \
stdlib.h \
string.h \
sys/types.h \
unistd.h \
cassert \
cerrno \
cstddef \
cstdio \
cstdlib \
cstring \
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
string"

UNISIM_SIMULATOR_ARM_SIMTEST_ISA_THUMB_FILES="\
top_thumb.isa \
"
UNISIM_SIMULATOR_ARM_SIMTEST_ISA_ARM32_FILES="\
top_arm32.isa \
"

UNISIM_SIMULATOR_ARM_SIMTEST_ISA_FILES="${UNISIM_SIMULATOR_ARM_SIMTEST_ISA_THUMB_FILES} ${UNISIM_SIMULATOR_ARM_SIMTEST_ISA_ARM32_FILES}"

UNISIM_SIMULATOR_ARM_SIMTEST_SOURCE_FILES="\
main.cc \
arch.cc \
"
UNISIM_SIMULATOR_ARM_SIMTEST_HEADER_FILES="${UNISIM_SIMULATOR_ARM_SIMTEST_ISA_FILES} \
arch.hh \
testutils.hh \
"

UNISIM_SIMULATOR_ARM_SIMTEST_EXTRA_FILES="\
config.h.in \
"

UNISIM_SIMULATOR_ARM_SIMTEST_TEMPLATE_FILES=
UNISIM_SIMULATOR_ARM_SIMTEST_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_ARM_SIMTEST_TESTBENCH_FILES=""

UNISIM_ARM_SIMTEST_ISA_ARM32_FILES="${UNISIM_LIB_ARM_SIMTEST_ISA_ARM32_FILES} ${UNISIM_SIMULATOR_ARM_SIMTEST_ISA_ARM32_FILES}"
UNISIM_ARM_SIMTEST_ISA_THUMB_FILES="${UNISIM_LIB_ARM_SIMTEST_ISA_THUMB_FILES} ${UNISIM_SIMULATOR_ARM_SIMTEST_ISA_THUMB_FILES}"

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_arm_simtest_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/arm_simtest

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

UNISIM_LIB_ARM_SIMTEST_FILES="${UNISIM_LIB_ARM_SIMTEST_SOURCE_FILES} ${UNISIM_LIB_ARM_SIMTEST_HEADER_FILES} ${UNISIM_LIB_ARM_SIMTEST_TEMPLATE_FILES} ${UNISIM_LIB_ARM_SIMTEST_DATA_FILES}"

for file in ${UNISIM_LIB_ARM_SIMTEST_FILES}; do
	mkdir -p "${DEST_DIR}/arm_simtest/$(dirname ${file})"
	has_to_copy=no
	if [ -e "${DEST_DIR}/arm_simtest/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/arm_simtest/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/arm_simtest/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/arm_simtest/${file}" || exit
	fi
done

UNISIM_SIMULATOR_ARM_SIMTEST_FILES="${UNISIM_SIMULATOR_ARM_SIMTEST_SOURCE_FILES} ${UNISIM_SIMULATOR_ARM_SIMTEST_HEADER_FILES} ${UNISIM_SIMULATOR_ARM_SIMTEST_EXTRA_FILES} ${UNISIM_SIMULATOR_ARM_SIMTEST_TEMPLATE_FILES} ${UNISIM_SIMULATOR_ARM_SIMTEST_DATA_FILES} ${UNISIM_SIMULATOR_ARM_SIMTEST_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATOR_ARM_SIMTEST_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/arm_simtest/${file}" ]; then
		if [ "${UNISIM_SIMULATOR_DIR}/${file}" -nt "${DEST_DIR}/arm_simtest/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATOR_DIR}/${file} ==> ${DEST_DIR}/arm_simtest/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/arm_simtest/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATOR_ARM_SIMTEST_DATA_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/${file}" ]; then
		if [ "${UNISIM_SIMULATOR_DIR}/${file}" -nt "${DEST_DIR}/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATOR_DIR}/${file} ==> ${DEST_DIR}/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}" || exit
	fi
done


mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/arm_simtest/config
mkdir -p ${DEST_DIR}/arm_simtest/m4
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

for file in ${UNISIM_LIB_ARM_SIMTEST_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/arm_simtest/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/arm_simtest/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/arm_simtest/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/arm_simtest/${file}" || exit
		has_to_build_arm_simtest_configure=yes
	fi
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Yves Lhuillier <yves.lhuillier@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - arm_simtest: an ARM V5 user level simulator
  - GenISSLib (will not be installed): an instruction set simulator generator
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


Building instructions:
  $ ./configure
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
	echo "AC_INIT([UNISIM Arm_Simtest Standalone simulator], [${ARM_SIMTEST_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-arm_simtest])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([arm_simtest])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib arm_simtest" > "${MAKEFILE_AM}"
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
	echo "=== configure help for arm_simtest"
else
	echo "=== configuring in arm_simtest (\${HERE}/arm_simtest) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/arm_simtest/configure \$@"
fi

if test ! -d \${HERE}/arm_simtest; then
	mkdir \${HERE}/arm_simtest
fi
cd \${HERE}/arm_simtest
\${MY_DIR}/arm_simtest/configure "\$@"
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
all: arm_simtest-all
clean: genisslib-clean arm_simtest-clean
distclean: genisslib-distclean arm_simtest-distclean
	rm -f \${HERE}/Makefile.cross
install: arm_simtest-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
arm_simtest-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/arm_simtest all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
arm_simtest-clean:
	@\\\$(MAKE) -C \${HERE}/arm_simtest clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
arm_simtest-distclean:
	@\\\$(MAKE) -C \${HERE}/arm_simtest distclean
arm_simtest-install:
	@\\\$(MAKE) -C \${HERE}/arm_simtest install
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
	echo "AC_INIT([UNISIM GENISSLIB], [${GENISSLIB_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])" > "${GENISSLIB_CONFIGURE_AC}"
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
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${GENISSLIB_MAKEFILE_AM}"
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


# arm_simtest

ARM_SIMTEST_CONFIGURE_AC="${DEST_DIR}/arm_simtest/configure.ac"
ARM_SIMTEST_MAKEFILE_AM="${DEST_DIR}/arm_simtest/Makefile.am"


if [ ! -e "${ARM_SIMTEST_CONFIGURE_AC}" ]; then
	has_to_build_arm_simtest_configure=yes
else
	if [ "$0" -nt "${ARM_SIMTEST_CONFIGURE_AC}" ]; then
		has_to_build_arm_simtest_configure=yes
	fi
fi

if [ ! -e "${ARM_SIMTEST_MAKEFILE_AM}" ]; then
	has_to_build_arm_simtest_configure=yes
else
	if [ "$0" -nt "${ARM_SIMTEST_MAKEFILE_AM}" ]; then
		has_to_build_arm_simtest_configure=yes
	fi
fi

if [ "${has_to_build_arm_simtest_configure}" = "yes" ]; then
	echo "Generating arm_simtest configure.ac"
	echo "AC_INIT([UNISIM Arm_Simtest C++ simulator], [${ARM_SIMTEST_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-arm_simtest-core])" > "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${ARM_SIMTEST_CONFIGURE_AC}"
        echo "CXXFLAGS=\"\$CXXFLAGS -std=c++11\"" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${ARM_SIMTEST_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-arm_simtest-${ARM_SIMTEST_VERSION}\"], [path of shared data relative to bin directory])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	SIM_VERSION_MAJOR=$(printf "${ARM_SIMTEST_VERSION}" | cut -f 1 -d .)
	SIM_VERSION_MINOR=$(printf "${ARM_SIMTEST_VERSION}" | cut -f 2 -d .)
	SIM_VERSION_PATCH=$(printf "${ARM_SIMTEST_VERSION}" | cut -f 3 -d .)
	SIM_VERSION="${ARM_SIMTEST_VERSION}"
	SIM_VERSION_CODENAME="Triumphalis Tarraco"
	SIM_AUTHOR="Yves Lhuillier (yves.lhuillier@cea.fr)"
	SIM_PROGRAM_NAME="UNISIM Arm_Simtest"
	SIM_LICENSE="BSD (See file COPYING)"
	SIM_COPYRIGHT="Copyright (C) 2007-2017, Commissariat a l'Energie Atomique"
	SIM_DESCRIPTION="UNISIM ARM SELF SIMULATOR TEST GENERATION"
	SIM_SCHEMATIC="arm_simtest/fig_schematic.pdf"
	echo "AC_DEFINE([SIM_VERSION_MAJOR], [${SIM_VERSION_MAJOR}], [Version major number])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION_MINOR], [${SIM_VERSION_MINOR}], [Version minor number])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION_PATCH], [${SIM_VERSION_PATCH}], [Version patch number])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION], [\"${SIM_VERSION}\"], [Version])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_VERSION_CODENAME], [\"${SIM_VERSION_CODENAME}\"], [Version code name])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_AUTHOR], [\"${SIM_AUTHOR}\"], [Author])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_PROGRAM_NAME], [\"${SIM_PROGRAM_NAME}\"], [Program name])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_COPYRIGHT], [\"${SIM_COPYRIGHT}\"], [Copyright])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_LICENSE], [\"${SIM_LICENSE}\"], [License])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_DESCRIPTION], [\"${SIM_DESCRIPTION}\"], [Description])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_DEFINE([SIM_SCHEMATIC], [\"${SIM_SCHEMATIC}\"], [Schematic])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${ARM_SIMTEST_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${ARM_SIMTEST_CONFIGURE_AC}"

	AM_ARM_SIMTEST_VERSION=$(printf ${ARM_SIMTEST_VERSION} | sed -e 's/\./_/g')
	echo "Generating arm_simtest Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libarm_simtest-${ARM_SIMTEST_VERSION}.a" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "libarm_simtest_${AM_ARM_SIMTEST_VERSION}_a_SOURCES = ${UNISIM_LIB_ARM_SIMTEST_SOURCE_FILES}" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-arm_simtest-${ARM_SIMTEST_VERSION}" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "unisim_arm_simtest_${AM_ARM_SIMTEST_VERSION}_SOURCES = ${UNISIM_SIMULATOR_ARM_SIMTEST_SOURCE_FILES}" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "unisim_arm_simtest_${AM_ARM_SIMTEST_VERSION}_CPPFLAGS = -DSIM_EXECUTABLE" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "unisim_arm_simtest_${AM_ARM_SIMTEST_VERSION}_LDADD = libarm_simtest-${ARM_SIMTEST_VERSION}.a" >> "${ARM_SIMTEST_MAKEFILE_AM}"

	echo "noinst_HEADERS = ${UNISIM_LIB_ARM_SIMTEST_HEADER_FILES} ${UNISIM_LIB_ARM_SIMTEST_TEMPLATE_FILES} ${UNISIM_SIMULATOR_ARM_SIMTEST_HEADER_FILES} ${UNISIM_SIMULATOR_ARM_SIMTEST_TEMPLATE_FILES}" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_ARM_SIMTEST_M4_FILES}" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-arm_simtest-${ARM_SIMTEST_VERSION}" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_ARM_SIMTEST_DATA_FILES} ${UNISIM_SIMULATOR_ARM_SIMTEST_DATA_FILES}" >> "${ARM_SIMTEST_MAKEFILE_AM}"

	echo -n "BUILT_SOURCES=" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/top_arm32.hh " >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/top_arm32.tcc " >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/top_thumb.hh " >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/top_thumb.tcc " >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo >> "${ARM_SIMTEST_MAKEFILE_AM}"
	
	echo -n "CLEANFILES=" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/top_arm32.hh " >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/top_arm32.tcc " >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/top_thumb.hh " >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo -n "\$(top_builddir)/top_thumb.tcc " >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo >> "${ARM_SIMTEST_MAKEFILE_AM}"
	
	echo "\$(top_builddir)/top_arm32.tcc: \$(top_builddir)/top_arm32.hh" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "\$(top_builddir)/top_arm32.hh: ${UNISIM_ARM_SIMTEST_ISA_ARM32_FILES}" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\t" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/top_arm32 -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_arm32.isa" >> "${ARM_SIMTEST_MAKEFILE_AM}"

	echo "\$(top_builddir)/top_thumb.tcc: \$(top_builddir)/top_thumb.hh" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "\$(top_builddir)/top_thumb.hh: ${UNISIM_ARM_SIMTEST_ISA_THUMB_FILES}" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\t" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/top_thumb -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_thumb.isa" >> "${ARM_SIMTEST_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/bin/\$\$(basename \$\${PROGRAM})\"; \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_builddir)/\$\${PROGRAM}\" \$(top_builddir)/bin/\$\$(basename \"\$\${PROGRAM}\"); \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\tdone\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\tfi\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-arm_simtest-${ARM_SIMTEST_VERSION}/\$\$(basename \$\${SHARED_DATA})\"; \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-arm_simtest-${ARM_SIMTEST_VERSION}'; \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_srcdir)/\$\${SHARED_DATA}\" \$(top_builddir)/share/unisim-arm_simtest-${ARM_SIMTEST_VERSION}/\$\$(basename \"\$\${SHARED_DATA}\"); \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\tdone\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"
	printf "\tfi\n" >> "${ARM_SIMTEST_MAKEFILE_AM}"

	${DISTCOPY} ${DEST_DIR}/INSTALL ${DEST_DIR}/arm_simtest
	${DISTCOPY} ${DEST_DIR}/README ${DEST_DIR}/arm_simtest
	${DISTCOPY} ${DEST_DIR}/AUTHORS ${DEST_DIR}/arm_simtest
	
	echo "Building arm_simtest configure"
	${SHELL} -c "cd ${DEST_DIR}/arm_simtest && aclocal -I m4 && autoconf --force && automake -ac"
fi

echo "Distribution is up-to-date"
