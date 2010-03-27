#!/bin/bash
HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ] || [ -z "$5" ] || [ -z "$6" ]; then
	echo "Usage: `basename $0` <ppcemu version> <ppcemu-system version> <embedded-ppc-g4-board version> <unisim repo> <systemc> <tlm2.0>"
	exit -1
fi

PPCEMU_VERSION=$1
PPCEMU_SYSTEM_VERSION=$2
EMBEDDED_PPC_G4_BOARD_VERSION=$3
UNISIM_REPO=$4
SYSTEMC=$5
TLM20=$6

WORKING_DIR=${HOME}/tmp/dist_source
rm -rf ${WORKING_DIR}
mkdir -p ${WORKING_DIR}

UNISIM_PPCEMU_SOURCE=${WORKING_DIR}/ppcemu_source
UNISIM_PPCEMU_SYSTEM_SOURCE=${WORKING_DIR}/ppcemu_system_source
UNISIM_EMBEDDED_PPC_G4_BOARD_SOURCE=${WORKING_DIR}/embedded_ppc_g4_board_source

# Fetch sources from repository
#svn --username guest --password "" export https://unisim.org/svn/devel ${UNISIM_REPO} || exit -1

# Extract sources
${UNISIM_REPO}/package/dist_ppcemu.sh ${PPCEMU_VERSION} ${UNISIM_PPCEMU_SOURCE} ${UNISIM_REPO}/unisim_tools ${UNISIM_REPO}/unisim_lib ${UNISIM_REPO}/unisim_simulators || exit -1
${UNISIM_REPO}/package/dist_ppcemu_system.sh ${PPCEMU_SYSTEM_VERSION} ${UNISIM_PPCEMU_SYSTEM_SOURCE} ${UNISIM_REPO}/unisim_tools ${UNISIM_REPO}/unisim_lib ${UNISIM_REPO}/unisim_simulators || exit -1
${UNISIM_REPO}/package/dist_embedded_ppc_g4_board.sh ${EMBEDDED_PPC_G4_BOARD_VERSION} ${UNISIM_EMBEDDED_PPC_G4_BOARD_SOURCE} ${UNISIM_REPO}/unisim_tools ${UNISIM_REPO}/unisim_lib ${UNISIM_REPO}/unisim_simulators || exit -1

# Build source tar.bz2 and zip
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

rm -rf ${WORKING_DIR}
