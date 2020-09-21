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

#include <unisim/component/cxx/processor/arm/isa/arm64/disasm.hh>
#include <arch.hh>
#include <sstream>
#include <fstream>

namespace review
{
  namespace {
    struct UpdatesMerger
    {
      typedef unisim::util::symbolic::Expr Expr;
      void operator () ( std::set<Expr>& updates, Expr const& l, Expr const& r ) { updates.insert( l ); }
    };
  }
  
  void
  ActionNode::simplify()
  {
    // {
    //   std::set<Expr> nupdates;
    //   for (std::set<Expr>::const_iterator itr = updates.begin(), end = updates.end(); itr != end; ++itr)
    //     nupdates.insert( ASExprNode::Simplify( *itr ) );
    //   std::swap(nupdates, updates);
    // }
    
    if (not cond.good())
      return;

    //    cond = ASExprNode::Simplify( cond );
    
    for (unsigned choice = 0; choice < 2; ++choice)
      if (ActionNode* next = nexts[choice])
        next->simplify();
    
    factorize( updates, nexts[0]->updates, nexts[1]->updates, UpdatesMerger() );
    
    bool leaf = true;
    for (unsigned choice = 0; choice < 2; ++choice)
      if (ActionNode* next = nexts[choice])
        {
          if (next->cond.good() or next->updates.size()) leaf = false;
          else { delete next; nexts[choice] = 0; }
        }
    
    if (leaf)
      cond = Expr();
    else if (unisim::util::symbolic::OpNodeBase const* onb = cond->AsOpNode())
      if (onb->op.code == onb->op.Not)
        {
          // If condition begins with a logical not, remove the not and
          //   swap if then else branches
          cond = onb->GetSub(0);
          std::swap( nexts[false], nexts[true] );
        }
      
  }

  Arch::Arch( Interface& iif )
    // , next_insn_addr(), next_insn_mode(ipjmp)
    // , ftop(0)
    : interface(iif)
    , path(iif.behavior.get())
    , stores()
    , gpr()
    , flags()
    , current_insn_addr()
    , next_insn_addr(newRegRead(Arch::PC()))
    , vector_views()
    , vector_data()
  {
    for (Flag reg; reg.next();)
      flags[reg.idx()] = BOOL(newRegRead( reg ));
    // General & Vector registers on-demand allocation (see *regtouch)
    gpr[31] = newRegRead(SP());
  }

  bool
  Arch::close( Arch const& ref )
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
  Arch::gregtouch( unsigned reg, bool write )
  {
    unsigned idx = interface.gregs.touch(reg,write);
    if (not gpr[reg].expr.node)
      gpr[reg].expr = new GRegRead( reg, idx );
  }

  bool
  Arch::concretize(unisim::util::symbolic::Expr cond)
  {
    if (unisim::util::symbolic::ConstNodeBase const* cnode = cond.ConstSimplify())
      return cnode->Get( bool() );

    bool predicate = path->proceed( cond );
    path = path->next( predicate );
    return predicate;
  }

  namespace
  {
    struct ExpectedAddress : public unisim::util::symbolic::ExprNode
    {
      ExpectedAddress() : unisim::util::symbolic::ExprNode() {}
      virtual ExpectedAddress* Mutate() const override { return new ExpectedAddress( *this ); }
      virtual int cmp(ExprNode const& rhs) const override { return 0; }
      virtual unsigned SubCount() const override { return 0; }
      virtual void Repr( std::ostream& sink ) const override { sink << "ExpectedAddress()"; }
      typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
      virtual ConstNodeBase const* Eval( unisim::util::symbolic::EvalSpace const& evs, ConstNodeBase const** ) const override
      {
        if (auto l = dynamic_cast<Arch::RelocEval const*>( &evs ))
          return new unisim::util::symbolic::ConstNode<uint64_t>( l->address );
        return 0;
      };
      typedef unisim::util::symbolic::ScalarType ScalarType;
      virtual ScalarType::id_t GetType() const override { return ScalarType::U64; }
    };
  }


