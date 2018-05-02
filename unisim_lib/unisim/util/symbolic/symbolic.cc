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
  
  ActionNode::ActionNode( ActionNode* _previous )
    : cond(), sinks(), previous(_previous), true_nxt(), false_nxt(), complete(false)
  {}

  ActionNode::~ActionNode()
  {
    delete false_nxt;
    delete true_nxt;
  }

  bool
  ActionNode::proceed( Expr const& _cond )
  {
    if (not cond.good())
      {
        cond = _cond;
        false_nxt = new ActionNode(this);
        true_nxt = new ActionNode(this);
        return false;
      }
  
    if (cond != _cond)
      throw std::logic_error( "unexpected condition" );
  
    if (not false_nxt->complete)
      return false;
  
    if (true_nxt->complete)
      throw std::logic_error( "unexpected path" );
  
    return true;
  }

  bool
  ActionNode::close()
  {
    complete = true;
    if (not previous)
      return true;
    if (this == previous->true_nxt)
      return previous->close();
    return false;
  }

  bool
  ActionNode::remove_dead_paths()
  {
    if (cond.good()) {
      bool leaf = true;
      if (false_nxt)
        {
          if (false_nxt->remove_dead_paths())
            {
              delete false_nxt;
              false_nxt = 0;
            }
          else
            leaf = false;
        }
      if (true_nxt)
        {
          if (true_nxt->remove_dead_paths())
            {
              delete true_nxt;
              true_nxt = 0;
            }
          else
            leaf = false;
        }
      if (not leaf)
        return false;
      else
        cond = Expr();
    }
    // This is a leaf; if no local sinks, signal dead path to parent
    return sinks.size() == 0;
  }

  void
  ActionNode::factorize()
  {
    if (not cond.good())
      return;
  
    false_nxt->factorize();
    true_nxt->factorize();
  
    std::vector<Expr> isect;
    {
      isect.resize( std::max( false_nxt->sinks.size(), true_nxt->sinks.size() ) );
      std::vector<Expr>::iterator end =
        std::set_intersection( false_nxt->sinks.begin(), false_nxt->sinks.end(),
                               true_nxt->sinks.begin(), true_nxt->sinks.end(),
                               isect.begin() );
      isect.resize( end - isect.begin() );
    }
  
    for (std::vector<Expr>::const_iterator itr = isect.begin(), end = isect.end(); itr != end; ++itr) {
      sinks.insert( *itr );
      false_nxt->sinks.erase( *itr );
      true_nxt->sinks.erase( *itr );
    }
  
    // If condition begins with a logical not, remove the not and
    //   swap if then else branches
    using unisim::util::symbolic::OpNodeBase;
    if (OpNodeBase const* onb = cond->AsOpNode())
      if (onb->op.code == onb->op.Not) {
        cond = onb->GetSub(0);
        std::swap( false_nxt, true_nxt );
      }
  }
} /* end of namespace symbolic */
} /* end of namespace util */
} /* end of namespace unisim */

