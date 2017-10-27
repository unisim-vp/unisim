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

UNISIM_DIR=$(cd $(dirname $(dirname $0)); pwd)
DEST_DIR=$(cd $1; pwd)
mkdir -p ${DEST_DIR}
UNISIM_TOOLS_DIR=${UNISIM_DIR}/unisim_tools
UNISIM_LIB_DIR=${UNISIM_DIR}/unisim_lib
UNISIM_SIMULATOR_DIR=${UNISIM_DIR}/unisim_simulators/cxx/armsec

ARMSEC_VERSION=$(cat ${UNISIM_SIMULATOR_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-armsec-${ARMSEC_VERSION}

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

UNISIM_LIB_ARMSEC_SOURCE_FILES="\
unisim/component/cxx/processor/arm/disasm.cc \
unisim/util/symbolic/symbolic.cc \
"

UNISIM_LIB_ARMSEC_ISA_THUMB_FILES="\
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

UNISIM_LIB_ARMSEC_ISA_ARM32_FILES="\
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

UNISIM_LIB_ARMSEC_ISA_FILES="${UNISIM_LIB_ARMSEC_ISA_THUMB_FILES} ${UNISIM_LIB_ARMSEC_ISA_ARM32_FILES}"

UNISIM_LIB_ARMSEC_HEADER_FILES="${UNISIM_LIB_ARMSEC_ISA_FILES} \
unisim/util/truth_table/truth_table.hh \
unisim/util/endian/endian.hh \
unisim/util/inlining/inlining.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/symbolic/symbolic.hh \
unisim/component/cxx/processor/arm/disasm.hh \
unisim/component/cxx/processor/arm/exception.hh \
unisim/component/cxx/processor/arm/execute.hh \
unisim/component/cxx/processor/arm/models.hh \
unisim/component/cxx/processor/arm/psr.hh \
unisim/component/cxx/processor/arm/register_field.hh \
"

UNISIM_LIB_ARMSEC_TEMPLATE_FILES="\
"

UNISIM_LIB_ARMSEC_M4_FILES="\
"

UNISIM_LIB_ARMSEC_DATA_FILES="\
"

ARMSEC_EXTERNAL_HEADERS="\
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
string \
"

UNISIM_SIMULATOR_ARMSEC_ISA_THUMB_FILES="\
top_thumb.isa \
"
UNISIM_SIMULATOR_ARMSEC_ISA_ARM32_FILES="\
top_arm32.isa \
"

UNISIM_ARMSEC_ISA_ARM32_FILES="${UNISIM_LIB_ARMSEC_ISA_ARM32_FILES} ${UNISIM_SIMULATOR_ARMSEC_ISA_ARM32_FILES}"
UNISIM_ARMSEC_ISA_THUMB_FILES="${UNISIM_LIB_ARMSEC_ISA_THUMB_FILES} ${UNISIM_SIMULATOR_ARMSEC_ISA_THUMB_FILES}"

UNISIM_SIMULATOR_ARMSEC_SOURCE_FILES="\
main.cc \
"

UNISIM_SIMULATOR_ARMSEC_HEADER_FILES="\
${UNISIM_SIMULATOR_ARMSEC_ISA_THUMB_FILES} \
${UNISIM_SIMULATOR_ARMSEC_ISA_ARM32_FILES} \
"

UNISIM_SIMULATOR_ARMSEC_EXTRA_FILES="\
config.h.in \
thumb2plan.sh \
arm32plan.sh \
damien_issues.sh \
significant_tests.sh \
"

UNISIM_SIMULATOR_ARMSEC_TEMPLATE_FILES="\
"

UNISIM_SIMULATOR_ARMSEC_TOP_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_ARMSEC_DATA_FILES="\
COPYING \
README \
INSTALL \
AUTHORS \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_ARMSEC_TESTBENCH_FILES=""

has_to_build() {
    [ ! -e "$1" -o "$2" -nt "$1" ]
}

dist_copy() {
    if has_to_build "$2" "$1"; then
	echo "$1 ==> $2"
	mkdir -p "$(dirname $2)"
	${DISTCOPY} -f "$1" "$2" || exit
        true
    fi
    false
}

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/armsec

UNISIM_TOOLS_GENISSLIB_FILES="${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES} ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES} ${UNISIM_TOOLS_GENISSLIB_DATA_FILES}"

for file in ${UNISIM_TOOLS_GENISSLIB_FILES}; do
    dist_copy "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/genisslib/${file}"
done

UNISIM_LIB_ARMSEC_FILES="${UNISIM_LIB_ARMSEC_SOURCE_FILES} ${UNISIM_LIB_ARMSEC_HEADER_FILES} ${UNISIM_LIB_ARMSEC_TEMPLATE_FILES} ${UNISIM_LIB_ARMSEC_DATA_FILES}"

for file in ${UNISIM_LIB_ARMSEC_FILES}; do
    dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/armsec/${file}"
done

UNISIM_SIMULATOR_ARMSEC_FILES="${UNISIM_SIMULATOR_ARMSEC_SOURCE_FILES} ${UNISIM_SIMULATOR_ARMSEC_HEADER_FILES} ${UNISIM_SIMULATOR_ARMSEC_EXTRA_FILES} ${UNISIM_SIMULATOR_ARMSEC_TEMPLATE_FILES} ${UNISIM_SIMULATOR_ARMSEC_DATA_FILES} ${UNISIM_SIMULATOR_ARMSEC_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATOR_ARMSEC_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/armsec/${file}"
done

for file in ${UNISIM_SIMULATOR_ARMSEC_TOP_DATA_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/armsec/config
mkdir -p ${DEST_DIR}/armsec/m4
mkdir -p ${DEST_DIR}/genisslib/config
mkdir -p ${DEST_DIR}/genisslib/m4

has_to_build_genisslib_configure=no
for file in ${UNISIM_TOOLS_GENISSLIB_M4_FILES}; do
    if dist_copy "${UNISIM_TOOLS_DIR}/${file}" "${DEST_DIR}/genisslib/${file}"; then has_to_build_genisslib_configure=yes; fi
done

has_to_build_armsec_configure=no
for file in ${UNISIM_LIB_ARMSEC_M4_FILES}; do
    if dist_copy "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/armsec/${file}"; then has_to_build_armsec_configure=yes; fi
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>
Gilles Mouchard <gilles.mouchard@cea.fr>
Réda Nouacer <reda.nouacer@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - armsec: an ARM V5 user level simulator
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

if has_to_build "${CONFIGURE_AC}" "$0" || has_to_build "${MAKEFILE_AM}" "$0"; then
    echo "Generating configure.ac"
cat <<EOF > "${CONFIGURE_AC}"
AC_INIT([UNISIM Armsec Standalone simulator], [${ARMSEC_VERSION}], [Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>, Réda Nouacer <reda.nouacer@cea.fr>], [unisim-armsec])
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects tar-pax])
AC_PATH_PROGS(SH, sh)
AC_PROG_INSTALL
AC_PROG_LN_S
AC_CONFIG_SUBDIRS([genisslib]) 
AC_CONFIG_SUBDIRS([armsec]) 
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF

    echo "Generating Makefile.am"
cat <<EOF > "${MAKEFILE_AM}"
SUBDIRS=genisslib armsec
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
	echo "=== configure help for armsec"
else
	echo "=== configuring in armsec (\${HERE}/armsec) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/armsec/configure \$@"
fi

if test ! -d \${HERE}/armsec; then
	mkdir \${HERE}/armsec
fi
cd \${HERE}/armsec
\${MY_DIR}/armsec/configure "\$@"
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
all: armsec-all
clean: genisslib-clean armsec-clean
distclean: genisslib-distclean armsec-distclean
	rm -f \${HERE}/Makefile.cross
install: armsec-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
armsec-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/armsec all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
armsec-clean:
	@\\\$(MAKE) -C \${HERE}/armsec clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
armsec-distclean:
	@\\\$(MAKE) -C \${HERE}/armsec distclean
armsec-install:
	@\\\$(MAKE) -C \${HERE}/armsec install
EOF_MAKEFILE_CROSS

chmod +x Makefile.cross

echo "\$(basename \$0): run 'make -f \${HERE}/Makefile.cross' or '\${HERE}/Makefile.cross' to build for \${host} host system type"
EOF_CONFIGURE_CROSS
	chmod +x "${CONFIGURE_CROSS}"
fi  # has_to_build_configure_cross = "yes"

# GENISSLIB

GENISSLIB_CONFIGURE_AC="${DEST_DIR}/genisslib/configure.ac"
GENISSLIB_MAKEFILE_AM="${DEST_DIR}/genisslib/Makefile.am"

if has_to_build "${GENISSLIB_CONFIGURE_AC}" "$0" || has_to_build "${GENISSLIB_MAKEFILE_AM}" "$0"; then
    has_to_build_genisslib_configure=yes
fi

if [ "${has_to_build_genisslib_configure}" = "yes" ]; then
    echo "Generating GENISSLIB configure.ac"
    cat<<EOF > "${GENISSLIB_CONFIGURE_AC}"
AC_INIT([UNISIM GENISSLIB], [${GENISSLIB_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])
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
AC_PROG_LN_S
AC_LANG([C++])
AC_CHECK_HEADERS([${GENISSLIB_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))
UNISIM_CHECK_LEXER_GENERATOR
UNISIM_CHECK_PARSER_GENERATOR
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF

    AM_GENISSLIB_VERSION=$(printf ${GENISSLIB_VERSION} | sed -e 's/\./_/g')
    echo "Generating GENISSLIB Makefile.am"
    cat <<EOF > "${GENISSLIB_MAKEFILE_AM}"
ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4
BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}
CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}
AM_YFLAGS = -d -p yy
AM_LFLAGS = -l
AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)
noinst_PROGRAMS = genisslib
genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}
genisslib_CPPFLAGS = -DGENISSLIB_VERSION=\"${GENISSLIB_VERSION}\"
noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}
EXTRA_DIST = ${UNISIM_TOOLS_GENISSLIB_M4_FILES}
# The following lines are a workaround caused by a bugFix in AUTOMAKE 1.12
# Note that parser_tokens.hh has been added to BUILT_SOURCES above
# assumption: parser.cc and either parser.h or parser.hh are generated at the same time
\$(top_builddir)/parser_tokens.hh: \$(top_builddir)/parser.cc
	if test -f "\$(top_builddir)/parser.h"; then\
		cp -f "\$(top_builddir)/parser.h" "\$(top_builddir)/parser_tokens.hh";\
	elif test -f "\$(top_builddir)/parser.hh"; then\
		cp -f "\$(top_builddir)/parser.hh" "\$(top_builddir)/parser_tokens.hh";\
	fi
