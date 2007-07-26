#!/bin/sh
echo "doing aclocal; autoconf; autoheader; automake -a in"
find . -name "configure.ac" -execdir sh -c "pwd; aclocal; autoconf; has_ac_config_headers=\`grep AC_CONFIG_HEADERS configure.ac\`; if test \"x\$has_ac_config_headers\" != \"x\"; then autoheader; fi; automake -a" \;
