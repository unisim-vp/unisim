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
  , flags()
  , current_insn_addr()
  , next_insn_addr(newRegRead(Scanner::PC()))
{
  for (Flag reg; reg.next();)
    flags[reg.idx()] = BOOL(newRegRead( reg ));
  // General & Vector registers on-demand allocation (see *regtouch)
  //gpr[31] = newRegRead(SP());
}

Scanner::~Scanner()
{
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
  for (unsigned reg = 0; reg < VECTORCOUNT; ++reg)
    if (interface.vregs.modified(reg))
      path->add_update( new VRegWrite( reg, interface.vregs.index(reg), vector_views[reg].GetConstStorage( &vectors[reg], NeonRegister(), VUConfig::BYTECOUNT )->expr ) );

  // Flags
  for (Flag reg; reg.next();)
    if (flags[reg.idx()].expr != ref.flags[reg.idx()].expr)
      path->add_update( newRegWrite( reg, flags[reg.idx()].expr ) );

  for (Expr const& store : stores)
    path->add_update( store );
    
  return complete;
}
  
void
Scanner::report( AccessReport acc, unsigned reg, bool is_write )
{
  if (acc == report_simd_access)
    {
      unsigned idx = interface.vregs.touch(reg, is_write);
      VectorView& vv = vector_views[reg];
      if (vv.transfer == vv.initial)
        {
          NeonRegister v( new VRegRead( reg, idx ) );
          *(vv.GetStorage( &vectors[reg], v, VUConfig::BYTECOUNT )) = v;
        }
      return;
    }

  // acc == report_gsr_access or acc == report_gzr_access
  if (reg == 31)
    {
      if (acc == report_gsr_access)
        dont("SP access");
      return;
    }

  unsigned idx = interface.gregs.touch(reg,is_write);
  if (not gpr[reg].expr.node)
    gpr[reg].expr = new GRegRead( reg, idx );
}

bool
Scanner::concretize(unisim::util::symbolic::Expr cond)
{
  if (unisim::util::symbolic::ConstNodeBase const* cnode = cond.ConstSimplify())
    return dynamic_cast<unisim::util::symbolic::ConstNode<bool> const&>(*cnode).value;

  bool predicate = path->proceed( cond );
  path = path->next( predicate );
  return predicate;
}

namespace
{
  struct OpTVU8
  {
    OpTVU8(int _reg0, int _elements, int _regs) : reg0(_reg0), elements(_elements), regs(_regs) {} int reg0, elements, regs;
    friend std::ostream& operator << (std::ostream& sink, OpTVU8 const& op)
    { return (sink << "tvu8[" << op.reg0 << ", " << op.elements << ", " << op.regs << "]"); }
    friend int strcmp(OpTVU8 const& a, OpTVU8 const& b)
    {
      if (int delta = a.reg0 - b.reg0) return delta;
      if (int delta = a.elements - b.elements) return delta;
      return a.regs - b.regs;
    }
  };
}

Scanner::U8
Scanner::GetTVU8(unsigned reg0, unsigned elements, unsigned regs, Scanner::U8 const& index, Scanner::U8 const& oob_value)
{
  // All regs from input list are potentially accessed
  for (unsigned reg = 0; reg < regs; ++reg)
    for (unsigned idx = 0; idx < elements; ++idx)
      GetVU8((reg0+reg)%32,idx);

  return unisim::util::sav::make_weirdop<U8>(OpTVU8(reg0, elements, regs), index, oob_value);
}
