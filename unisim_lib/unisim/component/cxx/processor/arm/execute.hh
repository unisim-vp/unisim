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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr), Yves Lhuillier (yves.lhuillier@cea.fr)
 */

/**************************************************************/
/* Disassembling methods                                      */
/**************************************************************/

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXECUTE_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXECUTE_HH__

#include <inttypes.h>
#include <cassert>

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
    uint32_t nzcv = core.CPSR().NZCV().Get();
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
  
  template <typename coreT>
  uint32_t
  ComputeImmShift( coreT& core, uint32_t shift_lhs, uint32_t shift, uint32_t shift_rhs )
  {
    if (shift_rhs) {
      switch (shift) {
      case 0: return shift_lhs << shift_rhs;
      case 1: return shift_lhs >> shift_rhs;
      case 2: return ((int32_t)shift_lhs) >> shift_rhs;
      case 3: return (shift_lhs >> shift_rhs) | (shift_lhs << (32 - shift_rhs));
      }
    } else {
      switch (shift) {
      case 0: return shift_lhs;
      case 1: return 0;
      case 2: return ((int32_t)shift_lhs) >> 31;
      case 3: return ((core.CPSR().C().Get() << 31) | (shift_lhs >> 1));
      }
    }

    assert( false );
    return 0;
  }
  
  template <typename coreT>
  void
  UpdateStatusImmShift( coreT& core, uint32_t res, uint32_t shift_lhs, uint32_t shift, uint32_t shift_rhs )
  {
    uint32_t carry = 0;

    if      ((shift_rhs == 0) and (shift == 0)) /* MOVS */
      carry = core.CPSR().C().Get();
    else if ((shift_rhs == 0) and (shift == 3)) /* RRX */
      carry = (shift_lhs & 1);
    else if (shift == 0)                        /* LSL */
      carry = (shift_lhs >> (32 - shift_rhs)) & 1;
    else                                        /* LSR, ASR, ROR */
      carry = (shift_lhs >> ((shift_rhs - 1) & 0x1f)) & 1;

    core.CPSR().N().Set( (res >> 31) & 1 );
    core.CPSR().Z().Set( res == 0 );
    core.CPSR().C().Set( carry );
    /* CPSR.V unaltered */
  }

  template <typename coreT>
  uint32_t
  ComputeRegShift( coreT& core, uint32_t value, uint32_t shift, uint32_t shift_val )
  {
    shift_val &= 0xff;

    switch (shift) {
    case 0: return (shift_val < 32) ? (value << shift_val) : 0;
    case 1: return (shift_val < 32) ? (value >> shift_val) : 0;
    case 2: return (shift_val < 32) ? (((int32_t)value) >> shift_val) : (((int32_t)value) >> 31);
    case 3: return (value >> (shift_val & 0x1f)) | (value << (32 - (shift_val & 0x1f)));
    }

    return 0;
  }
  
  template <typename coreT>
  void
  UpdateStatusRegShift( coreT& core, uint32_t res, uint32_t value, uint32_t shift, uint32_t shift_val )
  {
    shift_val &= 0xff;
    uint32_t carry = 0;

    if (shift_val == 0) carry = core.CPSR().C().Get();
    else {
      switch (shift) {
      case 0: carry =  (shift_val <= 32) ? (value >> (32 - shift_val)) & 1 : 0; break;
      case 1: carry =  (shift_val <= 32) ? (value >> (shift_val - 1)) & 1 : 0; break;
      case 2: carry =  (shift_val <= 32) ? (value >> (shift_val - 1)) & 1 : (value >> 31) & 1; break;
      case 3: carry =  (value >> ((shift_val - 1) & 0x1f)) & 1; break;
      default: assert( false );
      }
    }

    core.CPSR().N().Set( (res >> 31) & 1 );
    core.CPSR().Z().Set( res == 0 );
    core.CPSR().C().Set( carry );
    /* CPSR.V unaltered */
  }
  
  template <typename coreT>
  void
  UpdateStatusAdd( coreT& core, uint32_t res, uint32_t lhs, uint32_t rhs )
  {
    core.CPSR().N().Set( (res >> 31) & 1 );
    core.CPSR().Z().Set( res == 0 );
    core.CPSR().C().Set( ((lhs & rhs) | ((~res) & (lhs | rhs))) >> 31 );
    core.CPSR().V().Set( ((lhs & rhs & (~res)) | ((~lhs) & (~rhs) & res)) >> 31 );
  }

  /* In ARM isa, the substraction carry correspond to the complementary                                                                          
   * addition's carry.                                                                                                                           
   */
  template <typename coreT>
  void
  UpdateStatusSub( coreT& core, uint32_t res, uint32_t lhs, uint32_t rhs )
  {
    core.CPSR().N().Set( (res >> 31) & 1 );
    core.CPSR().Z().Set( res == 0 );
    core.CPSR().C().Set( ((lhs & (~rhs)) | ((~res) & (lhs | (~rhs)))) >> 31 );
    core.CPSR().V().Set( ((lhs & (~rhs) & (~res)) | ((~lhs) & rhs & res)) >> 31 );
  }

  template <typename coreT>
  void
  ComputeMoveToPSR( coreT& core, uint32_t operand, uint32_t mask, bool isSPSR )
  {
    static uint32_t const msr_unallocmask = 0x06ffff00UL;
    static uint32_t const msr_usermask    = 0xf8000000UL;
    static uint32_t const msr_privmask    = 0x000000dfUL;
    static uint32_t const msr_statemask   = 0x01000020UL;
    
    if ( operand & msr_unallocmask ) // unpredictable
      return;
  
    /* creating the byte mask */
    uint32_t byte_mask = 0;
    if ((mask & 0x01) == 0x01) byte_mask |= 0x000000ff;
    if ((mask & 0x02) == 0x02) byte_mask |= 0x0000ff00;
    if ((mask & 0x04) == 0x04) byte_mask |= 0x00ff0000;
    if ((mask & 0x08) == 0x08) byte_mask |= 0xff000000;
  
    uint32_t run_mode = core.CPSR().M().Get(); /* get running mode */
    if (isSPSR == 0)
      {
        uint32_t reg_mask = 0;
        if ( run_mode != core.USER_MODE ) // we are in a privileged mode
          {
            if ( operand & msr_statemask )
              return; // unpredictable
            reg_mask = byte_mask &
              ( msr_usermask | msr_privmask );
          }
        else
          {
            reg_mask = byte_mask & msr_usermask;
          }
        uint32_t reg = (core.CPSR().bits() & ~reg_mask) | (operand & reg_mask);
        core.CPSR().bits() = reg;
      
        /* check if the running mode did change, if so switch registers */
        uint32_t new_run_mode = reg & core.RUNNING_MODE_MASK;
        if ( run_mode != new_run_mode )
          core.SetGPRMapping(run_mode, new_run_mode);
      }
    else // isSPSR == 1
      {
        // check that the mode has SPSR
        if ( !((run_mode == core.USER_MODE) || (run_mode == core.SYSTEM_MODE)) )
          {
            uint32_t reg_mask = byte_mask & (msr_usermask | msr_privmask | msr_statemask);
            uint32_t reg = (core.SPSR().bits() & ~reg_mask) | (operand & reg_mask);
            core.SPSR().bits() = reg;
          }
        else
          return; // unpredictable
      }
  }

  struct LSMIter {
    enum mode_t { DA=0, IA=1, DB=2, IB=3 };
    LSMIter( uint32_t mode, uint32_t reglist, uint32_t addr )
      : m_reglist( reglist ), m_addr( addr )
    {
      switch (mode_t( mode )) {
      case DA: m_dir = -1; m_reg = 16; m_incb =  0; m_inca = -4; m_addr += 4; break;
      case IA: m_dir = +1; m_reg = -1; m_incb =  0; m_inca = +4; m_addr -= 4; break;
      case DB: m_dir = -1; m_reg = 16; m_incb = -4; m_inca =  0; break;
      case IB: m_dir = +1; m_reg = -1; m_incb = +4; m_inca =  0; break;
      default: assert( false );
      }
    
    }
    bool
    next() {
      m_addr += m_inca;
      do    { m_reg += m_dir; if (m_reg & -16) return false; }
      while (((m_reglist >> m_reg) & 1) == 0);
      m_addr += m_incb;
      return true;
    }
    uint32_t addr() const { return m_addr; }
    uint32_t reg() const { return m_reg; }

    uint32_t m_reglist, m_addr, m_inca, m_incb;
    int32_t  m_dir, m_reg;
  };

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXECUTE_HH__ */
