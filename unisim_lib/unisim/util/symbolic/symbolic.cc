/*
 *  Copyright (c) 2017-2023,
 *  Commissariat a l'Energie Atomique (CEA),
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
 
#include <unisim/util/symbolic/symbolic.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/endian/endian.hh>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace unisim {
namespace util {
namespace symbolic {

  char const*
  Expr::dbgrepr() const
  {
    static std::string result_string;
    std::ostringstream buf;
    node->Repr(buf);
    result_string = buf.str();
    return result_string.c_str();
  }

  void
  ValueType::GetName(std::ostream& sink) const
  {
    unsigned bitsize = GetBitSize();
    switch (encoding)
      {
      default:                  sink << "?"; break;
      case ValueType::FLOAT:    sink << 'F'; break;
      case ValueType::UNSIGNED: sink << "U"; break;
      case ValueType::SIGNED:   sink << "S"; break;
      case ValueType::BOOL:
        sink << "BOOL";
        if (bitsize == 1) return;
        break;
      }
    sink << bitsize;
  }
  
  ValueType const* CValueType(ValueType::encoding_t encoding, unsigned bitsize)
  {
    typedef long double f80_t;
    switch (encoding)
      {
      case ValueType::FLOAT:
        switch (bitsize) {
        default: break;
        case 32: return CValueType(float());
        case 64: return CValueType(double());
        case 80: return CValueType(f80_t());
        } break;
      case ValueType::UNSIGNED:
        switch (bitsize) {
        default: break;;
        case  8: return CValueType(uint8_t());
        case 16: return CValueType(uint16_t());
        case 32: return CValueType(uint32_t());
        case 64: return CValueType(uint64_t());
        } break;
      case ValueType::SIGNED:
        switch (bitsize) {
        default: break;;
        case  8: return CValueType(int8_t());
        case 16: return CValueType(int16_t());
        case 32: return CValueType(int32_t());
        case 64: return CValueType(int64_t());
        } break;
      case ValueType::BOOL:
        if (bitsize != 1) break;
        return CValueType(bool());
      default:
        break;
      }
    struct Bad {}; throw Bad();
    return 0;
  }

  ValueType const* NoValueType()
  {
    static struct NoType: public ValueType
    {
      NoType() : ValueType(ValueType::NA) {}
      virtual unsigned GetBitSize() const override { return 0; }
      virtual void GetName(std::ostream& sink) const override { sink << "NoType"; }
    } type_desc;
    return &type_desc;
  }

  std::ostream&
  operator << (std::ostream& sink, Expr const& expr )
  {
    if (expr.good())
      expr->Repr( sink );
    else
      sink << "*null*";
    
    return sink;
  }
  
  ConstNodeBase const*
  Expr::Eval( EvalSpace const& evs ) const
  {
    unsigned subcount = node->SubCount();
    Expr dispose[subcount];
    ConstNodeBase const* cnbs[subcount];
    for (unsigned idx = 0; idx < subcount; ++idx)
      {
        if (not (cnbs[idx] = node->GetSub(idx).Eval( evs )))
          return 0;
        dispose[idx] = cnbs[idx];
      }
    return node->Eval( evs, &cnbs[0] );
  }
    
  ConstNodeBase const*
  Expr::ConstSimplify()
  {
    unsigned subcount = node->SubCount();
    Expr args[subcount];
    ConstNodeBase const* cnbs[subcount];
    bool const_args = true, simplified = false;
    for (unsigned idx = 0; idx < subcount; ++idx)
      {
        if (not (cnbs[idx] = (args[idx] = node->GetSub(idx)).ConstSimplify()))
          const_args = false;
        if (args[idx] != node->GetSub(idx))
          simplified = true;
      }
    
    if (const_args)
      {
        if (ConstNodeBase const* cn = node->Eval( EvalSpace(), &cnbs[0] ))
          {
            *this = Expr( cn );
            return cn;
          }
      }

    if (simplified)
      {
        ExprNode* nn = node->Mutate();
        for (unsigned idx = 0; idx < subcount; ++idx)
          const_cast<Expr&>( nn->GetSub(idx) ) = args[idx];
        *this = Expr( nn );
      }
    
    return 0;
  }
    
  void
  OpNodeBase::Repr( std::ostream& sink ) const
  {
    sink << op.c_str() << "( ";
    char const* sep = "";
    for (unsigned idx = 0, end = SubCount(); idx < end; sep = ", ", ++idx)
      sink << sep << GetSub(idx);
    sink << " )";
  }

  void
  ConstNodeBase::Repr( std::ostream& sink ) const
  {
    typedef long double f80_t;
    ValueType const* tp = GetType();
    tp->GetName(sink);
    sink << '(';
    switch (tp->encoding)
      {
      case ValueType::UNSIGNED:
        if (tp->GetBitSize() > 64) std::cerr << "Warning: oversized unsigned number.\n";
        sink << GetBits(0);
        break;
      case ValueType::SIGNED:
        if (tp->GetBitSize() > 64) std::cerr << "Warning: oversized signed number.\n";
        sink << int64_t(GetBits(0));
        break;
      case ValueType::FLOAT:
        sink << GetFloat(f80_t());
        break;
      default:
        sink << "???";
        break;
      }
    sink << ')';
  }

  void
  CastNodeBase::Repr( std::ostream& sink ) const
  {
    GetType()->GetName(sink);
    sink << "( " << src << " )";
  }

  void FP::DefaultNaNNode::Repr( std::ostream& sink ) const { sink << "DefaultNaN()"; }
  void FP::IsNaNNode::Repr( std::ostream& sink ) const { sink << "IsNaN(" << src << ", " << int(signaling) << ", " << int(quiet) << ")"; }
  void FP::MulAddNode::Repr( std::ostream& sink ) const { sink << "MulAdd( " << acc << ", " << left << ", " << right << " )"; }
  void FP::IsInvalidMulAddNode::Repr( std::ostream& sink ) const { sink << "IsInvalidMulAdd( " << acc << ", " << left << ", " << right << " )"; }
  void FP::FtoFNode::Repr( std::ostream& sink ) const { sink << "FtoF( " << src << " )"; }
  void FP::FtoINodeBase::Repr( std::ostream& sink ) const { sink << "FtoI( " << src << " )"; }
  void FP::ItoFNodeBase::Repr( std::ostream& sink ) const { sink << "ItoF( " << src << " )"; }

  bool   EvalMul( bool, bool ) { throw std::logic_error( "No * for bool." ); }
  
  long double   EvalMod( long double l, long double r ) { throw std::logic_error( "No ^ for long double." ); }
  double   EvalMod( double l, double r ) { throw std::logic_error( "No ^ for double." ); }
  float    EvalMod( float l, float r ) { throw std::logic_error( "No ^ for float." ); }
  
  long double   EvalXor( long double l, long double r ) { throw std::logic_error( "No ^ for long double." ); }
  double   EvalXor( double l, double r ) { throw std::logic_error( "No ^ for double." ); }
  float    EvalXor( float l, float r ) { throw std::logic_error( "No ^ for float." ); }
  
  long double   EvalAnd( long double l, long double r ) { throw std::logic_error( "No & for long double." ); }
  double   EvalAnd( double l, double r ) { throw std::logic_error( "No & for double." ); }
  float    EvalAnd( float l, float r ) { throw std::logic_error( "No & for float." ); }
  
  long double   EvalOr( long double l, long double r ) { throw std::logic_error( "No | for long double." ); }
  double   EvalOr( double l, double r ) { throw std::logic_error( "No | for double." ); }
  float    EvalOr( float l, float r ) { throw std::logic_error( "No | for float." ); }
  
  long double   EvalNot( long double val ) { throw std::logic_error( "No ~ for long double." ); }
  double   EvalNot( double val ) { throw std::logic_error( "No ~ for double." ); }
  float    EvalNot( float val ) { throw std::logic_error( "No ~ for float." ); }
  
  bool   EvalSHL( bool, shift_type ) { throw std::logic_error( "No << for bool." ); }
  long double   EvalSHL( long double, shift_type ) { throw std::logic_error( "No << for long double." ); }
  double   EvalSHL( double, shift_type ) { throw std::logic_error( "No << for double." ); }
  float    EvalSHL( float, shift_type ) { throw std::logic_error( "No << for float." ); }
  
  long double   EvalSHR( long double, shift_type ) { throw std::logic_error( "No >> for long double." ); }
  double   EvalSHR( double, shift_type ) { throw std::logic_error( "No >> for double." ); }
  float    EvalSHR( float, shift_type ) { throw std::logic_error( "No >> for float." ); }
  
  uint32_t EvalByteSwap( uint32_t v ) { return unisim::util::endian::ByteSwap( v ); }
  uint16_t EvalByteSwap( uint16_t v ) { return unisim::util::endian::ByteSwap( v ); }
  
  uint32_t EvalBitScanReverse( uint32_t v ) { return unisim::util::arithmetic::BitScanReverse( v ); }

  uint32_t EvalBitScanForward( uint32_t v ) { return unisim::util::arithmetic::BitScanForward( v ); }
  
  uint32_t EvalPopCount( uint32_t v ) { return unisim::util::arithmetic::PopCount( v ); }
  uint64_t EvalPopCount( uint64_t v ) { return unisim::util::arithmetic::PopCount( v ); }
  
  uint32_t EvalRotateRight( uint32_t v, shift_type s ) { return unisim::util::arithmetic::RotateRight( v, s ); }
  uint64_t EvalRotateRight( uint64_t v, shift_type s ) { return unisim::util::arithmetic::RotateRight( v, s ); }

  uint32_t EvalRotateLeft( uint32_t v, shift_type s ) { return unisim::util::arithmetic::RotateLeft( v, s ); }

} /* end of namespace symbolic */
} /* end of namespace util */
} /* end of namespace unisim */

