#!/bin/sh
if test "x$1" = x || test "x$2" = x || test "x$3" = x || test "x$4" = x || test "x$5" = x ; then
    echo "Usage <UNISIM build_tool dir> <SystemC dir> <GNU/win32 dir> <SDL/win32 dir> <boost/win32 dir>"
    exit
fi

PACKAGE_NAME="unisim-lib" \
VERSION="1.0-1" \
MAINTAINER="Gilles Mouchard <gilles.mouchard@cea.fr>" \
DESCRIPTION="UNISIM Library" \
./build-package.sh "--with-unisim-tools=$1" "--with-systemc=$2" "CXXFLAGS=-m32 -O3 -g3 -DSC_INCLUDE_DYNAMIC_PROCESSES -DDEBUG_NETSTUB" "CPPFLAGS=-I$3/include -I$3/include/libxml2" "--with-sdl=$4" "--with-boost=$5"