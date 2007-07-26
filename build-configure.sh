#!/bin/sh
export m4_path="`pwd`/m4"
echo "doing aclocal -I$m4_path; autoconf; autoheader; automake -a in"
find . -name "configure.ac" -execdir sh -c "pwd; aclocal -I\$m4_path; autoconf; has_ac_config_headers=\`grep AC_CONFIG_HEADERS configure.ac\`; if test \"x\$has_ac_config_headers\" != \"x\"; then autoheader; fi; automake -a" \;
