#!/bin/bash

function Usage
{
	echo "Usage:"
	echo "  $0 debian <version> [systemc]"
	echo "  $0 redhat <version> [systemc]"
	echo "  $0 mingw32 <version> [unisim-bootstrap-mingw32]"
	echo "  - version: version of UNISIM"
	echo "  - systemc: SystemC installation directory (debian only)"
	echo "  - systemc: UNISIM bootstrap for mingw32 (optional, mingw32 only)"
}

if test "x$1" = x || test "x$2" = x; then
	Usage
	exit
fi

TARGET=$1
VERSION=$2

HERE=`pwd`
ARCH=i386
MAINTAINER="Gilles Mouchard <gilles.mouchard@cea.fr>"

case ${TARGET} in
	*mingw32*)
		UNISIM_PREFIX=
		;;
	*debian*)
		UNISIM_PREFIX=/usr
		;;
esac

UNISIM_TOOLS_SHORT_NAME=unisim-tools
UNISIM_TOOLS_LONG_NAME=${UNISIM_TOOLS_SHORT_NAME}-${VERSION}
UNISIM_TOOLS_SOURCE_PACKAGE_FILENAME=${UNISIM_TOOLS_LONG_NAME}.tar.gz
UNISIM_TOOLS_WINDOWS_PACKAGE_FILENAME=${UNISIM_TOOLS_LONG_NAME}.exe
UNISIM_TOOLS_DEBIAN_PACKAGE_FILENAME=${UNISIM_TOOLS_SHORT_NAME}_${VERSION}_${ARCH}.deb
UNISIM_TOOLS_TEMPORARY_SOURCE_DIR=${HOME}/tmp/${UNISIM_TOOLS_LONG_NAME}
UNISIM_TOOLS_TEMPORARY_INSTALL_DIR=${HOME}/tmp/${UNISIM_TOOLS_SHORT_NAME}_${VERSION}_${ARCH}
UNISIM_TOOLS_TEMPORARY_CONFIG_DIR=${HOME}/tmp/${UNISIM_TOOLS_LONG_NAME}_config
UNISIM_TOOLS_LICENSE_FILE="share/unisim_tools/COPYING"
UNISIM_TOOLS_DESCRIPTION="UNISIM Tools"
UNISIM_TOOLS_DEPS="libc6 (>= 2.5)"

UNISIM_LIB_SHORT_NAME=unisim-lib
UNISIM_LIB_LONG_NAME=${UNISIM_LIB_SHORT_NAME}-${VERSION}
UNISIM_LIB_SOURCE_PACKAGE_FILENAME=${UNISIM_LIB_LONG_NAME}.tar.gz
UNISIM_LIB_WINDOWS_PACKAGE_FILENAME=${UNISIM_LIB_LONG_NAME}.exe
UNISIM_LIB_DEBIAN_PACKAGE_FILENAME=${UNISIM_LIB_SHORT_NAME}_${VERSION}_${ARCH}.deb
UNISIM_LIB_TEMPORARY_SOURCE_DIR=${HOME}/tmp/${UNISIM_LIB_LONG_NAME}
UNISIM_LIB_TEMPORARY_INSTALL_DIR=${HOME}/tmp/${UNISIM_LIB_SHORT_NAME}_${VERSION}_${ARCH}
UNISIM_LIB_TEMPORARY_CONFIG_DIR=${HOME}/tmp/${UNISIM_LIB_LONG_NAME}_config
UNISIM_LIB_LICENSE_FILE="share/unisim_lib/COPYING"
UNISIM_LIB_DESCRIPTION="UNISIM Library"
UNISIM_LIB_DEPS="libncurses5 (>= 5.5), libreadline5 (>= 5.2), libxml2 (>= 2.6.27), libsdl1.2debian (>= 1.2.11),  libstdc++6 (>= 4.1.2), libc6 (>= 2.5), libgcc1 (>= 4.1.2)"

