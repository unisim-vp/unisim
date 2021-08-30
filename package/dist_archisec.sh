#!/bin/bash

SIMPKG=archisec
SIMPKG_SRCDIR=cxx/archisec
SIMPKG_DSTDIR=.

source "$(dirname $0)/dist_common.sh"

import_genisslib || exit

# ARMv7

import dist_armsec

# ARMv8

import unisim/component/cxx/processor/arm || exit
import unisim/component/cxx/processor/arm/isa/arm64 || exit

# x86

import unisim/component/cxx/processor/intel || exit
import unisim/component/cxx/processor/intel/isa || exit

# Common

import unisim/util/arithmetic || exit
import unisim/util/endian || exit
import unisim/util/random || exit
import unisim/util/symbolic/vector || exit
import unisim/util/symbolic/binsec || exit

import libc/inttypes || exit
import libc/stdint || exit
import std/algorithm || exit
import std/bitset || exit
import std/cassert || exit
import std/cmath || exit
import std/cstdio || exit
import std/cstdlib || exit
import std/fstream || exit
import std/iomanip || exit
import std/iosfwd || exit
import std/iostream || exit
import std/map || exit
import std/memory || exit
import std/ostream || exit
import std/set || exit
import std/sstream || exit
import std/string || exit
import std/vector || exit
import sys/mman || exit

copy isa_thumb isa_arm32 isa source header template data

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="$(files header) $(files template)"

UNISIM_LIB_SIMULATOR_DATA_FILES="\
"

UNISIM_SIMULATOR_SOURCE_FILES="\
main.cc \
arch.cc \
decode.cc \
"

UNISIM_SIMULATOR_ISA_FILES="\
top_thumb.isa \
top_arm32.isa \
aarch64dec.isa \
"

UNISIM_SIMULATOR_HEADER_FILES="\
arch.hh \
${UNISIM_SIMULATOR_ISA_FILES} \
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
decoder.ml \
unisim_archisec.ml
"

UNISIM_SIMULATOR_FILES="${UNISIM_SIMULATOR_SOURCE_FILES} ${UNISIM_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_DATA_FILES}"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Yves Lhuillier <yves.lhuillier@cea.fr>
Frédéric Recoules <frederic.recoules@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - amd64dba: an amd64/intel64 unit tests generator

See INSTALL for installation instructions.
EOF

cat << EOF > "${DEST_DIR}/dune-project"
(lang dune 2.7)

(package
 (name unisim_archisec)
 (synopsis "UNISIM-VP decoder")
 (description "UNISIM-VP decoder.")
 (depends
  (ocaml (>= 4.05))
  (conf-gcc :build)
  (conf-g++ :build)))
EOF

cat <<EOF > "${DEST_DIR}/dune"
(subdir
 genisslib
 (rule
  (target genisslib)
  (deps (source_tree .))
  (action
   (no-infer
    (progn
     (run ./configure)
     (run make))))))

(rule
 (target .unisim-tree)
 (deps (source_tree unisim))
 (action
  (write-file %{target} "")))

(subdir
 unisim/util
 (include_subdirs unqualified)
 (foreign_library
 (archive_name util)
 (language cxx)
 (names (:standard))
 (flags :standard -I ../..)
 (extra_deps ../../.unisim-tree)))

(subdir
 unisim/component/cxx/processor/intel
 (include_subdirs unqualified)
 (foreign_library
 (archive_name intel)
 (language cxx)
 (names (:standard))
 (flags :standard -I ../../../../..)
 (extra_deps ../../../../../.unisim-tree)))

(rule
 (targets top_arm32.hh top_arm32.tcc)
 (deps
  (:gen genisslib/genisslib)
  top_arm32.isa)
 (action
  (no-infer (run %{gen} -o top_arm32 -w 8 -I . top_arm32.isa))))

(subdir
 unisim/component/cxx/processor/arm
 (foreign_library
 (archive_name arm)
 (language cxx)
 (names (:standard))
 (flags :standard -I ../../../../..)
 (extra_deps ../../../../../.unisim-tree)))

(rule
 (targets top_thumb.hh top_thumb.tcc)
 (deps
  (:gen genisslib/genisslib)
  top_thumb.isa)
 (action
  (no-infer (run %{gen} -o top_thumb -w 8 -I . top_thumb.isa))))

(subdir
 unisim/component/cxx/processor/arm/isa/arm64
 (foreign_library
 (archive_name arm64)
 (language cxx)
 (names (:standard))
 (flags :standard -I ../../../../../../..)
 (extra_deps ../../../../../../../.unisim-tree)))

(rule
 (targets aarch64dec.hh aarch64dec.tcc)
 (deps
  (:gen genisslib/genisslib)
  aarch64dec.isa)
 (action
  (no-infer (run %{gen} -o aarch64dec -w 8 -I . aarch64dec.isa))))

(library
 (public_name unisim_archisec)
 (modules decoder)
 (foreign_archives
  unisim/util/util
  unisim/component/cxx/processor/arm/arm
  unisim/component/cxx/processor/arm/isa/arm64/arm64
  unisim/component/cxx/processor/intel/intel)
 (foreign_stubs
  (language cxx)
  (names (:standard))
  (flags :standard -I .)
  (extra_deps .unisim-tree top_arm32.tcc top_thumb.tcc aarch64dec.tcc))
 (c_library_flags :standard -lstdc++))

(executable
 (public_name unisim_archisec)
 (modules unisim_archisec)
 (libraries unisim_archisec))
EOF


echo "Distribution is up-to-date"
