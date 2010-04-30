#!/bin/bash
HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi

#NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`

case `uname` in
	MINGW*)
		EXE_SUFFIX=".exe"
		;;
	*)
		EXE_SUFFIX=
		;;
esac

function usage
{
	echo "Usage: `basename $0` <OS> <genisslib version> <ppcemu version> <ppcemu-system version> <embedded-ppc-g4-board version> <tms320c3x version> <armemu version> <systemc> <tlm2.0>"
	echo "OS: deb | rpm | mingw"
}

function BuildRPM
{
	NAME="$1"
	shift
	VERSION="$1"
	shift
	RELEASE="$1"
	shift
	URL="$1"
	shift
	LICENSE="$1"
	shift
	SUMMARY="$1"
	shift
	DESCRIPTION="$1"
	shift
	GROUP="$1"
	shift
	CATEGORIES="$1"
	shift
	MAINTAINER="$1"
	shift
	FILES=`printf "$1" | sed -e 's/ /\n/g'`
	shift
	SYSTEM_FILES=`printf "$1" | sed -e 's/ /\n/g'`
	shift
	START_ICON="$1"
	shift
	START_PROGRAM="$1"
	shift
	START_PARAMS="$1"
	shift

	TOP_DIR="${HOME}/tmp/rpm"

	if [ -d "${TOP_DIR}" ]; then
		echo "ERROR! Directory ${TOP_DIR} already exists. Please remove ${TOP_DIR} before."
		exit -1
	fi

	SPEC="${TOP_DIR}/SPECS/${NAME}.spec"

	mkdir -p ${TOP_DIR}/{BUILD,RPMS/{i586,noarch,x86_64},SOURCES,SRPMS,SPECS,tmp} 
	cp -f "${NAME}-${VERSION}.tar.gz" "${TOP_DIR}/SOURCES"

	echo "%define _topdir     ${HOME}/tmp/rpm" > "${SPEC}"
	echo "%define name        ${NAME}" >> "${SPEC}"
	echo "%define release     ${RELEASE}" >> "${SPEC}"
	echo "%define version     ${VERSION}" >> "${SPEC}"
	echo "%define buildroot %{_topdir}/%{name}-%{version}-root" >> "${SPEC}"

	echo "BuildRoot: %{buildroot}" >> "${SPEC}"
	echo "Summary:   ${SUMMARY}" >> "${SPEC}"
	echo "License:   ${LICENSE}" >> "${SPEC}"
	echo "Name:      %{name}" >> "${SPEC}"
	echo "Version:   %{version}" >> "${SPEC}"
	echo "Release:   %{release}" >> "${SPEC}"
	echo "Source:    %{name}-%{version}.tar.gz" >> "${SPEC}"
	echo "Group:     ${GROUP}" >> "${SPEC}"
	echo "URL:       ${URL}" >> "${SPEC}"
	if ! [ -z "${START_PROGRAM}" ] && ! [ -z "${START_ICON}" ]; then
		echo "BuildRequires: desktop-file-utils" >> "${SPEC}"
	fi

	echo "%description" >> "${SPEC}"
	echo "${DESCRIPTION}" >> "${SPEC}"

	echo "%prep" >> "${SPEC}"
	echo "%setup -q" >> "${SPEC}"


	echo "%build" >> "${SPEC}"
	printf "if [ -f 'configure' ]; then\n" >> "${SPEC}"
	printf "\t./configure --prefix=%%{_prefix} " >> "${SPEC}"
	for arg in "${@}"; do
		printf "\'${arg}\' " >> "${SPEC}"
	done
	printf "\n" >> "${SPEC}"
	printf "elif [ -f 'CMakeLists.txt' ]; then\n" >> "${SPEC}"
	printf "\tcmake . -DCMAKE_INSTALL_PREFIX=%%{_prefix} " >> "${SPEC}"
	for arg in "${@}"; do
		printf "\'${arg}\' " >> "${SPEC}"
	done
	printf "\n" >> "${SPEC}"
	printf "fi\n" >> "${SPEC}"
	echo "%make" >> "${SPEC}"

	echo "%install" >> "${SPEC}"
	printf "if [ -f configure ]; then\n" >> "${SPEC}"
	printf "\tmake install prefix=%%{buildroot}%%{_prefix}\n" >> "${SPEC}"
	printf "elif [ -f 'CMakeLists.txt' ]; then\n" >> "${SPEC}"
	printf "\tmake install DESTDIR=%%{buildroot}\n" >> "${SPEC}"
	printf "fi\n" >> "${SPEC}"

	if ! [ -z "${START_PROGRAM}" ] && ! [ -z "${START_ICON}" ]; then
		echo "mkdir -p %{buildroot}%{_datadir}/applications" >> "${SPEC}"
		DESKTOP_ENTRY_FILE="%{buildroot}%{_datadir}/applications/${NAME}-${VERSION}.desktop"
		echo "echo '[Desktop Entry]' > \"${DESKTOP_ENTRY_FILE}\"" >> "${SPEC}"
		echo "echo 'Type=Application' >> \"${DESKTOP_ENTRY_FILE}\"" >> "${SPEC}"
		echo "echo 'Name=${NAME}' >> \"${DESKTOP_ENTRY_FILE}\"" >> "${SPEC}"
		echo "echo 'Comment=${DESCRIPTION}' >> \"${DESKTOP_ENTRY_FILE}\"" >> "${SPEC}"
		echo "echo 'Icon=%{_prefix}/${START_ICON}' >> \"${DESKTOP_ENTRY_FILE}\"" >> "${SPEC}"
		echo "echo 'Exec=%{_prefix}/${START_PROGRAM} ${START_PARAMS}' >> \"${DESKTOP_ENTRY_FILE}\"" >> "${SPEC}"
		echo "echo 'Categories=${CATEGORIES};' >> \"${DESKTOP_ENTRY_FILE}\"" >> "${SPEC}"
		echo "echo 'Terminal=true' >> \"${DESKTOP_ENTRY_FILE}\"" >> "${SPEC}"
		echo "desktop-file-install --dir=%{buildroot}%{_datadir}/applications %{buildroot}%{_datadir}/applications/${NAME}-${VERSION}.desktop" >> "${SPEC}"
	fi

	echo "%files" >> "${SPEC}"
	echo "%defattr(-,root,root)" >> "${SPEC}"
	for FILE in ${FILES}; do
		echo "%{_prefix}/${FILE}" >> "${SPEC}"
	done

	if ! [ -z "${START_PROGRAM}" ] && ! [ -z "${START_ICON}" ]; then
		echo "%{_datadir}/applications/${NAME}-${VERSION}.desktop" >> "${SPEC}"
	fi

	rpmbuild -v -bb --clean "${SPEC}" || exit -1

	mv -f ${TOP_DIR}/RPMS/*/${NAME}-${VERSION}-${RELEASE}.*.rpm "${HERE}"

	rm -rf "${TOP_DIR}"
}

function find_deb_dependencies
{
	local PKG_DEPS=""
	local TMP=""
	for FILE in $@; do
		if [ -f ${FILE} ] && [ -x ${FILE} ]; then
			local FILE_HEAD=`head -c 4 ${FILE}`
			local ELF_HEAD=`echo -e "\0177ELF"`
			if [ "${FILE_HEAD}" = "${ELF_HEAD}" ]; then
				local SO_DEPS=`ldd ${FILE} | sed -e 's/^.*=>//' -e 's/^.*not a dynamic executable//' | cut -f 1 -d "("`
				#echo "SO_DEPS=\"${SO_DEPS}\""
				for SO_DEP in ${SO_DEPS}; do
					local PKGS=`dpkg -S ${SO_DEP} 2> /dev/null | cut -f 1 -d ":"`
					#echo "PKGS=\"${PKGS}\""
					for PKG in ${PKGS}; do
						local PKG_VERSION=`dpkg -s ${PKG} 2> /dev/null | grep Version | cut -f 2- -d ":" | sed 's/^[ \t]*//'`
						local regex="([0-9][0-9]*:)*[0-9][0-9]*(\.[0-9][0-9]*)*"
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
	
	local PKG_DEPS=`echo -e "${TMP}" | sort -u | sed ':a;N;$!ba;s/\n/, /g'`
	echo ${PKG_DEPS}
}

