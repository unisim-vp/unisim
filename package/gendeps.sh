#!/bin/bash
INCLUDE_UNTRACKED_FILES=no # should be always no

# if [ -z "$1" ]; then
# 	echo "Usage: $0 <directory>"
# 	echo "Example: $0 unisim/kernel/service"
# 	exit
# fi

shopt -s nullglob

UNISIM_DIR=$(cd $(dirname $(cd $(dirname $0); pwd)); pwd)
PACKAGE_DIR="${UNISIM_DIR}/package"
UNISIM_LIB_DIR="${UNISIM_DIR}/unisim_lib"

function discover_file_deps
{
	local BASE="$1"
	local FILENAME="$2"
	
	local DIRPATH
	if [ -z "${BASE}" ]; then
		DIRPATH="${UNISIM_LIB_DIR}"
	else
		DIRPATH="${UNISIM_LIB_DIR}/${BASE}"
	fi
	
	local FILEPATH="${DIRPATH}/${FILENAME}"
	
	if [ "${INCLUDE_UNTRACKED_FILES}" = "yes" ] || git ls-files --error-unmatch "${UNISIM_LIB_DIR}/${BASE}/${FILENAME}" &> /dev/null; then
		mkdir -p "${PACKAGE_DIR}/${BASE}"
		local cc_regex='\.cc$|\.cpp$'
		local hh_regex='\.hh$|\.h$\.hpp$'
		local tcc_regex='\.tcc$|\.tpp$'
		local isa_regex='\.isa$'
		local interfaces_regex='^unisim/service/interfaces'
		
		local PKG_DEPS_TXT="${PACKAGE_DIR}/${BASE}/pkg_deps.txt"
		
		if [[ "${FILEPATH}" =~ ${cc_regex} ]] || [[ "${FILEPATH}" =~ ${hh_regex} ]] || [[ "${FILEPATH}" =~ ${tcc_regex} ]] || [[ "${FILEPATH}" =~ ${isa_regex} ]]; then
			local INCLUDE_DEP
			for INCLUDE_DEP in $(grep -e '^# *include' "${FILEPATH}" | grep "unisim/" | sed -e 's/^# *include *"//g' -e 's/^# *include *<//g' -e 's/".*$//g' -e 's/>.*$//g'); do
				local PKG_DEP
				if [[ "${INCLUDE_DEP}" =~ ${interfaces_regex} ]]; then
					PKG_DEP=$(echo "${INCLUDE_DEP}" | sed -e 's/\.hh$//')
				else
					PKG_DEP=$(dirname "${INCLUDE_DEP}")
				fi
				
				if [ "${BASE}" != "${PKG_DEP}" ]; then
					echo "${PKG_DEP}" >> "${PKG_DEPS_TXT}"
				fi
			done
		fi
		
		if [[ "${FILEPATH}" =~ ${isa_regex} ]]; then
			local INCLUDE_DEP
			for INCLUDE_DEP in $(grep -e '^ *include' "${FILEPATH}" | grep "unisim/" | sed -e 's/^ *include *"//g' -e 's/".*$//g'); do
				local PKG_DEP
				if [[ "${INCLUDE_DEP}" =~ ${interfaces_regex} ]]; then
					PKG_DEP=$(echo "${INCLUDE_DEP}" | sed -e 's/\.hh$//')
				else
					PKG_DEP=$(dirname "${INCLUDE_DEP}")
				fi
				
				if [ "${BASE}" != "${PKG_DEP}" ]; then
					echo "${PKG_DEP}" >> "${PKG_DEPS_TXT}"
				fi
			done
		fi
		if [ -e "${PKG_DEPS_TXT}" ]; then
			sort -u -o "${PKG_DEPS_TXT}" "${PKG_DEPS_TXT}"
		fi
	fi
}

