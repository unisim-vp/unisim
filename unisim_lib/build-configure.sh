#!/bin/bash
export PATH=/usr/local/bin:/bin:/usr/bin:/usr/X11R6/bin
export m4_path="`pwd`/m4"
echo "doing aclocal -I$m4_path; autoconf; autoheader; automake -a in"
find . -name "configure.ac" -execdir /bin/bash -c \
"current_path=\`pwd\`; \
has_to_build=\"false\"; \
if test -e configure.ac; \
then \
  if test -e configure; \
  then \
    if test ${m4_path}/acinclude.m4 -nt configure || test ${m4_path}/unisim.m4 -nt configure; \
    then \
      is_configure_ac_modified=\"true\"; \
      has_to_build=\"true\"; \
    fi; \
    if test configure.ac -nt configure; \
    then \
      is_configure_ac_modified=\"true\"; \
      has_to_build=\"true\"; \
    fi; \
  else \
    is_configure_missing=\"true\"; \
    has_to_build=\"true\"; \
  fi; \
fi;\
if test -e Makefile.am; \
then \
  if test -e Makefile.in; \
  then \
    if test ${m4_path}/aminclude.am -nt Makefile.in || test ${m4_path}/unisim.am -nt Makefile.in; \
    then \
      is_makefile_am_modified=\"true\"; \
      has_to_build=\"true\"; \
    fi; \
    if test Makefile.am -nt Makefile.in; \
    then \
      is_makefile_am_modified=\"true\"; \
      has_to_build=\"true\"; \
    fi; \
  else \
    is_makefile_missing=\"true\"; \
    has_to_build=\"true\"; \
  fi; \
fi; \
message_to_display=\"\$current_path (\"; \
if test \"x\$has_to_build\" == \"xtrue\"; \
then \
  message_to_display=\"building:  \$current_path\";\
  if test \"x\$is_configure_ac_modified\" != \"x\"; \
  then \
    message_to_display=\$message_to_display\" (configure.ac modified)\"; \
  fi; \
  if test \"x\$is_configure_missing\" != \"x\"; \
  then \
    message_to_display=\$message_to_display\" (configure missing)\"; \
  fi; \
  if test \"x\$is_makefile_am_modified\" != \"x\"; \
  then \
    message_to_display=\$message_to_display\" (Makefile.am modified)\"; \
  fi; \
  if test \"x\$is_makefile_missing\" != \"x\"; \
  then \
    message_to_display=\$message_to_display\" (Makefile.in missing)\"; \
  fi; \
  echo \"\$message_to_display\"; \
  aclocal -I \$m4_path; \
  autoconf; \
  has_ac_config_headers=\`grep AC_CONFIG_HEADERS configure.ac\`; \
  if test \"x\$has_ac_config_headers\" != \"x\"; \
  then \
    autoheader; \
  fi; \
  automake -a; \
else \
  echo \"skipping:  \$current_path\"; \
fi" \;
