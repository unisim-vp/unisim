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
  return ut::make_weirdop<FPT>( FIRoundOp(x87frnd_mode), src );
}

} /* namespace unisim */ } /* namespace component */ } /* namespace cxx */ } /* namespace processor */ } /* namespace intel */

#include <unisim/component/cxx/processor/intel/isa/intel.tcc>
#include <arch.hh>

namespace ut
{
  void
  Arch::fxam()
  {
    f64_t value = this->fread( 0 );
    u8_t fpclass = u8_t( ut::make_weirdop<u8_t>( "fxam", value ) );

    flagwrite( FLAG::C0, ut::make_weirdop<bit_t>( "fxam.C0", fpclass ) );
    flagwrite( FLAG::C1, ut::make_weirdop<bit_t>( "fxam.sign", value ) );
    flagwrite( FLAG::C2, ut::make_weirdop<bit_t>( "fxam.C2", fpclass ) );
    flagwrite( FLAG::C3, ut::make_weirdop<bit_t>( "fxam.C3", fpclass ) );
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
    unisim::component::cxx::processor::intel::Mode mode( 1, 0, 1 ); // x86_64
    unisim::component::cxx::processor::intel::InputCode<ut::Arch> ic( mode, &ct.bytes[0], addr );
    Operation* op = getoperation( ic );
    if (not op)
      {
        uint8_t const* oc = ic.opcode();
        unsigned opcode_length = ic.vex() ? 5 - (oc[0] & 1) : oc[0] == 0x0f ? (oc[1] & ~2) == 0x38 ? 4 : 3 : 2;
        ct.length = ic.opcode_offset + opcode_length;
        throw ut::Untestable("#UD");
      }
    ct.length = op->length;
    std::ostringstream buf;
    op->disasm(buf);
    disasm = buf.str();
    return op;
  }

  Arch::Arch( Interface& iif )
    : interface(iif), path(iif.behavior.get())
    , next_insn_addr(new RIPRead), next_insn_mode(ipjmp)
    , ftop(0)
  {
    for (FLAG reg; reg.next();)
      flagvalues[reg.idx()] = newRegRead( reg );
  }

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

  bool
  Arch::close( Arch const& ref )
  {
    bool complete = path->close();

    // Instruction Pointer
    path->updates.insert( Expr( new RIPWrite( next_insn_addr, next_insn_mode ) ) );
    
    // Scalar integer registers
    for (unsigned reg = 0; reg < REGCOUNT; ++reg)
      if (eregdiff(reg))
        path->updates.insert( new GRegWrite( reg, interface.gregs.index(reg), eregread( reg, REGSIZE, 0 ) ) );

    // Vector Registers
    for (unsigned reg = 0; reg < VUConfig::REGCOUNT; ++reg)
      if (vmm_diff(reg))
        path->updates.insert( new VRegWrite( reg, interface.vregs.index(reg), umms[reg].GetConstStorage( &vmm_storage[reg][0], VmmRegister(), VUConfig::BYTECOUNT )->expr ) );
    
    // FPU registers
    for (unsigned reg = 0; reg < 8; ++reg)
      if (fpdiff(reg))
        path->updates.insert( new FRegWrite( reg, interface.fregs.index(reg), fpregs[reg] ) );

    // Flags
    for (FLAG reg; reg.next();)
      if (flagvalues[reg.idx()] != ref.flagvalues[reg.idx()])
        path->updates.insert( newRegWrite( reg, flagvalues[reg.idx()] ) );

    for (Expr const& store : stores)
      path->updates.insert( store );
    
    return complete;
  }

  bool
  Arch::Cond(unisim::util::symbolic::Expr cond)
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
    , behavior(std::make_shared<ActionNode>())
    , base_addr()
    , relocs()
    , has_write(false)
    , has_jump(false)
  {
    // Performing an abstract execution to check the validity of
    // the opcode, and to compute the interface of the operation
    ut::Arch reference( *this );
      
    for (bool end = false; not end;)
      {
        ut::Arch arch( *this );
        arch.step( op );
        end = arch.close( reference );
      }

    if (addrs.size())
      {
        typedef decltype(this->relocs) Relocs;
        
        struct AG
        {
          typedef unisim::util::symbolic::Expr Expr;
      
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
              }
          }
          
          AG(Relocs& _relocs) : relocs(_relocs) {} Relocs& relocs;
        } ag(relocs);
        ag.process( base_addr, new ExpectedAddress() );
        // remove the invalid relocations
        for (Relocs::iterator itr = relocs.begin(), end = relocs.end(); itr != end; )
          {
            if (itr->second.good()) ++itr; else itr = relocs.erase(itr);
          }
      }

    if (gregs.accessed(4))
      throw ut::Untestable("SP access");
    if (has_jump)
      throw ut::Untestable("has jump");
    
    behavior->simplify();
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
