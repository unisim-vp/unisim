#!/bin/bash
case $(lsb_release -s -i) in
	Ubuntu* | Debian*)
		HOST=i586-mingw32msvc
		;;
	Mageia* | Mandriva*)
		HOST=i586-pc-mingw32
		;;
	RedHat* | Fedora* | SUSE*)
		HOST=i686-pc-mingw32
		;;
	*)
		echo "FATAL! Unknown Linux distribution. Aborting..."
		exit -1
esac

#BUILD=i686-pc-linux-gnu
SDL_VERSION=1.2.14
LIBXML2_VERSION=2.7.8
EXPAT_VERSION=2.0.1
CROSS_GDB_VERSION=7.2
CROSS_GDB_ARCHITECTURES="powerpc-440fp-linux-gnu powerpc-7450-linux-gnu armel-linux-gnu m6811-elf"
ZLIB_VERSION=1.2.5
BOOST_VERSION=1_47_0

HERE=`pwd`
MY_DIR=$(cd $(dirname $0); pwd)

TMP_DIR=${HOME}/tmp

DOWNLOADS_DIR=${HERE}/downloads
BUILD_DIR=${TMP_DIR}/mingw32-bootstrap-build
INSTALL_DIR=${TMP_DIR}/mingw32-bootstrap-install
PACKAGES_DIR=${HERE}/mingw32-packages

mkdir -p ${DOWNLOADS_DIR}
mkdir -p ${BUILD_DIR}
rm -rf ${INSTALL_DIR}
mkdir -p ${INSTALL_DIR}
mkdir -p ${PACKAGES_DIR}

NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`

function Patch
{
	NAME=$1
	PATCH=$2
	echo "========================================="
	echo "=              Patching                 ="
	echo "========================================="
	cd ${BUILD_DIR}/${NAME}
	patch -p1 < ${PATCH} || exit 
}

function Configure
{
	NAME=$1
	NUM_FIXED_ARGS=1
	
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
	cd ${BUILD_DIR}/${NAME}
	./configure --prefix=/usr "${args[@]}" || exit
}

function Compile
{
	NAME=$1
	echo "========================================="
	echo "=              Compiling                ="
	echo "========================================="
	cd ${BUILD_DIR}/${NAME}
	make -j ${NUM_PROCESSORS} all || exit
}

function Install
{
	NAME=$1
	echo "========================================="
	echo "=              Installing               ="
	echo "========================================="
	cd ${BUILD_DIR}/${NAME}
	make install prefix=${INSTALL_DIR}/${NAME} || exit
}

function Download
{
	NAME=$1
	ARCHIVE_NAME=$2
	ARCHIVE_URL=$3

	ARCHIVE=${DOWNLOADS_DIR}/${ARCHIVE_NAME}
	if test ! -f ${ARCHIVE}; then
		cd ${DOWNLOADS_DIR}
		printf "Downloading ${ARCHIVE_URL}"
		success=no
		until [ $success = yes ]; do
			if wget --timeout=60 -t 1 -q  -O ${ARCHIVE_NAME} ${ARCHIVE_URL}; then
				success=yes
			else
				printf "."
			fi
		done
		printf "\n"
	fi

	cd ${BUILD_DIR} || exit
	rm -rf ${NAME}

	echo "Unpacking ${ARCHIVE_NAME}"
	ext=`echo "${ARCHIVE}" | awk -F . '{print $NF}'`
	case ${ext} in
		bz2)
			tar jxf ${ARCHIVE} || exit
			;;
		gz)
			tar zxf ${ARCHIVE} || exit
			;;
		zip)
			unzip -qo ${ARCHIVE}
			;;
		lzma)
			tar Yxf ${ARCHIVE} || exit
			;;
	esac
}

function InstallBinArchive
{
	ARCHIVE_NAME=$1
	ARCHIVE_URL=$2
	if [ -z "$3" ]; then
		SUBDIR=.
	else
		SUBDIR=$3
	fi

	ARCHIVE=${PACKAGES_DIR}/${ARCHIVE_NAME}
	if test ! -f ${ARCHIVE}; then
		cd ${BUILD_DIR}
		printf "Downloading ${ARCHIVE_URL}"
		success=no
		until [ $success = yes ]; do
			if wget --timeout=60 -t 1 -q -O ${ARCHIVE} ${ARCHIVE_URL}; then
				success=yes
			else
				printf "."
			fi
		done
		printf "\n"
	fi

	echo "Unpacking ${ARCHIVE_NAME}"
	mkdir -p ${INSTALL_DIR}/${SUBDIR}
	cd ${INSTALL_DIR}/${SUBDIR}
	ext=`echo "${ARCHIVE}" | awk -F . '{print $NF}'`
	case ${ext} in
		bz2)
			tar jxf ${ARCHIVE} || exit
			;;
		gz)
			tar zxf ${ARCHIVE} || exit
			;;
		zip)
			unzip -qo ${ARCHIVE}
			;;
		lzma)
			tar Yxf ${ARCHIVE} || exit
			;;
	esac
}

function Package
{
	NAME=$1

	cd ${INSTALL_DIR}/${NAME}
	tar jcvf ${PACKAGES_DIR}/${NAME}-mingw32.tar.bz2 * || exit -1
}

function Clean
{
	rm -rf ${INSTALL_DIR}/*
	rm -rf ${BUILD_DIR}/*
}

CMD=$1

case ${CMD} in
	all)
		# expat
		if [ ! -e ${PACKAGES_DIR}/expat-${EXPAT_VERSION}-mingw32.tar.bz2 ]; then
			Clean
			Download expat-${EXPAT_VERSION} expat-${EXPAT_VERSION}.tar.gz http://ovh.dl.sourceforge.net/expat/expat-${EXPAT_VERSION}.tar.gz
			Configure expat-${EXPAT_VERSION} --host=${HOST}
			Compile expat-${EXPAT_VERSION}
			Install expat-${EXPAT_VERSION}
			Package expat-${EXPAT_VERSION}
			Clean
		fi

		# zlib
		if [ ! -e ${PACKAGES_DIR}/zlib-${ZLIB_VERSION}-mingw32.tar.bz2 ]; then
			Clean
			Download zlib-${ZLIB_VERSION} zlib-${ZLIB_VERSION}.tar.gz http://zlib.net/zlib-${ZLIB_VERSION}.tar.gz
			make -C ${BUILD_DIR}/zlib-${ZLIB_VERSION} -f win32/Makefile.gcc PREFIX=${HOST}- BINARY_PATH=${INSTALL_DIR}/zlib-${ZLIB_VERSION}/bin INCLUDE_PATH=${INSTALL_DIR}/zlib-${ZLIB_VERSION}/include LIBRARY_PATH=${INSTALL_DIR}/zlib-${ZLIB_VERSION}/lib SHARED_MODE=1 install
			mv ${INSTALL_DIR}/zlib-${ZLIB_VERSION}/lib/libzdll.a ${INSTALL_DIR}/zlib-${ZLIB_VERSION}/lib/libz.dll.a
			Package zlib-${ZLIB_VERSION}
			Clean
		fi

		# cross-GDB
		for CROSS_GDB_ARCH in ${CROSS_GDB_ARCHITECTURES}
		do
			if [ ! -e ${PACKAGES_DIR}/cross-gdb-${CROSS_GDB_ARCH}-7.2-mingw32.tar.bz2 ]; then
				Clean
				Download gdb-7.2 gdb-7.2.tar.bz2 ftp://ftp.gnu.org/pub/gnu/gdb/gdb-7.2.tar.bz2
				InstallBinArchive expat-2.0.1-mingw32.tar.bz2 '' expat-2.0.1
				mv ${BUILD_DIR}/gdb-7.2 ${BUILD_DIR}/cross-gdb-${CROSS_GDB_ARCH}-7.2
				Configure cross-gdb-${CROSS_GDB_ARCH}-7.2 --host=${HOST} --target=${CROSS_GDB_ARCH} \
							--disable-sim --disable-werror \
							--with-libexpat-prefix=${INSTALL_DIR}/expat-2.0.1
				Compile cross-gdb-${CROSS_GDB_ARCH}-7.2
				Install cross-gdb-${CROSS_GDB_ARCH}-7.2
				Package cross-gdb-${CROSS_GDB_ARCH}-7.2
				Clean
			fi
		done

		# SDL
		if [ ! -e ${PACKAGES_DIR}/SDL-${SDL_VERSION}-mingw32.tar.bz2 ]; then
			Clean
			Download SDL-${SDL_VERSION} SDL-${SDL_VERSION}.tar.gz http://www.libsdl.org/release/SDL-${SDL_VERSION}.tar.gz
			Configure SDL-${SDL_VERSION} --host=${HOST}
			Compile SDL-${SDL_VERSION}
			Install SDL-${SDL_VERSION}
			Package SDL-${SDL_VERSION}
			Clean
		fi

		# libxml2
		if [ ! -e ${PACKAGES_DIR}/libxml2-${LIBXML2_VERSION}-mingw32.tar.bz2 ]; then
			Clean
			Download libxml2-${LIBXML2_VERSION} libxml2-${LIBXML2_VERSION}.tar.gz ftp://xmlsoft.org/libxml2/libxml2-${LIBXML2_VERSION}.tar.gz
			InstallBinArchive zlib-${ZLIB_VERSION}-mingw32.tar.bz2 '' zlib-${ZLIB_VERSION}
			Configure libxml2-${LIBXML2_VERSION} --host=${HOST} --without-python --with-zlib=${INSTALL_DIR}/zlib-${ZLIB_VERSION} CPPFLAGS=-DLIBXML_STATIC
			Compile libxml2-${LIBXML2_VERSION}
			Install libxml2-${LIBXML2_VERSION}
			Package libxml2-${LIBXML2_VERSION}
			Clean
		fi

		if [ ! -e ${PACKAGES_DIR}/boost_${BOOST_VERSION}-mingw32.tar.bz2 ]; then
			Clean
			Download boost_${BOOST_VERSION} boost_${BOOST_VERSION}.tar.bz2 http://ovh.dl.sourceforge.net/boost/boost_${BOOST_VERSION}.tar.bz2
			InstallBinArchive zlib-${ZLIB_VERSION}-mingw32.tar.bz2 '' zlib-${ZLIB_VERSION}
			cd ${BUILD_DIR}/boost_${BOOST_VERSION}
			mkdir -p ${INSTALL_DIR}/boost_${BOOST_VERSION}
			./bootstrap.sh --without-icu || exit -1
			echo "using gcc : $(${HOST}-gcc -v 2>&1 | tail -1 | awk '{print $3}' | cut -f 1 -d '-') : ${HOST}-g++
				:
				<rc>${HOST}-windres
				<archiver>${HOST}-ar
		;" > user-config.jam
			./bjam -j ${NUM_PROCESSORS} toolset=gcc target-os=windows variant=release threading=multi threadapi=win32\
			link=shared runtime-link=shared --prefix=${INSTALL_DIR}/boost_${BOOST_VERSION} --user-config=user-config.jam \
			--without-mpi --without-python -sNO_BZIP2=1 -sZLIB_BINARY=z.dll \
			-sZLIB_INCLUDE=${INSTALL_DIR}/zlib-${ZLIB_VERSION}/include -sZLIB_LIBPATH=${INSTALL_DIR}/zlib-${ZLIB_VERSION}/lib \
			--layout=tagged install || exit -1
			Package boost_${BOOST_VERSION}
			Clean
		fi
		;;
	clean)
		Clean
		rm -rf ${DOWNLOADS_DIR}/*
		rm -rf ${BUILD_DIR}/*
		rm -rf ${INSTALL_DIR}/*
		rm -rf ${PACKAGES_DIR}/*
		;;
	*)
		echo "Usage: $(basename $0) <all | clean>"
		exit -1;
esac

exit 0
