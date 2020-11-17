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
        typedef unisim::util::symbolic::ScalarType ScalarType;
        virtual ScalarType::id_t GetType() const override { return ScalarType::U32; }
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
  /* Setup GP register map (inputs+scratches) */
  unsigned const gcount = gregs.used()+1;
  unsigned grmap[gcount];
  grmap[gcount-1] = 1; // NZCV scratch
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
  
  /* Load GP registers and NZCV scratch (x1)*/
  unsigned offset = 0;
  for (unsigned idx = 0; idx < gcount; ++idx, offset += 4)
    text.write( 0xc0002000 | grmap[idx] << 25 | 8 << 14 | offset ); // ld [%o0 + <offset>], %r
  /* Load NZVC register */
  // text.write( 0x01003c00 |  9 << 25 ); // sethi  %hi(0xf00000), %o1
  // text.write( 0x80080000 |  1 << 25 | 9 << 14 | 1 << 0 ); // and %o1, %g1, %g1
  text.write( 0x80902000 |  0 << 25 |  0 << 14 | 1 << 0 ); // orcc %g0, 1, %g0
  text.write( 0x81480000 | 9 << 25 ); // rd %psr, %o1
  text.write( 0x81880000 | 9 << 14 |  1 <<  0 ); // wr %o1, %g1, %psr
  /* Execute tested instruction */
  text.write(memcode);
  /* Store NZCV register */
  text.write( 0x81480000 | 1 << 25 ); // rd %psr, %g1
  /* Store GP registers */
  // Delay code emission to reserve the delay slot
  unsigned retloc = gcount - 1;
  for (unsigned idx = 0; idx < gcount; ++idx, offset += 4)
    {
      if (idx == retloc)
        text.write( 0x81c3e008 ); // retl
      text.write( 0xc0202000 | grmap[idx] << 25 | 8 << 14 | offset ); // st %r, [%o0 + <offset>]
    }
}

uintptr_t
Interface::workcells() const
{
  return 1 + gregs.used();
}
    
void
Interface::memaccess( unisim::util::symbolic::Expr const& addr, bool is_write )
{
  uint32_t zaddr;
  if (auto z = addr.Eval( Scanner::AddrEval() ))
    { Expr dispose(z); zaddr = z->Get( uint32_t() ); }
  else
    throw "WTF";
  addrs.insert(zaddr);

  if (zaddr == *addrs.begin())
    base_addr = addr;
}

uintptr_t
Interface::icc_index() const
{
  return gregs.used();
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
  if (idx < 1)
    { sink << "nzvc"; return; }
  throw Ouch();
}


bool
TestLess::operator () ( Interface const& a, Interface const& b ) const
{
  if (int delta = a.gilname.compare( b.gilname ))
    return delta < 0;
  
  struct Comparator
  {
    int process( unisim::util::sav::ActionNode const& a, unisim::util::sav::ActionNode const& b ) const
    {
      if (int delta = a.updates.size() - b.updates.size()) return delta;
      auto rci = b.updates.begin();
      for (Expr const& update : a.updates)
        { if (int delta = process( update,  *rci )) return delta; ++rci; }
          
      if (int delta = process( a.cond, b.cond )) return delta;
      for (int idx = 0; idx < 2; ++idx)
        {
          if     (not a.nexts[idx])
            { if (b.nexts[idx]) return -1; }
          else if(b.nexts[idx])
            { if (int delta = process( *a.nexts[idx], *b.nexts[idx] )) return delta; }
        }
      return 0;
    }

    typedef unisim::util::symbolic::Expr Expr;

    int process( Expr const& a, Expr const& b ) const
    {
      // Do not compare null expressions
      if (not b.node) return a.node ?  1 : 0;
      if (not a.node) return b.node ? -1 : 0;
      
      /* First compare actual types */
      const std::type_info* til = &typeid(*a.node);
      const std::type_info* tir = &typeid(*b.node);
      if (til < tir) return -1;
      if (til > tir) return +1;
        
      /* Same types, call derived comparator except for Constants (compare popcount)*/
      typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
      if (auto an = dynamic_cast<ConstNodeBase const*>(a.node))
        {
          // return 0; /* XXX: temporarily considering all constants equivalent */
          uint32_t av = an->Get(uint32_t()), bv = dynamic_cast<ConstNodeBase const&>(*b.node).Get( uint32_t() );
          if (int delta = __builtin_popcountll(av) - __builtin_popcountll(bv))
            return delta;
        }
      else if (auto vr = dynamic_cast<unisim::util::sav::VirtualRegister const*>(a.node))
        {
          unsigned ai = vr->idx, bi = dynamic_cast<unisim::util::sav::VirtualRegister const&>(*b.node).idx;
          if (int delta = int(ai) - int(bi))
            return delta;
        }
      else if (int delta = a.node->cmp( *b.node ))
        return delta;

      /* Compare sub operands recursively */
      unsigned subcount = a.node->SubCount();
      if (int delta = int(subcount) - int(b.node->SubCount()))
        return delta;
      for (unsigned idx = 0; idx < subcount; ++idx)
        if (int delta = process( a.node->GetSub(idx), b.node->GetSub(idx)))
          return delta;

      /* equal to us*/
      return 0;
    }
  } comparator;

  return comparator.process( *a.behavior, *b.behavior ) < 0;
}
