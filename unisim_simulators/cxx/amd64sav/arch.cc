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

#include <arch.hh>
#include <sstream>
#include <fstream>

namespace unisim { namespace component { namespace cxx { namespace processor { namespace intel {
          
struct FIRoundOp
{
  FIRoundOp(int _rm) : rm(_rm) {} int rm;
  friend int strcmp( FIRoundOp const& a, FIRoundOp const&b ) { return a.rm - b.rm; }
  friend std::ostream& operator << (std::ostream& sink, FIRoundOp const& op) { return (sink << "firound." << op.rm); }
};
  
template <typename FPT> FPT firound( FPT const& src, int x87frnd_mode )
{
  //return FPT( unisim::util::symbolic::Expr( new ut::FIRound<typename FPT::value_type>( src.expr, x87frnd_mode ) ) );
  return review::make_weirdop<FPT>( FIRoundOp(x87frnd_mode), src );
}

} /* namespace unisim */ } /* namespace component */ } /* namespace cxx */ } /* namespace processor */ } /* namespace intel */

#include <unisim/component/cxx/processor/intel/isa/intel.tcc>
#include <arch.hh>

namespace review
{
  void
  Arch::fxam()
  {
    f64_t value = this->fread( 0 );
    u8_t fpclass = u8_t( review::make_weirdop<u8_t>( "fxam", value ) );

    flagwrite( FLAG::C0, review::make_weirdop<bit_t>( "fxam.C0", fpclass ) );
    flagwrite( FLAG::C1, review::make_weirdop<bit_t>( "fxam.sign", value ) );
    flagwrite( FLAG::C2, review::make_weirdop<bit_t>( "fxam.C2", fpclass ) );
    flagwrite( FLAG::C3, review::make_weirdop<bit_t>( "fxam.C3", fpclass ) );
  }
  
  void
  Arch::FTop::Repr( std::ostream& sink ) const
  {
    sink << "FpuStackTop";
  }

  void
  Arch::vmm_touch( unsigned reg, bool write )
  {
    unsigned idx = interface.vregs.touch(reg,write);
    if (umms[reg].transfer == umms[reg].initial)
      {
        VmmRegister v( new VRegRead( reg, idx ) );
        *(umms[reg].GetStorage( &vmm_storage[reg][0], v, VUConfig::BYTECOUNT )) = v;
      }
  }

  bool
  Arch::vmm_diff( unsigned reg )
  {
    auto const& umm = umms[reg];
    if (umm.transfer == umm.initial)
      return false;
    if (not interface.vregs.modified(reg))
      return false;
    if (umm.size != VUConfig::BYTECOUNT)
      return true; /* some parts have been zeroed */

    struct
    {
      bool seek( Expr const& from, int at )
      {
        if (auto vt = dynamic_cast<VUConfig::VTransBase const*>( from.node ))
          {
            at -= vt->rshift;
            if (at < 0 or at >= int(vt->size()))
              return false;
            return seek( vt->expr(), at );
          }
        if (auto vm = dynamic_cast<VUConfig::VMix const*>( from.node ))
          {
            return (seek( vm->l, at ) or seek( vm->r, at ));
          }
        if (auto vr = dynamic_cast<VRegRead const*>( from.node ))
          {
            rpos = at;
            ridx = vr->idx;
            return true;
          }
        return false;
      }
      bool check( Expr const& from, unsigned idx, unsigned pos )
      {
        return seek( from, 0 ) and ridx == idx and rpos == pos;
      }
      unsigned ridx, rpos;
    } source;

    unsigned vidx = interface.vregs.index(reg);
    if (umm.transfer != &VUnion::Transfer<u8_t>)
      {
        VUConfig::Byte buf[VUConfig::BYTECOUNT];
        umm.transfer( &buf[0], &vmm_storage[reg][0], VUConfig::BYTECOUNT, false );
        for (unsigned idx = 0; idx < VUConfig::BYTECOUNT; ++idx)
          {
            u8_t regbyte;
            VUConfig::TypeInfo<u8_t>::FromBytes( regbyte, &buf[idx] );
            if (not source.check(regbyte.expr, vidx, idx ))
              return true;
          }
      }
    else
      {
        u8_t const* regbytes = reinterpret_cast<u8_t const*>( &vmm_storage[reg][0] );
        for (unsigned idx = 0; idx < VUConfig::BYTECOUNT; ++idx)
          if (not source.check(regbytes[idx].expr, vidx, idx ))
            return true;
      }
    
    return false;
  }