  Interface::Interface( Operation const& op, uint32_t code, std::string const& disasm )
    : memcode(code)
    , asmcode(disasm)
    , gregs()
    , vregs()
    , behavior(std::make_shared<ActionNode>())
    , base_addr()
    , relocs()
    , has_write(false)
  {
    // Performing an abstract execution to check the validity of
    // the opcode, and to compute the interface of the operation
    review::Arch reference( *this );

    for (bool end = false; not end;)
      {
        review::Arch arch( *this );
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
            throw review::Untestable(buf.str());
          }
      }
    
    if (addrs.size())
      {
        uint64_t span = (*addrs.rbegin() - *addrs.begin());
        if (span > uint64_t(1024))
          {
            //std::cerr << "[SOMA] span: " << std::hex << span << std::dec << "; " << disasm << std::endl;
            throw review::Untestable("spread out memory accesses");
          }
        typedef decltype(this->relocs) Relocs;
        
        struct AG
        {
          typedef unisim::util::symbolic::Expr Expr;

          void check(Expr const& front)
          {
            if (auto vr = dynamic_cast<Arch::VirtualRegister const*>(front.node))
              {
                /* Permanently invalidate relocation */
                relocs[vr->reg] = Expr();
                return;
              }
            for (unsigned idx = 0, end = front->SubCount(); idx < end; ++idx)
              check( front->GetSub(idx) );
          }
      
          void process (Expr const& front, Expr const& back)
          {
            if (auto vr = dynamic_cast<Arch::VirtualRegister const*>(front.node))
              {
                unsigned target = vr->reg;
                auto itr = relocs.lower_bound(target);
                if (itr == relocs.end() or itr->first > target)
                  relocs.emplace_hint(itr, Relocs::value_type( target, back ));
                else
                  itr->second = Expr();
              }
            else if (unisim::util::symbolic::OpNodeBase const* onb = front->AsOpNode())
              {
                if      (onb->op.code == onb->op.Add)
                  {
                    process( front->GetSub(0), make_operation( "Sub", back, front->GetSub(1) ) );
                    process( front->GetSub(1), make_operation( "Sub", back, front->GetSub(0) ) );
                  }
                else if (onb->op.code == onb->op.Sub)
                  {
                    process( front->GetSub(0), make_operation( "Add", back, front->GetSub(1) ) );
                    process( front->GetSub(1), make_operation( "Sub", front->GetSub(0), back ) );
                  }
                else
                  check( front );
              }
            else
              check( front );
          }
          
          AG(Relocs& _relocs) : relocs(_relocs) {} Relocs& relocs;
        } ag(relocs);
        
        ag.process( base_addr, new ExpectedAddress() );
        
        // Remove the invalid relocations
        for (Relocs::iterator itr = relocs.begin(), end = relocs.end(); itr != end;)
          {
            if (itr->second.good()) ++itr; else itr = relocs.erase(itr);
          }
        
        if (relocs.size() == 0)
          {
            throw review::Untestable("malformed address");
          }
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
      unsigned idx = gcount;
      if (idx & 1)
        { idx -= 1; text.write( 0xf8400400 | 1 << 12 | 0 << 5 | grmap[idx] << 0 ); } // LDR <Xt>, [X0], 8
      while (idx >= 2)
        { idx -= 2; text.write( 0xa8c00000 | 2 << 15 | grmap[idx+1] << 10 | 0 << 5 | grmap[idx] << 0 ); } // LDP <Xt1>, <Xt2>, [X0], 16
    }
    /* Load NZCV register */
    text.write( 0xd51b4201 ); // msr NZCV, x1
    /* Load Neon registers */
    {
      unsigned idx = vcount;
      if (idx & 1)              
        { idx -= 1; text.write( 0x3cc00400 | 16 << 12 | 0 << 5 | grmap[idx] << 0 ); } // LDR <Qt>, [X0], 16
      while (idx >= 2)
        { idx -= 2; text.write( 0xacc00000 | 2 << 15 | vrmap[idx+1] << 10 | 0 << 5 | vrmap[idx] << 0 ); } // LDP <Qt1>, <Qt2>, [X0], 32
    }
    /* Execute tested instruction */
    text.write(memcode);
    /* Store NZCV register*/
    text.write( 0xd53b4201 ); // mrs NZCV, x1
    /* Store GP registers */
    {
      unsigned idx = gcount;
      if (idx & 1)
        { idx -= 1; text.write( 0xf8000400 | 1 << 12 | 0 << 5 | grmap[idx] << 0 ); } // STR <Xt>, [X0], 8
      while (idx >= 2)
        { idx -= 2; text.write( 0xa8800000 | 2 << 15 | grmap[idx+1] << 10 | 0 << 5 | grmap[idx] << 0 ); } // STP <Xt1>, <Xt2>, [X0], 16
    }
    /* Store Neon registers */
    {
      unsigned idx = vcount;
      if (idx & 1)
        { idx -= 1; text.write( 0x3c800400 | 16 << 12 | 0 << 5 | vrmap[idx] << 0 ); } // STR <Qt>, [X0], 16
      while (idx >= 2)
        { idx -= 2; text.write( 0xac800000 | 2 << 15 | vrmap[idx+1] << 10 | 0 << 5 | vrmap[idx] << 0 ); } // STP <Qt1>, <Qt2>, [X0], 32
    }
    // Return
    text.write( 0xd65f03c0 ); // RET
   }

  uintptr_t
  Interface::workquads() const
  {
    uintptr_t const
      regwsize = 8*gregs.used(),
      vecwsize = Arch::VUConfig::BYTECOUNT*vregs.used();
    uintptr_t offset = 0;
    offset += 8;        // placeholder for ?
    offset += regwsize; // placeholder for reg values 
    offset += vecwsize; // placeholder for vec values 
    if (offset % 8) throw "WTF";
    return offset / 8;
  }
    
  struct AddrLess
  {
    typedef unisim::util::symbolic::Expr Expr;

    bool operator () ( Expr const& a, Expr const& b ) const
    {
      struct Comparator
      {
        struct VI
        {
          std::map<int,int> dic;
          int operator [] (int x)
          {
            auto itr = dic.lower_bound(x);
            if (itr == dic.end() or itr->first > x)
              itr = dic.insert( itr, std::map<int,int>::value_type(x, int(dic.size())) );
            return itr->second;
          }
        } avi, bvi;
    
        int process( Expr const& a, Expr const& b )
        {
          // Do not compare null expressions
          if (not b.node) return a.node ?  1 : 0;
          if (not a.node) return b.node ? -1 : 0;
          
          /* First compare actual types */
          const std::type_info* til = &typeid(*a.node);
          const std::type_info* tir = &typeid(*b.node);
          if (til < tir) return -1;
          if (til > tir) return +1;
          
          if (dynamic_cast<unisim::util::symbolic::ConstNodeBase const*>(a.node))
            return 0;
          
          if (auto vr = dynamic_cast<Arch::VirtualRegister const*>(a.node))
            {
              if (int delta = avi[vr->idx] - bvi[dynamic_cast<Arch::VirtualRegister const&>(*b.node).idx])
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
      
      return comparator.process( a, b ) < 0;
    }
  };

  void
  Interface::memaccess( unisim::util::symbolic::Expr const& addr, bool is_write )
  {
    has_write |= is_write;

    uint64_t zaddr;
    if (auto z = addr.Eval( Arch::AddrEval() ))
      { Expr dispose(z); zaddr = z->Get( uint64_t() ); }
    else
      throw "WTF";
    addrs.insert(zaddr);
    if (zaddr == *addrs.begin())
      base_addr = addr;
    
    static std::ofstream sink( "addresses.txt" );
    static std::set<unisim::util::symbolic::Expr,AddrLess> addrs;
    if (addrs.insert(addr).second)
      sink << addr << std::endl;
  }
}

