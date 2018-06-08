/*
 *  Copyright (c) 2007-2017,
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
#include <vector>
#include <algorithm>

namespace unisim {
namespace util {
namespace symbolic {

  std::ostream&
  operator << (std::ostream& sink, Expr const& expr )
  {
    if (expr.good())
      expr->Repr( sink );
    else
      sink << "*null*";
    
    return sink;
  }
  std::ostream& ConstNodeBase::warn() { return std::cerr; }
  
  double   EvalMod( double l, double r ) { throw std::logic_error( "No ^ for double." ); }
  float    EvalMod( float l, float r ) { throw std::logic_error( "No ^ for float." ); }
  
  double   EvalXor( double l, double r ) { throw std::logic_error( "No ^ for double." ); }
  float    EvalXor( float l, float r ) { throw std::logic_error( "No ^ for float." ); }
  
  double   EvalAnd( double l, double r ) { throw std::logic_error( "No & for double." ); }
  float    EvalAnd( float l, float r ) { throw std::logic_error( "No & for float." ); }
  
  double   EvalOr( double l, double r ) { throw std::logic_error( "No | for double." ); }
  float    EvalOr( float l, float r ) { throw std::logic_error( "No | for float." ); }
  
  double   EvalNot( double val ) { throw std::logic_error( "No ~ for double." ); }
  float    EvalNot( float val ) { throw std::logic_error( "No ~ for float." ); }
  
  double   EvalSHL( double, uint8_t ) { throw std::logic_error( "No << for double." ); }
  float    EvalSHL( float, uint8_t ) { throw std::logic_error( "No << for float." ); }
  
  double   EvalSHR( double, uint8_t ) { throw std::logic_error( "No >> for double." ); }
  float    EvalSHR( float, uint8_t ) { throw std::logic_error( "No >> for float." ); }
  
  uint32_t EvalByteSwap( uint32_t v ) { return unisim::util::endian::ByteSwap( v ); }
  uint16_t EvalByteSwap( uint16_t v ) { return unisim::util::endian::ByteSwap( v ); }
  
  uint32_t EvalBitScanReverse( uint32_t v ) { return unisim::util::arithmetic::BitScanReverse( v ); }

  uint32_t EvalBitScanForward( uint32_t v ) { return unisim::util::arithmetic::BitScanForward( v ); }
  
  uint32_t EvalRotateRight( uint32_t v, uint8_t s ) { return unisim::util::arithmetic::RotateRight( v, s ); }

  uint32_t EvalRotateLeft( uint32_t v, uint8_t s ) { return unisim::util::arithmetic::RotateLeft( v, s ); }
  
  void
  SEStats::count( Expr const& e )
  {
    struct SECounter
    {
      SECounter( std::map<Expr,unsigned>& _stats ) : stats(_stats) {} std::map<Expr,unsigned>& stats;
      
      void Recurse( Expr const& expr )
      {
        for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
          Process( expr->GetSub(idx) );
      }

      void Process( Expr const& expr )
      {
        if (not expr->SubCount())
          return;

        if (stats[expr]++)
          return;

        Recurse( expr );
      }
    } sec( *this );

    sec.Process( e );
  }
  
} /* end of namespace symbolic */
} /* end of namespace util */
} /* end of namespace unisim */