function list_files
{
	local BASE="$1"
	shift
	local LIST_NAME="$1"
	shift
	local FILENAME_LIST=("$@")
	
	mkdir -p "${PACKAGE_DIR}/${BASE}"
	
	if [ ${#FILENAME_LIST[@]} -ne 0 ]; then
		printf "" > "${PACKAGE_DIR}/${BASE}/${LIST_NAME}_list.txt"
		for FILENAME in "${FILENAME_LIST[@]}"; do
			if [ "${INCLUDE_UNTRACKED_FILES}" = "yes" ] || git ls-files --error-unmatch "${UNISIM_LIB_DIR}/${BASE}/${FILENAME}" &> /dev/null; then
				echo "${BASE}/${FILENAME}" >> "${PACKAGE_DIR}/${BASE}/${LIST_NAME}_list.txt"
			fi
		done
	fi
}

function list_interfaces_files()
{
	cd "${UNISIM_LIB_DIR}/unisim/service/interfaces"
	local HEADER_FILENAME
	for HEADER_FILENAME in *.hh; do
		if [ "${INCLUDE_UNTRACKED_FILES}" = "yes" ] || git ls-files --error-unmatch "${UNISIM_LIB_DIR}/unisim/service/interfaces/${HEADER_FILENAME}" &> /dev/null; then
			local PKG_NAME=$(echo "${HEADER_FILENAME}" | sed -e 's/\.hh$//')
			mkdir -p "${PACKAGE_DIR}/unisim/service/interfaces/${PKG_NAME}"
			echo "unisim/service/interfaces/${HEADER_FILENAME}" >> "${PACKAGE_DIR}/unisim/service/interfaces/${PKG_NAME}/header_list.txt"
		fi
	done
}

function discover_interfaces_file_deps()
{
	cd "${UNISIM_LIB_DIR}/unisim/service/interfaces"
	local interfaces_regex='^unisim/service/interfaces'
	local HEADER_FILENAME
	for HEADER_FILENAME in *.hh *.h *.hpp; do
		if [ "${INCLUDE_UNTRACKED_FILES}" = "yes" ] || git ls-files --error-unmatch "${UNISIM_LIB_DIR}/unisim/service/interfaces/${HEADER_FILENAME}" &> /dev/null; then
			local PKG_NAME=$(echo "${HEADER_FILENAME}" | sed -e 's/\.hh$//')
			local PKG_DEPS_TXT="${PACKAGE_DIR}/unisim/service/interfaces/${PKG_NAME}/pkg_deps.txt"
			for INCLUDE_DEP in $(grep -e '^# *include' "${HEADER_FILENAME}" | grep "unisim/" | sed -e 's/^# *include *"//g' -e 's/^# *include *<//g' -e 's/".*$//g' -e 's/>.*$//g'); do
				local PKG_DEP
				if [[ "${INCLUDE_DEP}" =~ ${interfaces_regex} ]]; then
					PKG_DEP=$(echo "${INCLUDE_DEP}" | sed -e 's/\.hh$//')
				else
					PKG_DEP=$(dirname "${INCLUDE_DEP}")
				fi
				
				if [ "${BASE}" != "${PKG_DEP}" ]; then
					echo "${PKG_DEP}" >> "${PKG_DEPS_TXT}"
				fi
			done
		fi
	done
}

function crawl_directory
{
	local SAVE_PWD=${PWD}
	local BASE="$1"
	
	if [ -n "${BASE}" ]; then
		local PKG_DEPS_TXT="${PACKAGE_DIR}/${BASE}/pkg_deps.txt"
		if [ -e "${PKG_DEPS_TXT}" ]; then
			rm -f "${PKG_DEPS_TXT}"
		fi
	fi
	
	local DIRPATH
	if [ -z "${BASE}" ]; then
		DIRPATH="${UNISIM_LIB_DIR}"
	else
		DIRPATH="${UNISIM_LIB_DIR}/${BASE}"
	fi
	
	cd "${DIRPATH}"
	
	if [ "${BASE}" = "unisim/service/interfaces" ]; then
		list_interfaces_files
		discover_interfaces_file_deps
	else
		local SOURCE_LIST=(*.cc *.cpp)
		list_files "${BASE}" source "${SOURCE_LIST[@]}"
		local HEADER_LIST=(*.hh *.h *.hpp)
		list_files "${BASE}" header "${HEADER_LIST[@]}"
		local TEMPLATE_LIST=(*.tcc *.tpp)
		list_files "${BASE}" template "${TEMPLATE_LIST[@]}"
		local ISA_LIST=(*.isa)
		list_files "${BASE}" isa "${ISA_LIST[@]}"
		local DATA_LIST=(*.xml *.svg *.css *.js *.json *.ini *.ico *.png)
		list_files "${BASE}" data "${DATA_LIST[@]}"

		local FILENAME
		for FILENAME in *; do
			FILEPATH="${DIRPATH}/${FILENAME}"
			if [ -d "${FILEPATH}" ]; then
				if [ -z "${BASE}" ]; then
					crawl_directory "${FILENAME}"
				else
					crawl_directory "${BASE}/${FILENAME}"
				fi
			elif [ -f "${FILEPATH}" ]; then
				discover_file_deps "${BASE}" "${FILENAME}"
			fi
		done
	fi
	cd "${SAVE_PWD}"
}

crawl_directory "unisim"

echo "unisim/component/cxx/processor/arm/isa/arm32" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb2" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav7/pkg_deps.txt"

echo "unisim/component/cxx/processor/arm/isa/arm32" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb2" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pmsav7/pkg_deps.txt"

echo "unisim/component/cxx/processor/arm/isa/arm64" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav8/pkg_deps.txt"

sed -i '/unisim\/component\/cxx\/processor\/arm\/cache/d' "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pkg_deps.txt"

mv "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/isa_arm32_list.txt"
mv "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/isa_thumb_list.txt"
mv "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb2/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb2/isa_thumb2_list.txt"

mv "${PACKAGE_DIR}/unisim/component/cxx/processor/powerpc/isa/book_vle/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/powerpc/isa/book_vle/isa_vle_list.txt"
