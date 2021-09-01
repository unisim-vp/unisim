#!/bin/bash

SIMPKG=archisec
SIMPKG_SRCDIR=.
SIMPKG_DSTDIR=.
ARCHISEC_SRCDIR=cxx/archisec

source "$(dirname $0)/dist_common.sh"

function dist_copy_and_patch() {
	if has_to_build "$2" "$1"; then
		echo "$1 ==> $2"
		mkdir -p "$(dirname $2)"
		${DISTCOPY} -f "$1" "$2" || exit
		patch "$2" "$3" || exit
		return 0
	fi
	return 1
}

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

ARMSEC_SRCDIR=cxx/armsec
ARMSEC_DSTDIR=unisim/component/cxx/processor/arm
ARMSEC_ISA_FILES="\
top_thumb.isa \
top_arm32.isa \
"

for file in ${ARMSEC_ISA_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${ARMSEC_SRCDIR}/${file}" \
    "${DEST_DIR}/${ARMSEC_DSTDIR}/${file}"
done
dist_copy_and_patch "${UNISIM_SIMULATOR_DIR}/${ARMSEC_SRCDIR}/main.cc" \
		    "${DEST_DIR}/${ARMSEC_DSTDIR}/main.cc" \
		    "${UNISIM_SIMULATOR_DIR}/${ARCHISEC_SRCDIR}/armsec.patch"

AARCH64_SRCDIR=cxx/aarch64dba
AARCH64_DSTDIR=unisim/component/cxx/processor/arm/isa/arm64
AARCH64_ISA_FILES="\
aarch64dec.isa \
"

for file in ${AARCH64_ISA_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${AARCH64_SRCDIR}/${file}" \
    "${DEST_DIR}/${AARCH64_DSTDIR}/${file}"
done
dist_copy_and_patch "${UNISIM_SIMULATOR_DIR}/${AARCH64_SRCDIR}/main.cc" \
		    "${DEST_DIR}/${AARCH64_DSTDIR}/main.cc" \
		    "${UNISIM_SIMULATOR_DIR}/${ARCHISEC_SRCDIR}/aarch64.patch"

AMD64_SRCDIR=cxx/amd64dba
AMD64_DSTDIR=unisim/component/cxx/processor/amd64
AMD64_ISA_FILES="\
arch.hh \
arch.cc \
decode.cc \
"

for file in ${AMD64_ISA_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${AMD64_SRCDIR}/${file}" \
    "${DEST_DIR}/${AMD64_DSTDIR}/${file}"
done
dist_copy_and_patch "${UNISIM_SIMULATOR_DIR}/${AMD64_SRCDIR}/main.cc" \
		    "${DEST_DIR}/${AMD64_DSTDIR}/main.cc" \
		    "${UNISIM_SIMULATOR_DIR}/${ARCHISEC_SRCDIR}/amd64.patch"


UNISIM_SIMULATOR_SOURCE_FILES="\
COPYING \
NEWS \
ChangeLog \
arm32dba.cc arm32dba.ml \
aarch64dba.cc aarch64dba.ml \
amd64dba.cc amd64dba.ml \
main.ml \
"

for file in ${UNISIM_SIMULATOR_SOURCE_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${ARCHISEC_SRCDIR}/${file}" \
	      "${DEST_DIR}/${file}"
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
 (synopsis "UNISIM-VP DBA decoder")
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

(rule
 (targets top_arm32.hh top_arm32.tcc)
 (deps
  (:gen genisslib/genisslib)
  (:src unisim/component/cxx/processor/arm/top_arm32.isa))
 (action
  (no-infer (run %{gen} -o top_arm32 -w 8 -I . %{src}))))

(rule
 (targets top_thumb.hh top_thumb.tcc)
 (deps
  (:gen genisslib/genisslib)
  (:src unisim/component/cxx/processor/arm/top_thumb.isa))
 (action
  (no-infer (run %{gen} -o top_thumb -w 8 -I . %{src}))))

(subdir
 unisim/component/cxx/processor/arm
 (foreign_library
 (archive_name arm)
 (language cxx)
 (names (:standard))
 (flags :standard -I ../../../../..)
 (extra_deps
  ../../../../../.unisim-tree
  ../../../../../top_arm32.hh ../../../../../top_arm32.tcc
  ../../../../../top_thumb.hh ../../../../../top_thumb.tcc)))

(library
 ;(public_name unisim_archisec.arm32dba)
 (name arm32dba)
 (modules arm32dba)
 (foreign_archives
  unisim/component/cxx/processor/arm/arm
  unisim/util/util)
 (foreign_stubs
  (language cxx)
  (names arm32dba)
  (flags :standard))
 (c_library_flags :standard -lstdc++))

(rule
 (targets aarch64dec.hh aarch64dec.tcc)
 (deps
  (:gen genisslib/genisslib)
  (:src unisim/component/cxx/processor/arm/isa/arm64/aarch64dec.isa))
 (action
  (no-infer (run %{gen} -o aarch64dec -w 8 -I . %{src}))))

(subdir
 unisim/component/cxx/processor/arm/isa/arm64
 (foreign_library
 (archive_name arm64)
 (language cxx)
 (names (:standard))
 (flags :standard -I ../../../../../../..)
 (extra_deps
  ../../../../../../../.unisim-tree
  ../../../../../../../aarch64dec.hh ../../../../../../../aarch64dec.tcc)))

(library
 ;(public_name unisim_archisec.aarch64dba)
 (name aarch64dba)
 (modules aarch64dba)
 (foreign_archives
  unisim/component/cxx/processor/arm/isa/arm64/arm64
  unisim/util/util)
 (foreign_stubs
  (language cxx)
  (names aarch64dba)
  (flags :standard))
 (c_library_flags :standard -lstdc++))

(subdir
 unisim/component/cxx/processor/intel
 (include_subdirs unqualified)
 (foreign_library
 (archive_name intel)
 (language cxx)
 (names (:standard))
 (flags :standard -I ../../../../..)
 (extra_deps ../../../../../.unisim-tree)))

(subdir
 unisim/component/cxx/processor/amd64
 (foreign_library
 (archive_name amd64)
 (language cxx)
 (names (:standard))
 (flags :standard -I . -I ../../../../..)
 (extra_deps ../../../../../.unisim-tree)))


(library
 ;(public_name unisim_archisec.amd64dba)
 (name amd64dba)
 (modules amd64dba)
 (foreign_archives
  unisim/component/cxx/processor/amd64/amd64
  unisim/component/cxx/processor/intel/intel
  unisim/util/util)
 (foreign_stubs
  (language cxx)
  (names amd64dba)
  (flags :standard))
 (c_library_flags :standard -lstdc++))

(executable
 (public_name unisim_archisec)
 (name main)
 (modules main)
 (libraries arm32dba aarch64dba amd64dba))
EOF


echo "Distribution is up-to-date"
