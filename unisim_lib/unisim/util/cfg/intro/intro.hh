/*
 *  Copyright (c) 2024,
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

#ifndef __UNISIM_UTIL_CFG_INTRO_HH__
#define __UNISIM_UTIL_CFG_INTRO_HH__

#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/floating_point/floating_point.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/service/interfaces/instruction_collecting.hh>
#include <cstdint>

namespace unisim {
namespace util {
namespace cfg {
namespace intro {

  template <typename Bool> struct AssertBool {};
  template <>              struct AssertBool<bool> { static void check() {} };

  template <typename VALUE_TYPE>
  struct XValue
  {
    typedef VALUE_TYPE value_type;
    typedef XValue<value_type> this_type;

    XValue() : value(), determined(false) {}
    XValue( value_type _value, bool _determined ) : value(_value), determined(_determined) {}
    explicit XValue( value_type _value ) : value(_value), determined(true) {}

    template <typename SRC_VALUE_TYPE>  explicit XValue( XValue<SRC_VALUE_TYPE> const& other ) : value(other.value), determined(other.determined) {}

    template <typename SHT> this_type operator << ( SHT sh ) const { return this_type( value << sh, determined ); }
    template <typename SHT> this_type operator >> ( SHT sh ) const { return this_type( value >> sh, determined ); }
    template <typename SHT> this_type& operator <<= ( SHT sh ) { value <<= sh; return *this; }
    template <typename SHT> this_type& operator >>= ( SHT sh ) { value >>= sh; return *this; }
    template <typename SHT> this_type operator << ( XValue<SHT> const& sh ) const { return this_type( value << sh.value, determined and sh.determined ); }
    template <typename SHT> this_type operator >> ( XValue<SHT> const& sh ) const { return this_type( value >> sh.value, determined and sh.determined ); }
    template <typename SHT> this_type& operator <<= ( XValue<SHT> const&  sh ) { value <<= sh.value; return *this; }
    template <typename SHT> this_type& operator >>= ( XValue<SHT> const&  sh ) { value >>= sh.value; return *this; }

    this_type operator - () const { return this_type( -value, determined ); }
    this_type operator ~ () const { return this_type( ~value, determined ); }

    this_type& operator += ( this_type const& other ) { value += other.value; determined &= other.determined; return *this; }
    this_type& operator -= ( this_type const& other ) { value -= other.value; determined &= other.determined; return *this; }
    this_type& operator *= ( this_type const& other ) { value *= other.value; determined &= other.determined; return *this; }
    this_type& operator /= ( this_type const& other ) { value /= other.value; determined &= other.determined; return *this; }
    this_type& operator %= ( this_type const& other ) { value %= other.value; determined &= other.determined; return *this; }
    this_type& operator ^= ( this_type const& other ) { value ^= other.value; determined &= other.determined; return *this; }
    this_type& operator &= ( this_type const& other ) { value &= other.value; determined &= other.determined; return *this; }
    this_type& operator |= ( this_type const& other ) { value |= other.value; determined &= other.determined; return *this; }

    this_type operator + ( this_type const& other ) const { return this_type( value + other.value, determined and other.determined ); }
    this_type operator - ( this_type const& other ) const { return this_type( value - other.value, determined and other.determined ); }
    this_type operator * ( this_type const& other ) const { return this_type( value * other.value, determined and other.determined ); }
    this_type operator / ( this_type const& other ) const { return (determined and other.determined) ? this_type( value / other.value ) : this_type(); }
    this_type operator % ( this_type const& other ) const { return this_type( value % other.value, determined and other.determined ); }
    this_type operator ^ ( this_type const& other ) const { return this_type( value ^ other.value, determined and other.determined ); }
    this_type operator & ( this_type const& other ) const { return this_type( value & other.value, determined and other.determined ); }
    this_type operator | ( this_type const& other ) const { return this_type( value | other.value, determined and other.determined ); }

    XValue<bool> operator == ( this_type const& other ) const { return XValue<bool>( value == other.value, determined and other.determined ); }
    XValue<bool> operator != ( this_type const& other ) const { return XValue<bool>( value != other.value, determined and other.determined ); }
    XValue<bool> operator <= ( this_type const& other ) const { return XValue<bool>( value <= other.value, determined and other.determined ); }
    XValue<bool> operator >= ( this_type const& other ) const { return XValue<bool>( value >= other.value, determined and other.determined ); }
    XValue<bool> operator < ( this_type const& other ) const  { return XValue<bool>( value < other.value, determined and other.determined ); }
    XValue<bool> operator > ( this_type const& other ) const  { return XValue<bool>( value > other.value, determined and other.determined ); }

    XValue<bool> operator ! () const { AssertBool<value_type>::check(); return XValue<bool>( not value, determined ); }

    XValue<bool> operator && ( XValue<bool> const& other ) const { AssertBool<value_type>::check(); return XValue<bool>( value and other.value, determined and other.determined ); }

    XValue<bool> operator || ( XValue<bool> const& other ) const { AssertBool<value_type>::check(); return XValue<bool>( value or other.value, determined and other.determined ); }

    value_type value;
    bool determined;
  };

  template <typename T, class F>
  XValue<T> XApply( F const& f, XValue<T> const& l, XValue<T> const& r ) { return XValue<T>( f(l.value, r.value), l.determined and r.determined ); }

  template <typename T, class F>
  XValue<T> XApply( F const& f, XValue<T> const& v ) { return XValue<T>( f(v.value), v.determined ); }

  template <typename I> XValue<I> Minimum( XValue<I> const& l, XValue<I> const& r ) { return XApply( std::min<I>, l, r ); }
  template <typename I> XValue<I> Maximum( XValue<I> const& l, XValue<I> const& r ) { return XApply( std::max<I>, l, r ); }

  template <typename U> U ByteSwap( U const& v ) { return U( unisim::util::endian::ByteSwap( v.value ), v.determined ); }
  template <typename U> U BitScanReverse(U const& u) { return U(unisim::util::arithmetic::BitScanReverse(u.value), u.determined); }
  template <typename U> U BitScanForward(U const& u) { return U(unisim::util::arithmetic::BitScanForward(u.value), u.determined); }
  template <typename U> U PopCount(U const& u) { return U(unisim::util::arithmetic::PopCount(u.value), u.determined ); }

  template <typename F> XValue<F> FAbs(XValue<F> const& f) { return XApply(unisim::util::floating_point::_FAbs<F>, f); }
  template <typename F> XValue<F> Zeroes(XValue<F> const& f) { return XApply(unisim::util::floating_point::_Zeroes<F>, f); }
  template <typename F> XValue<F> ClearSignaling(XValue<F> const& f) { return XApply(unisim::util::floating_point::_ClearSignaling<F>, f); }
  // template <typename F> XValue<F> FAbs(XValue<F> const& f) { return F(unisim::util::floating_point::_FAbs<F>(f.value), f.determined); }
  // template <typename F> F FAbs(F const& f) { return F(unisim::util::floating_point::_FAbs<typename F::value_type>(f.value), f.determined); }
  template <typename F> F FMin( F const& l, F const& r ) { return XApply( std::min<typename F::value_type>, l, r ); }
  template <typename F> F FMax( F const& l, F const& r ) { return XApply( std::max<typename F::value_type>, l, r ); }
  template <typename F> F FMinNumber( F const& l, F const& r ) { return XApply( unisim::util::floating_point::_FMinNumber<typename F::value_type>, l, r ); }
  template <typename F> F FMaxNumber( F const& l, F const& r ) { return XApply( unisim::util::floating_point::_FMaxNumber<typename F::value_type>, l, r ); }
  template <typename F> XValue<bool> IsZero(F const& f) { return XValue<bool>(unisim::util::floating_point::_IsZero( f.value ), f.determined); }
  template <typename F> XValue<bool> IsNaN(F const& f) { return XValue<bool>(unisim::util::floating_point::_IsNaN( f.value ), f.determined); }
  template <typename F> XValue<bool> IsInf(F const& f) { return XValue<bool>(unisim::util::floating_point::_IsInf( f.value ), f.determined); }
  template <typename F> XValue<bool> IsSignaling(F const& f) { return XValue<bool>(unisim::util::floating_point::_IsSignaling( f.value ), f.determined); }
  template <typename F> XValue<bool> IsDenormal(F const& f) { return XValue<bool>(unisim::util::floating_point::_IsDenormal( f.value ), f.determined); }

  template <typename UTP, typename STP>
  UTP RotateRight( UTP const& v, STP const& _s ) { XValue<uint8_t> s(_s); return UTP( unisim::util::arithmetic::RotateRight( v.value, s.value ), v.determined and s.determined ); }
  template <typename UTP, typename STP>
  UTP RotateLeft( UTP const& v, STP const& _s ) { XValue<uint8_t> s(_s); return UTP( unisim::util::arithmetic::RotateLeft( v.value, s.value ), v.determined and s.determined ); }

  template <typename TP>
  TP ConditionalMove( XValue<bool> const& cond, TP const& tval, TP const& fval )
  {
    return TP(cond.value ? tval.value : fval.value, cond.determined and ((cond.value and tval.determined) or (not cond.value and fval.determined)));
  }

  struct BranchInfo
  {
    enum mode_t { NoJump = 0, Direct, Indirect, NA };
    enum hint_t { NoHint = 0, Call, Return };

    BranchInfo() : address(), mode(NA), pass(false), hint(NoHint) {}
    template <class X> void update( bool branch, X const& x, hint_t _hint ) { update( branch, x.determined, x.value, _hint ); }
    bool startupdate() { if (mode == NA) { mode = NoJump; return true; } return false; }
    void update( bool branch, bool known, uint64_t target, hint_t _hint );
    bool has_jump() const { return mode != NoJump; }
    bool is_indirect() const { return mode == Indirect; }
    template <class OP, typename ADDR>
    void Collect(OP const* op, ADDR insn_addr, ADDR next_addr, unisim::service::interfaces::InstructionCollecting<ADDR>* icif) const;

    uint64_t address;
    unsigned mode : 2;
    unsigned pass : 1;
    unsigned hint : 2;
  };

  template <class OP, typename ADDR>
  void BranchInfo::Collect(OP const* op, ADDR insn_addr, ADDR next_addr, unisim::service::interfaces::InstructionCollecting<ADDR>* icif) const
  {
    unisim::service::interfaces::InstructionInfo<ADDR> insn_info;
    uint32_t encoding = op->GetEncoding();
    uint32_t bytes = OP::decoder_type::little_endian ? unisim::util::endian::LittleEndian2Host(encoding) : unisim::util::endian::BigEndian2Host(encoding);
    unsigned insn_size = op->GetLength() / 8;
    insn_info.addr = insn_addr;
    insn_info.size = insn_size;
    insn_info.opcode = (uint8_t const*)&bytes;
    insn_info.fallthrough = insn_addr + insn_size;
    insn_info.target = next_addr;
    insn_info.mode = this->is_indirect() ? insn_info.INDIRECT : insn_info.DIRECT;
    if (this->has_jump())
      {
        switch (this->hint)
          {
          default:                 insn_info.type = this->pass ? insn_info.BRANCH : insn_info.JUMP; break;
          case BranchInfo::Call:   insn_info.type = insn_info.CALL; break;
          case BranchInfo::Return: insn_info.type = insn_info.RETURN; break;
          }
        if (not this->is_indirect())
          insn_info.target = this->address;
      }
    else
      insn_info.type = insn_info.STANDARD;
    icif->CollectInstruction(insn_info);
  }

} /* end of namespace intro */
} /* end of namespace cfg */
} /* end of namespace util */
} /* end of namespace unisim */

#endif // __UNISIM_UTIL_CFG_INTRO_HH__
