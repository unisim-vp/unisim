#!/bin/bash
function replace()
{
	local SEARCH_FOR="$(echo -n "$1" | tr $'\n' $'\r')"
	local REPLACE_BY="$(echo -n "$2" | tr $'\n' $'\r')"
	local SED_REGEX="$(echo "${SEARCH_FOR}" | sed -e 's/[]\/$*.^[\#-]/\\&/g')"
	local SED_REPLACEMENT="$(echo "${REPLACE_BY}" | sed -e 's/[\\\/#]/\\&/g')"
# 	printf '%q\n' "SED_REGEX=${SED_REGEX}" >&2
# 	printf '%q\n' "SED_REPLACEMENT=${SED_REPLACEMENT}" >&2
	tr $'\n' $'\r' | sed -e "s/${SED_REGEX}/${SED_REPLACEMENT}/g" | tr $'\r' $'\n'
}

function replace_f()
{
	local SEARCH_FOR="$(cat "$1" | tr $'\n' $'\r')"
	local REPLACE_BY="$(cat "$2" | tr $'\n' $'\r')"
	local SED_REGEX="$(echo "${SEARCH_FOR}" | sed -e 's/[]\/$*.^[\#-]/\\&/g')"
	local SED_REPLACEMENT="$(echo "${REPLACE_BY}" | sed -e 's/[\\\/#]/\\&/g')"
# 	printf '%q\n' "SED_REGEX=${SED_REGEX}" >&2
# 	printf '%q\n' "SED_REPLACEMENT=${SED_REPLACEMENT}" >&2
	tr $'\n' $'\r' | sed -e "s/${SED_REGEX}/${SED_REPLACEMENT}/g" | tr $'\r' $'\n'
}

function delete()
{
	local SEARCH_FOR="$1"
	local LINE_COUNT
	if [ $# -ge 2 ]; then
		LINE_COUNT=$2
	else
		LINE_COUNT=1
	fi
	[ ${LINE_COUNT} -ge 1 ] || LINE_COUNT=1
	local SED_REGEX="$(echo "${SEARCH_FOR}" | sed -e 's/[]\/$*.^[\#-]/\\&/g')"
#  	printf '%q\n' "SED_REGEX=${SED_REGEX}" >&2
# 	echo "LINE_COUNT=${LINE_COUNT}" >&2
	if [ ${LINE_COUNT} -eq 1 ]; then
		sed -e "/${SED_REGEX}/d"
	else
		sed -e "/${SED_REGEX}/,+$((${LINE_COUNT}-1))d"
	fi
}

function trim()
{
	local ARG="$1"
	# remove leading whitespace characters
	ARG="${ARG#"${ARG%%[![:space:]]*}"}"
	# remove trailing whitespace characters
	ARG="${ARG%"${ARG##*[![:space:]]}"}"
	echo "${ARG}"
}


for FILE in dist_*.sh; do
	NAME="$(echo "${FILE}" | sed -e 's/^dist_//' -e 's/\.sh$//')"
# 	echo "${NAME}"
	DIR="$(sed -n 's/^SIMPKG_SRCDIR=\(.*\)$/\1/p' "${FILE}")"
	[ -n "${DIR}" ] || continue
	[ "${DIR}" != '.' ] || continue
	OUT_FILE="../unisim_simulators/${DIR}/project.conf"
	[ ! -f "${OUT_FILE}" ] || continue
	echo "${OUT_FILE} is missing"
	continue
	[ -f "../unisim_simulators/${DIR}/VERSION" ] || continue
	VERSION="$(cat "../unisim_simulators/${DIR}/VERSION")"
# 	echo "${VERSION}"
	DISPLAY_NAME="$(sed -n 's/AC_INIT(\[\([^]]*\).*$/\1/p' "${FILE}")"
	AUTHORS_ON_LINE="$(sed -n 's/^AC_INIT(\[[^]]*\] *, *\[[^]]*\] *, *\[\([^]]*\).*$/\1/p' "${FILE}")"
	set -f
	IFS=',' read -r -a AUTHORS_ARRAY <<< "${AUTHORS_ON_LINE}"
	AUTHORS=''
	for AUTHOR in "${AUTHORS_ARRAY[@]}"; do
# 		echo "AUTHOR=\"${AUTHOR}\""
		AUTHORS+="author '$(trim "${AUTHOR}")'"$'\n'
	done
# 	echo "${AUTHORS}"
 	mkdir -p "${HOME}/tmp/projects/${NAME}"
 	cat "${FILE}" \
		| delete AC_INIT \
		| delete SIMPKG= \
		| delete SIMPKG_SRCDIR= \
		| delete 'copy m4' \
		| delete 'source "$(dirname $0)/dist_common.sh"' \
		| delete 'echo "Distribution is up-to-date"' \
		| replace import_genisslib subproject_genisslib \
		| replace output_simulator output_project \
		| replace build_simulator build_project \
		| replace SIMULATOR_VERSION PROJECT_VERSION \
		| replace SIMPKG PROJECT_NAME \
		| replace unisim- '' \
		| replace unisim_ '' \
		| replace_f <(cat << EOF
#!/bin/bash
EOF
	) <(cat << EOF
project unisim-${NAME}
version ${VERSION}
display_name '${DISPLAY_NAME}'
${AUTHORS}
EOF
	) \
	| replace_f <(cat << EOF
for file in \${UNISIM_SIMULATOR_FILES}; do
	dist_copy "\${UNISIM_SIMULATOR_DIR}/\${file}" "\${DEST_DIR}/\${file}"
done
EOF
) <(cat << EOF
dist_copy \${UNISIM_SIMULATOR_FILES}
EOF
) \
		| replace_f <(cat << EOF
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects tar-pax])
AC_PATH_PROGS(SH, sh)
AC_PROG_CXX
AC_PROG_INSTALL
LT_INIT
AC_SUBST(LIBTOOL_DEPS)
AC_PROG_LN_S
AC_LANG([C++])
AM_PROG_CC_C_O
case "\\\${host}" in
	*mingw*)
		CPPFLAGS="-U__STRICT_ANSI__ \\\${CPPFLAGS}"
		;;
	*)
		;;
esac
EOF
	) <(cat << EOF
lines \$(ac_common_header)
EOF
	) \
		| replace_f <(cat << EOF
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/\${PROJECT_NAME}-\${PROJECT_VERSION}"], [path of shared data relative to bin directory])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF
	) <(cat << EOF
lines \$(ac_common_footer)
EOF
	) \
		| replace_f <(cat << EOF
ACLOCAL_AMFLAGS=-I m4
AM_CPPFLAGS=-I\\\$(top_srcdir) -I\\\$(top_builddir)
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: \\\$(LIBTOOL_DEPS)
	\\\$(SHELL) ./config.status libtool
EOF
	) <(cat << EOF
lines \$(am_common_header)
EOF
	) \
		| sed -e 's/^\(copy .* data\)$/\1 m4/g' \
		| cat -s \
	> "${OUT_FILE}"
# 	> "${HOME}/tmp/projects/${NAME}/project.conf"
done