UNISIM_SIMULATORS_SHORT_NAME=unisim-simulators
UNISIM_SIMULATORS_LONG_NAME=${UNISIM_SIMULATORS_SHORT_NAME}-${VERSION}
UNISIM_SIMULATORS_SOURCE_PACKAGE_FILENAME=${UNISIM_SIMULATORS_LONG_NAME}.tar.gz
UNISIM_SIMULATORS_WINDOWS_PACKAGE_FILENAME=${UNISIM_SIMULATORS_LONG_NAME}.exe
UNISIM_SIMULATORS_DEBIAN_PACKAGE_FILENAME=${UNISIM_SIMULATORS_SHORT_NAME}_${VERSION}_${ARCH}.deb
UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR=${HOME}/tmp/${UNISIM_SIMULATORS_LONG_NAME}
UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR=${HOME}/tmp/${UNISIM_SIMULATORS_SHORT_NAME}_${VERSION}_${ARCH}
UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR=${HOME}/tmp/${UNISIM_SIMULATORS_LONG_NAME}_config
UNISIM_SIMULATORS_LICENSE_FILE="share/unisim_simulators/COPYING"
UNISIM_SIMULATORS_DESCRIPTION="UNISIM Simulators"
UNISIM_SIMULATORS_DEPS="libncurses5 (>= 5.5), libreadline5 (>= 5.2), libxml2 (>= 2.6.27), libsdl1.2debian (>= 1.2.11),  libstdc++6 (>= 4.1.2), libc6 (>= 2.5), libgcc1 (>= 4.1.2)"

case ${TARGET} in
	*debian* | *mingw32*)
		if test ! -f ${HERE}/${UNISIM_TOOLS_SOURCE_PACKAGE_FILENAME}; then
			Usage
			echo "File ${HERE}/${UNISIM_TOOLS_SOURCE_PACKAGE_FILENAME} is needed. Use \"make dist\" to build it."
			exit
		fi
		
		if test ! -f ${HERE}/${UNISIM_LIB_SOURCE_PACKAGE_FILENAME}; then
			Usage
			echo "File ${HERE}/${UNISIM_LIB_LONG_NAME}.tar.gz is needed. Use \"make dist\" to build it."
			exit
		fi
		
		if test ! -f ${HERE}/${UNISIM_SIMULATORS_SOURCE_PACKAGE_FILENAME}; then
			Usage
			echo "File ${HERE}/${UNISIM_SIMULATORS_SOURCE_PACKAGE_FILENAME} is needed. Use \"make dist\" to build it."
			exit
		fi
		;;
	*redhat*)
		if test ! -f ${HERE}/${UNISIM_TOOLS_DEBIAN_PACKAGE_FILENAME}; then
			Usage
			echo "File ${HERE}/${UNISIM_TOOLS_DEBIAN_PACKAGE_FILENAME} is needed. Build first a debian package ($0 debian ....)."
			exit
		fi
		
		if test ! -f ${HERE}/${UNISIM_LIB_DEBIAN_PACKAGE_FILENAME}; then
			Usage
			echo "File ${HERE}/${UNISIM_LIB_DEBIAN_PACKAGE_FILENAME} is needed. Build first a debian package ($0 debian ....)."
			exit
		fi
		
		if test ! -f ${HERE}/${UNISIM_SIMULATORS_DEBIAN_PACKAGE_FILENAME}; then
			Usage
			echo "File ${HERE}/${UNISIM_SIMULATORS_DEBIAN_PACKAGE_FILENAME} is needed. Build first a debian package ($0 debian ....)."
			exit
		fi

		cd ${HERE}
		fakeroot alien --to-rpm ${UNISIM_TOOLS_DEBIAN_PACKAGE_FILENAME} || exit
		fakeroot alien --to-rpm ${UNISIM_LIB_DEBIAN_PACKAGE_FILENAME} || exit
		fakeroot alien --to-rpm ${UNISIM_SIMULATORS_DEBIAN_PACKAGE_FILENAME}
		exit
		;;
esac

