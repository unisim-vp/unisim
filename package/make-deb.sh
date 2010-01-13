#!/bin/bash

PACKAGE_NAME=$1
VERSION=$2
HERE=`pwd`
MAINTAINER="Gilles Mouchard <gilles.mouchard@cea.fr>"
ARCH=i386

function find_deb_dependencies
{
	PKG_DEPS=""
	TMP=""
	for FILE in $@; do
		if [ -f ${FILE} ] && [ -x ${FILE} ]; then
			FILE_HEAD=`head -c 4 ${FILE}`
			ELF_HEAD=`echo -e "\0177ELF"`
			if [ "${FILE_HEAD}" = "${ELF_HEAD}" ]; then
				SO_DEPS=`ldd ${FILE} | sed 's/^.*=>//' | cut -f 1 -d "("`
				#echo "SO_DEPS=\"${SO_DEPS}\""
				for SO_DEP in ${SO_DEPS}; do
					PKGS=`dpkg -S ${SO_DEP} 2> /dev/null | cut -f 1 -d ":"`
					#echo "PKGS=\"${PKGS}\""
					for PKG in ${PKGS}; do
						PKG_VERSION=`dpkg -s ${PKG} 2> /dev/null | grep Version | cut -f 2- -d ":" | sed 's/^[ \t]*//'`
						regex="([0-9][0-9]*:)*[0-9][0-9]*(\.[0-9][0-9]*)*"
						if [[ ${PKG_VERSION} =~ ${regex} ]]; then
							PKG_VERSION=$BASH_REMATCH
						fi
						if [ -n "${TMP}" ]; then
							TMP+="\n"
						fi
						TMP+="${PKG} (>= ${PKG_VERSION})"
					done
				done
			fi
		fi
	done
	
	PKG_DEPS=`echo -e "${TMP}" | sort -u | sed ':a;N;$!ba;s/\n/, /g'`
	echo ${PKG_DEPS}
}

function Package {
	PACKAGE_NAME=$1
	VERSION=$2
	INSTALL_DIR=$3
	DESCRIPTION=${PACKAGE_NAME}

	CONTROL_FILE=${INSTALL_DIR}/DEBIAN/control
	PREINST_FILE=${INSTALL_DIR}/DEBIAN/preinst
	TEMPLATES_FILE=${INSTALL_DIR}/DEBIAN/templates
	MD5SUMS_FILE=${INSTALL_DIR}/DEBIAN/md5sums

	echo "Packaging ${PACKAGE_NAME} version ${VERSION} located into ${INSTALL_DIR}"
	# compute the installed size
	installed_size=`du ${INSTALL_DIR} -s -k | cut -f 1`

	echo "Installed size is ${installed_size} KB"
	
	# build list of installed files
	cd ${INSTALL_DIR}
	echo "Listing files"
	file_list=`find .`
	
	# create DEBIAN dubdirectory
	mkdir -p ${INSTALL_DIR}/DEBIAN
	
	# Fill-in DEBIAN/md5sums
	echo "Computing md5 sums"
	for file in ${file_list}
	do
		if test -f $file; then
			md5sum $file | sed 's:./::' >> ${MD5SUMS_FILE}
		fi
	done

	# Finding package dependencies
	echo "Searching dependencies...Please wait"
	DEPS=`find_deb_dependencies ${file_list}`
	
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
	
	echo "Building ${PACKAGE_NAME}_${VERSION}_${ARCH}.deb"
	fakeroot dpkg-deb --build ${INSTALL_DIR}
	mv ${INSTALL_DIR}/../${PACKAGE_NAME}_${VERSION}_${ARCH}.deb ${HERE}/.
}

Package "$1" "$2" "$3"
