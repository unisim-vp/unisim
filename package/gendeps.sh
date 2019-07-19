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
			if grep -e '^# *include *<systemc>' "${FILEPATH}" &> /dev/null; then
				echo "m4/systemc" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<sys/socket\.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/bsd_sockets" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<sys/times\.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/times" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<cxxabi\.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/cxxabi" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<libxml/.*\.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/libxml2" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<dlfcn\.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/get_exec_path" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<tvs/.*\.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/tvs" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<editline/.*\.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/libedit" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<pthread.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/pthread" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<boost/.*\.hpp>' "${FILEPATH}" &> /dev/null; then
				echo "m4/boost" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<cacti.*\.hh>' "${FILEPATH}" &> /dev/null; then
				echo "m4/cacti" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<SDL\.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/sdl" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<artimon\.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/artimon" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<artimon\.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/artimon" >> "${PKG_DEPS_TXT}"
			fi
			if grep -e '^# *include *<lua\.h>' "${FILEPATH}" &> /dev/null; then
				echo "m4/lua" >> "${PKG_DEPS_TXT}"
			fi
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
			echo "unisim/service/interfaces/${HEADER_FILENAME}" > "${PACKAGE_DIR}/unisim/service/interfaces/${PKG_NAME}/header_list.txt"
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
			printf "" > "${PKG_DEPS_TXT}"
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
				TRACKED_FILES_COUNT=$(git ls-files "${FILENAME}" | wc -l)
				if [ "${INCLUDE_UNTRACKED_FILES}" = "yes" ] || git ls-files --error-unmatch "${FILENAME}" &> /dev/null; then
					if [ -z "${BASE}" ]; then
						crawl_directory "${FILENAME}"
					else
						crawl_directory "${BASE}/${FILENAME}"
					fi
				fi
			elif [ -f "${FILEPATH}" ]; then
				discover_file_deps "${BASE}" "${FILENAME}"
			fi
		done
	fi
	cd "${SAVE_PWD}"
}

mkdir -p "${PACKAGE_DIR}/m4"

mkdir -p "${PACKAGE_DIR}/m4/times"
echo "m4/times.m4" > "${PACKAGE_DIR}/m4/times/m4_list.txt"
echo "UNISIM_CHECK_TIMES" > "${PACKAGE_DIR}/m4/times/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/endian"
echo "m4/endian.m4" > "${PACKAGE_DIR}/m4/endian/m4_list.txt"
echo "UNISIM_CHECK_ENDIAN" > "${PACKAGE_DIR}/m4/endian/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/cxxabi"
echo "m4/cxxabi.m4" > "${PACKAGE_DIR}/m4/cxxabi/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/cxxabi/pkg_deps.txt"
echo "UNISIM_CHECK_CXXABI" > "${PACKAGE_DIR}/m4/cxxabi/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/libxml2"
echo "m4/libxml2.m4" > "${PACKAGE_DIR}/m4/libxml2/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/libxml2/pkg_deps.txt"
echo "m4/zlib" > "${PACKAGE_DIR}/m4/libxml2/pkg_deps.txt"
echo "UNISIM_CHECK_LIBXML2" > "${PACKAGE_DIR}/m4/libxml2/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/zlib"
echo "m4/zlib.m4" > "${PACKAGE_DIR}/m4/zlib/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/zlib/pkg_deps.txt"
echo "UNISIM_CHECK_ZLIB" > "${PACKAGE_DIR}/m4/zlib/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/bsd_sockets"
echo "m4/bsd_sockets.m4" > "${PACKAGE_DIR}/m4/bsd_sockets/m4_list.txt"
echo "UNISIM_CHECK_BSD_SOCKETS" > "${PACKAGE_DIR}/m4/bsd_sockets/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/curses"
echo "m4/curses.m4" > "${PACKAGE_DIR}/m4/curses/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/curses/pkg_deps.txt"