case ${TARGET} in
	*mingw32*)
		UNISIM_BOOTSTRAP_MINGW32_SHORT_NAME=unisim-bootstrap-mingw32
		UNISIM_BOOTSTRAP_MINGW32_LONG_NAME=${UNISIM_BOOTSTRAP_MINGW32_SHORT_NAME}-${VERSION}
		if test "x$3" = x; then
			UNISIM_BOOTSTRAP_MINGW32_DIR=${HOME}/tmp/${UNISIM_BOOTSTRAP_MINGW32_LONG_NAME}
			rm -rf ${UNISIM_BOOTSTRAP_MINGW32_DIR}
			mkdir -p ${UNISIM_BOOTSTRAP_MINGW32_DIR}
			cd ${UNISIM_BOOTSTRAP_MINGW32_DIR}
			mkdir -p archives
			mkdir -p source
			mkdir -p patches
			mkdir -p install
			cp `dirname $0`/patch-systemc-2.2.0-mingw32 patches/.
			cp `dirname $0`/patch-gdb-6.2-m68hc1x-mingw32 patches/.
			cp `dirname $0`/COPYING .
			`dirname $0`/compile-unisim-bootstrap-mingw32.sh || exit
			`dirname $0`/compile-unisim-bootstrap-mingw32.sh clean || exit
			rm -rf source
		else
			UNISIM_BOOTSTRAP_MINGW32_DIR=$3
		fi
		SYSTEMC_INSTALL_DIR=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/systemc
		;;
	*debian*)
		if test "x$3" = x; then
			Usage
			exit
		fi
		SYSTEMC_INSTALL_DIR=$3
		;;
esac

NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`

function Package {
	PACKAGE_NAME=$1
	INSTALL_DIR=$2
	LICENSE_FILE=$3
	DEPS=$4
	DESCRIPTION=$5

	case ${TARGET} in
		*mingw32*)
			ISS_FILENAME=${PACKAGE_NAME}-${VERSION}.iss
			rm -rf ${INSTALL_DIR}/dist
			rm -f ${ISS_FILENAME}
			
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
			echo "DefaultDirName={pf}\\${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
			echo "DefaultGroupName=${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
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
			rm -rf ${INSTALL_DIR}/dist
			rm -f ${ISS_FILENAME}
			;;
		*debian*)
			CONTROL_FILE=${INSTALL_DIR}/DEBIAN/control
			MD5SUMS_FILE=${INSTALL_DIR}/DEBIAN/md5sums
		
			# compute the installed size
			installed_size=`du ${INSTALL_DIR} -s -k | cut -f 1`
			
			# build list of installed files
			cd ${INSTALL_DIR}
			file_list=`find .`
			
			# create DEBIAN dubdirectory
			mkdir -p ${INSTALL_DIR}/DEBIAN
			
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
			echo "Architecture: ${ARCH}" >> ${CONTROL_FILE}
			echo "Depends: ${DEPS}" >> ${CONTROL_FILE}
			echo "Installed-Size: ${installed_size}" >> ${CONTROL_FILE}
			echo "Maintainer: ${MAINTAINER}" >> ${CONTROL_FILE}
			echo "Description: ${DESCRIPTION}" >> ${CONTROL_FILE}
			echo "" >> ${CONTROL_FILE}
			
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
			fakeroot dpkg-deb --build ${INSTALL_DIR}
			mv ${INSTALL_DIR}/../${PACKAGE_NAME}_${VERSION}_${ARCH}.deb ${HERE}/.
			;;
	esac
}

function Configure
{
	SOURCE_DIR=$1
	CONFIG_DIR=$2
	INSTALL_DIR=$3
	NUM_FIXED_ARGS=3
	
	declare -a args
	i=0
	j=0
	for arg in "$@"
	do
			if test ${i} -ge ${NUM_FIXED_ARGS}; then
					args[${j}]=${arg}
					j=$((${j}+1))
			fi
			i=$((${i}+1))
	done

	echo "========================================="
	echo "=              Configuring              ="
	echo "========================================="
	rm -rf ${CONFIG_DIR}
	mkdir -p ${CONFIG_DIR}
	cd ${CONFIG_DIR}
	${SOURCE_DIR}/configure -C --prefix=${INSTALL_DIR}${UNISIM_PREFIX} "${args[@]}" || exit
}

function Compile
{
	INSTALL_DIR=$1
	echo "========================================="
	echo "=              Compiling                ="
	echo "========================================="
	rm -rf ${INSTALL_DIR}
	fakeroot make -j ${NUM_PROCESSORS} install || exit
}

case ${TARGET} in
	*mingw32*)
		Package unisim-bootstrap-mingw32 ${UNISIM_BOOTSTRAP_MINGW32_DIR} COPYING "" "${UNISIM_BOOTSTRAP_MINGW32_DESCRIPTION}"
		;;
	*debian*)
		;;
esac

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

Configure ${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR} ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} CXXFLAGS="-m32 -O3 -g3"
Compile ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}

case ${TARGET} in
	*mingw32*)
		Configure ${UNISIM_LIB_TEMPORARY_SOURCE_DIR} ${UNISIM_LIB_TEMPORARY_CONFIG_DIR} ${UNISIM_LIB_TEMPORARY_INSTALL_DIR} \
          --host=i586-mingw32msvc \
          --with-unisim-tools=${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -g3 -DSC_INCLUDE_DYNAMIC_PROCESSES -DDEBUG_NETSTUB" \
          --with-sdl=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/SDL \
          --with-boost=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/boost \
          --with-zlib=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/zlib \
          --with-readline=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/readline \
          --with-libxml2=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/libxml2 \
          --enable-release
		;;
	*debian*)
		Configure ${UNISIM_LIB_TEMPORARY_SOURCE_DIR} ${UNISIM_LIB_TEMPORARY_CONFIG_DIR} ${UNISIM_LIB_TEMPORARY_INSTALL_DIR} \
          --host=i686-pc-linux-gnu \
          --with-unisim-tools=${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -g3 -DSC_INCLUDE_DYNAMIC_PROCESSES -DDEBUG_NETSTUB" \
          --enable-release
		;;
esac

Compile ${UNISIM_LIB_TEMPORARY_INSTALL_DIR}
Package ${UNISIM_LIB_SHORT_NAME} ${UNISIM_LIB_TEMPORARY_INSTALL_DIR} ${UNISIM_LIB_LICENSE_FILE} "${UNISIM_LIB_DEPS}" "${UNISIM_LIB_DESCRIPTION}"

case ${TARGET} in
	*mingw32*)
		Configure ${UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR} ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR} ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR} \
          --host=i586-mingw32msvc \
          --with-unisim-lib=${UNISIM_LIB_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -g3 -DSC_INCLUDE_DYNAMIC_PROCESSES -DDEBUG_NETSTUB" \
          --with-sdl=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/SDL \
          --with-boost=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/boost \
          --with-zlib=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/zlib \
          --with-readline=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/readline \
          --with-libxml2=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/libxml2 \
          --enable-release
		;;
	*debian*)
		Configure ${UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR} ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR} ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR} \
          --host=i686-pc-linux-gnu \
          --with-unisim-lib=${UNISIM_LIB_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -g3 -DSC_INCLUDE_DYNAMIC_PROCESSES -DDEBUG_NETSTUB" \
          --enable-release
		;;
esac

Compile ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}

case ${TARGET} in
	*mingw32*)
		cp ${UNISIM_BOOTSTRAP_MINGW32_DIR}/libxml2/bin/libxml2-2.dll ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}/bin
		cp ${UNISIM_BOOTSTRAP_MINGW32_DIR}/SDL/bin/SDL.dll ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}/bin
		cp ${UNISIM_BOOTSTRAP_MINGW32_DIR}/readline/bin/readline5.dll ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}/bin
		cp ${UNISIM_BOOTSTRAP_MINGW32_DIR}/readline/bin/history5.dll ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}/bin
		;;
esac

Package ${UNISIM_SIMULATORS_SHORT_NAME} ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR} ${UNISIM_SIMULATORS_LICENSE_FILE} "${UNISIM_SIMULATORS_DEPS}" "${UNISIM_SIMULATORS_DESCRIPTION}"

case ${TARGET} in
	*mingw32*)
		Configure ${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR} ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} \
          --host=i586-mingw32msvc
		;;
	*debian*)
		;;
esac

Package ${UNISIM_TOOLS_SHORT_NAME} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} ${UNISIM_TOOLS_LICENSE_FILE} "${UNISIM_TOOLS_DEPS}" "${UNISIM_TOOLS_DESCRIPTION}"

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
