#!/bin/sh
SOURCE_DIR=../../build_tool \
PACKAGE_NAME="unisim-build-tool" \
VERSION="1.0-1" \
ARCH="i386" \
DEPS="libc6 (>= 2.5)" \
MAINTAINER="Gilles Mouchard <gilles.mouchard@cea.fr>" \
DESCRIPTION="UNISIM build tools" \
INSTALL_PATH="usr" \
./build-package.sh "CXXFLAGS=-m32 -O3 -g3" "CPPFLAGS=-I/usr/include/libxml2"
