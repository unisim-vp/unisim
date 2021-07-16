/*
 *  Copyright (c) 2021,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include "architecture.hh"
#include <iostream>
#include <iomanip>


void
Arch::MemDump64(uint64_t addr)
{
  addr &= -8;

  unsigned const size = 8;
  uint8_t buffer[size];
  mem.read( &buffer[0], addr, sizeof buffer );

  std::cerr << "@" << std::setfill('0') << std::setw(16) << std::hex << addr << std::dec << ": ";
  std::cerr << "0x";
  for (unsigned nibble = size*2; nibble-- > 0; )
    {
      uint8_t nbits = (buffer[nibble/2] >> (nibble * 4 & 4)) & 15;
      std::cerr << "0123456789abcdef"[nbits];
    }
  std::cerr << ", " << '"';
  for (unsigned idx = 0; idx < size; ++idx)
    {
      uint8_t byte = buffer[idx];
      std::cerr << (byte < 32 or byte > 126 ? '.' : char(byte));
    }
  std::cerr << '"' << std::endl;
}

