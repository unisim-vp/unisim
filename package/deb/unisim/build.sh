#!/bin/sh
PACKAGE_NAME="unisim"
VERSION="1.0-1"
ARCH="i386"
PACKAGE_LONG_NAME=${PACKAGE_NAME}_${VERSION}_${ARCH}
PACKAGE_FILENAME=${PACKAGE_LONG_NAME}.deb
HERE=`pwd`
CONTROL_FILE=${HERE}/control

if test "x$1" == x || test "x$2" == x || test "x$3" == x; then
    echo "Usage <source dir> <UNISIM build_tool dir> <SystemC dir>"
    exit
fi

TEMPORARY_INSTALL_DIR=$HOME/tmp/${PACKAGE_LONG_NAME}
TEMPORARY_CONFIG_DIR=$HOME/tmp/${PACKAGE_LONG_NAME}_config

rm -rf ${TEMPORARY_CONFIG_DIR}
mkdir -p ${TEMPORARY_CONFIG_DIR}
cd ${TEMPORARY_CONFIG_DIR}
$1/configure --prefix=${TEMPORARY_INSTALL_DIR}/usr --with-unisim-build-tool=$2 --with-systemc=$3 CPPFLAGS=-I/usr/include/libxml2 CXXFLAGS="-m32 -O3 -g3 -DSC_INCLUDE_DYNAMIC_PROCESSES"
rm -rf ${TEMPORARY_INSTALL_DIR}
make install
mkdir -p ${TEMPORARY_INSTALL_DIR}/DEBIAN
cp ${CONTROL_FILE} ${TEMPORARY_INSTALL_DIR}/DEBIAN/.
dpkg-deb --build ${TEMPORARY_INSTALL_DIR}
mv ${TEMPORARY_INSTALL_DIR}/../${PACKAGE_FILENAME} ${HERE}/.
rm -rf ${TEMPORARY_INSTALL_DIR}
rm -rf ${TEMPORARY_CONFIG_DIR}
