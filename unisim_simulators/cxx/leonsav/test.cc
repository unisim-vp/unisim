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

#include <test.hh>
#include <scanner.hh>
#include <unisim/component/cxx/processor/sparc/isa/sv8/disasm.hh>
#include <sstream>

Interface::Interface( Operation const& op, uint32_t code, std::string const& disasm )
  : memcode(code)
  , asmcode(disasm)
  , gilname(op.GetName())
  , gregs()
  , yaccess(false)
    //  , vregs()
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
      if (0x3cfc >> reg & 1)
        continue;
      if (gregs.accessed(reg))
        {
          std::ostringstream buf;
          buf << "reserved register access (" << unisim::component::cxx::processor::sparc::isa::sv8::DisasmGPR(reg) << ")";
          throw unisim::util::sav::Untestable(buf.str());
        }
    }
    
  if (addrs.size())
    {
      if (uint32_t(*addrs.rbegin() - *addrs.begin()) > 1024)
        throw unisim::util::sav::Untestable("spread out memory accesses");

      struct ExpectedAddress : public unisim::util::symbolic::ExprNode
      {
        ExpectedAddress() : unisim::util::symbolic::ExprNode() {}
        virtual ExpectedAddress* Mutate() const override { return new ExpectedAddress( *this ); }
        virtual int cmp(ExprNode const& rhs) const override { return 0; }
        virtual unsigned SubCount() const override { return 0; }
        virtual void Repr( std::ostream& sink ) const override { sink << "ExpectedAddress()"; }
        typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
        typedef unisim::util::symbolic::ValueType ValueType;
        virtual ValueType const* GetType() const override { return unisim::util::symbolic::CValueType(uint32_t()); }
        virtual ConstNodeBase const* Eval( unisim::util::symbolic::EvalSpace const& evs, ConstNodeBase const** ) const override
        {
          if (auto l = dynamic_cast<Scanner::RelocEval const*>( &evs ))
            return new unisim::util::symbolic::ConstNode<uint32_t>( l->address );
          return 0;
        };
      };
      
      if (not addressings.solve(base_addr, new ExpectedAddress()))
        throw unisim::util::sav::Untestable("malformed address");
    }

  behavior->simplify();
}
  
void
Interface::gencode(Text& text) const
{
  struct DelayText
  {
    void write( uint32_t _insn ) { if (last) text.write(last); last = _insn; }
    DelayText(Text& _text) : text(_text), last(0), offset(0) {} Text& text; uint32_t last; uint32_t offset;
    void ld(int rt) { write(0xc0002000 | rt << 25 | 8 << 14 | offset); offset += 4; } // ld [%o0 + <offset>], %r
    void st(int rt) { write(0xc0202000 | rt << 25 | 8 << 14 | offset); offset += 4; } // st %r, [%o0 + <offset>]
  } dtext(text);
  /* Setup GP register map */
  unsigned const gcount = gregs.used();
  unsigned grmap[gcount];
  for (unsigned reg = 0; reg < gregs.count(); ++reg)
    {
      if (not gregs.accessed(reg)) continue;
      grmap[gregs.index(reg)] = reg;
    }
  // /* Setup Neon register map */
  // unsigned const vcount = vregs.used();
  // unsigned vrmap[vcount];
  // for (unsigned reg = 0; reg < vregs.count(); ++reg)
  //   {
  //     if (not vregs.accessed(reg)) continue;
  //     vrmap[vregs.index(reg)] = reg;
  //   }
  
  /* Load GP registers */
  unsigned offset = 0;
  for (unsigned idx = 0; idx < gcount; ++idx, offset += 4)
    dtext.ld( grmap[idx] );
  /* Load NZVC register */
  dtext.ld( 1 ); // %g1
  // dtext.write( 0x01003c00 |  9 << 25 ); // sethi  %hi(0xf00000), %o1
  // dtext.write( 0x80080000 |  1 << 25 | 9 << 14 | 1 << 0 ); // and %o1, %g1, %g1
  dtext.write( 0x80902000 |  0 << 25 |  0 << 14 | 1 << 0 ); // orcc %g0, 1, %g0
  dtext.write( 0x81480000 | 9 << 25 ); // rd %psr, %o1
  dtext.write( 0x81880000 | 9 << 14 |  1 <<  0 ); // wr %o1, %g1, %psr
  /* Load Y register if needed */
  if (yaccess)
    {
      dtext.ld( 1 ); // %g1
      dtext.write( 0x81802000 | 1 << 14 ); // wr %g1, 0, %y
    }
  /* Execute tested instruction */
  dtext.write(memcode);
  /* Store GP registers */
  for (unsigned idx = 0; idx < gcount; ++idx, offset += 4)
    dtext.st( grmap[idx] );
  /* Store NZCV register */
  dtext.write( 0x81480000 | 1 << 25 ); // rd %psr, %g1
  dtext.st( 1 );
  /* Store Y if needed */
  if (yaccess)
    {
      dtext.write( 0x81400000 | 1 << 25 ); // rd %y, %g1
      dtext.st( 1 );
    }
  // Return and flush delay slot
  text.write( 0x81c3e008 );
  dtext.write(0);
}

uintptr_t
Interface::workcells() const
{
  return 0
    + 1 /*NZVC*/
    + gregs.used()
    + yaccess;
}
    
void
Interface::memaccess( unisim::util::symbolic::Expr const& addr, bool is_write )
{
  uint32_t zaddr;
  if (auto z = addr.Eval( Scanner::AddrEval() ))
    { Expr dispose(z); zaddr = dynamic_cast<unisim::util::symbolic::ConstNode<uint32_t> const&>(*z).value; }
  else
    throw "WTF";
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
          { sink << unisim::component::cxx::processor::sparc::isa::sv8::DisasmGPR(reg); return; }
      throw Ouch();
    }
  idx -= gregs.used();
  if (idx-- == 0)
    { sink << "nzvc"; return; }
  if (yaccess and idx-- == 0)
    { sink << "%y"; return; }
  
  throw Ouch();
}


bool
TestLess::operator () ( Interface const& a, Interface const& b ) const
{
  if (int delta = a.gilname.compare( b.gilname ))
    return delta < 0;
  
  return unisim::util::sav::Comparator().process( *a.behavior, *b.behavior ) < 0;
}