mkdir -p "${PACKAGE_DIR}/m4/libedit"
echo "m4/libedit.m4" > "${PACKAGE_DIR}/m4/libedit/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/libedit/pkg_deps.txt"
echo "UNISIM_CHECK_LIBEDIT" > "${PACKAGE_DIR}/m4/libedit/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/systemc"
echo "m4/systemc.m4" > "${PACKAGE_DIR}/m4/systemc/m4_list.txt"
echo "UNISIM_CHECK_SYSTEMC" > "${PACKAGE_DIR}/m4/systemc/ac_list.txt"
echo "m4/pthread" > "${PACKAGE_DIR}/m4/systemc/pkg_deps.txt"

mkdir -p "${PACKAGE_DIR}/m4/check_lib"
echo "m4/check_lib.m4" > "${PACKAGE_DIR}/m4/check_lib/m4_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/get_exec_path"
echo "m4/get_exec_path.m4" > "${PACKAGE_DIR}/m4/get_exec_path/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/get_exec_path/pkg_deps.txt"
echo "UNISIM_CHECK_GET_EXECUTABLE_PATH" > "${PACKAGE_DIR}/m4/get_exec_path/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/real_path"
echo "m4/real_path.m4" > "${PACKAGE_DIR}/m4/real_path/m4_list.txt"
echo "UNISIM_CHECK_REAL_PATH" > "${PACKAGE_DIR}/m4/real_path/ac_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/real_path/pkg_deps.txt"

mkdir -p "${PACKAGE_DIR}/m4/pthread"
echo "m4/pthread.m4" > "${PACKAGE_DIR}/m4/pthread/m4_list.txt"
echo "UNISIM_CHECK_PTHREAD" > "${PACKAGE_DIR}/m4/pthread/ac_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/pthread/pkg_deps.txt"

mkdir -p "${PACKAGE_DIR}/m4/tvs"
echo "m4/tvs.m4" > "${PACKAGE_DIR}/m4/tvs/m4_list.txt"
echo "UNISIM_CHECK_TVS" > "${PACKAGE_DIR}/m4/tvs/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/boost"
echo "m4/boost.m4" > "${PACKAGE_DIR}/m4/boost/m4_list.txt"
echo "UNISIM_CHECK_BOOST(1.53.0)" > "${PACKAGE_DIR}/m4/boost/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/cacti"
echo "m4/cacti.m4" > "${PACKAGE_DIR}/m4/cacti/m4_list.txt"
echo "UNISIM_CHECK_CACTI" > "${PACKAGE_DIR}/m4/cacti/ac_list.txt"

crawl_directory "unisim"

echo "unisim/component/cxx/processor/arm/isa/arm32" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb2" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav7/pkg_deps.txt"

echo "unisim/component/cxx/processor/arm/isa/arm32" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb2" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pmsav7/pkg_deps.txt"

echo "unisim/component/cxx/processor/arm/isa/arm64" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav8/pkg_deps.txt"

echo "m4/endian" > "${PACKAGE_DIR}/unisim/util/endian/pkg_deps.txt"
echo "m4/real_path" >> "${PACKAGE_DIR}/unisim/kernel/service/pkg_deps.txt"

sed -i '/unisim\/component\/cxx\/processor\/arm\/cache/d' "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pkg_deps.txt"

mv "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/isa_arm32_list.txt"
mv "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/isa_thumb_list.txt"
mv "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb2/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb2/isa_thumb2_list.txt"

mv "${PACKAGE_DIR}/unisim/component/cxx/processor/powerpc/isa/book_vle/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/powerpc/isa/book_vle/isa_vle_list.txt"

sed -i '/unisim\/component\/cxx\/processor\/hcs12x\/xb\.isa/d' "${PACKAGE_DIR}/unisim/component/cxx/processor/hcs12x/isa_list.txt"
sed -i '/unisim\/component\/cxx\/processor\/hcs12x\/s12xgate\.isa/d' "${PACKAGE_DIR}/unisim/component/cxx/processor/hcs12x/isa_list.txt"

echo "unisim/component/cxx/processor/hcs12x/xb.isa" >  "${PACKAGE_DIR}/unisim/component/cxx/processor/hcs12x/isa_xb_list.txt"
echo "unisim/component/cxx/processor/hcs12x/s12xgate.isa" >  "${PACKAGE_DIR}/unisim/component/cxx/processor/hcs12x/isa_s12xgate_list.txt"