  unisim::util::symbolic::Expr&
  Arch::fpaccess( unsigned reg, bool write )
  {
    throw unisim::util::sav::Untestable("X87");
    unsigned idx = interface.fregs.touch(reg,write);
    unisim::util::symbolic::Expr& res = fpregs[reg];
    if (not res.node)
      res = new FRegRead( reg, idx );
    return res;
  }

  bool
  Arch::fpdiff( unsigned reg )
  {
    if (not fpregs[reg].node)
      return false;
    if (not interface.fregs.modified(reg))
      return false;

    if (auto fr = dynamic_cast<FRegRead const*>( fpregs[reg].node ))
      return fr->idx != interface.fregs.index(reg);

    return true;
  }

  void
  Arch::eregtouch( unsigned reg, bool write )
  {
    unsigned idx = interface.gregs.touch(reg,write);
    if (not regvalues[reg][0].node)
      regvalues[reg][0] = new GRegRead( reg, idx );
  }

  bool
  Arch::eregdiff( unsigned reg )
  {
    if (not regvalues[reg][0].node)
      return false;
    if (not interface.gregs.modified(reg))
      return false;
    
    for (unsigned ipos = 1; ipos < REGSIZE; ++ipos)
      if (regvalues[reg][ipos].node)
        return true;
    if (auto rr = dynamic_cast<GRegRead const*>( regvalues[reg][0].node ))
      return rr->idx != interface.gregs.index(reg);
    return true;
  }
  
  Arch::Expr
  Arch::eregread( unsigned reg, unsigned size, unsigned pos )
  {
    eregtouch( reg, false );

    using unisim::util::symbolic::ExprNode;
    using unisim::util::symbolic::make_const;
    
    struct
    {
      Expr ui( unsigned sz, Expr const& src ) const
      {
        switch (sz) {
        default: throw 0;
        case 1: return new unisim::util::symbolic::CastNode<uint8_t,uint64_t>( src );
        case 2: return new unisim::util::symbolic::CastNode<uint16_t,uint64_t>( src );
        case 4: return new unisim::util::symbolic::CastNode<uint32_t,uint64_t>( src );
        case 8: return new unisim::util::symbolic::CastNode<uint64_t,uint64_t>( src );
        }
        return 0;
      }
    } cast;
    
    if (not regvalues[reg][pos].node)
      {
        // requested read is in the middle of a larger value
        unsigned src = pos;
        do { src = src & (src-1); } while (not regvalues[reg][src].node);
        unsigned shift = 8*(pos - src);
        return cast.ui( size, make_operation( "Lsr", regvalues[reg][src], make_const( shift ) ) );
      }
    else if (not regvalues[reg][(pos|size)&(REGSIZE-1)].node)
      {
        // requested read is in lower bits of a larger value
        return cast.ui( size, regvalues[reg][pos] );
      }
    else if ((size > 1) and (regvalues[reg][pos|(size >> 1)].node))
      {
        // requested read is a concatenation of multiple source values
        Expr concat = cast.ui( size, regvalues[reg][pos] );
        for (unsigned idx = 0; ++idx < size;)
          {
            if (not regvalues[reg][pos+idx].node)
              continue;
            concat = make_operation( "Or", make_operation( "Lsl", cast.ui( size, regvalues[reg][idx] ), make_const( 8*idx ) ), concat );
          }
        return concat;
      }
    
    // requested read is directly available
    return regvalues[reg][pos];
  }
  
