/*
 *  Copyright (c) 2019-2023,
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

#include <test.hh>
#include <scanner.hh>
#include <unisim/component/cxx/processor/arm/isa/arm64/disasm.hh>
#include <sstream>

Interface::Interface( Operation const& op, uint32_t code, std::string const& disasm )
  : memcode(code)
  , asmcode(disasm)
  , gilname(op.GetName())
  , gregs()
  , vregs()
  , behavior(std::make_shared<unisim::util::sav::ActionNode>())
  , addrs()
  , base_addr()
  , addressings()
{
  // Performing an abstract execution to check the validity of
  // the opcode, and to compute the interface of the operation
  Scanner reference( *this );

  for (bool end = false; not end;)
    {
      Scanner arch( *this );
      arch.step( op );
      end = arch.close( reference );
    }

  for (unsigned reg = 0; reg < 32; ++reg)
    {
      if (0x0004fefc >> reg & 1)
        continue;
      if (gregs.accessed(reg))
        {
          std::ostringstream buf;
          buf << "reserved register access (" << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(reg) << ")";
          throw unisim::util::sav::Untestable(buf.str());
        }
    }
    
  if (addrs.size())
    {
      if (uint64_t(*addrs.rbegin() - *addrs.begin()) > 1024)
        throw unisim::util::sav::Untestable("spread out memory accesses");

      if (not addressings.solve(base_addr, new Scanner::ExpectedAddress()))
        throw unisim::util::sav::Untestable("malformed address");
    }

  behavior->simplify();
}
  
void Interface::gencode(Text& text) const
{
  /* Setup GP register map (inputs+scratches) */
  unsigned const gcount = gregs.used()+1;
  unsigned grmap[gcount];
  grmap[gcount-1] = 1; // NZCV scratch
  for (unsigned reg = 0; reg < gregs.count(); ++reg)
    {
      if (not gregs.accessed(reg)) continue;
      grmap[gregs.index(reg)] = reg;
    }
  /* Setup Neon register map */
  unsigned const vcount = vregs.used();
  unsigned vrmap[vcount];
  for (unsigned reg = 0; reg < vregs.count(); ++reg)
    {
      if (not vregs.accessed(reg)) continue;
      vrmap[vregs.index(reg)] = reg;
    }
    
  /* Load GP registers and NZCV scratch (x1)*/
  {
    unsigned idx = 0;
    if (gcount&1)
      text.write( 0xf8400400 | 8 << 12 | 0 << 5 | grmap[idx++] << 0 ); // LDR <Xt>, [X0], 8
    for (;idx < gcount; idx+=2)
      text.write( 0xa8c00000 | 2 << 15 | grmap[idx+1] << 10 | 0 << 5 | grmap[idx] << 0 ); // LDP <Xt1>, <Xt2>, [X0], 16
  }
  /* Load NZCV register */
  text.write( 0xd51b4201 ); // msr NZCV, x1
  /* Load Neon registers */
  {
    unsigned idx = 0;
    if (vcount&1)
      text.write( 0x3cc00400 | 16 << 12 | 0 << 5 | vrmap[idx++] << 0 ); // LDR <Qt>, [X0], 16
    for (;idx < vcount; idx+=2)
      text.write( 0xacc00000 | 2 << 15 | vrmap[idx+1] << 10 | 0 << 5 | vrmap[idx] << 0 ); // LDP <Qt1>, <Qt2>, [X0], 32
  }
  /* Execute tested instruction */
  text.write(memcode);
  /* Store NZCV register*/
  text.write( 0xd53b4201 ); // mrs NZCV, x1
  /* Store GP registers */
  {
    unsigned idx = 0;
    if (gcount&1)
      text.write( 0xf8000400 | 8 << 12 | 0 << 5 | grmap[idx++] << 0 ); // STR <Xt>, [X0], 8
    for (;idx < gcount; idx+=2)
      text.write( 0xa8800000 | 2 << 15 | grmap[idx+1] << 10 | 0 << 5 | grmap[idx] << 0 ); // STP <Xt1>, <Xt2>, [X0], 16
  }
  /* Store Neon registers */
  {
    unsigned idx = 0;
    if (vcount&1)
      text.write( 0x3c800400 | 16 << 12 | 0 << 5 | vrmap[idx++] << 0 ); // STR <Qt>, [X0], 16
    for (;idx < vcount; idx+=2)
      text.write( 0xac800000 | 2 << 15 | vrmap[idx+1] << 10 | 0 << 5 | vrmap[idx] << 0 ); // STP <Qt1>, <Qt2>, [X0], 32
  }
  // Return
  text.write( 0xd65f03c0 ); // RET
}

uintptr_t
Interface::workcells() const
{
  uintptr_t const
    regwsize = 8*gregs.used(),
    vecwsize = Scanner::VUConfig::BYTECOUNT*vregs.used();
  uintptr_t offset = 0;
  offset += 8;        // placeholder for NZCV
  offset += regwsize; // placeholder for reg values 
  offset += vecwsize; // placeholder for vec values 
  if (offset % 8) throw "WTF";
  return offset / 8;
}
    
void
Interface::memaccess( unisim::util::symbolic::Expr const& addr, bool is_write )
{
  uint64_t zaddr;
  {
    struct GetAddr : public unisim::util::symbolic::Evaluator
    {
      using ConstNodeBase = unisim::util::symbolic::ConstNodeBase;

      ConstNodeBase const* Simplify( unsigned, Expr& expr ) const override
      {
        if (auto reg = dynamic_cast<Scanner::GRegRead const*>(expr.node))
          return new unisim::util::symbolic::ConstNode<uint64_t>( uint64_t(reg->reg) << 60 );
        if (dynamic_cast<Scanner::PC const*>(expr.node))
          Scanner::dont("pc-relative addressing");
        if (dynamic_cast<Scanner::SP const*>(expr.node))
          Scanner::dont("sp-relative addressing");
        if (dynamic_cast<Scanner::Flag const*>(expr.node))
          Scanner::dont("flag-relative addressing");
        if (ConstNodeBase const* res = expr.Simplify(*this))
          return res;
        throw *this;
        return 0;
      }
    } evaluator;

    Expr scratch = addr;
    zaddr = dynamic_cast<unisim::util::symbolic::ConstNode<uint64_t> const&>(*evaluator.Simplify(0, scratch)).value;
  }
  addrs.insert(zaddr);

  if (zaddr == *addrs.begin())
    base_addr = addr;
}

void
Interface::field_name(unsigned idx, std::ostream& sink) const
{
  struct Ouch {};
  if (idx < gregs.used())
    {
      for (unsigned reg = 0; ; ++reg)
        if (gregs.accessed(reg) and gregs.index(reg) == idx)
          { sink << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(reg); return; }
      throw Ouch();
    }
  idx -= gregs.used();
  if (idx < 1)
    { sink << "nzcv"; return; }
  idx -= 1;
  if (idx < vregs.used()*2)
    {
      unsigned sub = idx & 1, vidx = idx >> 1;
      for (unsigned reg = 0; ; ++reg)
        if (vregs.accessed(reg) and vregs.index(reg) == vidx)
          { sink << unisim::component::cxx::processor::arm::isa::arm64::DisasmQ(reg) << (sub ? ".hi" : ".lo"); return; }
      throw Ouch();
    }
  throw Ouch();
}


bool
TestLess::operator () ( Interface const& a, Interface const& b ) const
{
  if (int delta = a.gilname.compare( b.gilname ))
    return delta < 0;
  
  return unisim::util::sav::Comparator().process( *a.behavior, *b.behavior ) < 0;
}
