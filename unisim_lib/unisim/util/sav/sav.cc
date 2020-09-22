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

} /* end of namespace sav */
} /* end of namespace util */
} /* end of namespace unisim */