function BuildDEB
{
	NAME="$1"
	shift
	VERSION="$1"
	shift
	RELEASE="$1"
	shift
	URL="$1"
	shift
	LICENSE="$1"
	shift
	SUMMARY="$1"
	shift
	DESCRIPTION="$1"
	shift
	GROUP="$1"
	shift
	CATEGORIES="$1"
	shift
	MAINTAINER="$1"
	shift
	FILES=`printf "$1" | sed -e 's/ /\n/g'`
	shift
	SYSTEM_FILES=`printf "$1" | sed -e 's/ /\n/g'`
	shift
	START_ICON="$1"
	shift
	START_PROGRAM="$1"
	shift
	START_PARAMS="$1"
	shift

	PREFIX=/usr
	TOP_DIR="${HOME}/tmp/deb"

	if [ -d "${TOP_DIR}" ]; then
		echo "ERROR! Directory ${TOP_DIR} already exists. Please remove ${TOP_DIR} before."
		exit -1
	fi

	mkdir -p ${TOP_DIR}/{BUILD,INSTALL,SOURCES} 
	cp -f "${NAME}-${VERSION}.tar.gz" "${TOP_DIR}/SOURCES"

	SOURCES_DIR="${TOP_DIR}/SOURCES"
	BUILD_DIR="${TOP_DIR}/BUILD"
	INSTALL_DIR="${TOP_DIR}/INSTALL/${NAME}-${VERSION}"

	mkdir -p ${BUILD_DIR}

	# uncompress source
	cd ${BUILD_DIR}
	tar zxvf "${SOURCES_DIR}/${NAME}-${VERSION}.tar.gz" || exit -1

	# configure
	cd ${BUILD_DIR}/${NAME}-${VERSION}
	if [ -f 'configure' ]; then
		./configure --prefix=${PREFIX} "${@}" || exit -1
	elif [ -f 'CMakeLists.txt' ]; then
		cmake . -DCMAKE_INSTALL_PREFIX=${PREFIX} "${@}" || exit -1
	fi

	# build
	NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`
	make -j ${NUM_PROCESSORS} || exit -1

	# install
	if [ -f 'configure' ]; then
		fakeroot make install prefix=${INSTALL_DIR}${PREFIX} || exit -1
	elif [ -f 'CMakeLists.txt' ]; then
		fakeroot make install DESTDIR=${INSTALL_DIR} || exit -1
	fi

	if ! [ -z "${START_PROGRAM}" ] && ! [ -z "${START_ICON}" ]; then
		mkdir -p "${INSTALL_DIR}${PREFIX}/share/applications"
		DESKTOP_ENTRY_FILE="${INSTALL_DIR}${PREFIX}/share/applications/${NAME}-${VERSION}.desktop"
		echo '[Desktop Entry]' > "${DESKTOP_ENTRY_FILE}"
		echo 'Type=Application' >> "${DESKTOP_ENTRY_FILE}"
		echo "Name=${NAME}" >> "${DESKTOP_ENTRY_FILE}"
		echo "Comment=${DESCRIPTION}" >> "${DESKTOP_ENTRY_FILE}"
		echo "Icon=${PREFIX}/${START_ICON}" >> "${DESKTOP_ENTRY_FILE}"
		echo "Exec=${PREFIX}/${START_PROGRAM} ${START_PARAMS}" >> "${DESKTOP_ENTRY_FILE}"
		echo "Categories=${CATEGORIES};" >> "${DESKTOP_ENTRY_FILE}"
		echo 'Terminal=true' >> "${DESKTOP_ENTRY_FILE}"
		desktop-file-install --dir=${INSTALL_DIR}${PREFIX}/share/applications ${INSTALL_DIR}${PREFIX}/share/applications/${NAME}-${VERSION}.desktop
		FILES="${FILES} share/applications/${NAME}-${VERSION}.desktop"
	fi

	local CONTROL_FILE="${INSTALL_DIR}/DEBIAN/control"
	local PREINST_FILE="${INSTALL_DIR}/DEBIAN/preinst"
	local TEMPLATES_FILE="${INSTALL_DIR}/DEBIAN/templates"
	local MD5SUMS_FILE="${INSTALL_DIR}/DEBIAN/md5sums"

	# compute the installed size
	local INSTALLED_SIZE=0

	# create DEBIAN dubdirectory
	mkdir -p ${INSTALL_DIR}/DEBIAN
	
	# Fill-in DEBIAN/md5sums
	printf "Computing md5 sums..."
	cd "${INSTALL_DIR}"
	for FILE in ${FILES}
	do
		if [ -f "${INSTALL_DIR}${PREFIX}/${FILE}" ]; then
			echo "Adding ${FILE}"
			md5sum ".${PREFIX}/${FILE}" | sed 's/\.\///g' >> ${MD5SUMS_FILE}
			INSTALLED_SIZE=$((${INSTALLED_SIZE} + `stat -c%s "${INSTALL_DIR}${PREFIX}/${FILE}"`))
		else
			echo "WARNING! No such file or directory: ${FILE}"
		fi
	done
	printf "Done\n"
	echo "Installed size is $(((${INSTALLED_SIZE} + 1023) /1024)) KB"

	# Finding package dependencies
	printf "Searching dependencies..."
	cd "${INSTALL_DIR}${PREFIX}"
	local DEPS=`find_deb_dependencies ${FILES}`
	printf "Done\n"
	
	ARCH=i386
	# Fill-in DEBIAN/control
	echo "Package: ${NAME}" > ${CONTROL_FILE}
	echo "Version: ${VERSION}" >> ${CONTROL_FILE}
	echo "Section: ${SECTION}" >> ${CONTROL_FILE}
	echo "Priority: optional" >> ${CONTROL_FILE}
	echo "Architecture: ${ARCH}" >> ${CONTROL_FILE}
	echo "Depends: ${DEPS}" >> ${CONTROL_FILE}
	echo "Installed-Size: $(((${INSTALLED_SIZE} + 1023) /1024))" >> ${CONTROL_FILE}
	echo "Maintainer: ${MAINTAINER}" >> ${CONTROL_FILE}
	echo "Description: ${DESCRIPTION}" >> ${CONTROL_FILE}
	echo "" >> ${CONTROL_FILE}

	echo "========================================="
	echo "=            DEBIAN/md5sums             ="
	echo "========================================="
	cat ${MD5SUMS_FILE}
	
	echo "========================================="
	echo "=            DEBIAN/control             ="
	echo "========================================="
	cat ${CONTROL_FILE}

	# Build the package
	echo "========================================="
	echo "=            Package build              ="
	echo "========================================="
	
	cd ${HERE}
	echo "Building ${NAME}_${VERSION}_${ARCH}.deb"
	fakeroot dpkg-deb --build "${INSTALL_DIR}" "${HERE}" &> /dev/null || exit -1

	rm -rf "${TOP_DIR}"
}

function BuildWinInstaller
{
	NAME="$1"
	shift
	VERSION="$1"
	shift
	RELEASE="$1"
	shift
	URL="$1"
	shift
	LICENSE="$1"
	shift
	SUMMARY="$1"
	shift
	DESCRIPTION="$1"
	shift
	GROUP="$1"
	shift
	CATEGORIES="$1"
	shift
	MAINTAINER="$1"
	shift
	FILES="$1"
	shift
	SYSTEM_FILES=`printf "$1" | sed -e 's/ /\n/g'`
	shift
	START_ICON="$1"
	shift
	START_PROGRAM="$1"
	shift
	START_PARAMS="$1"
	shift

	PREFIX=/
	TOP_DIR="${HOME}/tmp/mingw"

	if [ -d "${TOP_DIR}" ]; then
		echo "ERROR! Directory ${TOP_DIR} already exists. Please remove ${TOP_DIR} before."
		exit -1
	fi

	mkdir -p ${TOP_DIR}/{BUILD,INSTALL,SOURCES} 
	cp -f "${NAME}-${VERSION}.tar.gz" "${TOP_DIR}/SOURCES"

	SOURCES_DIR="${TOP_DIR}/SOURCES"
	BUILD_DIR="${TOP_DIR}/BUILD"
	INSTALL_DIR="${TOP_DIR}/INSTALL/${NAME}-${VERSION}"

	mkdir -p ${BUILD_DIR}

	# uncompress source
	cd ${BUILD_DIR}
	tar zxvf "${SOURCES_DIR}/${NAME}-${VERSION}.tar.gz" || exit -1

	# configure
	cd ${BUILD_DIR}/${NAME}-${VERSION}
	if [ -f 'configure' ]; then
		./configure --prefix=${PREFIX} "${@}" || exit -1
	elif [ -f 'CMakeLists.txt' ]; then
		cmake . -DCMAKE_INSTALL_PREFIX=${PREFIX} "${@}" || exit -1
	fi

	# build
	NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`
	make || exit -1

	# install
	if [ -f 'configure' ]; then
		fakeroot make install prefix=${INSTALL_DIR}${PREFIX} || exit -1
	elif [ -f 'CMakeLists.txt' ]; then
		fakeroot make install DESTDIR=${INSTALL_DIR}${PREFIX} || exit -1
	fi

	ISS_FILENAME=${INSTALL_DIR}/${NAME}-${VERSION}.iss
	
	# Fill-in dist.iss
	echo "[Setup]" > ${ISS_FILENAME}
	echo "AppName=${NAME}" >> ${ISS_FILENAME}
	echo "AppVerName=${NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "AppPublisher=CEA" >> ${ISS_FILENAME}
	echo "AppPublisherURL=${URL}" >> ${ISS_FILENAME}
	echo "AppSupportURL=${URL}" >> ${ISS_FILENAME}
	echo "AppUpdatesURL=${URL}" >> ${ISS_FILENAME}
	echo "DefaultDirName={sd}\\${NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "DefaultGroupName=${NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "AllowNoIcons=yes" >> ${ISS_FILENAME}
	echo "OutputDir=dist" >> ${ISS_FILENAME}
	echo "OutputBaseFilename=setup-${NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "Compression=lzma" >> ${ISS_FILENAME}
	echo "SolidCompression=yes" >> ${ISS_FILENAME}
	echo "" >> ${ISS_FILENAME}
	echo "[Languages]" >> ${ISS_FILENAME}
	echo "Name: \"english\"; MessagesFile: \"compiler:Default.isl\"" >> ${ISS_FILENAME}
	echo "" >> ${ISS_FILENAME}
	echo "[Tasks]" >> ${ISS_FILENAME}
	if ! [ -z "${START_PROGRAM}" ] && ! [ -z "${START_ICON}" ]; then
		echo "Name: \"desktopicon\"; Description: \"{cm:CreateDesktopIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked" >> ${ISS_FILENAME}
		echo "Name: \"quicklaunchicon\"; Description: \"{cm:CreateQuickLaunchIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked" >> ${ISS_FILENAME}
	fi
	echo "" >> ${ISS_FILENAME}
	echo "[Files]" >> ${ISS_FILENAME}
	cd "${INSTALL_DIR}"

	for SYSTEM_FILE in ${SYSTEM_FILES}; do
		mkdir -p ${INSTALL_DIR}/bin
		cp -f ${SYSTEM_FILE} ${INSTALL_DIR}/bin
		FILES="${FILES} bin/`basename ${SYSTEM_FILE}`"
	done

	for FILE in ${FILES}
	do
		if [ -f "${INSTALL_DIR}/${FILE}" ]; then
			STRIPPED_FILENAME=`echo ${FILE} | sed 's/\.\///g'`
			echo "Source: \"${STRIPPED_FILENAME}\"; DestDir: \"{app}/`dirname ${STRIPPED_FILENAME}`\"; Flags: ignoreversion" >> ${ISS_FILENAME}
		fi
	done
	cd "${HERE}"

	if ! [ -z "${START_PROGRAM}" ] && ! [ -z "${START_ICON}" ]; then
		echo "" >> ${ISS_FILENAME}
		echo "[Icons]" >> ${ISS_FILENAME}
		echo "Name: \"{group}\\${NAME}\"; Filename: \"{app}\\${START_PROGRAM}\"; IconFilename: \"{app}\\${START_ICON}\"; Parameters: \"${START_PARAMS}\"" >> ${ISS_FILENAME}
		echo "Name: \"{commondesktop}\\${NAME}\"; Filename: \"{app}\\${START_PROGRAM}\"; IconFilename: \"{app}\\${START_ICON}\"; Parameters: \"${START_PARAMS}\"; Tasks: desktopicon" >> ${ISS_FILENAME}
		echo "Name: \"{userappdata}\\${NAME}\"; Filename: \"{app}\\${START_PROGRAM}\"; IconFilename: \"{app}\\${START_ICON}\"; Tasks: quicklaunchicon" >> ${ISS_FILENAME}
	fi
	
	echo "========================================="
	echo "=            Inno Setup Script          ="
	echo "========================================="
	cat ${ISS_FILENAME}
	
	# Build the package
	echo "========================================="
	echo "=            Package build              ="
	echo "========================================="

	/c/Program\ Files/Inno\ Setup\ 5/ISCC.exe ${ISS_FILENAME} || exit -1
	cp -f ${INSTALL_DIR}/dist/setup-${NAME}-${VERSION}.exe ${HERE}

	rm -rf "${TOP_DIR}"
}

function BuildPackage
{
	local OS="$1"
	shift

	case ${OS} in
		deb)
			BuildDEB "${@}"
			;;
		rpm)
			BuildRPM "${@}"
			;;
		mingw)
			BuildWinInstaller "${@}"
			;;
		*)
			usage
			exit -1
			;;
	esac
}

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ] || [ -z "$5" ] || [ -z "$6" ] || [ -z "$7" ] || [ -z "$8" ] || [ -z "$8" ]; then
	usage
	exit -1
