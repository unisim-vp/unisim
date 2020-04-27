# Installation of Binsec decoder modules

## Getting UNISIM-VP

UNISIM-VP Binsec decoder modules are build on the UNISIM-VP C++ library and shouldn't need anything else fancy.

Basically you will need:
  - GNU C++ compiler
  - GNU C++ standard library
  - GNU bash
  - GNU make
  - GNU autoconf
  - GNU automake

UNISIM-VP is available on frama-c's git, so you just have to clone its base directory (to a directory $USIMDIR of your choice).


    $ git clone git@git.frama-c.com:unisim-vp/unisim.git "$USIMDIR"/unisim

The official branch for binsec stable decoders is armsec, so better stick to it.

    $ cd "$USIMDIR"/unisim
    $ git checkout armsec

## Installing a decoder

Each decoder (currently) is a standalone C++/automake project.  You can obtain a distribution of each project using special distribution scripts.
To setup a decoder project, choose a directory (here suppose $DECODER points to it). And use the distribution script of your choice (here we use dist_armsec.sh).
    
    $ mkdir "$DECODER"
    $ cd "$DECODER"
    $ "$USIMDIR"/unisim/package/dist_armsec.sh "$PWD"
    
All decoders follow a standard configure-make-make-install process.
In the following we perform a no-option and in-place build (suit yourself if you prefer something else).
Moreover, we skip the `make install` which is not mandatory.

    $ ./configure
    $ make

That's it, the decoder is ready.

    $ armsec/unisim-armsec-1.0.9 arm 0x4000 0xe2543210
    
    (address . 0x00004000)
    (opcode . 0xe2543210)
    (size . 4)
    (mnemonic . "subs       r3, r4, #1")
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

# ARMSEC: the armv7 instruction decoder

ARMSEC is an ARMv7 instruction set decoder. It supports all the instruction set (including NEON, VFP and the variable-length THUMB2 encoding).
The decoder is available through the `dist_armsec.sh` distribution script and the installation steps described above.

usage: `unisim-armsec-1.0.9 arm|thumb <address> <encoding>`

# AARCH64DBA: the armv8 instruction decoder

AARCH64DBA is an ARMv8 (64 bits) instruction set decoder. It supports a significant part of the instruction set.
The decoder is available through the `dist_aarch64dba.sh` distribution script and the installation steps described above.


# AMD64DBA: the x86_64 instruction decoder

AMD64DBA is an x86_64/amd64 (64 bits) instruction set decoder. It supports a significant part of the instruction set.
The decoder is available through the `dist_amd64dba.sh` distribution script and the installation steps described above.


# MIPSELSEC: the mips instrucrtion decoder

MIPSELSEC is a mips (> isa32r2, 32 bits little-endian) instruction set decoder. It partially supports the instruction set.
The decoder is available through the `dist_mipselsec.sh` distribution script and the installation steps described above.

Note: because of DBA's notion of branches and MIPS delay slots, the decoder currently doesn't handle branches correctly.  The decoder is here to serve as feasability demo.




