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

Scanner::Scanner( Interface& iif )
  : interface(iif)
{
}

Scanner::~Scanner()
{
}

// bool
// Scanner::close( Scanner const& ref )
// {
//   bool complete = path->close();

//   // Scalar integer registers
//   for (unsigned reg = 0; reg < GREGCOUNT; ++reg)
//     if (gpr[reg].expr != ref.gpr[reg].expr)
//       path->add_update( new GRegWrite( reg, interface.gregs.index(reg), gpr[reg].expr ) );

//   // Vector Registers
//   for (unsigned reg = 0; reg < VREGCOUNT; ++reg)
//     if (interface.vregs.modified(reg))
//       path->add_update( new VRegWrite( reg, interface.vregs.index(reg), vector_views[reg].GetConstStorage( &vector_data[reg][0], NeonRegister(), VUConfig::BYTECOUNT )->expr ) );

//   // Flags
//   for (Flag reg; reg.next();)
//     if (flags[reg.idx()].expr != ref.flags[reg.idx()].expr)
//       path->add_update( newRegWrite( reg, flags[reg.idx()].expr ) );

//   for (Expr const& store : stores)
//     path->add_update( store );
    
//   return complete;
// }
  
void
Scanner::gregtouch( unsigned reg, bool write )
{
  unsigned idx = interface.gregs.touch(reg,write);
  if (not gprs[reg].expr.node)
    gprs[reg].expr = new GRegRead( reg, idx );
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