fi

OS="$1"
GENISSLIB_VERSION="$2"
PPCEMU_VERSION="$3"
PPCEMU_SYSTEM_VERSION="$4"
EMBEDDED_PPC_G4_BOARD_VERSION="$5"
TMS320C3X_VERSION="$6"
ARMEMU_VERSION="$7"
SYSTEMC="$8"
TLM20="$9"

# NAME="$1"
# VERSION="$1"
# RELEASE="$1"
# URL="$1"
# LICENSE="$1"
# SUMMARY="$1"
# DESCRIPTION="$1"
# GROUP="$1"
# CATEGORIES="$1"
# MAINTAINER="$1"
# FILES=`printf "$1" | sed -e 's/ /\n/g'`
# SYSTEM_FILES=`printf "$1" | sed -e 's/ /\n/g'`
# START_ICON="$1"
# START_PROGRAM="$1"
# START_PARAMS="$1"
# BUILD="$1"
# CONFIGURE="$1"

# BuildPackage \
# 	"${OS}" \
# 	"unisim-genisslib" \
# 	"${GENISSLIB_VERSION}" \
# 	"1" \
# 	"http://www.unisim-vp.com" \
# 	"GPL" \
# 	"UNISIM genisslib" \
# 	"UNISIM Instruction set simulator generator" \
# 	"Development/Tools" \
# 	"Development" \
# 	"Gilles Mouchard <gilles.mouchard@cea.fr>" \
# 	"bin/unisim-genisslib-${GENISSLIB_VERSION}${EXE_SUFFIX}" \
#     "/bin/libgcc_s_dw2-1.dll" \
# 	"" \
# 	"" \
# 	"" \
# 	"CXXFLAGS=-O3 -g"
# 
# BuildPackage \
# 	"${OS}" \
# 	"unisim-ppcemu" \
# 	"${PPCEMU_VERSION}" \
# 	"1" \
# 	"http://www.unisim-vp.com" \
# 	"BSD" \
# 	"UNISIM ppcemu" \
# 	"UNISIM ppcemu is user level PowerPC simulator with support of ELF32 binaries and Linux system call translation." \
# 	"Emulators" \
# 	"Development;Emulator;ConsoleOnly" \
# 	"Gilles Mouchard <gilles.mouchard@cea.fr>" \
# 	"bin/unisim-ppcemu-${PPCEMU_VERSION}${EXE_SUFFIX} \
# 	bin/unisim-ppcemu-debug-${PPCEMU_VERSION}${EXE_SUFFIX} \
# 	share/unisim-ppcemu-${PPCEMU_VERSION}/AUTHORS \
# 	share/unisim-ppcemu-${PPCEMU_VERSION}/COPYING \
# 	share/unisim-ppcemu-${PPCEMU_VERSION}/ChangeLog \
# 	share/unisim-ppcemu-${PPCEMU_VERSION}/INSTALL \
# 	share/unisim-ppcemu-${PPCEMU_VERSION}/NEWS \
# 	share/unisim-ppcemu-${PPCEMU_VERSION}/README \
# 	share/unisim-ppcemu-${PPCEMU_VERSION}/gdb_powerpc.xml \
# 	share/unisim-ppcemu-${PPCEMU_VERSION}/template_default_config.xml \
# 	share/unisim-ppcemu-${PPCEMU_VERSION}/unisim.ico" \
#     "/bin/libgcc_s_dw2-1.dll /bin/libxml2-2.dll" \
# 	"share/unisim-ppcemu-${PPCEMU_VERSION}/unisim.ico" \
# 	"bin/unisim-ppcemu-${PPCEMU_VERSION}${EXE_SUFFIX}" \
# 	"-s enable-press-enter-at-exit=true" \
# 	"--with-systemc=${SYSTEMC}" \
# 	"CXXFLAGS=-O3 -g"
# 
# BuildPackage \
# 	"${OS}" \
# 	"unisim-ppcemu-system" \
# 	"${PPCEMU_SYSTEM_VERSION}" \
# 	"1" \
# 	"http://www.unisim-vp.com" \
# 	"BSD" \
# 	"UNISIM ppcemu-system" \
# 	"UNISIM ppcemu-system is a full system simulator of a \"PowerMac G4 PCI\" like machine (MPC7447A/MPC107) with Linux boot support." \
# 	"Emulators" \
# 	"Development;Emulator;ConsoleOnly" \
# 	"Gilles Mouchard <gilles.mouchard@cea.fr>" \
# 	"bin/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}${EXE_SUFFIX} \
# 	bin/unisim-ppcemu-system-debug-${PPCEMU_SYSTEM_VERSION}${EXE_SUFFIX} \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/AUTHORS \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/COPYING \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/ChangeLog \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/INSTALL \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/NEWS \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/README \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/gdb_powerpc.xml \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/device_tree_pmac_g4.xml \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/pc_linux_fr_keymap.xml \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/pc_linux_us_keymap.xml \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/pc_windows_keymap.xml \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/vmlinux \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/initrd.img \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/template_default_config.xml \
# 	share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/unisim.ico" \
#     "/bin/libgcc_s_dw2-1.dll /bin/libxml2-2.dll /bin/SDL.dll" \
# 	"share/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}/unisim.ico" \
# 	"bin/unisim-ppcemu-system-${PPCEMU_SYSTEM_VERSION}${EXE_SUFFIX}" \
# 	"-s enable-press-enter-at-exit=true" \
# 	"--with-systemc=${SYSTEMC}" \
# 	"CXXFLAGS=-O3 -g"
# 
# BuildPackage \
# 	"${OS}" \
# 	"unisim-embedded-ppc-g4-board" \
# 	"${EMBEDDED_PPC_G4_BOARD_VERSION}" \
# 	"1" \
# 	"http://www.unisim-vp.com" \
# 	"BSD" \
# 	"UNISIM embedded-ppc-g4-board" \
# 	"UNISIM embedded-ppc-g4-board is a full system simulator of an MPC7447A/MPC107 board with support of ELF32 binaries and targeted for industrial applications." \
# 	"Emulators" \
# 	"Development;Emulator;ConsoleOnly" \
# 	"Gilles Mouchard <gilles.mouchard@cea.fr>" \
# 	"bin/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}${EXE_SUFFIX} \
# 	bin/unisim-embedded-ppc-g4-board-debug-${EMBEDDED_PPC_G4_BOARD_VERSION}${EXE_SUFFIX} \
# 	bin/unisim-embedded-ppc-g4-board-no-pci-stub-${EMBEDDED_PPC_G4_BOARD_VERSION}${EXE_SUFFIX} \
# 	bin/unisim-embedded-ppc-g4-board-no-pci-stub-debug-${EMBEDDED_PPC_G4_BOARD_VERSION}${EXE_SUFFIX} \
# 	share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}/AUTHORS \
# 	share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}/COPYING \
# 	share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}/ChangeLog \
# 	share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}/INSTALL \
# 	share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}/NEWS \
# 	share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}/README \
# 	share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}/gdb_powerpc.xml \
# 	share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}/template_default_config.xml \
# 	share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}/unisim.ico" \
#     "/bin/libgcc_s_dw2-1.dll /bin/libxml2-2.dll" \
# 	"share/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}/unisim.ico" \
# 	"bin/unisim-embedded-ppc-g4-board-${EMBEDDED_PPC_G4_BOARD_VERSION}${EXE_SUFFIX}" \
# 	"-s enable-press-enter-at-exit=true" \
# 	"--with-systemc=${SYSTEMC}" \
# 	"CXXFLAGS=-O3 -g"
# 
# BuildPackage \
# 	"${OS}" \
# 	"unisim-tms320c3x" \
# 	"${TMS320C3X_VERSION}" \
# 	"1" \
# 	"http://www.unisim-vp.com" \
# 	"BSD" \
# 	"UNISIM tms320c3x" \
# 	"UNISIM tms320c3x is a TMS320C3X DSP simulator with support of TI COFF binaries, and TI C I/O (RTS run-time)." \
# 	"Emulators" \
# 	"Development;Emulator;ConsoleOnly" \
# 	"Gilles Mouchard <gilles.mouchard@cea.fr>" \
# 	"bin/unisim-tms320c3x-${TMS320C3X_VERSION}${EXE_SUFFIX} \
# 	bin/unisim-tms320c3x-debug-${TMS320C3X_VERSION}${EXE_SUFFIX} \
# 	share/unisim-tms320c3x-${TMS320C3X_VERSION}/AUTHORS \
# 	share/unisim-tms320c3x-${TMS320C3X_VERSION}/COPYING \
# 	share/unisim-tms320c3x-${TMS320C3X_VERSION}/ChangeLog \
# 	share/unisim-tms320c3x-${TMS320C3X_VERSION}/INSTALL \
# 	share/unisim-tms320c3x-${TMS320C3X_VERSION}/NEWS \
# 	share/unisim-tms320c3x-${TMS320C3X_VERSION}/README \
# 	share/unisim-tms320c3x-${TMS320C3X_VERSION}/unisim.ico \
# 	share/unisim-tms320c3x-${TMS320C3X_VERSION}/template_default_config.xml \
# 	share/unisim-tms320c3x-${TMS320C3X_VERSION}/fibo.out \
# 	share/unisim-tms320c3x-${TMS320C3X_VERSION}/c31boot.out \
# 	share/unisim-tms320c3x-${TMS320C3X_VERSION}/tms320c3x_manual.pdf" \
#     "/bin/libgcc_s_dw2-1.dll /bin/libxml2-2.dll" \
# 	"share/unisim-tms320c3x-${TMS320C3X_VERSION}/unisim.ico" \
# 	"bin/unisim-tms320c3x-${TMS320C3X_VERSION}${EXE_SUFFIX}" \
# 	"-s enable-press-enter-at-exit=true fibo.out" \
# 	"CXXFLAGS=-O3 -g"

