/*
 *  Copyright (c) 2019-2020,
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

#include <scanner.hh>
#include <sstream>
#include <fstream>

Scanner::Scanner( Interface& iif )
  // , next_insn_addr(), next_insn_mode(ipjmp)
  // , ftop(0)
  : interface(iif)
  , path(iif.behavior.get())
  , stores()
  , gpr()
  , flags()
  , current_insn_addr()
  , next_insn_addr(newRegRead(Scanner::PC()))
  , vector_views()
  , vector_data()
{
  for (Flag reg; reg.next();)
    flags[reg.idx()] = BOOL(newRegRead( reg ));
  // General & Vector registers on-demand allocation (see *regtouch)
  gpr[31] = newRegRead(SP());
}

bool
Scanner::close( Scanner const& ref )
{
  bool complete = path->close();

  // Scalar integer registers
  for (unsigned reg = 0; reg < GREGCOUNT; ++reg)
    if (gpr[reg].expr != ref.gpr[reg].expr)
      path->add_update( new GRegWrite( reg, interface.gregs.index(reg), gpr[reg].expr ) );

  // Vector Registers
  for (unsigned reg = 0; reg < VREGCOUNT; ++reg)
    if (interface.vregs.modified(reg))
      path->add_update( new VRegWrite( reg, interface.vregs.index(reg), vector_views[reg].GetConstStorage( &vector_data[reg][0], NeonRegister(), VUConfig::BYTECOUNT )->expr ) );

  // Flags
  for (Flag reg; reg.next();)
    if (flags[reg.idx()].expr != ref.flags[reg.idx()].expr)
      path->add_update( newRegWrite( reg, flags[reg.idx()].expr ) );

  for (Expr const& store : stores)
    path->add_update( store );
    
  return complete;
}
  
void
Scanner::gregtouch( unsigned reg, bool write )
{
  unsigned idx = interface.gregs.touch(reg,write);
  if (not gpr[reg].expr.node)
    gpr[reg].expr = new GRegRead( reg, idx );
}

bool
Scanner::concretize(unisim::util::symbolic::Expr cond)
{
  if (unisim::util::symbolic::ConstNodeBase const* cnode = cond.ConstSimplify())
    return cnode->Get( bool() );

  bool predicate = path->proceed( cond );
  path = path->next( predicate );
  return predicate;
}


// struct AddrLess
// {
//   typedef unisim::util::symbolic::Expr Expr;

//   bool operator () ( Expr const& a, Expr const& b ) const
//   {
//     struct Comparator
//     {
//       struct VI
//       {
//         std::map<int,int> dic;
//         int operator [] (int x)
//         {
//           auto itr = dic.lower_bound(x);
//           if (itr == dic.end() or itr->first > x)
//             itr = dic.insert( itr, std::map<int,int>::value_type(x, int(dic.size())) );
//           return itr->second;
//         }
//       } avi, bvi;
    
//       int process( Expr const& a, Expr const& b )
//       {
//         // Do not compare null expressions
//         if (not b.node) return a.node ?  1 : 0;
//         if (not a.node) return b.node ? -1 : 0;
          
//         /* First compare actual types */
//         const std::type_info* til = &typeid(*a.node);
//         const std::type_info* tir = &typeid(*b.node);
//         if (til < tir) return -1;
//         if (til > tir) return +1;
          
//         if (dynamic_cast<unisim::util::symbolic::ConstNodeBase const*>(a.node))
//           return 0;
          
//         if (auto vr = dynamic_cast<unisim::util::sav::VirtualRegister const*>(a.node))
//           {
//             if (int delta = avi[vr->idx] - bvi[dynamic_cast<unisim::util::sav::VirtualRegister const&>(*b.node).idx])
//               return delta;
//           }
//        else if (int delta = a.node->cmp( *b.node ))
//           return delta;

//         /* Compare sub operands recursively */
//         unsigned subcount = a.node->SubCount();
//         if (int delta = int(subcount) - int(b.node->SubCount()))
//           return delta;
//         for (unsigned idx = 0; idx < subcount; ++idx)
//           if (int delta = process( a.node->GetSub(idx), b.node->GetSub(idx)))
//             return delta;

//         /* equal to us*/
//         return 0;
//       }
//     } comparator;
      
//     return comparator.process( a, b ) < 0;
//   }
// };

