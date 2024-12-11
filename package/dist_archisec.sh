#!/bin/bash

SIMPKG=archisec
SIMPKG_SRCDIR=.
SIMPKG_DSTDIR=.
ARCHISEC_SRCDIR=cxx/archisec
VERSION=0.0.9

source "$(dirname $0)/dist_common.sh"

function includes()
{
    local SEARCH_PATH=$1/$2
    local SRC_FILES=$(find ${SEARCH_PATH} -name "*.cc")
    for i in $(gcc -MM -I$1 -I${SEARCH_PATH} ${SRC_FILES} |\
		   tr '\n\\' ' ' |\
		   sed -e 's/[^ ][^ ]*\.o: [ ]*[^ ][^ ]*//g' |\
		   sort -u); do
        realpath --relative-to=${SEARCH_PATH} $i
    done
}

# ARMv7

import unisim/component/cxx/processor/arm/isa/arm32 || exit
import unisim/component/cxx/processor/arm/isa/thumb2 || exit
import unisim/component/cxx/processor/arm/isa/thumb || exit

# ARMv8

import unisim/component/cxx/processor/arm/isa/arm64 || exit
import unisim/component/cxx/processor/arm/regs64 || exit

# x86

import unisim/component/cxx/processor/intel/isa || exit

# PPC64

import unisim/component/cxx/processor/powerpc/isa/book_i/branch || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/cond_reg || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/floating_point || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/legacy_int_mac || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/legacy_move_assist || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/syscall || exit
import unisim/component/cxx/processor/powerpc/isa/book_ii || exit
import unisim/component/cxx/processor/powerpc/isa/book_iii_e || exit
import unisim/component/cxx/processor/powerpc/isa/book_e || exit
import unisim/component/cxx/processor/powerpc/isa || exit

# Common

import unisim/component/cxx/vector || exit
import unisim/util/symbolic/vector || exit
import unisim/util/symbolic/binsec || exit
import unisim/util/arithmetic || exit
import unisim/util/floating_point || exit
import unisim/util/endian || exit
import unisim/util/likely || exit

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

copy source header template data

AARCH32_SRCDIR=cxx/aarch32dba
AARCH32_DSTDIR=aarch32
AARCH32_SRC_FILES="\
decoder.hh \
decoder.cc \
"
AARCH32_ISA_FILES="\
top_arm32 \
top_thumb \
"