  void
  Arch::eregwrite( unsigned reg, unsigned size, unsigned pos, Expr const& xpr )
  {
    eregtouch( reg, true );
    Expr nxt[REGSIZE];
    
    for (unsigned ipos = pos, isize = size, cpos;
         cpos = (ipos^isize) & (REGSIZE-1), (not regvalues[reg][ipos].node) or (not regvalues[reg][cpos].node);
         isize *= 2, ipos &= -isize
         )
      {
        nxt[cpos] = eregread( reg, isize, cpos );
      }
    
    for (unsigned ipos = 0; ipos < REGSIZE; ++ipos)
      {
        if (nxt[ipos].node)
          regvalues[reg][ipos] = nxt[ipos];
      }
    
    regvalues[reg][pos] = xpr;
    
    for (unsigned rem = 1; rem < size; ++rem)
      {
        regvalues[reg][pos+rem] = 0;
      }
  }

  Operation*
  AMD64::decode( uint64_t addr, MemCode& ct, std::string& disasm )
  {
    try
      {
        unisim::component::cxx::processor::intel::Mode mode( 1, 0, 1 ); // x86_64
        unisim::component::cxx::processor::intel::InputCode<Arch> ic( mode, &ct.bytes[0], addr );
        Operation* op = getoperation( ic );
        if (not op)
          {
            uint8_t const* oc = ic.opcode();
            unsigned opcode_length = ic.vex() ? 5 - (oc[0] & 1) : oc[0] == 0x0f ? (oc[1] & ~2) == 0x38 ? 4 : 3 : 2;
            ct.length = ic.opcode_offset + opcode_length;
            throw unisim::util::sav::Untestable("#UD");
          }
        ct.length = op->length;
        std::ostringstream buf;
        op->disasm(buf);
        disasm = buf.str();
        return op;
      }
    catch (unisim::component::cxx::processor::intel::CodeBase::PrefixError const& perr)
      {
        ct.length = perr.len;
        throw unisim::util::sav::Untestable("#UD");
      }
    return 0;
  }

  Arch::Arch( Interface& iif )
    : interface(iif), path(iif.behavior.get())
    , next_insn_addr(new RIPRead), next_insn_mode(ipjmp)
    , ftop(0)
  {
    for (FLAG reg; reg.next();)
      flagvalues[reg.idx()] = newRegRead( reg );
  }

