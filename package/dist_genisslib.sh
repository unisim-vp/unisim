#!/bin/bash

UNISIM_TOOLS_GENISSLIB_HEADER_FILES="\
action.hh \
cli.hh \
errtools.hh \
isa.hh \
riscgenerator.hh \
specialization.hh \
variable.hh \
bitfield.hh \
comment.hh \
fwd.hh \
main.hh \
product.hh \
scanner.hh \
clex.hh \
strtools.hh \
vect.hh \
ciscgenerator.hh \
conststr.hh \
generator.hh \
operation.hh \
referencecounting.hh \
sourcecode.hh \
subdecoder.hh \
"

UNISIM_TOOLS_GENISSLIB_SOURCE_FILES="\
scanner.cc \
clex.cc \
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
errtools.cc \
"

UNISIM_TOOLS_GENISSLIB_DATA_FILES="COPYING INSTALL NEWS README AUTHORS ChangeLog"

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
vector \
"

Usage()
{
	echo "Usage:"
	echo "  $0 <destination directory> [<unisim repository>]"
}

if [ -z "$1" ]; then
	Usage
	exit -1
fi

UNISIM_DIR=$(cd $(dirname $(dirname $0)); pwd)
mkdir -p "$1"
DEST_DIR=$(cd "$1"; pwd)
UNISIM_TOOLS_DIR=${UNISIM_DIR}/unisim_tools

GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)

if [ -z "${DISTCOPY}" ]; then
	DISTCOPY=cp
fi

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

has_to_build_genisslib_configure=no

UNISIM_TOOLS_GENISSLIB_FILES="${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES} ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES} ${UNISIM_TOOLS_GENISSLIB_DATA_FILES}"

for file in ${UNISIM_TOOLS_GENISSLIB_FILES}; do
	dist_copy "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/${file}"
done

mkdir -p ${DEST_DIR}/config

# Some imported files (m4 macros) impact configure generation
has_to_build_genisslib_configure=no

# GENISSLIB

GENISSLIB_CONFIGURE_AC="${DEST_DIR}/configure.ac"
GENISSLIB_MAKEFILE_AM="${DEST_DIR}/Makefile.am"

if has_to_build "${GENISSLIB_CONFIGURE_AC}" "$0"; then
	echo "Generating GENISSLIB configure.ac"
	cat <<EOF > "${GENISSLIB_CONFIGURE_AC}"
AC_INIT([UNISIM GENISSLIB], [${GENISSLIB_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])
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
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF
	has_to_build_genisslib_configure=yes
fi

if [ -z "${GILINSTALL}" ]; then
	GILINSTALL=bin
fi

if has_to_build "${GENISSLIB_MAKEFILE_AM}" "$0"; then
	AM_GENISSLIB_VERSION=$(printf ${GENISSLIB_VERSION} | sed -e 's/\./_/g')
	echo "Generating GENISSLIB Makefile.am"
	cat <<EOF > "${GENISSLIB_MAKEFILE_AM}"
AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)
${GILINSTALL}_PROGRAMS = genisslib
genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}
genisslib_CPPFLAGS = -DGENISSLIB_VERSION=\"${GENISSLIB_VERSION}\"
noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}
EOF
	has_to_build_genisslib_configure=yes
fi

if [ "${has_to_build_genisslib_configure}" = "yes" ]; then
	echo "Building GENISSLIB configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && autoheader && automake -ac && rm -rf autom4te.cache"
fi

echo "Distribution is up-to-date"
