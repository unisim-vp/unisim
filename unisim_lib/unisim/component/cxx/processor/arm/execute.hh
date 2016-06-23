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

#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/util/truth_table/truth_table.hh>
#include <inttypes.h>
#include <stdexcept>

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
  typename coreT::BOOL
  CheckCondition( coreT& core, uint32_t cond )
  {
    util::truth_table::InBit<uint16_t,3> const N;
    util::truth_table::InBit<uint16_t,2> const Z;
    util::truth_table::InBit<uint16_t,1> const C;
    util::truth_table::InBit<uint16_t,0> const V;
    
    typedef typename coreT::U16 U16;
    typedef typename coreT::U32 U32;

    static U16 const condition_truth_tables[] = {
      U16((                  Z).tt), // eq; equal
      U16((              not Z).tt), // ne; not equal
      U16((                  C).tt), // cs/hs; unsigned higuer or same
      U16((              not C).tt), // cc/lo; unsigned lower
      U16((                  N).tt), // mi; negative
      U16((              not N).tt), // pl; positive or zero
      U16((                  V).tt), // vs; overflow set
      U16((              not V).tt), // vc; overflow clear
      U16((   not (not C or Z)).tt), // hi; unsigned higher
      U16((       (not C or Z)).tt), // ls; unsigned lower or same
      U16((      not (N xor V)).tt), // ge; signed greater than or equal
      U16((          (N xor V)).tt), // lt; signed less than
      U16((not(Z or (N xor V))).tt), // gt; signed greater than
      U16((   (Z or (N xor V))).tt), // le; signed less than or equal
      U16(                  0xffff),    // al; always
      U16(                  0x0000),    // <und>; never (illegal)
    };
    if (cond >= 15) throw std::logic_error("invalid condition code");
    U32 nzcv = core.CPSR().Get( NZCV );
    return ((condition_truth_tables[cond] >> nzcv) & U16(1)) != U16(0);
  }
  
  template <typename coreT>
  typename coreT::U32
  ComputeImmShift( coreT& core, typename coreT::U32 const& shift_lhs, unsigned shift, unsigned shift_rhs )
  {
    typedef typename coreT::S32 S32;
    typedef typename coreT::U32 U32;
    if (shift_rhs) {
      switch (shift) {
      case 0: return shift_lhs << shift_rhs;
      case 1: return shift_lhs >> shift_rhs;
      case 2: return U32(S32(shift_lhs) >> shift_rhs);
      case 3: return (shift_lhs >> shift_rhs) | (shift_lhs << (32 - shift_rhs));
      }
    } else {
      switch (shift) {
      case 0: return shift_lhs;
      case 1: return U32(0);
      case 2: return U32(S32(shift_lhs) >> 31);
      case 3: return ((core.CPSR().Get( C ) << 31) | (shift_lhs >> 1));
      }
    }
    
    throw std::logic_error("bad ComputeImmShift arguments");
    return U32(0);
  }
  
  template <typename coreT>
  void
  UpdateStatusImmShift( coreT& core, typename coreT::U32 const& res, typename coreT::U32 const& shift_lhs, unsigned shift, unsigned shift_rhs )
  {
    typedef typename coreT::U32 U32;
    U32 carry(0);

    if      ((shift_rhs == 0) and (shift == 0)) /* MOVS */
      carry = core.CPSR().Get( C );
    else if ((shift_rhs == 0) and (shift == 3)) /* RRX */
      carry = (shift_lhs & U32(1));
    else if (shift == 0)                        /* LSL */
      carry = (shift_lhs >> (32 - shift_rhs)) & U32(1);
    else                                        /* LSR, ASR, ROR */
      carry = (shift_lhs >> ((shift_rhs - 1) & 0x1f)) & U32(1);

    core.CPSR().Set( N, (res >> 31) & U32(1) );
    core.CPSR().Set( Z, U32(res == U32(0)) );
    core.CPSR().Set( C, carry );
    /* CPSR.V unaltered */
  }

  template <typename coreT>
  typename coreT::U32
  ComputeRegShift( coreT& core, typename coreT::U32 const& value, unsigned shift, typename coreT::U32 const& shift_val )
  {
    typedef typename coreT::U32 U32;
    typedef typename coreT::S32 S32;
    U32 shift8 = shift_val & U32(0xff);
    U32 shift5 = shift_val & U32(0x1f);
    // shift overflow mask: all ones if shift is valid, all zeros otherwise
    U32 shof_mask = ~(U32( (S32(shift8 | (U32(31) - shift8)) >> 31) ));
    
    switch (shift) {
    case 0: return (value << shift5) & shof_mask;
    case 1: return (value >> shift5) & shof_mask;
    case 2: return (U32(S32(value) >> shift5) & shof_mask) | (U32(S32(value) >> 31) & ~shof_mask);
    case 3: return RotateRight(value, shift5);
    }
    
    return U32(0);
  }
  
  template <typename coreT>
  void
  UpdateStatusRegShift( coreT& core, typename coreT::U32 const& res, typename coreT::U32 const& value, unsigned shift, typename coreT::U32 const& shift_val )
  {
    typedef typename coreT::U32 U32;
    typedef typename coreT::S32 S32;
    U32 shift8 = shift_val & U32(0xff);
    // Wether output carry is input carry
    U32 select_carry = U32(shift8 == U32(0));
    U32 shvalm1 = shift8 - U32(1);
    // Wether output carry comes from a bit of input value
    U32 select_bit = U32((shvalm1 >> 5) == U32(0));
    
    U32 carry(0);
    switch (shift) {
    case 0: carry = (value >> (U32(32) - shift8)) & select_bit; break;
    case 1: carry = (value >> shvalm1) & select_bit; break;
    case 2: carry = ((value >> shvalm1) & select_bit) | (U32(S32(value) >> 31) & (select_bit ^ U32(1))); break;
    case 3: carry = (value >> (shvalm1 & U32(0x1f))) & U32(1); break;
    default: throw std::logic_error("bad UpdateStatusRegShift arguments");
    }
    
    carry = (carry & ~select_carry) | (core.CPSR().Get( C ) & select_carry);

    core.CPSR().Set( N, (res >> 31) & U32(1) );
    core.CPSR().Set( Z, U32(res == U32(0)) );
    core.CPSR().Set( C, carry );
    /* CPSR.V unaltered */
  }
  
  template <typename coreT>
  void
  UpdateStatusAdd( coreT& core, typename coreT::U32 const& res, typename coreT::U32 const& lhs, typename coreT::U32 const& rhs )
  {
    typedef typename coreT::U32 U32;
    core.CPSR().Set( N, (res >> 31) & U32(1) );
    core.CPSR().Set( Z, U32(res == U32(0)) );
    core.CPSR().Set( C, ((lhs & rhs) | ((~res) & (lhs | rhs))) >> 31 );
    core.CPSR().Set( V, ((lhs & rhs & (~res)) | ((~lhs) & (~rhs) & res)) >> 31 );
  }

  /* In ARM isa, the substraction carry correspond to the complementary                                                                          
   * addition's carry.                                                                                                                           
   */
  template <typename coreT>
  void
  UpdateStatusSub( coreT& core, typename coreT::U32 const& res, typename coreT::U32 const& lhs, typename coreT::U32 const& rhs )
  {
    typedef typename coreT::U32 U32;
    core.CPSR().Set( N, (res >> 31) & U32(1) );
    core.CPSR().Set( Z, U32(res == U32(0)) );
    core.CPSR().Set( C, ((lhs & (~rhs)) | ((~res) & (lhs | (~rhs)))) >> 31 );
    core.CPSR().Set( V, ((lhs & (~rhs) & (~res)) | ((~lhs) & rhs & res)) >> 31 );
  }
  
  template <unsigned SIZE> struct _SWP_MSB { static uint32_t const mask = (_SWP_MSB<SIZE*2>::mask >> SIZE) | _SWP_MSB<SIZE*2>::mask; };
  template <> struct _SWP_MSB<32u> { static uint32_t const mask = 0x80000000; };
  
  template <unsigned SIZE>
  struct SWP
  {
    static unsigned const size = SIZE;
    static unsigned const msb2lsb = SIZE-1;
    static uint32_t const msbmask = _SWP_MSB<SIZE>::mask;
    static uint32_t const lsbmask = msbmask >> msb2lsb;
  };

  template <typename U32T, typename SWPT>
  U32T
  SignedSat(U32T& res, U32T const& overflow, U32T const& underflow, SWPT const& )
  {
    // In the following we saturate (if needed) the packed res value
    // by arithmetic and logic means. BIC masks represent bits that
    // should be cleared, and BIS masks represent bits that should be
    // set. The caller provides overflow (respectively underflow)
    // value that should have its packed MSB set in case of overflow
    // (respectively underflow).
    
    U32T of_bic = overflow & U32T(SWPT::msbmask);
    U32T of_bis = (((of_bic ^ U32T(SWPT::msbmask)) >> SWPT::msb2lsb) | of_bic) - U32T(SWPT::lsbmask);
    
    U32T uf_bis = underflow & U32T(SWPT::msbmask);
    U32T uf_bic = (((uf_bis ^ U32T(SWPT::msbmask)) >> SWPT::msb2lsb) | uf_bis) - U32T(SWPT::lsbmask);
    
    res = (res | (of_bis | uf_bis)) & ~(of_bic | uf_bic);
    
    return U32T((of_bic | uf_bis) != U32T(0));
  }
  
  template <typename U32T, typename SWPT>
  U32T
  UnsignedPSat(U32T& res, U32T const& overflow, SWPT const& )
  {
    // In the following we saturate (if needed) the packed res value
    // by arithmetic and logic means. BIC masks represent bits that
    // should be cleared, and BIS masks represent bits that should be
    // set. The caller provides overflow (respectively underflow)
    // value that should have its packed MSB set in case of overflow
    // (respectively underflow).
    
    U32T of_bis = overflow & U32T(SWPT::msbmask);
    of_bis |= (((of_bis ^ U32T(SWPT::msbmask)) >> SWPT::msb2lsb) | of_bis) - U32T(SWPT::lsbmask);
    
    res |= of_bis;
    
    return U32T(of_bis != U32T(0));
  }
  
  template <typename U32T, typename SWPT>
  U32T
  UnsignedNSat(U32T& res, U32T const& no_uflow, SWPT const& )
  {
    // In the following we saturate (if needed) the packed res value
    // by arithmetic and logic means. BIC masks represent bits that
    // should be cleared, and BIS masks represent bits that should be
    // set. The caller provides overflow (respectively underflow)
    // value that should have its packed MSB set in case of overflow
    // (respectively underflow).
    
    U32T uf_bam = no_uflow & U32T(SWPT::msbmask);
    uf_bam |= (((uf_bam ^ U32T(SWPT::msbmask)) >> SWPT::msb2lsb) | uf_bam) - U32T(SWPT::lsbmask);
    
    res &= uf_bam;
    
    return U32T(uf_bam != U32T(-1));
  }
  
  template <typename coreT>
  void
  CPSRWriteByInstr( coreT& core, typename coreT::U32 const& value, uint8_t mask, bool is_excpt_return )
  {
    typedef typename coreT::U32 U32;
    typedef typename coreT::BOOL BOOL;
    U32 write_mask(((mask & 1 ? 0xff : 0) <<  0) |
                   ((mask & 2 ? 0xff : 0) <<  8) |
                   ((mask & 4 ? 0xff : 0) << 16) |
                   ((mask & 8 ? 0xff : 0) << 24));
    
    if (core.Cond( (value & write_mask & U32(core.PSR_UNALLOC_MASK)) == U32(0) ))
      core.UnpredictableInsnBehaviour();

    
    BOOL const is_secure( true ); // IsSecure()
    BOOL const nmfi( false ); // Non Maskable FIQ (SCTLR.NMFI == '1');
    BOOL const scr_aw( false ); // prevents Non-secure masking of asynchronous aborts that are taken to Monitor mode
    BOOL const scr_fw( false ); // prevents Non-secure masking of FIQs that are taken to Monitor mode
    BOOL const have_virt_ext( false ); // HaveVirtExt()

    BOOL privileged = core.CPSR().Get(M) != U32(core.USER_MODE);
    
    // ITSTATE, ISETSTATE are only written when returning from exception
    if (not is_excpt_return) {
      RegisterField<10,6>().Set( write_mask, 0 ); // IT<7:2>
      RegisterField<25,2>().Set( write_mask, 0 ); // IT<1:0>
      J.Set( write_mask, 0 );
      T.Set( write_mask, 0 );
    }
    
    BOOL writeA = BOOL(A.Get( write_mask )) and (privileged and (is_secure or  scr_aw or have_virt_ext));
    A.Set( write_mask, U32(writeA) );
    BOOL writeI = BOOL(I.Get( write_mask )) and (privileged);
    I.Set( write_mask, U32(writeI) );
    BOOL writeF = BOOL(F.Get( write_mask )) and (privileged and (not nmfi or not BOOL(F.Get( value ))) and (is_secure or scr_fw or have_virt_ext));
    F.Set( write_mask, U32(writeF) );
    BOOL writeM = BOOL(M.Get( write_mask )) and (privileged);
    M.Set( write_mask, -U32(writeM) );
    
    // TODO: Check for attempts to enter modes only permitted in
    // Secure state from Non-secure state. These are Monitor mode
    // ('10110'), and FIQ mode ('10001') if the Security Extensions
    // have reserved it. The definition of UNPREDICTABLE does not
    // permit the resulting behavior to be a security hole.

    U32 new_cpsr = (core.CPSR().Get( ALL32 ) & ~write_mask) | (value & write_mask);
    
    core.CurrentMode().Swap(core); // OUT
    core.CPSR().Set( ALL32, new_cpsr );
    core.CurrentMode().Swap(core); // IN
  }
  
  template <typename coreT>
  void
  SPSRWriteByInstr( coreT& core, typename coreT::U32 const& value, uint8_t mask )
  {
    typedef typename coreT::U32 U32;
    U32 write_mask(((mask & 1 ? 0xff : 0) <<  0) |
                   ((mask & 2 ? 0xff : 0) <<  8) |
                   ((mask & 4 ? 0xff : 0) << 16) |
                   ((mask & 8 ? 0xff : 0) << 24));
    U32 spsr = core.CurrentMode().GetSPSR();
    spsr = (spsr & ~write_mask) | (value & write_mask);
    core.CurrentMode().SetSPSR( spsr );
  }

  struct LSMIter
  {
    enum mode_t { DA=0, IA=1, DB=2, IB=3 };
    LSMIter( uint32_t mode, uint32_t reglist )
      : m_reglist( reglist ), m_offset( 0 )
    {
      switch (mode_t( mode )) {
      case DA: m_dir = -1; m_reg = 16; m_incb =  0; m_inca = -4; m_offset += 4; break;
      case IA: m_dir = +1; m_reg = -1; m_incb =  0; m_inca = +4; m_offset -= 4; break;
      case DB: m_dir = -1; m_reg = 16; m_incb = -4; m_inca =  0; break;
      case IB: m_dir = +1; m_reg = -1; m_incb = +4; m_inca =  0; break;
      default: throw std::logic_error("Bad LSM mode");
      }
    
    }
    bool
    next() {
      m_offset += m_inca;
      do    { m_reg += m_dir; if (m_reg & -16) return false; }
      while (((m_reglist >> m_reg) & 1) == 0);
      m_offset += m_incb;
      return true;
    }
    uint32_t offset() const { return m_offset; }
    uint32_t reg() const { return m_reg; }

    uint32_t m_reglist, m_offset, m_inca, m_incb;
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
    
    VFPExpandImm( uint8_t neg, uint8_t exp, uint8_t man )
      : m( man ), e( exp ^ 4 ), s( neg ) {}
    
    template <typename T>
    T toFP() { return (s?-1:1) * (T( ((1 << 4) | m) << e ) / 128); }
    operator float () { return this->toFP<float>(); }
    operator double () { return this->toFP<double>(); }
    
    template <unsigned bcT>
    void toBytes( uint8_t (&bytes)[bcT] ) const
    {
      int32_t exp = (int8_t((e^4) << 4)) >> 4;
      if (bcT == 4) {
        uint32_t bits = 0;
        bits |= uint32_t( m ) << 19;                   /* inserting mantissa */
        bits |= (uint32_t((exp & 0xff) ^ 0x80) << 23);   /* inserting exponent */
        bits |= uint32_t( s ) << 31;                   /* inserting sign */
        for (unsigned byte = 0, bit = 0; byte < bcT; byte += 1, bit += 8) bytes[byte] = bits >> bit;
      } else if (bcT == 8) {
        uint64_t bits = 0;
        bits |= uint64_t( m ) << 48;                   /* inserting mantissa */
        bits |= (uint64_t((exp & 0x7ff) ^ 0x400) << 52); /* inserting exponent */
        bits |= uint64_t( s ) << 63;                   /* inserting sign */
        for (unsigned byte = 0, bit = 0; byte < bcT; byte += 1, bit += 8) bytes[byte] = bits >> bit;
      } else throw std::logic_error("unexpected FP size");
    }
  };
  
  enum FPExc { InvalidOp=0, DivideByZero=1, Overflow=2, Underflow=3,
               Inexact=4, InputDenorm=7 };

  template <typename ARCH, typename fpscrT>
  void FPProcessException( FPExc exception, ARCH& arch, fpscrT& fpscr )
  {
    typedef typename ARCH::U32 U32;
    
    switch (exception) {
    case InvalidOp:    if (arch.Cond( fpscr.Get( IOE ) )) throw exception; else fpscr.Set( IOC, U32(1) ); break;
    case DivideByZero: if (arch.Cond( fpscr.Get( DZE ) )) throw exception; else fpscr.Set( DZC, U32(1) ); break;
    case Overflow:     if (arch.Cond( fpscr.Get( OFE ) )) throw exception; else fpscr.Set( OFC, U32(1) ); break;
    case Underflow:    if (arch.Cond( fpscr.Get( UFE ) )) throw exception; else fpscr.Set( UFC, U32(1) ); break;
    case Inexact:      if (arch.Cond( fpscr.Get( IXE ) )) throw exception; else fpscr.Set( IXC, U32(1) ); break;
    case InputDenorm:  if (arch.Cond( fpscr.Get( IDE ) )) throw exception; else fpscr.Set( IDC, U32(1) ); break;
    }
  }

  template <typename operT, typename ARCH, typename fpscrT>
  struct __FPProcessNaN__
  {
    operT& result;
    ARCH& arch;
    fpscrT& fpscr;
    operT const* firstSNaN;
    operT const* firstQNaN;
    
  
    __FPProcessNaN__( operT& res, ARCH& _arch, fpscrT& _fpscr )
      : result( res ), arch( _arch ), fpscr( _fpscr ), firstSNaN(), firstQNaN()
    {}
  
    __FPProcessNaN__&
    operator << ( operT const& op )
    {
      typedef typename ARCH::BOOL BOOL;
      
      if (not firstSNaN)
        {
          if      (arch.Cond( ARCH::FP::IsSNaN( op, fpscr ) ))                        firstSNaN = &op;
          else if (arch.Cond( BOOL(not firstQNaN) and ARCH::FP::IsQNaN( op, fpscr ))) firstQNaN = &op;
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
        ARCH::FP::SetQuietBit( result, fpscr );
        FPProcessException( InvalidOp, arch, fpscr );
      }
      if (arch.Cond( fpscr.Get( DN ) )) {
        ARCH::FP::SetDefaultNan( result, fpscr );
      } 
      return true;
    }
  };

  template <typename operT, typename ARCH, typename fpscrT>
  __FPProcessNaN__<operT, ARCH, fpscrT>
  FPProcessNaN( operT& res, ARCH& arch, fpscrT& fpscr )
  { return __FPProcessNaN__<operT, ARCH, fpscrT>( res, arch, fpscr ); }

  template <typename operT, typename ARCH, typename fpscrT>
  void
  FPFlushToZero( operT& op, ARCH& arch, fpscrT& fpscr )
  {
    if (ARCH::FP::FlushToZero( op, fpscr ))
      FPProcessException( InputDenorm, arch, fpscr );
  }

  template <typename operT, typename ARCH, typename fpscrT>
  void FPAdd( operT& result, operT& op1, operT& op2, ARCH& arch, fpscrT& fpscr )
  {
    if (arch.Cond( fpscr.Get( FZ ) )) {
      FPFlushToZero( op1, arch, fpscr );
      FPFlushToZero( op2, arch, fpscr );
    }
    if (FPProcessNaN( result, arch, fpscr ) << op1 << op2) return;
    
    ARCH::FP::Add( result, op1, op2, fpscr );
  }
  
  template <typename operT, typename ARCH, typename fpscrT>
  void FPSub( operT& result, operT& op1, operT& op2, ARCH& arch, fpscrT& fpscr )
  {
    if (arch.Cond( fpscr.Get( FZ ) )) {
      FPFlushToZero( op1, arch, fpscr );
      FPFlushToZero( op2, arch, fpscr );
    }
    if (FPProcessNaN( result, arch, fpscr ) << op1 << op2) return;
    
    ARCH::FP::Sub( result, op1, op2, fpscr );
  }

  template <typename operT, typename ARCH, typename fpscrT>
  void FPDiv( operT& result, operT& op1, operT& op2, ARCH& arch, fpscrT& fpscr )
  {
    if (arch.Cond( fpscr.Get( FZ ) )) {
      FPFlushToZero( op1, arch, fpscr );
      FPFlushToZero( op2, arch, fpscr );
    }
    if (FPProcessNaN( result, arch, fpscr ) << op1 << op2) return;
    
    ARCH::FP::Div( result, op1, op2, fpscr );
  }
  
  template <typename operT, typename ARCH, typename fpscrT>
  void FPMul( operT& result, operT& op1, operT& op2, ARCH& arch, fpscrT& fpscr )
  {
    if (arch.Cond( fpscr.Get( FZ ) )) {
      FPFlushToZero( op1, arch, fpscr );
      FPFlushToZero( op2, arch, fpscr );
    }
    if (FPProcessNaN( result, arch, fpscr ) << op1 << op2) return;
    
    ARCH::FP::Mul( result, op1, op2, fpscr );
  }
  
  template <typename operT, typename ARCH, typename fpscrT>
  void FPMulAdd( operT& acc, operT& op1, operT& op2, ARCH& arch, fpscrT& fpscr )
  {
    if (arch.Cond( fpscr.Get( FZ ) )) {
      FPFlushToZero( op1, arch, fpscr );
      FPFlushToZero( op2, arch, fpscr );
      FPFlushToZero( acc, arch, fpscr );
    }
    if (FPProcessNaN( acc, arch, fpscr ) << acc << op1 << op2) return;
    
    // TODO: Process the (QNaN + 0*inf) case
    ARCH::FP::MulAdd( acc, op1, op2, fpscr );
  }
  
  template <typename operT, typename ARCH, typename fpscrT>
  void FPSqrt( operT& result, operT& op, ARCH& arch, fpscrT& fpscr )
  {
    if (arch.Cond( fpscr.Get( FZ ) )) {
      FPFlushToZero( op, arch, fpscr );
    }
    if (FPProcessNaN( result, arch, fpscr ) << op) return;
    
    ARCH::FP::Sqrt( result, op, fpscr );
  }
  
  
  template <typename operT, typename ARCH, typename fpscrT>
  void FPCompare( operT& op1, operT& op2, bool quiet_nan_exc, ARCH& arch, fpscrT& fpscr )
  {
    typedef typename ARCH::BOOL BOOL;
    typedef typename ARCH::S32 S32;
    typedef typename ARCH::U32 U32;
    
    if (arch.Cond( fpscr.Get( FZ ) )) {
      FPFlushToZero( op1, arch, fpscr );
      FPFlushToZero( op2, arch, fpscr );
    }
    BOOL hasSNaN = ARCH::FP::IsSNaN( op1, fpscr ) or ARCH::FP::IsSNaN( op2, fpscr );
    BOOL hasQNaN = ARCH::FP::IsQNaN( op1, fpscr ) or ARCH::FP::IsQNaN( op2, fpscr );
    if (arch.Cond(hasSNaN or hasQNaN)) {
      fpscr.Set( NZCV, U32(3) ); /* N=0,Z=0,C=1,V=1 */
      if (arch.Cond(hasSNaN or BOOL(quiet_nan_exc)))
        FPProcessException( InvalidOp, arch, fpscr );
    }
    else {
      S32 fc = ARCH::FP::Compare( op1, op2, fpscr );
      S32 const zero(0);
      
      if      (arch.Cond( fc == zero ))
        fpscr.Set( NZCV, U32(6) ); /* N=0,Z=1,C=1,V=0 */
      else if (arch.Cond( fc < zero ))
        fpscr.Set( NZCV, U32(8) ); /* N=1,Z=0,C=0,V=0 */
      else  /* fc > zero */
        fpscr.Set( NZCV, U32(2) ); /* N=0,Z=0,C=1,V=0 */
    }
  }

  struct Reject { void operator = ( bool condition ) const { if (condition) throw *this; } };
  
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXECUTE_HH__ */
