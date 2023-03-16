/*
 *  Copyright (c) 2009-2023,
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

#include <unisim/util/symbolic/binsec/binsec.hh>
#include <unisim/util/symbolic/symbolic.hh>
// #include <ppc64dec.hh>
// #include <iostream>
// #include <iomanip>
// #include <cstdlib>
// #include <set>
#include <inttypes.h>

namespace ppc64 {

struct Arch
{
  //   =====================================================================
  //   =                             Data Types                            =
  //   =====================================================================
    
  typedef unisim::util::symbolic::SmartValue<double>   F64;
  typedef unisim::util::symbolic::SmartValue<float>    F32;
  typedef unisim::util::symbolic::SmartValue<bool>     BOOL;
  typedef unisim::util::symbolic::SmartValue<uint8_t>  U8;
  typedef unisim::util::symbolic::SmartValue<uint16_t> U16;
  typedef unisim::util::symbolic::SmartValue<uint32_t> U32;
  typedef unisim::util::symbolic::SmartValue<uint64_t> U64;
  typedef unisim::util::symbolic::SmartValue<int8_t>   S8;
  typedef unisim::util::symbolic::SmartValue<int16_t>  S16;
  typedef unisim::util::symbolic::SmartValue<int32_t>  S32;
  typedef unisim::util::symbolic::SmartValue<int64_t>  S64;
    
  typedef unisim::util::symbolic::FP                   FP;
  typedef unisim::util::symbolic::Expr                 Expr;
  typedef unisim::util::symbolic::ValueType            ValueType;
  typedef unisim::util::symbolic::binsec::ActionNode   ActionNode;


  Arch(U64 const& insn_addr);
  
  bool  close( Arch const& ref, uint64_t linear_nia );

  // bool concretize( Expr cexp )
  // {
  //   if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
  //     return dynamic_cast<unisim::util::symbolic::ConstNode<bool> const&>(*cnode).value;

  //   bool predicate = path->proceed( cexp );
  //   path = path->next( predicate );
  //   return predicate;
  // }
  
  // template <typename T>
  // bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
  // {
  //   if (not cond.expr.good())
  //     throw std::logic_error( "Not a valid condition" );

  //   return concretize( BOOL(cond).expr );
  // }
  
  // template <typename T>
  // bool Cond( unisim::util::symbolic::SmartValue<T> const& cond )
  // {
  //   if (not cond.expr.good())
  //     throw std::logic_error( "Not a valid condition" );
      
  //   Expr cexp( BOOL(cond).expr );
  //   if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
  //     return cnode->Get( bool() );
      
  //   bool predicate = path->proceed( cexp );
  //   path = path->next( predicate );
  //   return predicate;
  // }

  //   =====================================================================
  //   =                      Control Transfer methods                     =
  //   =====================================================================
  
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
  // void BranchTo(U64 const& npc, branch_type_t bt)
  // {
  //   next_instruction_address = npc;
  //   branch_type = bt;
  // }

  ActionNode*      path;
  U64              current_instruction_address;
  U64              next_instruction_address;
  branch_type_t    branch_type;
};

} // end of namespace ppc64

