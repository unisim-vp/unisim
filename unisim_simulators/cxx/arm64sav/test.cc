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

      struct ExpectedAddress : public unisim::util::symbolic::ExprNode
      {
        ExpectedAddress() : unisim::util::symbolic::ExprNode() {}
        virtual ExpectedAddress* Mutate() const override { return new ExpectedAddress( *this ); }
        virtual int cmp(ExprNode const& rhs) const override { return 0; }
        virtual unsigned SubCount() const override { return 0; }
        virtual void Repr( std::ostream& sink ) const override { sink << "ExpectedAddress()"; }
        typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
        typedef unisim::util::symbolic::ScalarType ScalarType;
        virtual ScalarType::id_t GetType() const override { return ScalarType::U64; }
        virtual ConstNodeBase const* Eval( unisim::util::symbolic::EvalSpace const& evs, ConstNodeBase const** ) const override
        {
          if (auto l = dynamic_cast<Scanner::RelocEval const*>( &evs ))
            return new unisim::util::symbolic::ConstNode<uint64_t>( l->address );
          return 0;
        };
      };
      
      if (not addressings.solve(base_addr, new ExpectedAddress()))
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
  if (auto z = addr.Eval( Scanner::AddrEval() ))
    { Expr dispose(z); zaddr = z->Get( uint64_t() ); }
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
          uint64_t av = an->Get(uint64_t()), bv = dynamic_cast<ConstNodeBase const&>(*b.node).Get( uint64_t() );
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
