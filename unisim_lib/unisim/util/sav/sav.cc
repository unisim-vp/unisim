/*
 *  Copyright (c) 2019-2020,
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
 
#include <unisim/util/sav/sav.hh>

namespace unisim {
namespace util {
namespace sav {
  namespace {
    struct UpdatesMerger
    {
      typedef unisim::util::symbolic::Expr Expr;
      void operator () ( std::set<Expr>& updates, Expr const& l, Expr const& r ) { updates.insert( l ); }
    };
  }

  void
  ActionNode::simplify()
  {
    // {
    //   std::set<Expr> nupdates;
    //   for (std::set<Expr>::const_iterator itr = updates.begin(), end = updates.end(); itr != end; ++itr)
    //     nupdates.insert( ASExprNode::Simplify( *itr ) );
    //   std::swap(nupdates, updates);
    // }
    
    if (not cond.good())
      return;

    //    cond = ASExprNode::Simplify( cond );
    
    for (unsigned choice = 0; choice < 2; ++choice)
      if (ActionNode* next = nexts[choice])
        next->simplify();
    
    factorize( updates, nexts[0]->updates, nexts[1]->updates, UpdatesMerger() );
    
    bool leaf = true;
    for (unsigned choice = 0; choice < 2; ++choice)
      if (ActionNode* next = nexts[choice])
        {
          if (next->cond.good() or next->updates.size()) leaf = false;
          else { delete next; nexts[choice] = 0; }
        }
    
    if (leaf)
      cond = Expr();
    else if (unisim::util::symbolic::OpNodeBase const* onb = cond->AsOpNode())
      if (onb->op.code == onb->op.Not)
        {
          // If condition begins with a logical not, remove the not and
          //   swap if then else branches
          cond = onb->GetSub(0);
          std::swap( nexts[false], nexts[true] );
        }
      
  }

  namespace
  {
    template <typename T>
    void HexStore( std::ostream& sink, T value )
    {
      unsigned const digits = 2*sizeof (T);
      char buf[1 + digits];
      buf[digits] = '\0';
      for (int idx = digits; --idx >= 0;)
        {
          buf[idx] = "0123456789abcdef"[value & 0xf];
          value >>= 4;
        }
      sink << &buf[0];
    }

    template <typename T>
    void HexLoad( T& value, char const*& buf )
    {
      unsigned const digits = 2*sizeof (T);
      T scratch = 0;
      char const* ptr = buf;
      for (char const* end = &buf[digits]; ptr < end; ++ptr)
        {
          char ch = *ptr;
          if      ('0' <= ch and ch <= '9') scratch = (scratch << 4) | T( ch - '0' +  0 );
          else if ('a' <= ch and ch <= 'f') scratch = (scratch << 4) | T( ch - 'a' + 10 );
          else if ('A' <= ch and ch <= 'F') scratch = (scratch << 4) | T( ch - 'A' + 10 );
          else break;
        }
      value = scratch;
      buf = ptr;
    }
  }

  TestbedBase::TestbedBase(char const* seed, uint8_t* elems, uintptr_t count)
  {
    for (unsigned idx = 0, end = 5; idx < end; ++idx)
      HexLoad( rng.State(idx), seed );
    for (unsigned idx = 0, end = count; idx < end; ++idx)
      HexLoad( elems[idx], seed );
    HexLoad( counter, seed );
  }

  void TestbedBase::serial( std::ostream& sink, uint8_t* elems, uintptr_t count ) const
  {
    for (unsigned idx = 0, end = 5; idx < end; ++idx)
      HexStore( sink, rng.State(idx) );
    for (unsigned idx = 0, end = count; idx < end; ++idx)
      HexStore( sink, elems[idx] );
    HexStore( sink, counter );
  }
  
  bool
  Addressings::solve( Expr const& base_addr, Expr const& expected_address )
  {
    struct AG
    {
      typedef unisim::util::symbolic::Expr Expr;

      void check(Expr const& front)
      {
        if (dynamic_cast<Source const*>(front.node))
          {
            /* Permanently invalidate solutions */
            solutions[front] = Expr();
            return;
          }
        for (unsigned idx = 0, end = front->SubCount(); idx < end; ++idx)
          check( front->GetSub(idx) );
      }
      
      void process (Expr const& front, Expr const& back)
      {
        if (dynamic_cast<Source const*>(front.node))
          {
            /* We found a potential source. Associate it with its equation. */
            auto itr = solutions.lower_bound(front);
            if (itr == solutions.end() or itr->first > front)
              solutions.emplace_hint(itr, Solutions::value_type( front, back ));
            else /* Register appears multiple time in address generation. No can do. */
              itr->second = Expr();
          }
        else if (unisim::util::symbolic::OpNodeBase const* onb = front->AsOpNode())
          {
            if      (onb->op.code == onb->op.Add)
              {
                process( front->GetSub(0), make_operation( "Sub", back, front->GetSub(1) ) );
                process( front->GetSub(1), make_operation( "Sub", back, front->GetSub(0) ) );
              }
            else if (onb->op.code == onb->op.Sub)
              {
                process( front->GetSub(0), make_operation( "Add", back, front->GetSub(1) ) );
                process( front->GetSub(1), make_operation( "Sub", front->GetSub(0), back ) );
              }
            else
              check( front );
          }
        else
          check( front );
      }
          
      AG(Solutions& _solutions) : solutions(_solutions) {} Solutions& solutions;
    } ag(solutions);
        
    ag.process( base_addr, expected_address );
        
    // Remove the invalid solutions
    for (Solutions::iterator itr = solutions.begin(), end = solutions.end(); itr != end;)
      {
        if (itr->second.good()) ++itr; else itr = solutions.erase(itr);
      }

    return solutions.size() != 0;
  }
  
} /* end of namespace sav */
} /* end of namespace util */
} /* end of namespace unisim */

