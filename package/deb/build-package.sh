#!/bin/sh
if test "x${PACKAGE_NAME}" = x; then
	echo "Please set PACKAGE_NAME"
	exit
fi

if test "x${VERSION}" = x; then
	echo "Please set VERSION"
	exit
fi

if test "x${ARCH}" = x; then
	echo "Please set ARCH"
	exit
fi

if test "x${DESCRIPTION}" = x; then
	echo "Please set DESCRIPTION"
	exit
fi

if test "x${MAINTAINER}" = x; then
	echo "Please set MAINTAINER"
	exit
fi

if test "x${INSTALL_PATH}" = x; then
	echo "Please set INSTALL_PATH"
	exit
fi

HERE=`pwd`
TARBALL_NAME=${PACKAGE_NAME}-${VERSION}
PACKAGE_LONG_NAME=${PACKAGE_NAME}_${VERSION}_${ARCH}
PACKAGE_FILENAME=${PACKAGE_LONG_NAME}.deb
TEMPORARY_SOURCE_DIR=${HOME}/tmp/${TARBALL_NAME}
TEMPORARY_INSTALL_DIR=${HOME}/tmp/${PACKAGE_LONG_NAME}
TEMPORARY_CONFIG_DIR=${HOME}/tmp/${PACKAGE_LONG_NAME}_config
CONTROL_FILE=${TEMPORARY_INSTALL_DIR}/DEBIAN/control
MD5SUMS_FILE=${TEMPORARY_INSTALL_DIR}/DEBIAN/md5sums

echo "========================================="
echo "=              uncompress               ="
echo "========================================="
cd ${HOME}/tmp
rm -rf ${TARBALL_NAME}
tar zxvf ${HERE}/${TARBALL_NAME}.tar.gz || exit

# configure in the temporary configure directory
echo "========================================="
echo "=              configure                ="
echo "========================================="
rm -rf ${TEMPORARY_CONFIG_DIR}
mkdir -p ${TEMPORARY_CONFIG_DIR}
cd ${TEMPORARY_CONFIG_DIR}
${TEMPORARY_SOURCE_DIR}/configure --prefix=${TEMPORARY_INSTALL_DIR}/${INSTALL_PATH} "$@" || exit

# install in the temporary install directory
echo "========================================="
echo "=             make install              ="
echo "========================================="
rm -rf ${TEMPORARY_INSTALL_DIR}
NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`
fakeroot make -j ${NUM_PROCESSORS} install || exit

# compute the installed size
installed_size=`du ${TEMPORARY_INSTALL_DIR} -s -k | cut -f 1`

# build list of installed files
cd ${TEMPORARY_INSTALL_DIR}
file_list=`find .`

# create DEBIAN dubdirectory
mkdir -p ${TEMPORARY_INSTALL_DIR}/DEBIAN

# Fill-in DEBIAN/md5sums
for file in $file_list
do
     if test -f $file; then
           md5sum $file | sed 's:./::' >> ${MD5SUMS_FILE}
     fi
done

# Fill-in DEBIAN/control
echo "Package: ${PACKAGE_NAME}" > ${CONTROL_FILE}
echo "Version: ${VERSION}" >> ${CONTROL_FILE}
echo "Section: devel" >> ${CONTROL_FILE}
echo "Priority: optional" >> ${CONTROL_FILE}
echo "Architecture: i386" >> ${CONTROL_FILE}
echo "Depends: ${DEPS}" >> ${CONTROL_FILE}
echo "Installed-Size: ${installed_size}" >> ${CONTROL_FILE}
echo "Maintainer: ${MAINTAINER}" >> ${CONTROL_FILE}
echo "Description: ${DESCRIPTION}" >> ${CONTROL_FILE}

echo "========================================="
echo "=            DEBIAN/control             ="
echo "========================================="
cat ${CONTROL_FILE}

echo "========================================="
echo "=            DEBIAN/md5sums             ="
echo "========================================="
cat ${MD5SUMS_FILE}

# Build the package
echo "========================================="
echo "=            Package build              ="
echo "========================================="
fakeroot dpkg-deb --build ${TEMPORARY_INSTALL_DIR}
mv ${TEMPORARY_INSTALL_DIR}/../${PACKAGE_FILENAME} ${HERE}/.

echo "========================================="
echo "=       Clean temporary directories     ="
echo "========================================="
# cleanup every temporary directories
rm -rf ${TEMPORARY_INSTALL_DIR}
rm -rf ${TEMPORARY_CONFIG_DIR}
rm -rf ${TEMPORARY_SOURCE_DIR}
