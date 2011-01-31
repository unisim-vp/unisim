/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#include "unisim/component/cxx/memory/flash/am29lv/config.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace flash {
namespace am29lv {

// AM29LV800B Configuration
const uint64_t AM29LV800BConfig::PROGRAMMING_TIME[AM29LV800BConfig::MAX_IO_WIDTH] = { 9000, 110000 }; // 9 us/11us

const SECTOR_ADDRESS_RANGE<AM29LV800BConfig::ADDRESS> AM29LV800BConfig::SECTOR_MAP[AM29LV800BConfig::NUM_SECTORS] = {
	{ 0x00000, 65536 }, // SA0/64 KB
	{ 0x10000, 65536 }, // SA1/64 KB
	{ 0x20000, 65536 }, // SA2/64 KB
	{ 0x30000, 65536 }, // SA3/64 KB
	{ 0x40000, 65536 }, // SA4/64 KB
	{ 0x50000, 65536 }, // SA5/64 KB
	{ 0x60000, 65536 }, // SA6/64 KB
	{ 0x70000, 65536 }, // SA7/64 KB
	{ 0x80000, 65536 }, // SA8/64 KB
	{ 0x90000, 65536 }, // SA9/64 KB
	{ 0xa0000, 65536 }, // SA10/64 KB
	{ 0xb0000, 65536 }, // SA11/64 KB
	{ 0xc0000, 65536 }, // SA12/64 KB
	{ 0xd0000, 65536 }, // SA13/64 KB
	{ 0xe0000, 65536 }, // SA14/64 KB
	{ 0xf0000, 32768 }, // SA15/32 KB
	{ 0xf8000,  8192 }, // SA16/8 KB
	{ 0xfa000,  8192 }, // SA17/8 KB
	{ 0xfc000, 16384 }  // SA18/16 KB
};

const uint8_t AM29LV800BConfig::MANUFACTURER_ID[MAX_IO_WIDTH] = { 0x01, 0x00 };
const uint8_t AM29LV800BConfig::DEVICE_ID[MAX_IO_WIDTH] = { 0xda, 0x22 };
const uint8_t AM29LV800BConfig::PROTECTED[MAX_IO_WIDTH] = { 0x01, 0x00 };
const uint8_t AM29LV800BConfig::UNPROTECTED[MAX_IO_WIDTH] = { 0x00, 0x00 };

// S1-[command,addr,data/action]->S2
const TRANSITION<AM29LV800BConfig::ADDRESS, AM29LV800BConfig::MAX_IO_WIDTH, AM29LV800BConfig::STATE> AM29LV800BConfig::FSM[AM29LV800BConfig::NUM_TRANSITIONS] = {
	{ AM29LV800BConfig::STATE_INITIAL, 0, CMD_WRITE, false, 0xaaa, false, { 0xaa, 0x00 }, STATE_INITIAL, 1, ACT_NOP }, // (I,0) -[W,AAA,AA/-]->(I,1)
	{ AM29LV800BConfig::STATE_INITIAL, 0, CMD_READ, true, 0, true, { 0,0 }, STATE_INITIAL, 0, ACT_READ }, // (I,0) -[R,*,*/READ]->(I,0)
	{ AM29LV800BConfig::STATE_INITIAL, 0, CMD_WRITE, true, 0, true, { 0,0 }, STATE_INITIAL, 0, ACT_NOP }, // (I,0) -[W,*,*/-]->(I,0)
	{ AM29LV800BConfig::STATE_INITIAL, 1, CMD_WRITE, false, 0x555, false, { 0x55, 0x00 }, STATE_INITIAL, 2, ACT_NOP }, // (I,1) -[W,555,55/-]->(I,2)
	{ AM29LV800BConfig::STATE_INITIAL, 1, CMD_WRITE, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_INITIAL, 0, ACT_NOP }, // (I,1) -[W,*,*/-]->(I,0)
	{ AM29LV800BConfig::STATE_INITIAL, 1, CMD_READ, true, 0, true, { 0, 0 }, STATE_INITIAL, 1, ACT_READ }, // (I,1) -[R,*,*/READ]->(I,1)
	{ AM29LV800BConfig::STATE_INITIAL, 2, CMD_WRITE, false, 0xaaa, false, { 0x90, 0x00 }, AM29LV800BConfig::STATE_AUTOSELECT, 3, ACT_NOP }, // (I,2) -[W,AAA,90/-]->(AUTOSELECT,2)
	{ AM29LV800BConfig::STATE_INITIAL, 2, CMD_WRITE, false, 0xaaa, false, { 0xa0, 0x00 }, AM29LV800BConfig::STATE_PROGRAM, 3, ACT_NOP }, // (I,2) -[W,AAA,A0/-]->(PROGRAM,2)
	{ AM29LV800BConfig::STATE_INITIAL, 2, CMD_WRITE, false, 0xaaa, false, { 0x20, 0x00 }, AM29LV800BConfig::STATE_UNLOCKED, 0, ACT_NOP }, // (I,2) -[W,AAA,20/-]->(UNLOCKED,0)
	{ AM29LV800BConfig::STATE_INITIAL, 2, CMD_WRITE, false, 0xaaa, false, { 0xaa, 0x00 }, AM29LV800BConfig::STATE_ERASE, 3, ACT_NOP }, // (I,2) -[W,AAA,AA/-]->(ERASE,2)
	{ AM29LV800BConfig::STATE_INITIAL, 2, CMD_WRITE, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_INITIAL, 0, ACT_NOP }, // (I,2) -[W,*,*/-]->(I,0)
	{ AM29LV800BConfig::STATE_INITIAL, 2, CMD_READ, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_INITIAL, 2, ACT_READ }, // (I,2) -[R,*,*/-]->(I,2)
	{ AM29LV800BConfig::STATE_AUTOSELECT, 3, CMD_READ, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_INITIAL, 0, ACT_READ_AUTOSELECT }, // (I,2) -[R,*,*/READ_AUTOSELECT]->(I,0)
	{ AM29LV800BConfig::STATE_PROGRAM, 3, CMD_WRITE, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_INITIAL, 0, ACT_PROGRAM }, // (PROGRAM,3) -[W,*,*/PROGRAM]->(I,0)
	{ AM29LV800BConfig::STATE_PROGRAM, 3, CMD_READ, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_PROGRAM, 3, ACT_READ }, // (PROGRAM,3) -[R,*,*/READ]->(PROGRAM,3)
	{ AM29LV800BConfig::STATE_UNLOCKED, 0, CMD_WRITE, true, 0, false, { 0xa0, 0x00 }, AM29LV800BConfig::STATE_UNLOCKED_PROGRAM, 1, ACT_NOP }, // (UNLOCKED,0) -[W,*,A0/-]->(UNLOCKED_PROGRAM,1)
	{ AM29LV800BConfig::STATE_UNLOCKED, 0, CMD_WRITE, true, 0, false, { 0x90, 0x00 }, AM29LV800BConfig::STATE_UNLOCKED_RESET, 1, ACT_NOP }, // (UNLOCKED,0) -[W,*,90/-]->(UNLOCKED_RESET,1)
	{ AM29LV800BConfig::STATE_UNLOCKED, 0, CMD_WRITE, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_UNLOCKED, 0, ACT_NOP }, // (UNLOCKED,0) -[W,*,*/-]->(UNLOCKED,0)
	{ AM29LV800BConfig::STATE_UNLOCKED, 0, CMD_READ, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_UNLOCKED, 0, ACT_READ }, // (UNLOCKED,0) -[R,*,*/READ]->(UNLOCKED,0)
	{ AM29LV800BConfig::STATE_UNLOCKED_PROGRAM, 1, CMD_WRITE, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_UNLOCKED, 0, ACT_PROGRAM }, // (UNLOCKED_PROGRAM,1) -[W,*,*/PROGRAM]->(UNLOCKED,0)
	{ AM29LV800BConfig::STATE_UNLOCKED_PROGRAM, 1, CMD_READ, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_UNLOCKED_PROGRAM, 1, ACT_READ }, // (UNLOCKED_PROGRAM,1) -[R,*,*/READ]->(UNLOCKED_PROGRAM,1)
	{ AM29LV800BConfig::STATE_UNLOCKED_RESET, 1, CMD_WRITE, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_INITIAL, 0, ACT_NOP }, // (UNLOCKED_RESET,1) -[W,*,*/-]->(I,0)
	{ AM29LV800BConfig::STATE_UNLOCKED_RESET, 1, CMD_READ, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_UNLOCKED_RESET, 1, ACT_READ }, // (UNLOCKED_RESET,1) -[R,*,*/READ]->(UNLOCKED_RESET,1)
	{ AM29LV800BConfig::STATE_ERASE, 3, CMD_WRITE, false, 0x555, false, { 0x55, 0x00 }, AM29LV800BConfig::STATE_ERASE, 4, ACT_NOP }, // (ERASE,3) -[W,555,55/-]->(ERASE,4)
	{ AM29LV800BConfig::STATE_ERASE, 3, CMD_WRITE, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_INITIAL, 0, ACT_NOP }, // (ERASE,3) -[W,*,*/-]->(I,0)
	{ AM29LV800BConfig::STATE_ERASE, 3, CMD_READ, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_ERASE, 3, ACT_READ }, // (ERASE,3) -[R,*,*/READ]->(ERASE,3)
	{ AM29LV800BConfig::STATE_ERASE, 4, CMD_WRITE, false, 0xaaa, false, { 0x10, 0x00 }, AM29LV800BConfig::STATE_INITIAL, 0, ACT_CHIP_ERASE }, // (ERASE,4) -[W,AAA,10/CHIP_ERASE]->(I,0)
	{ AM29LV800BConfig::STATE_ERASE, 4, CMD_WRITE, true, 0, false, { 0x30, 0x00 }, AM29LV800BConfig::STATE_INITIAL, 0, ACT_SECTOR_ERASE }, // (ERASE,4) -[W,*,30/SECTOR_ERASE]->(I,0)
	{ AM29LV800BConfig::STATE_ERASE, 4, CMD_WRITE, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_INITIAL, 0, ACT_NOP }, // (ERASE,4) -[W,*,*/-]->(I,0)
	{ AM29LV800BConfig::STATE_ERASE, 4, CMD_READ, true, 0, true, { 0, 0 }, AM29LV800BConfig::STATE_ERASE, 4, ACT_READ } // (ERASE,4) -[R,*,*/READ]->(ERASE,4)
};

} // end of namespace am29lv
} // end of namespace flash
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
