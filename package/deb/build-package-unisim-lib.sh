#!/bin/sh
if test "x$1" = x || test "x$2" = x ; then
    echo "Usage <UNISIM build_tool dir> <SystemC dir>"
    exit
fi

VERSION="1.0-1" \
PACKAGE_NAME="unisim-lib" \
ARCH="i386" \
DEPS="libncurses5 (>= 5.5), libreadline5 (>= 5.2), libxml2 (>= 2.6.27), libsdl1.2debian (>= 1.2.11),  libstdc++6 (>= 4.1.2), libc6 (>= 2.5), libgcc1 (>= 4.1.2)" \
MAINTAINER="Gilles Mouchard <gilles.mouchard@cea.fr>" \
DESCRIPTION="UNISIM library" \
INSTALL_PATH="usr" \
./build-package.sh --host=i686-pc-linux-gnu "CXX=g++" "--with-unisim-tools=$1" "--with-systemc=$2" "CXXFLAGS=-m32 -O3 -g3 -DSC_INCLUDE_DYNAMIC_PROCESSES -DDEBUG_NETSTUB" "CPPFLAGS=-I/usr/include/libxml2"
