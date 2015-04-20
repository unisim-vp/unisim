#!/bin/bash
VERSION_MAJOR=$(cat ieee1666/util/version.h | sed -n 's/#define SC_VERSION_MAJOR \([0-9][0-9]*\).*/\1/p')
VERSION_MINOR=$(cat ieee1666/util/version.h | sed -n 's/#define SC_VERSION_MINOR \([0-9][0-9]*\).*/\1/p')
VERSION_PATCH=$(cat ieee1666/util/version.h | sed -n 's/#define SC_VERSION_PATCH \([0-9][0-9]*\).*/\1/p')
VERSION="${VERSION_MAJOR}\\.${VERSION_MINOR}\\.${VERSION_PATCH}"
sed -i "s/AC_INIT(\[libieee1666\], \[[0-9\.]*\])/AC_INIT(\[libieee1666\], \[${VERSION}\])/" configure.ac
mkdir -p config
libtoolize
aclocal -I m4
autoheader
autoconf
automake -ac
