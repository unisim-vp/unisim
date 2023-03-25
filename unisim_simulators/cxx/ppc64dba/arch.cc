/*
 *  Copyright (c) 2009-2023,
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

#include "arch.hh"

namespace ppc64 {

  Arch::Arch( U64 const& insn_addr )
    : path(0)
    , current_instruction_address(insn_addr)
    , next_instruction_address(insn_addr + U64(4))
    , branch_type(B_JMP)
    , cr(*this)
    , xer(*this)
    , fpscr(*this)
  {
    for (IRegID reg; reg.next();)
      regvalues[reg.idx()] = newRegRead( reg );
    for (FRegID reg; reg.next();)
      fregvalues[reg.idx()].expr = newRegRead( reg );
  }
  
  bool
  Arch::close( Arch const& ref, uint64_t linear_nia )
  {
    bool complete = path->close();
    /* TODO: branch_type */
    path->sinks.insert( Expr( new unisim::util::symbolic::binsec::Branch( next_instruction_address.expr ) ) );

    for (IRegID reg; reg.next();)
      if (regvalues[reg.idx()].expr != ref.regvalues[reg.idx()].expr)
        path->sinks.insert( Expr( newRegWrite( reg, regvalues[reg.idx()].expr ) ) );

     return complete;
  }

  void
  Arch::IRegID::Repr(std::ostream& sink) const
  {
    sink << c_str();
  }
  
  void
  Arch::FRegID::Repr(std::ostream& sink) const
  {
    sink << c_str();
  }
  
} // end of namespace ppc64