for file in ${AARCH32_SRC_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${AARCH32_SRCDIR}/${file}" \
              "${DEST_DIR}/${AARCH32_DSTDIR}/${file}"
done

for isa in ${AARCH32_ISA_FILES}; do
    ${UNISIM_TOOLS_DIR}/genisslib/genisslib.py \
		       --prefix ${DEST_DIR} \
		       -o ${AARCH32_DSTDIR}/${isa} \
		       -w 8 -I ${UNISIM_LIB_DIR} --source-lines off \
		       ${UNISIM_SIMULATOR_DIR}/${AARCH32_SRCDIR}/${isa}.isa
done

AARCH64_SRCDIR=cxx/aarch64dba
AARCH64_DSTDIR=aarch64
AARCH64_SRC_FILES="\
decoder.hh \
decoder.cc \
"

AARCH64_ISA_FILES="\
aarch64dec \
"

for file in ${AARCH64_SRC_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${AARCH64_SRCDIR}/${file}" \
    "${DEST_DIR}/${AARCH64_DSTDIR}/${file}"
done

for isa in ${AARCH64_ISA_FILES}; do
    ${UNISIM_TOOLS_DIR}/genisslib/genisslib.py \
		       --prefix ${DEST_DIR} \
		       -o ${AARCH64_DSTDIR}/${isa} \
		       -w 8 -I ${UNISIM_LIB_DIR} --source-lines off \
		       ${UNISIM_SIMULATOR_DIR}/${AARCH64_SRCDIR}/${isa}.isa
done

PPC64_SRCDIR=cxx/ppc64dba
PPC64_DSTDIR=ppc64
PPC64_SRC_FILES="\
types.hh \
arch.hh \
arch.cc \
decoder.hh \
decoder.cc \
"

PPC64_ISA_FILES="\
ppc64dec \
"

for file in ${PPC64_SRC_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${PPC64_SRCDIR}/${file}" \
    "${DEST_DIR}/${PPC64_DSTDIR}/${file}"
done

for isa in ${PPC64_ISA_FILES}; do
    ${UNISIM_TOOLS_DIR}/genisslib/genisslib.py \
		       --prefix ${DEST_DIR} \
		       -o ${PPC64_DSTDIR}/${isa} \
		       -w 8 -I ${UNISIM_LIB_DIR} --source-lines off \
		       ${UNISIM_SIMULATOR_DIR}/${PPC64_SRCDIR}/${isa}.isa
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
ppc64dba.cc ppc64dba.ml \
amd64dba.cc amd64dba.ml \
unittest.ml \
unittest.expected \
"

for file in ${UNISIM_SIMULATOR_SOURCE_FILES}; do
    dist_copy "${UNISIM_SIMULATOR_DIR}/${ARCHISEC_SRCDIR}/${file}" \
	      "${DEST_DIR}/${file}"
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS.md"
Yves Lhuillier <yves.lhuillier@cea.fr>
Frédéric Recoules <frederic.recoules@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/LICENSE.md"
Copyright (c) 2007-2024,
Commissariat a l'Énergie Atomique et aux Énergies Alternatives (CEA)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

 - Neither the name of CEA nor the names of its contributors may be used
   to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
EOF

cat << EOF > "${DEST_DIR}/README.md"
# UNISIM ARCHISEC [![License: BSD-3-Clause](https://img.shields.io/badge/license-BSD--3--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

UNISIM ARCHISEC is a companion project of the binary analysis platform
[BINSEC](https://github.com/binsec/binsec).
It exposes disassembly metadata and DBA (Dynamic Bitvector Automata)
semantics of several instruction set architectures:
- \`Arm32dba\` an ARM/Thumb (ARMv7) DBA decoder
- \`Aarch64dba\` an AARCH64 (ARMv8) DBA decoder
- \`Amd64dba\` an AMD64 (x86-64) DBA decoder
- \`Ppc64dba\` and PowerPC 64 DBA decoder

See [INSTALL](INSTALL.md) for installation instructions.
EOF

cat << EOF > "${DEST_DIR}/CHANGES.md"
## 0.0.9 (2024-09-13)

- support for NEON instructions in AARCH64
- handling some new VEX encoded instructions (x86)
- more systematic branchless semantics
  (e.g. [#37](https://github.com/binsec/binsec/issues/37))
- various bug fixes and code improvements

## 0.0.8 (2024-03-08)

- support for \`adcx\`, \`adox\` and \`mulx\` (x86)
- some bug fixes and code improvements

## 0.0.7 (2023-10-23)

- various bug fixes and code improvements

## 0.0.6 (2023-07-14)

- add PowerPC 64 bit decoder
- various bug fixes and code improvements

## 0.0.5 (2023-02-24)

- enable macOS Homebrew distribution

## 0.0.4 (2023-02-15)

- better handling of x86 flags
- various bug fixes and refactoring

## 0.0.3 (2022-07-24)

- handling some new VEX encoded instructions (x86)
- various bug fixes and refactoring

## 0.0.2 (2022-04-15)

- handling of SSE instructions for AMD64
- various bug fixes and code generator improvements

## 0.0.1 (2022-03-14)

- update build system (dune 3.0) and improve dependency tracking
- fix memory leak issues
- various bug fixes and code improvements

## 0.0.0 (2021-10-05)

Initial release.

OCaml binding for the UNISIM DBA (Dynamic Bitvector Automata) decoder.
- \`Arm32dba\` library exposes ARM/Thumb architecture;
- \`Aarch64dba\` library exposes AARCH64 architecture;
- \`Amd64dba\` library exposes AMD64 architecture.
EOF

cat << EOF > "${DEST_DIR}/INSTALL.md"
INSTALLATION
------------

Requirements:
  - GCC/G++
  - dune (>= 3.0)

Building instructions:
\`\`\`console
$ dune build @install
\`\`\`

Installing (optional):
\`\`\`console
$ dune install
\`\`\`
EOF

cat << EOF > "${DEST_DIR}/dune-project"
(lang dune 3.0)
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
 (description "
UNISIM ARCHISEC is a companion project of the binary analysis platform
BINSEC. It exposes disassembly metadata and DBA (Dynamic Bitvector Automata)
semantics of several instruction set architectures, including ARM and x86.")
 (depends
  (conf-gcc :build)
  (conf-g++ :build))
 (conflicts
  (binsec (< 0.5.0))))
EOF

cat <<EOF > "${DEST_DIR}/unisim_archisec.opam.template"
available: [ os = "linux" & (os-distribution != "ol" & os-distribution != "centos" | os-version >= 8) | os = "macos" & os-distribution = "homebrew" ]
EOF

echo -n > "${DEST_DIR}/dune"

UTIL_INCLUDES=$(includes ${DEST_DIR} unisim/util)

cat <<EOF >> "${DEST_DIR}/dune"
(env
  (_
   (c_flags :standard -std=gnu14 -g0)
   (cxx_flags :standard -std=gnu++14 -g0)))

(subdir
 unisim/util
 (include_subdirs unqualified)
 (library
  (package unisim_archisec)
  (name util)
  (foreign_stubs
   (language cxx)
   (names :standard)
   (flags :standard -I../..)
   (extra_deps ${UTIL_INCLUDES}))))
EOF

echo >> "${DEST_DIR}/dune"

ARM_INCLUDES=$(includes ${DEST_DIR} unisim/component/cxx/processor/arm/isa)
AARCH32_INCLUDES=$(includes ${DEST_DIR} aarch32)

cat <<EOF >> "${DEST_DIR}/dune"
(subdir
 unisim/component/cxx/processor/arm/isa
 (foreign_library
  (archive_name arm)
  (language cxx)
  (names (:standard \ simfloat))
  (flags (:standard -I../../../../../..))
  (extra_deps ${ARM_INCLUDES})))

(subdir
 aarch32
 (foreign_library
  (archive_name aarch32)
  (language cxx)
  (names (:standard))
  (flags :standard -I. -I..)
  (extra_deps ${AARCH32_INCLUDES})))

(library
 (public_name unisim_archisec.arm32dba)
 (name arm32dba)
 (modules arm32dba)
 (foreign_archives
  aarch32/aarch32
  unisim/component/cxx/processor/arm/isa/arm)
 (foreign_stubs
  (language cxx)
  (names arm32dba)
  (flags :standard -I.))
 (libraries util))
EOF

echo >> "${DEST_DIR}/dune"

ARM64_INCLUDES=$(includes ${DEST_DIR} \
			  unisim/component/cxx/processor/arm/isa/arm64)
AARCH64_INCLUDES=$(includes ${DEST_DIR} aarch64)

cat <<EOF >> "${DEST_DIR}/dune"
(subdir
 unisim/component/cxx/processor/arm/isa/arm64
 (foreign_library
  (archive_name arm64)
  (language cxx)
  (names (:standard))
  (flags :standard -I../../../../../../..)
  (extra_deps ${ARM64_INCLUDES})))

(subdir
 aarch64
 (foreign_library
  (archive_name aarch64)
  (language cxx)
  (names (:standard))
  (flags :standard -I. -I..)
  (extra_deps ${AARCH64_INCLUDES})))

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
  (flags :standard -I.))
 (libraries util))
EOF

echo >> "${DEST_DIR}/dune"

PPC_INCLUDES=$(includes ${DEST_DIR} unisim/component/cxx/processor/powerpc/isa)
PPC64_INCLUDES=$(includes ${DEST_DIR} ppc64)

cat <<EOF >> "${DEST_DIR}/dune"
(subdir
 unisim/component/cxx/processor/powerpc/isa
 (foreign_library
  (archive_name ppc)
  (language cxx)
  (names (:standard))
  (flags (:standard -I../../../../../..))
  (extra_deps ${PPC_INCLUDES})))

(subdir
 ppc64
 (foreign_library
  (archive_name ppc64)
  (language cxx)
  (names (:standard))
  (flags :standard -I. -I..)
  (extra_deps ${PPC64_INCLUDES})))

(library
 (public_name unisim_archisec.ppc64dba)
 (name ppc64dba)
 (modules ppc64dba)
 (foreign_archives
  ppc64/ppc64
  unisim/component/cxx/processor/powerpc/isa/ppc)
 (foreign_stubs
  (language cxx)
  (names ppc64dba)
  (flags :standard -I.))
 (libraries util))
EOF

echo >> "${DEST_DIR}/dune"

INTEL_INCLUDES=$(includes ${DEST_DIR} unisim/component/cxx/processor/intel)
AMD64_INCLUDES=$(includes ${DEST_DIR} amd64)

cat <<EOF >> "${DEST_DIR}/dune"
(subdir
 unisim/component/cxx/processor/intel
 (include_subdirs unqualified)
 (foreign_library
  (archive_name intel)
  (language cxx)
  (names (:standard))
  (flags :standard -I../../../../..)
  (extra_deps ${INTEL_INCLUDES})))

(subdir
 amd64
 (foreign_library
 (archive_name amd64)
 (language cxx)
 (names (:standard))
 (flags :standard -I. -I..)
 (extra_deps ${AMD64_INCLUDES})))

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
  (flags :standard -I.))
 (libraries util))
EOF

echo >> "${DEST_DIR}/dune"

cat <<EOF >> "${DEST_DIR}/dune"
(test
 (name unittest)
 (modes
  (best exe))
 (modules unittest)
 (libraries arm32dba aarch64dba ppc64dba amd64dba))
EOF

dune format-dune-file ${DEST_DIR}/dune > ${DEST_DIR}/dune.formatted
mv ${DEST_DIR}/dune.formatted ${DEST_DIR}/dune

echo "Distribution is up-to-date"
