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
#include <unisim/component/cxx/processor/arm/disasm.hh>
#include <algorithm>
#include <sstream>

Interface::Interface( std::string const& disasm )
  : asmcode(disasm)
  , gilname()
  , gregs()
  , grmap()
  , vregs()
  , behavior(std::make_shared<unisim::util::sav::ActionNode>())
  , memrange()
  , base_addr()
  , addressings()
  , insncode()
  , insnhalf(false)
  , itsensitive(false)
  , aligned(true)
{}

namespace
{
template <class INSN>
void init( Interface& self, INSN const& insn )
{
  self.gilname = insn.GetName();
  self.insnhalf = insn.GetLength() == 16;
  self.insncode = insn.GetEncoding();
  // Performing an abstract execution to check the validity of
  // the opcode, and to compute the interface of the operation
  Scanner reference( self );
    
  for (bool end = false; not end;)
    {
      Scanner arch( self );
      insn.execute(arch);
      end = arch.close( reference );
    }
  self.start();
}
}

Interface::Interface( unisim::component::cxx::processor::arm::isa::arm32::Operation<Scanner> const& insn, std::string const& disasm )
  : Interface(disasm)
{
  init(*this, insn);
}

Interface::Interface( unisim::component::cxx::processor::arm::isa::thumb::Operation<Scanner> const& insn, std::string const& disasm)
  : Interface(disasm)
{
  init(*this, insn);
}

void
Interface::start()
{
  // build sorted indexing
  for (unsigned reg = 0; reg < gregs.count(); ++reg)
    {
      if (not gregs.accessed(reg))
        continue;
      if (0xf003 >> reg & 1)
        {
          std::ostringstream buf;
          buf << "reserved register access (" << unisim::component::cxx::processor::arm::DisasmRegister(reg) << ")";
          throw unisim::util::sav::Untestable(buf.str());
        }
      grmap |= 1 << reg;
    }
    
  if (base_addr.good())
    {
      if (uint32_t(memspread()) >= 1024)
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

uintptr_t
Interface::workcells() const
{
  uint32_t size = 0;
  size += 4*gregs.used();
  size += 8*vregs.used();
  size += 4; // placeholder for nzcv
  size = std::max(size, memspread());
  if (size % 4) throw "WTF";
  return size / 4;
}
    
void
Interface::memaccess( unisim::util::symbolic::Expr const& addr, unsigned size, bool is_write, bool isaligned )
{
  aligned &= isaligned;
  uint32_t zaddr;
  if (auto z = addr.Eval( Scanner::AddrEval() ))
    { Expr dispose(z); zaddr = z->Get( uint32_t() ); }
  else
    throw "WTF";

  if (not base_addr.good() or int32_t(zaddr - memrange[1]) > 0)
    {
      memrange[1] = zaddr + size;
    }
  if (not base_addr.good() or int32_t(zaddr - memrange[0]) < 0)
    {
      memrange[0] = zaddr;
      base_addr = addr;
    }
}


bool
TestLess::operator () ( Interface const& a, Interface const& b ) const
{
  if (int delta = a.gilname.compare( b.gilname ))
    return delta < 0;
  if (int delta = int(a.itsensitive) - int(b.itsensitive))
    return delta < 0;
  if (int delta = int(a.aligned) - int(b.aligned))
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

uint32_t
Interface::vrmap() const
{
  uint32_t map = 0;
  for (unsigned reg = 0; reg < vregs.count(); ++reg)
    {
      if (not vregs.accessed(reg))
        continue;
      map |= 1 << reg;
    }
  return map;
}
  
unsigned Interface::Vitr::operator* () const { return __builtin_ctz(map); }
bool Interface::Vitr::operator != (Vitr const& rhs) const { return map != rhs.map; }
void Interface::Vitr::operator ++ () { map = map & (map -1); }
