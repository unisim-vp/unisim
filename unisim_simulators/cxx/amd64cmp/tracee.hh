/*
 *  Copyright (c) 2018-2022,
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

#ifndef __AMD64CMP_TRACEE_HH__
#define __AMD64CMP_TRACEE_HH__

#include <vector>
#include <string>
#include <inttypes.h>

struct Arch;

struct Tracee
{
  Tracee() : pid(-1) {}

  bool Process( std::vector<std::string> const& simargs );

  uint64_t GetInsnAddr() const;

  uint64_t GetReg(unsigned reg) const;

  void SetReg(unsigned reg, uint64_t value) const;

  uint64_t GetSBase(char reg) const;

  void GetVec(unsigned reg, uint8_t* bytes) const;

  uint64_t PeekData(uint64_t addr) const;

  void StepInstruction() const;

  void Load(Arch& arch) const;

  template <typename ACCESS>
  void MemAccess( ACCESS& access, uint64_t addr, unsigned size ) const
  {
    uint64_t cell = (addr % 8) ? this->PeekData(addr & -8) : 0;
    
    for (uint64_t offset = 0; offset < size; ++offset, ++addr)
      {
        unsigned cell_byte = addr % 8;
        if (cell_byte == 0)
          cell = this->PeekData(addr);
        access.Do( offset, cell >> 8*cell_byte );
      }
  }

  struct Stopped { int status; };

  struct Unexpected { char const* what; int code; };

  pid_t pid;
};

#endif // __AMD64CMP_TRACEE_HH__