  bool
  Arch::close( Arch const& ref )
  {
    bool complete = path->close();

    // Instruction Pointer
    path->add_update( new RIPWrite( next_insn_addr, next_insn_mode ) );
    
    // Scalar integer registers
    for (unsigned reg = 0; reg < REGCOUNT; ++reg)
      if (eregdiff(reg))
        path->add_update( new GRegWrite( reg, interface.gregs.index(reg), eregread( reg, REGSIZE, 0 ) ) );

    // Vector Registers
    for (unsigned reg = 0; reg < VUConfig::REGCOUNT; ++reg)
      if (vmm_diff(reg))
        path->add_update( new VRegWrite( reg, interface.vregs.index(reg), umms[reg].GetConstStorage( &vmm_storage[reg][0], VmmRegister(), VUConfig::BYTECOUNT )->expr ) );
    
    // FPU registers
    for (unsigned reg = 0; reg < 8; ++reg)
      if (fpdiff(reg))
        path->add_update( new FRegWrite( reg, interface.fregs.index(reg), fpregs[reg] ) );

    // Flags
    for (FLAG reg; reg.next();)
      if (flagvalues[reg.idx()] != ref.flagvalues[reg.idx()])
        path->add_update( newRegWrite( reg, flagvalues[reg.idx()] ) );

    for (Expr const& store : stores)
      path->add_update( store );
    
    return complete;
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


  Interface::Interface( Operation const& op, MemCode const& code, std::string const& disasm )
    : memcode(code)
    , asmcode(disasm)
    , gregs()
    , fregs()
    , vregs()
    , behavior(std::make_shared<unisim::util::sav::ActionNode>())
    , addrs()
    , base_addr()
    , addressings()
  {
    // Performing an abstract execution to check the validity of
    // the opcode, and to compute the interface of the operation
    Arch reference( *this );

    for (bool end = false; not end;)
      {
        Arch arch( *this );
        arch.step( op );
        end = arch.close( reference );
      }

    unsigned grexcl[] = {3,4,5,7,12,13,14,15};
    for (unsigned idx = 0, end = sizeof grexcl / sizeof grexcl[0]; idx < end; ++idx)
      {
        if (gregs.accessed(grexcl[idx]))
          {
            std::ostringstream buf;
            buf << "reserved register access ("
                << unisim::component::cxx::processor::intel::DisasmG( unisim::component::cxx::processor::intel::GOq(), grexcl[idx] )
                << ")";
            throw unisim::util::sav::Untestable(buf.str());
          }
      }
    
    if (addrs.size())
      {
        if (uint64_t(*addrs.rbegin() - *addrs.begin()) > 1024)
          throw unisim::util::sav::Untestable("spread out memory accesses");

        if (not addressings.solve<review::Arch::GRegRead>(base_addr, new ExpectedAddress()))
          throw unisim::util::sav::Untestable("malformed address");
      }

    behavior->simplify();
  }
  
  void Interface::gencode(Text& text) const
  {
    unsigned offset = 0;
    
    /* Load EFLAGS register */
    {
      struct { uint8_t opcode; uint8_t r_m : 3; uint8_t r_o : 3; uint8_t mod : 2; uint8_t disp; uint8_t popf; } i;
      /* FF /6, PUSH r/m64 x(%rdi) */
      i.opcode = 0xff; /* FF */
      i.mod = 1;
      i.r_o = 6; /* /6 */
      i.r_m = 7; /* %rdi */
      i.disp = offset;
      i.popf = 0x9d;
      uint8_t const* ptr = &i.opcode;
      text.write(ptr,4);
    }
    offset += 8;
    
    /* Load GP registers */
    for (unsigned reg = 0; reg < gregs.count(); ++reg)
      {
        if (not gregs.accessed(reg))
          continue;
        // if (reg == 4 or reg == 7) continue; /* rsp or rdi */
        struct { uint8_t rex; uint8_t opcode; uint8_t r_m : 3; uint8_t reg : 3; uint8_t mod : 2; uint8_t disp; } i;
        i.rex = 0x48 + 4*(reg >= 8);
        i.opcode = 0x8b;
        i.mod = 1;
        i.reg = reg % 8;
        i.r_m = 7; /*%rdi*/
        i.disp = offset + Arch::REGSIZE*gregs.index(reg);
        uint8_t const* ptr = &i.rex;
        text.write(ptr,4);
      }
    offset += Arch::REGSIZE*gregs.used();
    
    /* Load AVX registers */
    for (unsigned reg = 0; reg < vregs.count(); ++reg)
      {
        if (not vregs.accessed(reg))
          continue;
        struct { uint8_t vex; uint8_t pp : 2; uint8_t L : 1; uint8_t vvvv : 4; uint8_t R : 1; uint8_t opcode; uint8_t r_m : 3; uint8_t reg : 3; uint8_t mod : 2; uint8_t disp; } i;
        // VEX.256.F3.0F.WIG 6F /r
        i.vex = 0xc5; /* 2bytes VEX */
        i.pp = 2; /*F3*/
        i.L = 1; /*256*/
        i.vvvv = 0b1111;
        i.R = reg < 8;
        i.opcode = 0x6f; /*6F*/
        i.mod = 1;
        i.reg = reg % 8;
        i.r_m = 7; /*%rdi*/
        i.disp = offset + Arch::VUConfig::BYTECOUNT*vregs.index(reg);
        uint8_t const* ptr = &i.vex;
        text.write(ptr,5);
      }
    offset += Arch::VUConfig::BYTECOUNT*vregs.used();

    if (offset >= 128) throw 0;
    
    text.write(memcode.text(),memcode.length);

    /* Fix RDI to destination zone */
    {
      /* REX.W + 83 /0 ib ADD r/m64, imm8 */
      struct { uint8_t rex; uint8_t opcode; uint8_t r_m : 3; uint8_t reg : 3; uint8_t mod : 2; uint8_t imm; } i;
      i.rex = 0x48;
      i.opcode = 0x83; /* 83 */
      i.r_m = 7; /* %rdi */
      i.reg = 0; /* /0 */
      i.mod = 3; 
      i.imm = offset; /* ib */
      uint8_t const* ptr = &i.rex;
      text.write(ptr,4);
    }
    offset = 0;
    
    /* Store EFLAGS register */
    {
      struct { uint8_t pushf; uint8_t opcode; uint8_t r_m : 3; uint8_t r_o : 3; uint8_t mod : 2; uint8_t disp; } i;
      /* 8F /0, POP r/m64 x(%rdi) */
      i.pushf = 0x9c;
      i.opcode = 0x8f; /* 8F */
      i.mod = 1;
      i.r_o = 0; /* /0 */
      i.r_m = 7; /* %rdi */
      i.disp = offset;
      uint8_t const* ptr = &i.pushf;
      text.write(ptr,4);
    }
    offset += 8;
    
    /* Store GP registers */
    for (unsigned reg = 0; reg < gregs.count(); ++reg)
      {
        if (not gregs.accessed(reg))
          continue;
        // if (reg == 4 or reg == 7) continue; /* rsp or rdi */
        struct { uint8_t rex; uint8_t opcode; uint8_t r_m : 3; uint8_t reg : 3; uint8_t mod : 2; uint8_t disp; } i;
        i.rex = 0x48 + 4*(reg >= 8);
        i.opcode = 0x89;
        i.mod = 1;
        i.reg = reg % 8;
        i.r_m = 7; /*%rdi*/
        i.disp = offset + Arch::REGSIZE*gregs.index(reg);
        uint8_t const* ptr = &i.rex;
        text.write(ptr,4);
      }
    offset += Arch::REGSIZE*gregs.used();
    
    /* Store AVX registers */
    for (unsigned reg = 0; reg < vregs.count(); ++reg)
      {
        if (not vregs.accessed(reg))
          continue;
        struct { uint8_t vex; uint8_t pp : 2; uint8_t L : 1; uint8_t vvvv : 4; uint8_t R : 1; uint8_t opcode; uint8_t r_m : 3; uint8_t reg : 3; uint8_t mod : 2; uint8_t disp; } i;
        // VEX.256.F3.0F.WIG 7F /r
        i.vex = 0xc5; /* 2bytes VEX */
        i.pp = 2; /*F3*/
        i.L = 1; /*256*/
        i.vvvv = 0b1111;
        i.R = reg < 8;
        i.opcode = 0x7f; /*7F*/
        i.mod = 1;
        i.reg = reg % 8;
        i.r_m = 7; /*%rdi*/
        i.disp = offset + Arch::VUConfig::BYTECOUNT*vregs.index(reg);
        uint8_t const* ptr = &i.vex;
        text.write(ptr,5);
      }
    offset += Arch::VUConfig::BYTECOUNT*vregs.used();

    text.write((uint8_t const*)"\xc3",1);
  }

  uintptr_t
  Interface::workcells() const
  {
    uintptr_t offset = 0;
    offset += 8;
    offset += Arch::REGSIZE*gregs.used();
    offset += Arch::VUConfig::BYTECOUNT*vregs.used();
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
          
          if (auto vr = dynamic_cast<unisim::util::sav::VirtualRegister const*>(a.node))
            {
              if (int delta = avi[vr->idx] - bvi[dynamic_cast<unisim::util::sav::VirtualRegister const&>(*b.node).idx])
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
    uint64_t zaddr;
    if (auto z = addr.Eval( Arch::AddrEval() ))
      { Expr dispose(z); zaddr = z->Get( uint64_t() ); }
    else
      throw "WTF";
    addrs.insert(zaddr);

    if (zaddr == *addrs.begin())
      base_addr = addr;
  }
  
  bool AMD64::MemCode::get(std::istream& source)
  {
    unsigned idx = 0;
        
    for (bool nibble = false;;)
      {
        char ch;
        if (not source.get( ch ).good()) return false;
        if (ch == ' ')     { if (nibble) return false; continue; }
        if (ch == '\t')    { if (nibble) return false; break; }
        if (idx >= sizeof(bytes)) throw 0;

        bytes[idx] <<= 4;
        if      ('0' <= ch and ch <= '9') bytes[idx] |= ch - '0';
        else if ('a' <= ch and ch <= 'f') bytes[idx] |= ch - 'a' + 10;
        else return false;

        idx += nibble;
        nibble = not nibble;
      }
      
    length = idx;
    return true;
  }
}

