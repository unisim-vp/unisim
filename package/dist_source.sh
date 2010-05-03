#!/bin/bash
HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ] || [ -z "$5" ] || [ -z "$6" ] || [ -z "$7" ] || [ -z "$8" ]; then
	echo "Usage: `basename $0` <genisslib version> <ppcemu version> <ppcemu-system version> <embedded-ppc-g4-board version> <tms320c3x version> <unisim repo> <systemc> <tlm2.0>"
	exit -1
fi

GENISSLIB_VERSION=$1
PPCEMU_VERSION=$2
PPCEMU_SYSTEM_VERSION=$3
EMBEDDED_PPC_G4_BOARD_VERSION=$4
TMS320C3X_VERSION=$5
UNISIM_REPO=$6
SYSTEMC=$7
TLM20=$8

WORKING_DIR=${HOME}/tmp/dist_source
rm -rf ${WORKING_DIR}
mkdir -p ${WORKING_DIR}

UNISIM_PPCEMU_SOURCE=${WORKING_DIR}/ppcemu_source
UNISIM_PPCEMU_SYSTEM_SOURCE=${WORKING_DIR}/ppcemu_system_source
UNISIM_EMBEDDED_PPC_G4_BOARD_SOURCE=${WORKING_DIR}/embedded_ppc_g4_board_source
UNISIM_TMS320C3X_SOURCE=${WORKING_DIR}/tms320c3x
UNISIM_GENISSLIB_SOURCE=${WORKING_DIR}/genisslib
UNISIM_ARMEMU_SOURCE=${WORKING_DIR}/armemu_source
UNISIM_ARMEMU_BUILD=${WORKING_DIR}/armemu_build

# Fetch sources from repository
#svn --username guest --password "" export https://unisim.org/svn/devel ${UNISIM_REPO} || exit -1

# Extract sources
${UNISIM_REPO}/package/dist_genisslib.sh ${GENISSLIB_VERSION} ${UNISIM_GENISSLIB_SOURCE} ${UNISIM_REPO} || exit -1
${UNISIM_REPO}/package/dist_ppcemu.sh ${GENISSLIB_VERSION} ${PPCEMU_VERSION} ${UNISIM_PPCEMU_SOURCE} ${UNISIM_REPO} || exit -1
${UNISIM_REPO}/package/dist_ppcemu_system.sh ${GENISSLIB_VERSION} ${PPCEMU_SYSTEM_VERSION} ${UNISIM_PPCEMU_SYSTEM_SOURCE} ${UNISIM_REPO} || exit -1
${UNISIM_REPO}/package/dist_embedded_ppc_g4_board.sh ${GENISSLIB_VERSION} ${EMBEDDED_PPC_G4_BOARD_VERSION} ${UNISIM_EMBEDDED_PPC_G4_BOARD_SOURCE} ${UNISIM_REPO} || exit -1
${UNISIM_REPO}/package/dist_tms320c3x.sh ${GENISSLIB_VERSION} ${TMS320C3X_VERSION} ${UNISIM_TMS320C3X_SOURCE} ${UNISIM_REPO} || exit -1
${UNISIM_REPO}/package/dist_armemu.sh ${UNISIM_ARMEMU_SOURCE} ${UNISIM_REPO} || exit -1

# Build source tar.bz2 and zip
cd ${UNISIM_GENISSLIB_SOURCE}
./configure || exit -1
make dist-gzip || exit -1
make dist-bzip2 || exit -1
make dist-zip || exit -1
mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
cd ${HERE}
rm -rf ${UNISIM_GENISSLIB_SOURCE}

cd ${UNISIM_PPCEMU_SOURCE}
./configure --with-systemc=${SYSTEMC} --with-tlm20=${TLM20} || exit -1
make dist-gzip || exit -1
make dist-bzip2 || exit -1
make dist-zip || exit -1
mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
cd ${HERE}
rm -rf ${UNISIM_PPCEMU_SOURCE}

cd ${UNISIM_PPCEMU_SYSTEM_SOURCE}
./configure --with-systemc=${SYSTEMC} --with-tlm20=${TLM20}|| exit -1
make dist-gzip || exit -1
make dist-bzip2 || exit -1
make dist-zip || exit -1
mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
cd ${HERE}
rm -rf ${UNISIM_PPCEMU_SYSTEM_SOURCE}

cd ${UNISIM_EMBEDDED_PPC_G4_BOARD_SOURCE}
./configure --with-systemc=${SYSTEMC} --with-tlm20=${TLM20} || exit -1
make dist-gzip || exit -1
make dist-bzip2 || exit -1
make dist-zip || exit -1
mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
cd ${HERE}
rm -rf ${UNISIM_EMBEDDED_PPC_G4_BOARD_SOURCE}

cd ${UNISIM_TMS320C3X_SOURCE}
./configure || exit -1
make dist-gzip || exit -1
make dist-bzip2 || exit -1
make dist-zip || exit -1
mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
cd ${HERE}
rm -rf ${UNISIM_TMS320C3X_SOURCE}

mkdir -p ${UNISIM_ARMEMU_BUILD}
cd ${UNISIM_ARMEMU_BUILD}
cmake ${UNISIM_ARMEMU_SOURCE} -Dwith_osci_systemc=${SYSTEMC} -Dwith_osci_tlm2=${TLM20}
make package_source
mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
cd ${HERE}
rm -rf ${UNISIM_ARMEMU_BUILD}
rm -rf ${UNISIM_ARMEMU_SOURCE}

rm -rf ${WORKING_DIR}
