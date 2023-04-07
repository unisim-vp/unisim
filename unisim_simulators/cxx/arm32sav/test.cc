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
#include <unisim/component/cxx/processor/arm/isa/disasm.hh>
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
  {
    struct GetAddr : public unisim::util::symbolic::Evaluator
    {
      using ConstNodeBase = unisim::util::symbolic::ConstNodeBase;

      ConstNodeBase const* Simplify( unsigned, Expr& expr ) const override
      {
        if (auto reg = dynamic_cast<Scanner::GRegRead const*>(expr.node))
          return new unisim::util::symbolic::ConstNode<uint32_t>( uint64_t(reg->reg) << 28 );
        if (dynamic_cast<Scanner::Flag const*>(expr.node))
          Scanner::dont("flag-relative addressing");
        if (dynamic_cast<Scanner::PC const*>(expr.node))
          Scanner::dont("pc-relative addressing");
        if (ConstNodeBase const* res = expr.Simplify(*this))
          return res;
        throw *this;
        return 0;
      }
    } evaluator;

    Expr scratch = addr;
    zaddr = dynamic_cast<unisim::util::symbolic::ConstNode<uint32_t> const&>(*evaluator.Simplify(0, scratch)).value;
  }

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
  
  
  return unisim::util::sav::Comparator().process( *a.behavior, *b.behavior ) < 0;
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