\$(top_builddir)/genisslib-scanner.o: CXXFLAGS+=-Wno-error
EOF

    echo "Building GENISSLIB configure"
    ${SHELL} -c "cd ${DEST_DIR}/genisslib && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi


# armsec

ARMSEC_CONFIGURE_AC="${DEST_DIR}/armsec/configure.ac"
ARMSEC_MAKEFILE_AM="${DEST_DIR}/armsec/Makefile.am"

if has_to_build "${ARMSEC_CONFIGURE_AC}" "$0" || has_to_build "${ARMSEC_MAKEFILE_AM}" "$0"; then
    has_to_build_armsec_configure=yes
fi

if [ "${has_to_build_armsec_configure}" = "yes" ]; then
	echo "Generating armsec configure.ac"
        cat<<EOF > "${ARMSEC_CONFIGURE_AC}"
AC_INIT([UNISIM Armsec C++ simulator], [${ARMSEC_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-armsec-core])
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
AC_CHECK_HEADERS([${ARMSEC_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))
case "\${host}" in
	*mingw*)
		CPPFLAGS="-U__STRICT_ANSI__ \${CPPFLAGS}"
		;;
	*)
		;;
esac
GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib
AC_SUBST(GENISSLIB_PATH)
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/unisim-armsec-${ARMSEC_VERSION}"], [path of shared data relative to bin directory])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF

    AM_ARMSEC_VERSION=$(printf ${ARMSEC_VERSION} | sed -e 's/\./_/g')
    echo "Generating armsec Makefile.am"
    cat<<EOF > "${ARMSEC_MAKEFILE_AM}"
ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4
AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)
noinst_LIBRARIES = libarmsec-${ARMSEC_VERSION}.a
libarmsec_${AM_ARMSEC_VERSION}_a_SOURCES = ${UNISIM_LIB_ARMSEC_SOURCE_FILES}