BuildPackage \
	"${OS}" \
	"unisim-armemu" \
	"${ARMEMU_VERSION}" \
	"1" \
	"http://www.unisim-vp.com" \
	"BSD" \
	"UNISIM armemu" \
	"UNISIM armemu is a user level ARM simulator with support of ELF32 binaries and Linux system call translation." \
	"Emulators" \
	"Development;Emulator;ConsoleOnly" \
	"Gilles Mouchard <gilles.mouchard@cea.fr>" \
	"bin/unisim-armemu-${ARMEMU_VERSION}${EXE_SUFFIX} \
	share/unisim-armemu-${ARMEMU_VERSION}/AUTHORS.txt \
	share/unisim-armemu-${ARMEMU_VERSION}/COPYING.txt \
	share/unisim-armemu-${ARMEMU_VERSION}/INSTALL.txt \
	share/unisim-armemu-${ARMEMU_VERSION}/NEWS.txt \
	share/unisim-armemu-${ARMEMU_VERSION}/README.txt \
	lib/libunisim-armemu.so \
	lib/libunisim-armemu.so.0.2 \
	lib/libunisim-armemu.so.${ARMEMU_VERSION} \
	share/unisim-armemu-${ARMEMU_VERSION}/test/src/CMakeLists.txt \
	share/unisim-armemu-${ARMEMU_VERSION}/test/src/main.c \
	share/unisim-armemu-${ARMEMU_VERSION}/test/src/toolchain-armv5l.cmake \
	share/unisim-armemu-${ARMEMU_VERSION}/gdb_server/gdb_armv5l.xml \
	share/unisim-armemu-${ARMEMU_VERSION}/template-default-config.xml" \
    "/bin/libgcc_s_dw2-1.dll /bin/libxml2-2.dll" \
	"" \
	"" \
	"" \
	"-Dwith_osci_systemc=${SYSTEMC}" \
	"-Dwith_osci_tlm2=${TLM20}"
