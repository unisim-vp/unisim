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

if [ -z "${SIMPKG_SRCDIR}" ]; then
	echo "SIMPKG_SRCDIR is not set"
	exit -1
fi

PACKAGE_DIR=$(cd $(dirname $BASH_SOURCE); pwd)
UNISIM_DIR=$(cd $(dirname $(dirname $BASH_SOURCE)); pwd)
UNISIM_TOOLS_DIR="${UNISIM_DIR}/unisim_tools"
UNISIM_LIB_DIR="${UNISIM_DIR}/unisim_lib"
UNISIM_SIMULATOR_DIR="${UNISIM_DIR}/unisim_simulators/${SIMPKG_SRCDIR}"
SIMULATOR_VERSION=$(cat "${UNISIM_SIMULATOR_DIR}/VERSION")
AM_SIMULATOR_VERSION=$(printf '%s' "${SIMULATOR_VERSION}" | sed -e 's/\./_/g')
AM_SIMPKG=$(printf '%s' "${SIMPKG}" | sed -e 's/-/_/g')
has_to_build_simulator_configure=no
has_to_build_configure=no

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
					echo "Package \"${PKG}\" requires Packages \"${DEP_PKG}\""
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

function list_pkg()
{
	local SEPFMT="$1"
	shift
	while [ $# -ne 0 ]; do
		local ONLY_IN
		local LIST_NAME
		IFS=: read LIST_NAME ONLY_IN <<<$1
		local SEP=""
		for PACKAGE in ${UNISIM_LIB_PACKAGES[@]}; do
			LIST_FILENAME="${PACKAGE_DIR}/${PACKAGE}/${LIST_NAME}_list.txt"
			if [ -e "${LIST_FILENAME}" ]; then
				local FILENAME
				while IFS= read -r FILENAME; do
					if [[ ${FILENAME} != ${ONLY_IN}* ]]; then
						continue;
					fi
					echo -n "${SEP}${FILENAME}"
					SEP="$SEPFMT"
				done < "${LIST_FILENAME}"
			fi
		done
		
#		if [ "${SEP}" == "" ]; then
#			echo -e "\033[33mWARNING! There are no lines in List \"${LIST_NAME}\"\033[0m" >&2
#		fi
		shift
	done
}

function files()
{
	list_pkg " " "$@"
}

function lines()
{
	list_pkg $'\n' "$@"
	echo
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

function copy()
{
	local status=0
	for LIST_NAME in "$@"; do
		for FILE in $(files ${LIST_NAME}); do
				if ! dist_copy "${UNISIM_LIB_DIR}/${FILE}" "${DEST_DIR}/${FILE}"; then
					status=1
				fi
		done
	done
	return ${status}
}

function import_genisslib()
{
  dist_copy "${UNISIM_TOOLS_DIR}/genisslib/genisslib.py" "${DEST_DIR}/genisslib.py"
  import m4/ax_python
}

function output_simulator_configure_ac()
{
	local SIMULATOR_CONFIGURE_AC
	SIMULATOR_CONFIGURE_AC="${DEST_DIR}/configure.ac"
	if has_to_build "${SIMULATOR_CONFIGURE_AC}" "$0" || \
	   pkg_deps_changed "${SIMULATOR_CONFIGURE_AC}" || \
	   has_to_build "${SIMULATOR_CONFIGURE_AC}" "${UNISIM_SIMULATOR_DIR}/VERSION" || \
	   has_to_build "${SIMULATOR_CONFIGURE_AC}" "${PACKAGE_DIR}/dist_common.sh"; then
		echo "Generating ${SIMPKG} configure.ac"
		cat "$1" > "${SIMULATOR_CONFIGURE_AC}"
		has_to_build_simulator_configure=yes
	fi
}

function output_simulator_makefile_am()
{
	local SIMULATOR_MAKEFILE_AM
	SIMULATOR_MAKEFILE_AM="${DEST_DIR}/Makefile.am"
	
	if has_to_build "${SIMULATOR_MAKEFILE_AM}" "$0" || \
	   pkg_deps_changed "${SIMULATOR_MAKEFILE_AM}" || \
	   has_to_build "${SIMULATOR_MAKEFILE_AM}" "${UNISIM_SIMULATOR_DIR}/VERSION" || \
	   has_to_build "${SIMULATOR_MAKEFILE_AM}" "${PACKAGE_DIR}/dist_common.sh"; then
		echo "Generating ${SIMPKG} Makefile.am"
		cat "$1" > "${SIMULATOR_MAKEFILE_AM}"
		has_to_build_simulator_configure=yes
	fi
}

function build_simulator_configure()
{
	local SIMULATOR_CONFIG
	local SIMULATOR_CONFIGURE_AC
	local SIMULATOR_CONFIGURE
	local SIMULATOR_DIR
  SIMULATOR_CONFIG="${DEST_DIR}/config"
  SIMULATOR_CONFIGURE_AC="${DEST_DIR}/configure.ac"
  SIMULATOR_CONFIGURE="${DEST_DIR}/configure"
  SIMULATOR_DIR="${DEST_DIR}"
	
	mkdir -p "${SIMULATOR_CONFIG}"
	
	if has_to_build "${SIMULATOR_CONFIGURE}" "${PACKAGE_DIR}/dist_common.sh"; then
		has_to_build_simulator_configure=yes
	fi
	
	if [ "${has_to_build_simulator_configure}" = "yes" ]; then
		echo "Building ${SIMPKG} configure"
		local CMD="cd '${SIMULATOR_DIR}' && aclocal -I m4"
		# optionally run libtoolize only if LT_INIT is present in configure.ac
		if grep -qs "LT_INIT" "${SIMULATOR_CONFIGURE_AC}"; then
			CMD+=" && libtoolize --force"
		fi
		CMD+=" && autoconf --force && autoheader && automake -ac"
		echo "Running: ${CMD}"
		${SHELL} -c "${CMD}"
	fi
}
