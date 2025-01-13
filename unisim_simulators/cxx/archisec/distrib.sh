#!/bin/bash

ARCHISEC_DIR=$(cd $(dirname $BASH_SOURCE); pwd)
UNISIMVP_DIR=$(cd "$ARCHISEC_DIR/../../.."; pwd)


function gen_config()
{
    # Generating the config file that could be in ~/.config/unisimatic/config.yaml
cat << EOF
repositories:
  - name: unisim_lib
    directory: ${UNISIMVP_DIR}/unisim_lib
    package_dir: ../package
    enabled: true
    default: yes
  - name: genisslib
    directory: ${UNISIMVP_DIR}/unisim_tools/genisslib
    package_dir: package
    enabled: true
EOF
}

cat <(gen_config)
# Calling the distribution script
${UNISIMVP_DIR}/unisim_tools/unisimatic/unisimatic --config=<(gen_config) dist ${ARCHISEC_DIR}/distrib.conf $1





