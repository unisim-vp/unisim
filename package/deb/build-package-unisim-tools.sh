#!/bin/sh
PACKAGE_NAME="unisim-tools" \
VERSION="1.0-1" \
ARCH="i386" \
DEPS="libc6 (>= 2.5)" \
MAINTAINER="Gilles Mouchard <gilles.mouchard@cea.fr>" \
DESCRIPTION="UNISIM tools" \
INSTALL_PATH="usr" \
./build-package.sh "CXX=g++" "CXXFLAGS=-m32 -O3 -g3" "CPPFLAGS=-I/usr/include/libxml2"
