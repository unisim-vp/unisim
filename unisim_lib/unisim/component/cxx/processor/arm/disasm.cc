/*
 *  Copyright (c) 2010,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#include <inttypes.h>
#include <sstream>

#include "unisim/component/cxx/processor/arm/disasm.hh"
#include "unisim/component/cxx/processor/arm/masks.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using namespace std;

  std::ostream&
  operator << ( std::ostream& sink, DisasmObject const& dobj )
  {
    dobj( sink );
    return sink;
  }

  static char const* const  conds_dis[] =
    {"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le", "al", "<und>"};
  
  /* Condition opcode bytes disassembling method */
  void DisasmCondition::operator() ( std::ostream& sink ) const
  {
    if (m_cond < 14)
      sink << conds_dis[m_cond];
    else if (m_cond == 14)
      return;
    
    sink << "?";
    std::cerr << "ERROR(" << __FUNCTION__ << "): "
              << "unknown condition code (" << m_cond << ")" << std::endl;
    
  }

  void DisasmShImm::operator() ( std::ostream& sink ) const
  {
    if (m_offset) {
      switch (m_shift) {
      case 0: sink << ", lsl #"; break;
      case 1: sink << ", lsr #"; break;
      case 2: sink << ", asr #"; break;
      case 3: sink << ", ror #"; break;
      }
      sink << m_offset;
    } else {
      switch (m_shift) {
      case 0: break;
      case 1: sink << ", lsr #32"; break;
      case 2: sink << ", asr #32"; break;
      case 3: sink << ", rrx"; break;
      }
    }
  }
  
  static char const* const register_dis[] =
    {"r0","r1","r2","r3","r4","r5","r6","r7", "r8","r9","sl","fp","ip","sp","lr","pc"};

  static char const* const sornames[] = { "lsl", "lsr", "asr", "ror" };
  
  void DisasmShReg::operator() ( std::ostream& sink ) const
  {
    sink << sornames[m_shift] << " " << register_dis[m_reg];
  }

  /* Register disassembling method */
  void DisasmRegister::operator() ( std::ostream& sink ) const
  {
    sink << register_dis[m_reg];
  }
  
  /* Register list disassembling method */
  void DisasmRegList::operator() ( std::ostream& sink ) const
  {
    char const* sep = "";
    for (int reg = 0; reg < 16; ++reg) {
      if (((m_reglist >> reg) & 1) == 0) continue;
      sink << sep << register_dis[reg];
      sep = ", ";
    }
  }
  
  /* Multiple Load Store Mode disassembling method */
  void DisasmLSMMode::operator() ( std::ostream& sink ) const
  {
    static char const* const lsmmod[] = {"da","ia","db","ib"};
    sink << lsmmod[m_mode%4];
  }
  
  /* PSR mask disassembling method */
  void DisasmPSRMask::operator() ( std::ostream& sink ) const
  {
    if ((m_mask & 0x01) == 0x01) sink << "c";
    if ((m_mask & 0x02) == 0x02) sink << "x";
    if ((m_mask & 0x04) == 0x04) sink << "s";
    if ((m_mask & 0x08) == 0x08) sink << "f";
  }

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
