#!/bin/bash

SIMPKG=archisec
SIMPKG_SRCDIR=.
SIMPKG_DSTDIR=.
ARCHISEC_SRCDIR=cxx/archisec
VERSION=1.0.9

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

AARCH32_SRCDIR=cxx/armsec
AARCH32_DSTDIR=aarch32
AARCH32_ISA_FILES="\
decoder.hh \
decoder.cc \
top_thumb.isa \
top_arm32.isa \
"

for file in ${AARCH32_ISA_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${AARCH32_SRCDIR}/${file}" \
              "${DEST_DIR}/${AARCH32_DSTDIR}/${file}"
done

AARCH64_SRCDIR=cxx/aarch64dba
AARCH64_DSTDIR=aarch64
AARCH64_ISA_FILES="\
decoder.hh \
decoder.cc \
aarch64dec.isa \
"

for file in ${AARCH64_ISA_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${AARCH64_SRCDIR}/${file}" \
    "${DEST_DIR}/${AARCH64_DSTDIR}/${file}"
done

AMD64_SRCDIR=cxx/amd64dba
AMD64_DSTDIR=amd64
AMD64_ISA_FILES="\
decoder.hh \
decoder.cc \
arch.hh \
arch.cc \
decode.cc \
"

for file in ${AMD64_ISA_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${AMD64_SRCDIR}/${file}" \
    "${DEST_DIR}/${AMD64_DSTDIR}/${file}"
done

UNISIM_SIMULATOR_SOURCE_FILES="\
arm32dba.cc arm32dba.ml \
aarch64dba.cc aarch64dba.ml \
amd64dba.cc amd64dba.ml \
unittest.ml \
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

cat << EOF > "${DEST_DIR}/VERSION"
$VERSION
EOF

cat << EOF > "${DEST_DIR}/LICENSE"
Copyright (c) 2007-2021,
Commissariat a l'Energie Atomique (CEA)
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

 - Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

 - Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 - Neither the name of CEA nor the names of its contributors may be used to
   endorse or promote products derived from this software without specific prior
   written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - arm32dba:   an arm32/thumb dba decoder
  - aarch64dba: an aarch64 dba decoder
  - amd64dba:   an amd64/intel64 dba decoder

See INSTALL for installation instructions.
EOF

cat << EOF > "${DEST_DIR}/INSTALL"
INSTALLATION
------------

Requirements:
  - GCC/G++
  - OCaml (>= 4.05)
  - dune (>= 2.8)

Building instructions:
  $ dune build @install

Installing (optional):
  $ dune install
EOF

cat << EOF > "${DEST_DIR}/dune-project"
(lang dune 2.8)
(generate_opam_files true)

(name unisim_archisec)
(version "$VERSION")
(maintainers "BINSEC <binsec@saxifrage.saclay.cea.fr>")
(authors
  "Yves Lhuillier"
  "Frédéric Recoules"
)
(license BSD-3-Clause)
(homepage "https://binsec.github.io")
(source (github binsec/unisim_archisec))
(bug_reports "mailto:binsec@saxifrage.saclay.cea.fr")

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
 (library
  (package unisim_archisec)
  (name util)
  (foreign_stubs
   (language cxx)
   (names (:standard))
   (flags :standard -I ../..)
   (extra_deps ../../.unisim-tree))
  (c_library_flags :standard -lstdc++)))

(rule
 (targets top_arm32.hh top_arm32.tcc)
 (deps
  (:gen genisslib/genisslib)
  (:src aarch32/top_arm32.isa))
 (action
  (no-infer (run %{gen} -o top_arm32 -w 8 -I . %{src}))))

(rule
 (targets top_thumb.hh top_thumb.tcc)
 (deps
  (:gen genisslib/genisslib)
  (:src aarch32/top_thumb.isa))
 (action
  (no-infer (run %{gen} -o top_thumb -w 8 -I . %{src}))))

(subdir
 unisim/component/cxx/processor/arm
 (foreign_library
  (archive_name arm)
  (language cxx)
  (names (:standard))
  (flags :standard -I ../../../../..)
  (extra_deps ../../../../../.unisim-tree)))

(subdir
 aarch32
 (foreign_library
  (archive_name aarch32)
  (language cxx)
  (names (:standard))
  (flags :standard -I ..)
  (extra_deps
   ../.unisim-tree
   ../top_arm32.hh ../top_arm32.tcc
   ../top_thumb.hh ../top_thumb.tcc)))

(library
 (public_name unisim_archisec.arm32dba)
 (name arm32dba)
 (modules arm32dba)
 (foreign_archives
  aarch32/aarch32
  unisim/component/cxx/processor/arm/arm)
 (foreign_stubs
  (language cxx)
  (names arm32dba)
  (flags :standard -I .))
 (c_library_flags :standard -lstdc++)
 (libraries util))

(rule
 (targets aarch64dec.hh aarch64dec.tcc)
 (deps
  (:gen genisslib/genisslib)
  (:src aarch64/aarch64dec.isa))
 (action
  (no-infer (run %{gen} -o aarch64dec -w 8 -I . %{src}))))

(subdir
 unisim/component/cxx/processor/arm/isa/arm64
 (foreign_library
  (archive_name arm64)
  (language cxx)
  (names (:standard))
  (flags :standard -I ../../../../../../..)
  (extra_deps ../../../../../../../.unisim-tree)))

(subdir
 aarch64
 (foreign_library
  (archive_name aarch64)
  (language cxx)
  (names (:standard))
  (flags :standard -I ..)
  (extra_deps
   ../.unisim-tree
   ../aarch64dec.hh ../aarch64dec.tcc)))

(library
 (public_name unisim_archisec.aarch64dba)
 (name aarch64dba)
 (modules aarch64dba)
 (foreign_archives
  aarch64/aarch64
  unisim/component/cxx/processor/arm/isa/arm64/arm64)
 (foreign_stubs
  (language cxx)
  (names aarch64dba)
  (flags :standard -I .))
 (c_library_flags :standard -lstdc++)
 (libraries util))

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
 amd64
 (foreign_library
 (archive_name amd64)
 (language cxx)
 (names (:standard))
 (flags :standard -I .. -I.)
 (extra_deps ../.unisim-tree)))

(library
 (public_name unisim_archisec.amd64dba)
 (name amd64dba)
 (modules amd64dba)
 (foreign_archives
  amd64/amd64
  unisim/component/cxx/processor/intel/intel)
 (foreign_stubs
  (language cxx)
  (names amd64dba)
  (flags :standard -I .))
 (c_library_flags :standard -lstdc++)
 (libraries util))

(test
 (name unittest)
 (modes native)
 (modules unittest)
 (libraries arm32dba aarch64dba amd64dba))
EOF

cat << EOF > "${DEST_DIR}/unittest.expected"
Amd64dba.decode ~m64:false ~addr:0x4000 "55"
(address . 0x00004000)
(opcode . "55")
(size . 1)
(mnemonic . "push %ebp")
(0x00004000,0) nxt_esp<32> := (esp<32> - 0x00000004); goto 1
(0x00004000,1) @[nxt_esp<32>,<-,4] := ebp<32>; goto 2
(0x00004000,2) esp<32> := nxt_esp<32>; goto 3
(0x00004000,3) goto (0x00004001,0)

Amd64dba.decode ~m64:true ~addr:0x4000 "55"
(address . 0x0000000000004000)
(opcode . "55")
(size . 1)
(mnemonic . "push %rbp")
(0x0000000000004000,0) nxt_rsp<64> := (rsp<64> - 0x0000000000000008); goto 1
(0x0000000000004000,1) @[nxt_rsp<64>,<-,8] := rbp<64>; goto 2
(0x0000000000004000,2) rsp<64> := nxt_rsp<64>; goto 3
(0x0000000000004000,3) goto (0x0000000000004001,0)

Arm32dba.decode ~thumb:false ~addr:0x4000 0xe2543210
(address . 0x00004000)
(opcode . 0xe2543210)
(size . 4)
(mnemonic . "subs	r3, r4, #1")
(0x00004000,0) nxt_r3<32> := (r4<32> - 0x00000001); goto 1
(0x00004000,1) nxt_n<1> := (nxt_r3<32> <s 0x00000000); goto 2
(0x00004000,2) nxt_z<1> := (nxt_r3<32> = 0x00000000); goto 3
(0x00004000,3) nxt_c<1> := (r4<32> >=u 0x00000001); goto 4
(0x00004000,4) nxt_v<1> := (nxt_n<1> xor (r4<32> <s 0x00000001)); goto 5
(0x00004000,5) r3<32> := nxt_r3<32>; goto 6
(0x00004000,6) n<1> := nxt_n<1>; goto 7
(0x00004000,7) z<1> := nxt_z<1>; goto 8
(0x00004000,8) c<1> := nxt_c<1>; goto 9
(0x00004000,9) v<1> := nxt_v<1>; goto 10
(0x00004000,10) goto (0x00004004,0)

Arm32dba.decode ~thumb:true ~addr:0x4000 0x000af04f
(address . 0x00004000)
(opcode . 0x000af04f)
(size . 4)
(mnemonic . "mov.w	r0, #10")
(0x00004000,0) if (((((((((((((((1<1> or z<1>) or 0<1>) or 0<1>) or 0<1>) or 0<1>) or 0<1>) or 0<1>) or 0<1>) or 0<1>) or 0<1>) or 0<1>) or 0<1>) or 0<1>) or 0<1>) or 0<1>)  goto 2 else goto 1
(0x00004000,1) goto (0x00004004,0)
(0x00004000,2) r0<32> := 0x0000000a; goto 1

Aarch64dba.decode ~addr:0x4000 0x18000020
(address . 0x0000000000004000)
(opcode . 0x18000020)
(size . 4)
(mnemonic . "ldr	w0, 0x4004")
(0x0000000000004000,0) nxt_x0<64> := (extu @[0x0000000000004004,<-,4] 64); goto 1
(0x0000000000004000,1) x0<64> := nxt_x0<64>; goto 2
(0x0000000000004000,2) goto (0x0000000000004004,0)
EOF

echo "Distribution is up-to-date"