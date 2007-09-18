#!/bin/sh
if test "x$1" == x || test "x$2" == x || test "x$3" == x; then
    echo "Usage <source dir> <UNISIM build_tool dir> <SystemC dir>"
    exit
fi

PACKAGE_NAME="unisim"
VERSION="1.0-1"
ARCH="i386"
deps="libncurses5 (>= 5.5), libreadline5 (>= 5.2), libxml2 (>= 2.6.27), libsdl1.2debian (>= 1.2.11),  libstdc++6 (>= 4.1.2), libc6 (>= 2.5), libgcc1 (>= 4.1.2)"
maintainer="Gilles Mouchard <gilles.mouchard@cea.fr>"
description="UNISIM build tools"
install_path="usr"

PACKAGE_LONG_NAME=${PACKAGE_NAME}_${VERSION}_${ARCH}
PACKAGE_FILENAME=${PACKAGE_LONG_NAME}.deb
HERE=`pwd`
TEMPORARY_INSTALL_DIR=$HOME/tmp/${PACKAGE_LONG_NAME}
TEMPORARY_CONFIG_DIR=$HOME/tmp/${PACKAGE_LONG_NAME}_config
CONTROL_FILE=${TEMPORARY_INSTALL_DIR}/DEBIAN/control

# configure in the temporary configure directory
rm -rf ${TEMPORARY_CONFIG_DIR}
mkdir -p ${TEMPORARY_CONFIG_DIR}
cd ${TEMPORARY_CONFIG_DIR}
$1/configure --prefix=${TEMPORARY_INSTALL_DIR}/${install_path} --with-unisim-build-tool=$2 --with-systemc=$3 CPPFLAGS=-I/usr/include/libxml2 CXXFLAGS="-m32 -O3 -g3 -DSC_INCLUDE_DYNAMIC_PROCESSES"

# install in the temporary install directory
rm -rf ${TEMPORARY_INSTALL_DIR}
fakeroot make install

# compute the installed size
installed_size=`du ${TEMPORARY_INSTALL_DIR} -s -k | cut -f 1`
mkdir -p ${TEMPORARY_INSTALL_DIR}/DEBIAN

# Fill-in DEBIAN/control
echo "Package: ${PACKAGE_NAME}" > ${CONTROL_FILE}
echo "Version: ${VERSION}" >> ${CONTROL_FILE}
echo "Section: devel" >> ${CONTROL_FILE}
echo "Priority: optional" >> ${CONTROL_FILE}
echo "Architecture: i386" >> ${CONTROL_FILE}
echo "Depends: ${deps}" >> ${CONTROL_FILE}
echo "Installed-Size: ${installed_size}" >> ${CONTROL_FILE}
echo "Maintainer: ${maintainer}" >> ${CONTROL_FILE}
echo "Description: ${description}" >> ${CONTROL_FILE}

cat ${CONTROL_FILE}
# Build the package
fakeroot dpkg-deb --build ${TEMPORARY_INSTALL_DIR}
mv ${TEMPORARY_INSTALL_DIR}/../${PACKAGE_FILENAME} ${HERE}/.

# cleanup every temporary directories
rm -rf ${TEMPORARY_INSTALL_DIR}
rm -rf ${TEMPORARY_CONFIG_DIR}
