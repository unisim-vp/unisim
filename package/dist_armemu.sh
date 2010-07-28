#!/bin/bash
#!/bin/bash
function Usage
{
	echo "Usage:"
	echo "  $0 <destination directory> <unisim repository>"
}

if [ -z "$1" ] || [ -z "$2" ]; then
	Usage
	exit -1
fi

HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi

DEST_DIR=$1
UNISIM_TOOLS_DIR=$2/unisim_tools
UNISIM_LIB_DIR=$2/unisim_lib
UNISIM_SIMULATORS_DIR=$2/unisim_simulators

mkdir -p ${DEST_DIR}

echo "<?xml version='1.0' encoding='UTF-8'?>" > ${DEST_DIR}/puscomp_config.xml
echo "<repositories>" >> ${DEST_DIR}/puscomp_config.xml
echo "<entry>" >> ${DEST_DIR}/puscomp_config.xml
echo "<name>Unisim Tools</name>" >> ${DEST_DIR}/puscomp_config.xml
echo "<location>${UNISIM_TOOLS_DIR}</location>" >> ${DEST_DIR}/puscomp_config.xml
echo "</entry>" >> ${DEST_DIR}/puscomp_config.xml
echo "<entry>" >> ${DEST_DIR}/puscomp_config.xml
echo "<name>Unisim Library</name>" >> ${DEST_DIR}/puscomp_config.xml
echo "<location>${UNISIM_LIB_DIR}</location>" >> ${DEST_DIR}/puscomp_config.xml
echo "</entry>" >> ${DEST_DIR}/puscomp_config.xml
echo "<entry>" >> ${DEST_DIR}/puscomp_config.xml
echo "<name>Unisim Simulators</name>" >> ${DEST_DIR}/puscomp_config.xml
echo "<location>${UNISIM_SIMULATORS_DIR}</location>" >> ${DEST_DIR}/puscomp_config.xml
echo "</entry>" >> ${DEST_DIR}/puscomp_config.xml
echo "</repositories>" >> ${DEST_DIR}/puscomp_config.xml

cd ${MY_DIR}/puscomp
python3 ./puscomp.py --link -c ${DEST_DIR}/puscomp_config.xml -o ${DEST_DIR} unisim_simulator::tlm2::armemu

rm -f ${DEST_DIR}/puscomp_config.xml
