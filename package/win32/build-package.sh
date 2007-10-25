if test "x${PACKAGE_NAME}" = x; then
	echo "Please set PACKAGE_NAME"
	exit
fi

if test "x${VERSION}" = x; then
	echo "Please set VERSION"
	exit
fi

if test "x${LICENSE_FILE}" = x; then
	echo "Please set LICENSE_FILE"
	exit
fi

HERE=`pwd`
TARBALL_NAME=${PACKAGE_NAME}-${VERSION}
PACKAGE_LONG_NAME=${PACKAGE_NAME}-${VERSION}
PACKAGE_FILENAME=${PACKAGE_LONG_NAME}.exe
TEMPORARY_SOURCE_DIR=${HOME}/tmp/${TARBALL_NAME}
TEMPORARY_INSTALL_DIR=${HOME}/tmp/${PACKAGE_LONG_NAME}_install
TEMPORARY_CONFIG_DIR=${HOME}/tmp/${PACKAGE_LONG_NAME}_config
ISS_FILENAME=${PACKAGE_LONG_NAME}.iss

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
${TEMPORARY_SOURCE_DIR}/configure --host=i586-mingw32msvc --prefix=${TEMPORARY_INSTALL_DIR} "$@" -C || exit

# install in the temporary install directory
echo "========================================="
echo "=             make install              ="
echo "========================================="
rm -rf ${TEMPORARY_INSTALL_DIR}
NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep processor | wc -l`
fakeroot make -j ${NUM_PROCESSORS} install || exit

# build list of installed files
cd ${TEMPORARY_INSTALL_DIR}
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
echo "OutputBaseFilename=${PACKAGE_LONG_NAME}" >> ${ISS_FILENAME}
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
cp -f ${TEMPORARY_INSTALL_DIR}/dist/${PACKAGE_LONG_NAME}.exe ${HERE}

echo "========================================="
echo "=       Clean temporary directories     ="
echo "========================================="
# cleanup every temporary directories
rm -rf ${TEMPORARY_INSTALL_DIR}
rm -rf ${TEMPORARY_CONFIG_DIR}
rm -rf ${TEMPORARY_SOURCE_DIR}
