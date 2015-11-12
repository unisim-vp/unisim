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

#include "unisim/component/cxx/processor/arm/psr.hh"
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
    uint32_t nzcv = core.CPSR().Get( NZCV );
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
      case 3: return ((core.CPSR().Get( C ) << 31) | (shift_lhs >> 1));
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
      carry = core.CPSR().Get( C );
    else if ((shift_rhs == 0) and (shift == 3)) /* RRX */
      carry = (shift_lhs & 1);
    else if (shift == 0)                        /* LSL */
      carry = (shift_lhs >> (32 - shift_rhs)) & 1;
    else                                        /* LSR, ASR, ROR */
      carry = (shift_lhs >> ((shift_rhs - 1) & 0x1f)) & 1;

    core.CPSR().Set( N, (res >> 31) & 1 );
    core.CPSR().Set( Z, res == 0 );
    core.CPSR().Set( C, carry );
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

    if (shift_val == 0) carry = core.CPSR().Get( C );
    else {
      switch (shift) {
      case 0: carry =  (shift_val <= 32) ? (value >> (32 - shift_val)) & 1 : 0; break;
      case 1: carry =  (shift_val <= 32) ? (value >> (shift_val - 1)) & 1 : 0; break;
      case 2: carry =  (shift_val <= 32) ? (value >> (shift_val - 1)) & 1 : (value >> 31) & 1; break;
      case 3: carry =  (value >> ((shift_val - 1) & 0x1f)) & 1; break;
      default: assert( false );
      }
    }

    core.CPSR().Set( N, (res >> 31) & 1 );
    core.CPSR().Set( Z, res == 0 );
    core.CPSR().Set( C, carry );
    /* CPSR.V unaltered */
  }
  
  template <typename coreT>
  void
  UpdateStatusAdd( coreT& core, uint32_t res, uint32_t lhs, uint32_t rhs )
  {
    core.CPSR().Set( N, (res >> 31) & 1 );
    core.CPSR().Set( Z, res == 0 );
    core.CPSR().Set( C, ((lhs & rhs) | ((~res) & (lhs | rhs))) >> 31 );
    core.CPSR().Set( V, ((lhs & rhs & (~res)) | ((~lhs) & (~rhs) & res)) >> 31 );
  }

  /* In ARM isa, the substraction carry correspond to the complementary                                                                          
   * addition's carry.                                                                                                                           
   */
  template <typename coreT>
  void
  UpdateStatusSub( coreT& core, uint32_t res, uint32_t lhs, uint32_t rhs )
  {
    core.CPSR().Set( N, (res >> 31) & 1 );
    core.CPSR().Set( Z, res == 0 );
    core.CPSR().Set( C, ((lhs & (~rhs)) | ((~res) & (lhs | (~rhs)))) >> 31 );
    core.CPSR().Set( V, ((lhs & (~rhs) & (~res)) | ((~lhs) & rhs & res)) >> 31 );
  }
  
  template <typename coreT>
  void
  CPSRWriteByInstr( coreT& core, uint32_t value, uint8_t mask, bool is_excpt_return )
  {
    uint32_t write_mask =
      ((mask & 1 ? 0xff : 0) <<  0) |
      ((mask & 2 ? 0xff : 0) <<  8) |
      ((mask & 4 ? 0xff : 0) << 16) |
      ((mask & 8 ? 0xff : 0) << 24);
    
    if (value & write_mask & core.PSR_UNALLOC_MASK) core.UnpredictableInsnBehaviour();
    
    bool const is_secure = true; // IsSecure()
    bool const nmfi = false; // Non Maskable FIQ (SCTLR.NMFI == '1');
    bool const scr_aw = false; // prevents Non-secure masking of asynchronous aborts that are taken to Monitor mode
    bool const scr_fw = false; // prevents Non-secure masking of FIQs that are taken to Monitor mode
    bool const have_virt_ext = false; // HaveVirtExt()

    bool privileged = (core.CPSR().Get(M) != core.USER_MODE);
    
    // ITSTATE, ISETSTATE are only written when returning from exception
    if (not is_excpt_return) {
      RegisterField<10,6>().Set( write_mask, 0 ); // IT<7:2>
      RegisterField<25,2>().Set( write_mask, 0 ); // IT<1:0>
      J.Set( write_mask, 0 );
      T.Set( write_mask, 0 );
    }
    
    if (not    (privileged and (is_secure or  scr_aw or have_virt_ext)))
      A.Set( write_mask, 0 );
    if (not    (privileged))
      I.Set( write_mask, 0 );
    if (not    (privileged and (not nmfi or not F.Get( value )) and (is_secure or scr_fw or have_virt_ext)))
      F.Set( write_mask, 0 );
    if (not    (privileged))
      M.Set( write_mask, 0 );
    
    // TODO: Check for attempts to enter modes only permitted in
    // Secure state from Non-secure state. These are Monitor mode
    // ('10110'), and FIQ mode ('10001') if the Security Extensions
    // have reserved it. The definition of UNPREDICTABLE does not
    // permit the resulting behavior to be a security hole.

    uint32_t new_cpsr = (core.CPSR().Get( ALL32 ) & ~write_mask) | (value & write_mask);
    
    core.CurrentMode().Swap(core); // OUT
    core.CPSR().Set( ALL32, new_cpsr );
    core.CurrentMode().Swap(core); // IN
  }

  template <typename coreT>
  void
  SPSRWriteByInstr( coreT& core, uint32_t value, uint8_t mask )
  {
    uint32_t write_mask =
      ((mask & 1 ? 0xff : 0) <<  0) |
      ((mask & 2 ? 0xff : 0) <<  8) |
      ((mask & 4 ? 0xff : 0) << 16) |
      ((mask & 8 ? 0xff : 0) << 24);
    uint32_t spsr = core.CurrentMode().GetSPSR();
    spsr = (spsr & ~write_mask) | (value & write_mask);
    core.CurrentMode().SetSPSR( spsr );
  }

  struct LSMIter
  {
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
  
  /******************/
  /* Floating Point */
  /******************/
  
  struct VFPExpandImm
  {
    uint8_t m : 4;
    uint8_t e : 3;
    uint8_t s : 1;
    
    VFPExpandImm( uint8_t n, uint8_t exp, uint8_t man )
      : m( man ), e( exp ^ 4 ), s( n ) {}
    
    template <typename T>
    T toFP() { return (s?-1:1) * (T( ((1 << 4) | m) << e ) / 128); }
    operator float () { return this->toFP<float>(); }
    operator double () { return this->toFP<double>(); }
    
    template <unsigned bcT>
    void toBytes( uint8_t (&bytes)[bcT] )
    {
      int32_t exponent = (int8_t((e^4) << 4)) >> 4;
      if (bcT == 4) {
        uint32_t bits = 0;
        bits |= uint32_t( m ) << 19;                   /* inserting mantissa */
        bits |= (uint32_t((e & 0xff) ^ 0x80) << 23);   /* inserting exponent */
        bits |= uint32_t( s ) << 31;                   /* inserting sign */
        for (unsigned byte = 0, bit = 0; byte < bcT; byte += 1, bit += 8) bytes[byte] = bits >> bit;
      } else if (bcT == 8) {
        uint64_t bits = 0;
        bits |= uint64_t( m ) << 48;                   /* inserting mantissa */
        bits |= (uint64_t((e & 0x7ff) ^ 0x400) << 52); /* inserting exponent */
        bits |= uint64_t( s ) << 63;                   /* inserting sign */
        for (unsigned byte = 0, bit = 0; byte < bcT; byte += 1, bit += 8) bytes[byte] = bits >> bit;
      } else throw 0;
    }
  };
  
  enum FPExc { InvalidOp=0, DivideByZero=1, Overflow=2, Underflow=3,
               Inexact=4, InputDenorm=7 };

  template <typename fpscrT>
  void FPProcessException( FPExc exception, fpscrT& fpscr )
  {
    switch (exception) {
    case InvalidOp:    if (fpscr.Get( IOE )) throw exception; else fpscr.Set( IOC, 1 ); break;
    case DivideByZero: if (fpscr.Get( DZE )) throw exception; else fpscr.Set( DZC, 1 ); break;
    case Overflow:     if (fpscr.Get( OFE )) throw exception; else fpscr.Set( OFC, 1 ); break;
    case Underflow:    if (fpscr.Get( UFE )) throw exception; else fpscr.Set( UFC, 1 ); break;
    case Inexact:      if (fpscr.Get( IXE )) throw exception; else fpscr.Set( IXC, 1 ); break;
    case InputDenorm:  if (fpscr.Get( IDE )) throw exception; else fpscr.Set( IDC, 1 ); break;
    }
  }

  template <typename operT, typename fpscrT>
  struct __FPProcessNaN__
  {
    operT& result;
    fpscrT& fpscr;
    operT const* firstSNaN;
    operT const* firstQNaN;
  
    __FPProcessNaN__( operT& res, fpscrT& _fpscr )
      : result( res ), fpscr( _fpscr ), firstSNaN(), firstQNaN()
    {}
  
    __FPProcessNaN__&
    operator << ( operT const& op )
    {
      if (not firstSNaN)
        {
          if (FloatIsSNaN( op, fpscr ))                         firstSNaN = &op;
          else if (not firstQNaN and FloatIsQNaN( op, fpscr ))  firstQNaN = &op;
        }
      return *this;
    }
  
    operator bool () const
    {
      if (not firstSNaN) {
        if (not firstQNaN) return false;
        result = *firstQNaN;
      }
      else /* hasSNaN*/ {
        result = *firstSNaN;
        FloatSetQuietBit( result, fpscr );
        FPProcessException( InvalidOp, fpscr );
      }
      if (fpscr.Get( DN )) {
        FloatSetDefaultNan( result, fpscr );
      } 
      return true;
    }
  };

  template <typename operT, typename fpscrT>
  __FPProcessNaN__<operT, fpscrT>
  FPProcessNaN( operT& res, fpscrT& fpscr )
  { return __FPProcessNaN__<operT, fpscrT>( res, fpscr ); }

  template <typename operT, typename fpscrT>
  void FPAdd( operT& result, operT const& op1, operT const& op2, fpscrT& fpscr )
  {
    if (fpscr.Get( FZ )) {
      FloatFlushToZero( op1, fpscr );
      FloatFlushToZero( op2, fpscr );
    }
    if (FPProcessNaN( result, fpscr ) << op1 << op2) return;
    
    FloatAdd( result, op1, op2, fpscr );
  }
  
  template <typename operT, typename fpscrT>
  void FPSub( operT& result, operT const& op1, operT const& op2, fpscrT& fpscr )
  {
    if (fpscr.Get( FZ )) {
      FloatFlushToZero( op1, fpscr );
      FloatFlushToZero( op2, fpscr );
    }
    if (FPProcessNaN( result, fpscr ) << op1 << op2) return;
    
    FloatSub( result, op1, op2, fpscr );
  }

  template <typename operT, typename fpscrT>
  void FPDiv( operT& result, operT const& op1, operT const& op2, fpscrT& fpscr )
  {
    if (fpscr.Get( FZ )) {
      FloatFlushToZero( op1, fpscr );
      FloatFlushToZero( op2, fpscr );
    }
    if (FPProcessNaN( result, fpscr ) << op1 << op2) return;
    
    FloatDiv( result, op1, op2, fpscr );
  }
  
  template <typename operT, typename fpscrT>
  void FPMul( operT& result, operT const& op1, operT const& op2, fpscrT& fpscr )
  {
    if (fpscr.Get( FZ )) {
      FloatFlushToZero( op1, fpscr );
      FloatFlushToZero( op2, fpscr );
    }
    if (FPProcessNaN( result, fpscr ) << op1 << op2) return;
    
    FloatMul( result, op1, op2, fpscr );
  }
  
  template <typename operT, typename fpscrT>
  void FPMulAdd( operT& acc, operT const& op1, operT const& op2, fpscrT& fpscr )
  {
    if (fpscr.Get( FZ )) {
      FloatFlushToZero( op1, fpscr );
      FloatFlushToZero( op2, fpscr );
      FloatFlushToZero( acc, fpscr );
    }
    if (FPProcessNaN( acc, fpscr ) << acc << op1 << op2) return;
    
    // TODO: Process the (QNaN + 0*inf) case
    FloatMulAdd( acc, op1, op2, fpscr );
  }
  
  template <typename operT, typename fpscrT>
  void FPSqrt( operT& result, operT const& op, fpscrT& fpscr )
  {
    if (fpscr.Get( FZ )) {
      FloatFlushToZero( op, fpscr );
    }
    if (FPProcessNaN( result, fpscr ) << op) return;
    
    FloatSqrt( result, op, fpscr );
  }
  
  template <typename operT, typename fpscrT>
  void FPCompare( operT const& op1, operT const& op2, bool quiet_nan_exc, fpscrT& fpscr )
  {
    if (fpscr.Get( FZ )) {
      FloatFlushToZero( op1, fpscr );
      FloatFlushToZero( op2, fpscr );
    }
    bool hasSNaN = FloatIsSNaN( op1, fpscr ) or FloatIsSNaN( op2, fpscr );
    bool hasQNaN = FloatIsQNaN( op1, fpscr ) or FloatIsQNaN( op2, fpscr );
    if (hasSNaN or hasQNaN) {
      fpscr.Set( NZCV, 3 ); /* N=0,Z=0,C=1,V=1 */
      if (hasSNaN or quiet_nan_exc)
        FPProcessException( InvalidOp, fpscr );
    }
    else {
      int fc = FloatCompare( op1, op2, fpscr );
      if      (fc == 0)
        fpscr.Set( NZCV, 6 ); /* N=0,Z=1,C=1,V=0 */
      else if (fc < 0)
        fpscr.Set( NZCV, 8 ); /* N=1,Z=0,C=0,V=0 */
      else  /* fc > 0 */
        fpscr.Set( NZCV, 2 ); /* N=0,Z=0,C=1,V=0 */
    }
  }

  struct Reject { void operator = ( bool condition ) const { if (condition) throw *this; } };
  
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXECUTE_HH__ */
