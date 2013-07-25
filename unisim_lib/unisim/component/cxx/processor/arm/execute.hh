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

/**************************************************************/
/* Disassembling methods                                      */
/**************************************************************/

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXECUTE_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXECUTE_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

  namespace CondTruthTable {
    template <uintptr_t Tbit, uintptr_t Tbits = 16>
    struct Source {
      static uintptr_t const msb = (Tbits-1);
      static uint16_t const tt = (((msb >> Tbit) & 1) ? (uint16_t(1) << msb) : uint16_t(0)) | Source<Tbit,msb>::tt;
    };

    template <uintptr_t Tbit> struct Source<Tbit,1> { static uint16_t const tt = uint16_t(0); };

    typedef Source<3> N; typedef Source<2> Z; typedef Source<1> C; typedef Source<0> V;
  };
  
  /** Check the given condition against the current CPSR status.
   * Returns true if the condition matches CPSR, false otherwise.
   *
   * @param core the core for which the CPSR will be matched
   * @param cond the condition to check
   * @return true if the condition matches CPSR, false otherwise
   */
  template <typename coreT>
  bool
  CheckCondition( coreT& core, uint32_t cond )
  {
    using CondTruthTable::N; using CondTruthTable::Z; using CondTruthTable::C; using CondTruthTable::V;
    uint32_t nzcv = core.GetCPSR_NZCV();
    uint16_t const condition_truth_tables[] = {
      uint16_t(                      Z::tt ), // eq; equal
      uint16_t(                     ~Z::tt ), // ne; not equal
      uint16_t(                      C::tt ), // cs/hs; unsigned higuer or same
      uint16_t(                     ~C::tt ), // cc/lo; unsigned lower
      uint16_t(                      N::tt ), // mi; negative
      uint16_t(                     ~N::tt ), // pl; positive or zero
      uint16_t(                      V::tt ), // vs; overflow set
      uint16_t(                     ~V::tt ), // vc; overflow clear
      uint16_t(          ~(~C::tt | Z::tt) ), // hi; unsigned higher
      uint16_t(           (~C::tt | Z::tt) ), // ls; unsigned lower or same
      uint16_t(           ~(N::tt ^ V::tt) ), // ge; signed greater than or equal
      uint16_t(            (N::tt ^ V::tt) ), // lt; signed less than
      uint16_t( ~(Z::tt | (N::tt ^ V::tt)) ), // gt; signed greater than
      uint16_t(  (Z::tt | (N::tt ^ V::tt)) ), // le; signed less than or equal
      uint16_t(                     0xffff ), // al; always
      uint16_t(                     0x0000 ), // <und>; never (illegal)
    };
    assert( cond < 15 );
    return ((condition_truth_tables[cond] >> nzcv) & 1);
  }

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXECUTE_HH__ */