#armsec
bin_PROGRAMS = unisim-armsec-${ARMSEC_VERSION}
unisim_armsec_${AM_ARMSEC_VERSION}_SOURCES = ${UNISIM_SIMULATOR_ARMSEC_SOURCE_FILES}
unisim_armsec_${AM_ARMSEC_VERSION}_CPPFLAGS = -DSIM_EXECUTABLE
unisim_armsec_${AM_ARMSEC_VERSION}_LDADD = libarmsec-${ARMSEC_VERSION}.a

noinst_HEADERS = ${UNISIM_LIB_ARMSEC_HEADER_FILES} ${UNISIM_LIB_ARMSEC_TEMPLATE_FILES} ${UNISIM_SIMULATOR_ARMSEC_HEADER_FILES} ${UNISIM_SIMULATOR_ARMSEC_TEMPLATE_FILES}
EXTRA_DIST = ${UNISIM_LIB_ARMSEC_M4_FILES}
sharedir = \$(prefix)/share/unisim-armsec-${ARMSEC_VERSION}
dist_share_DATA = ${UNISIM_LIB_ARMSEC_DATA_FILES} ${UNISIM_SIMULATOR_ARMSEC_DATA_FILES}

BUILT_SOURCES=\
    \$(top_builddir)/top_arm32.hh\
    \$(top_builddir)/top_arm32.tcc\
    \$(top_builddir)/top_thumb.hh\
    \$(top_builddir)/top_thumb.tcc\
	
CLEANFILES=\
    \$(top_builddir)/top_arm32.hh\
    \$(top_builddir)/top_arm32.tcc\
    \$(top_builddir)/top_thumb.hh\
    \$(top_builddir)/top_thumb.tcc\
	
\$(top_builddir)/top_arm32.tcc: \$(top_builddir)/top_arm32.hh
\$(top_builddir)/top_arm32.hh: ${UNISIM_ARMSEC_ISA_ARM32_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/top_arm32 -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_arm32.isa

\$(top_builddir)/top_thumb.tcc: \$(top_builddir)/top_thumb.hh
\$(top_builddir)/top_thumb.hh: ${UNISIM_ARMSEC_ISA_THUMB_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/top_thumb -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_thumb.isa
     
EOF

    echo "Building armsec configure"
    ${SHELL} -c "cd ${DEST_DIR}/armsec && aclocal -I m4 && autoconf --force && automake -ac"
fi

echo "Distribution is up-to-date"
