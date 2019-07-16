#!/bin/bash
if [ -z "$1" ]; then
	echo "Usage:"
	echo "  $0 <destination directory>"
	exit -1
fi

if [ -z "${SIMPKG}" ]; then
	echo "SIMPKG is not set"
	exit -1
fi

PACKAGE_DIR=$(cd $(dirname $0); pwd)
UNISIM_DIR=$(cd $(dirname $(dirname $0)); pwd)
UNISIM_TOOLS_DIR=${UNISIM_DIR}/unisim_tools
UNISIM_LIB_DIR=${UNISIM_DIR}/unisim_lib
UNISIM_SIMULATOR_DIR=${UNISIM_DIR}/unisim_simulators/tlm2/${SIMPKG}
SIMULATOR_VERSION=$(cat ${UNISIM_SIMULATOR_DIR}/VERSION)
has_to_build_simulator_configure=no
has_to_build_configure=no
has_to_build_simulator_configure=no
unset UNISIM_LIB_SIMULATOR_SOURCE_FILES
unset UNISIM_LIB_SIMULATOR_HEADER_FILES
unset UNISIM_LIB_SIMULATOR_DATA_FILES
unset UNISIM_LIB_SIMULATOR_FILES
unset UNISIM_LIB_SIMULATOR_M4_FILES

declare -a UNISIM_LIB_PACKAGES
declare -A UNISIM_LIB_PACKAGE_IMPORTED

mkdir -p "$1"
DEST_DIR=$(cd "$1"; pwd)

if [ -z "${DISTCOPY}" ]; then
	DISTCOPY=cp
fi

function import()
{
	local PKG="$1"
	if [ -d "${PACKAGE_DIR}/${PKG}" ]; then
		if [ -z ${UNISIM_LIB_PACKAGE_IMPORTED["${PKG}"]} ]; then
			UNISIM_LIB_PACKAGE_IMPORTED["${PKG}"]=1
			
			local PKG_DEPS_TXT="${PACKAGE_DIR}/${PKG}/pkg_deps.txt"
			
			if [ -e "${PKG_DEPS_TXT}" ]; then
				local DEP_PKG
				while IFS= read -r DEP_PKG; do
					if ! import "${DEP_PKG}"; then
						echo -e "\033[31mERROR! Missing dependency: Package \"${PKG}\" requires Packages \"${DEP_PKG}\"\033[0m" >&2
						unset UNISIM_LIB_PACKAGE_IMPORTED["${PKG}"]
						return 1
					fi
				done < "${PKG_DEPS_TXT}"
			fi
			echo -e "Importing Package \"${PKG}\"" >&2
			UNISIM_LIB_PACKAGES+=("${PKG}")
		fi
		return 0
	fi

	echo -e "\033[31mERROR! Package \"${PKG}\" is missing\033[0m" >&2
	return 1
}

function files()
{
	local LIST_NAME="$1"
	local FOUND='no'
	for PACKAGE in ${UNISIM_LIB_PACKAGES[@]}; do
		LIST_FILENAME="${PACKAGE_DIR}/${PACKAGE}/${LIST_NAME}_list.txt"
		if [ -e "${LIST_FILENAME}" ]; then
			local FILENAME
			while IFS= read -r FILENAME; do
				FOUND='yes'
				echo -n " ${FILENAME}"
			done < "${LIST_FILENAME}"
		fi
	done
	
	if [ "${FOUND}" != 'yes' ]; then
		echo -e "\033[33mWARNING! There are no files in List \"${LIST_NAME}\"\033[0m" >&2
	fi
}

function lines()
{
	local LIST_NAME="$1"
	local FOUND='no'
	for PACKAGE in ${UNISIM_LIB_PACKAGES[@]}; do
		LIST_FILENAME="${PACKAGE_DIR}/${PACKAGE}/${LIST_NAME}_list.txt"
		if [ -e "${LIST_FILENAME}" ]; then
			local LINE
			while IFS= read -r LINE; do
				FOUND='yes'
				echo "${LINE}"
			done < "${LIST_FILENAME}"
		fi
	done
	
	if [ "${FOUND}" != 'yes' ]; then
		echo -e "\033[33mWARNING! There are no lines in List \"${LIST_NAME}\"\033[0m" >&2
	fi
}

