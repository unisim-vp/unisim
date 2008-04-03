#!/bin/bash
if test "x$1" = x || test "x$2" = x; then
	echo "Usage: build-packages.sh <version> <mingw32 dir>"
	exit
fi

NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`

VERSION=$1
UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR=$2
HERE=`pwd`
UNISIM_TOOLS_SHORT_NAME=unisim-tools
UNISIM_TOOLS_LONG_NAME=${UNISIM_TOOLS_SHORT_NAME}-${VERSION}
UNISIM_TOOLS_PACKAGE_FILENAME=${UNISIM_TOOLS_LONG_NAME}.exe
UNISIM_TOOLS_TEMPORARY_SOURCE_DIR=${HOME}/tmp/${UNISIM_TOOLS_LONG_NAME}
UNISIM_TOOLS_TEMPORARY_INSTALL_DIR=${HOME}/tmp/${UNISIM_TOOLS_LONG_NAME}_install
UNISIM_TOOLS_TEMPORARY_CONFIG_DIR=${HOME}/tmp/${UNISIM_TOOLS_LONG_NAME}_config
UNISIM_TOOLS_LICENSE_FILE="share/unisim_tools/COPYING"
UNISIM_TOOLS_ISS_FILENAME=${UNISIM_TOOLS_LONG_NAME}.iss

UNISIM_LIB_SHORT_NAME=unisim-lib
UNISIM_LIB_LONG_NAME=${UNISIM_LIB_SHORT_NAME}-${VERSION}
UNISIM_LIB_PACKAGE_FILENAME=${UNISIM_LIB_LONG_NAME}.exe
UNISIM_LIB_TEMPORARY_SOURCE_DIR=${HOME}/tmp/${UNISIM_LIB_LONG_NAME}
UNISIM_LIB_TEMPORARY_INSTALL_DIR=${HOME}/tmp/${UNISIM_LIB_LONG_NAME}_install
UNISIM_LIB_TEMPORARY_CONFIG_DIR=${HOME}/tmp/${UNISIM_LIB_LONG_NAME}_config
UNISIM_LIB_LICENSE_FILE="share/unisim_lib/COPYING"
UNISIM_LIB_ISS_FILENAME=${UNISIM_LIB_LONG_NAME}.iss

UNISIM_SIMULATORS_SHORT_NAME=unisim-simulators
UNISIM_SIMULATORS_LONG_NAME=${UNISIM_SIMULATORS_SHORT_NAME}-${VERSION}
UNISIM_SIMULATORS_PACKAGE_FILENAME=${UNISIM_SIMULATORS_LONG_NAME}.exe
UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR=${HOME}/tmp/${UNISIM_SIMULATORS_LONG_NAME}
UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR=${HOME}/tmp/${UNISIM_SIMULATORS_LONG_NAME}_install
UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR=${HOME}/tmp/${UNISIM_SIMULATORS_LONG_NAME}_config
UNISIM_SIMULATORS_LICENSE_FILE="share/unisim_simulators/COPYING"
UNISIM_SIMULATORS_ISS_FILENAME=${UNISIM_SIMULATORS_LONG_NAME}.iss

function Package {
	PACKAGE_NAME=$1
	INSTALL_DIR=$2
	LICENSE_FILE=$3
	ISS_FILENAME=$4
	# install in the temporary install directory
	echo "========================================="
	echo "=              Compiling                ="
	echo "========================================="
	rm -rf ${INSTALL_DIR}
	fakeroot make -j ${NUM_PROCESSORS} install || exit
	
	# build list of installed files
	cd ${INSTALL_DIR}
	file_list=`find .`
	
	# Fill-in dist.iss
	echo "[Setup]" > ${ISS_FILENAME}
	echo "AppName=${PACKAGE_NAME}" >> ${ISS_FILENAME}
	echo "AppVerName=${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "AppPublisher=CEA" >> ${ISS_FILENAME}
	echo "AppPublisherURL=http://www.unisim.org" >> ${ISS_FILENAME}
	echo "AppSupportURL=http://www.unisim.org" >> ${ISS_FILENAME}
	echo "AppUpdatesURL=http://www.unisim.org" >> ${ISS_FILENAME}
	echo "DefaultDirName={pf}\\${PACKAGE_LONG_NAME}" >> ${ISS_FILENAME}
	echo "DefaultGroupName=${PACKAGE_LONG_NAME}" >> ${ISS_FILENAME}
	echo "LicenseFile=./${LICENSE_FILE}" >> ${ISS_FILENAME}
	echo "OutputDir=dist" >> ${ISS_FILENAME}
	echo "OutputBaseFilename=${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "Compression=lzma" >> ${ISS_FILENAME}
	echo "SolidCompression=yes" >> ${ISS_FILENAME}
	echo "" >> ${ISS_FILENAME}
	echo "[Languages]" >> ${ISS_FILENAME}
	echo "Name: \"english\"; MessagesFile: \"compiler:Default.isl\"" >> ${ISS_FILENAME}
	echo "" >> ${ISS_FILENAME}
	echo "[Tasks]" >> ${ISS_FILENAME}
	echo "Name: \"desktopicon\"; Description: \"{cm:CreateDesktopIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked" >> ${ISS_FILENAME}
	echo "" >> ${ISS_FILENAME}
	echo "[Files]" >> ${ISS_FILENAME}
	for file in ${file_list}
	do
		if test -f ${file}; then
			stripped_filename=`echo ${file} | sed 's:./::'`
			echo "Source: \"${stripped_filename}\"; DestDir: \"{app}/`dirname ${stripped_filename}`\"; Flags: ignoreversion" >> ${ISS_FILENAME}
		fi
	done
	
	echo "========================================="
	echo "=            Inno Setup Script          ="
	echo "========================================="
	cat ${ISS_FILENAME}
	
	# Build the package
	echo "========================================="
	echo "=            Package build              ="
	echo "========================================="
	
	wine ~/.wine/drive_c/Program\ Files/Inno\ Setup\ 5/ISCC.exe ${ISS_FILENAME} || exit
	cp -f ${INSTALL_DIR}/dist/${PACKAGE_NAME}-${VERSION}.exe ${HERE}
}

echo "========================================="
echo "=        Uncompressing tarballs         ="
echo "========================================="
rm -rf ${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR}
cd ${HOME}/tmp
tar zxvf ${HERE}/${UNISIM_TOOLS_LONG_NAME}.tar.gz || exit

rm -rf ${UNISIM_LIB_TEMPORARY_SOURCE_DIR}
cd ${HOME}/tmp
tar zxvf ${HERE}/${UNISIM_LIB_LONG_NAME}.tar.gz || exit

rm -rf ${UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR}
cd ${HOME}/tmp
tar zxvf ${HERE}/${UNISIM_SIMULATORS_LONG_NAME}.tar.gz || exit

# configure in the temporary configure directory
echo "========================================="
echo "=    Configuring UNISIM Tools (Host)    ="
echo "========================================="
rm -rf ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR}
mkdir -p ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR}
cd ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR}
${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR}/configure \
  --prefix=${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} || exit

rm -rf ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
make -j ${NUM_PROCESSORS} install || exit

# configure in the temporary configure directory
echo "========================================="
echo "=   Configuring UNISIM Lib (Mingw32)    ="
echo "========================================="
rm -rf ${UNISIM_LIB_TEMPORARY_CONFIG_DIR}
mkdir -p ${UNISIM_LIB_TEMPORARY_CONFIG_DIR}
cd ${UNISIM_LIB_TEMPORARY_CONFIG_DIR}
${UNISIM_LIB_TEMPORARY_SOURCE_DIR}/configure \
  --host=i586-mingw32msvc \
  --prefix=${UNISIM_LIB_TEMPORARY_INSTALL_DIR} \
  --with-unisim-tools=${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} \
  --with-systemc=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/systemc \
  CXXFLAGS="-m32 -O3 -g3 -DSC_INCLUDE_DYNAMIC_PROCESSES -DDEBUG_NETSTUB" \
  --with-sdl=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/SDL \
  --with-boost=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/boost \
  --with-zlib=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/zlib \
  --with-readline=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/readline \
  --with-libxml2=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/libxml2 \
  --enable-release || exit

Package ${UNISIM_LIB_SHORT_NAME} ${UNISIM_LIB_TEMPORARY_INSTALL_DIR} ${UNISIM_LIB_LICENSE_FILE} ${UNISIM_LIB_ISS_FILENAME}

echo "==========================================="
echo "= Configuring UNISIM Simulators (Mingw32) ="
echo "==========================================="
rm -rf ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR}
mkdir -p ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR}
cd ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR}
${UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR}/configure \
  --host=i586-mingw32msvc \
  --prefix=${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR} \
  --with-unisim-lib=${UNISIM_LIB_TEMPORARY_INSTALL_DIR} \
  --with-systemc=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/systemc \
  CXXFLAGS="-m32 -O3 -g3 -DSC_INCLUDE_DYNAMIC_PROCESSES -DDEBUG_NETSTUB" \
  --with-sdl=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/SDL \
  --with-boost=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/boost \
  --with-zlib=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/zlib \
  --with-readline=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/readline \
  --with-libxml2=${UNISIM_BOOTSTRAP_MINGW32_INSTALL_DIR}/libxml2 \
  --enable-release || exit

Package ${UNISIM_SIMULATORS_SHORT_NAME} ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR} ${UNISIM_SIMULATORS_LICENSE_FILE} ${UNISIM_SIMULATORS_ISS_FILENAME}

# configure in the temporary configure directory
echo "========================================="
echo "=  Configuring UNISIM Tools (Mingw32)   ="
echo "========================================="
rm -rf ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR}
mkdir -p ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR}
cd ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR}
${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR}/configure \
  --host=i586-mingw32msvc \
  --prefix=${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} || exit

Package ${UNISIM_TOOLS_SHORT_NAME} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} ${UNISIM_TOOLS_LICENSE_FILE} ${UNISIM_TOOLS_ISS_FILENAME}

echo "========================================="
echo "=       Clean temporary directories     ="
echo "========================================="
# cleanup every temporary directories
rm -rf ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
rm -rf ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR}
rm -rf ${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR}

rm -rf ${UNISIM_LIB_TEMPORARY_INSTALL_DIR}
rm -rf ${UNISIM_LIB_TEMPORARY_CONFIG_DIR}
rm -rf ${UNISIM_LIB_TEMPORARY_SOURCE_DIR}

rm -rf ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}
rm -rf ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR}
rm -rf ${UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR}