function pkg_deps_changed()
{
	for PACKAGE in ${UNISIM_LIB_PACKAGES[@]}; do
		for FILE in ${PACKAGE_DIR}/${PACKAGE}/*; do
			if [ ! -e "$1" -o "${FILE}" -nt "$1" ]; then
				return 0;
			fi
		done
	done
	return 1;
}

function has_to_build() {
	if [ ! -e "$1" -o "$2" -nt "$1" ]; then
		return 0;
	else
		return 1;
	fi
}

function dist_copy() {
	if has_to_build "$2" "$1"; then
		echo "$1 ==> $2"
		mkdir -p "$(dirname $2)"
		${DISTCOPY} -f "$1" "$2" || exit
		return 0
	fi
	return 1
}

function make_pkg_configure_cross()
{
	local SIMPKG="$1"
	local CONFIGURE_CROSS="$2"
	cat << EOF_CONFIGURE_CROSS > "${CONFIGURE_CROSS}"
#!/bin/bash
HERE=\$(pwd)
MY_DIR=\$(cd \$(dirname \$0); pwd)

# remove --host from command line arguments
host=""
help=""
i=0
j=0
for arg in "\$@"
do
	case "\${arg}" in
		--host=*)
			host=\$(printf "%s" "\${arg}" | cut -f 2- -d '=')
			;;
		--help=* | --help)
			help="yes"
			args[\${j}]=\${arg}
			j=\$((\${j}+1))
			;;
		*)
			args[\${j}]=\${arg}
			j=\$((\${j}+1))
			;;
	esac
	i=\$((\${i}+1))
done

if test "\${help}" != "yes"; then
	if test -z "\${host}"; then
		echo "ERROR: No canonical name for the host system type was specified. Use --host=<canonical name> to specify a host system type (e.g. --host=i586-pc-mingw32)"
		exit -1
	fi
fi

if test "\${help}" = "yes"; then
	echo "=== configure help for genisslib"
else
	echo "=== configuring in genisslib (\${HERE}/genisslib)"
	echo "\$(basename \$0): running \${MY_DIR}/genisslib/configure \${args[@]}"
fi
if test ! -d \${HERE}/genisslib; then
	mkdir "\${HERE}/genisslib"
fi
cd "\${HERE}/genisslib"
\${MY_DIR}/genisslib/configure --disable-option-checking "\${args[@]}"
STATUS="\$?"
cd "\${HERE}"
if test \${STATUS} -ne 0; then
	exit \${STATUS}
fi

if test "\${help}" = "yes"; then
	echo "=== configure help for ${SIMPKG}"
else
	echo "=== configuring in ${SIMPKG} (\${HERE}/${SIMPKG}) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/${SIMPKG}/configure \$@"
fi

if test ! -d \${HERE}/${SIMPKG}; then
	mkdir \${HERE}/${SIMPKG}
fi
cd \${HERE}/${SIMPKG}
\${MY_DIR}/${SIMPKG}/configure "\$@"
STATUS="\$?"
cd "\${HERE}"
if test \${STATUS} -ne 0; then
	exit \${STATUS}
fi

if test "\${help}" = "yes"; then
	exit 0
fi

echo "\$(basename \$0): creating Makefile.cross"
cat << EOF_MAKEFILE_CROSS > Makefile.cross
#!/usr/bin/make -f
all: ${SIMPKG}-all
clean: genisslib-clean ${SIMPKG}-clean
distclean: genisslib-distclean ${SIMPKG}-distclean
	rm -f \${HERE}/Makefile.cross
install: ${SIMPKG}-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
${SIMPKG}-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
${SIMPKG}-clean:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
${SIMPKG}-distclean:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} distclean
${SIMPKG}-install:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG} install
EOF_MAKEFILE_CROSS

chmod +x Makefile.cross

echo "\$(basename \$0): run 'make -f \${HERE}/Makefile.cross' or '\${HERE}/Makefile.cross' to build for \${host} host system type"
EOF_CONFIGURE_CROSS
	chmod +x "${CONFIGURE_CROSS}"
}

function copy()
{
	local status=0
	for LIST_NAME in "$@"; do
		for FILE in $(files ${LIST_NAME}); do
			if ! dist_copy "${UNISIM_LIB_DIR}/${FILE}" "${DEST_DIR}/${SIMPKG}/${FILE}"; then
				status=1
			fi
		done
	done
	return ${status}
}

function import_genisslib()
{
	GILINSTALL=noinst ${UNISIM_DIR}/package/dist_genisslib.sh ${DEST_DIR}/genisslib
}
