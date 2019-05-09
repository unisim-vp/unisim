#include <arch.hh>

namespace unisim { namespace component { namespace cxx { namespace processor { namespace intel {

template <typename FPT> FPT firound( FPT const& src, int x87frnd_mode )
{
  return FPT( unisim::util::symbolic::Expr( new ut::FIRound<typename FPT::value_type>( src.expr, x87frnd_mode ) ) );
}

} /* namespace unisim */ } /* namespace component */ } /* namespace cxx */ } /* namespace processor */ } /* namespace intel */

#include <unisim/component/cxx/processor/intel/isa/intel.tcc>
#include <arch.hh>

namespace ut
{
  void
  Arch::FTop::Repr( std::ostream& sink ) const
  {
    sink << "FpuStackTop";
  }

  void FIRoundBase::Repr( std::ostream& sink ) const
  {
    sink << "FIRound<" << ScalarType(GetType()).name << ">( " << src << ", " << rmode << ")";
  }

  namespace
  {
    struct GRegRead : public unisim::util::symbolic::ExprNode
    {
      typedef unisim::util::symbolic::ExprNode   ExprNode;
      typedef unisim::util::symbolic::Expr       Expr;
      typedef unisim::util::symbolic::ScalarType ScalarType;
      typedef GRegRead this_type;
      
      GRegRead( unsigned _idx ) : idx(_idx) {} unsigned idx;
      virtual void Repr( std::ostream& sink ) const override
      {
        sink << "GRegRead(" << idx << ")";
      }
      virtual unsigned SubCount() const override { return 0; }
      virtual intptr_t cmp(ExprNode const& brhs) const override { return compare( dynamic_cast<this_type const&>( brhs ) ); }
      intptr_t compare(this_type const& rhs) const { return int(idx) - int(rhs.idx); }
      virtual ExprNode* Mutate() const override { return new this_type(*this); }
      ScalarType::id_t GetType() const { return ScalarType::U64; }
    };

    struct GRegWrite : public Arch::Update
    {
      typedef unisim::util::symbolic::ExprNode   ExprNode;
      typedef unisim::util::symbolic::Expr       Expr;
      typedef unisim::util::symbolic::ScalarType ScalarType;
      typedef GRegWrite this_type;
      GRegWrite( unsigned _idx, Expr const& _value ) : value(_value), idx(_idx) {}
      virtual this_type* Mutate() const override { return new this_type( *this ); }
      virtual void Repr( std::ostream& sink ) const override
      {
        sink << "GRegWrite(" << idx << ") := " << value;
      }
      virtual unsigned SubCount() const { return 1; }
      virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return value; }
      intptr_t compare( GRegWrite const& rhs ) const
      {
        if (intptr_t delta = value.cmp( rhs.value )) return delta;
        return int(idx) - int(rhs.idx);
      }
      virtual intptr_t cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<GRegWrite const&>( rhs ) ); }
      Expr value;
      unsigned idx;
    };
  }
  
  void
  Arch::vmm_touch( unsigned reg )
  {
    auto itr = vregmap.lower_bound( reg );
    if (itr != vregmap.end() and reg == itr->first)
      {
        VmmRegister v( new VRegRead( vregmap.insert( itr, std::make_pair( reg, vregmap.size() ) )->second ) );
        *(umms[reg].GetStorage( &vmm_storage[reg][0], v, VUConfig::BYTECOUNT )) = v;
      }
  }

  bool
  Arch::vmm_diff( unsigned reg )
  {
    auto itr = vregmap.lower_bound( reg );
    if (itr == vregmap.end() or reg < itr->first)
      return false;
    VmmRegister const* vi = umms[reg].GetConstStorage( &vmm_storage[reg][0], VmmRegister(), VUConfig::BYTECOUNT );
    if (VRegRead const* rr = dynamic_cast<VRegRead const*>( vi->expr.node ))
      return rr->idx != vregmap.at(reg);
    return true;
  }

  void
  Arch::eregtouch( unsigned reg )
  {
    auto itr = eregmap.lower_bound( reg );
    if (itr == eregmap.end() or reg < itr->first)
      regvalues[reg][0] = new GRegRead( eregmap.insert( itr, std::make_pair( reg, eregmap.size() ) )->second );
  }

  bool
  Arch::eregdiff( unsigned reg )
  {
    if (not regvalues[reg][0].node)
      return false;
    for (unsigned ipos = 1; ipos < REGSIZE; ++ipos)
      if (regvalues[reg][ipos].node)
        return true;
    if (auto rr = dynamic_cast<GRegRead const*>( regvalues[reg][0].node ))
      return rr->idx != eregmap.at(reg);
    return true;
  }
  
  Arch::Expr
  Arch::eregread( unsigned reg, unsigned size, unsigned pos )
  {
    eregtouch( reg );

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
    eregtouch( reg );
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
  Arch::fetch( uint64_t address, uint8_t const* bytes )
  {
    typedef  unisim::component::cxx::processor::intel::InputCode<Arch> InputCode;
    return getoperation( InputCode( unisim::component::cxx::processor::intel::Mode( 1, 0, 1 ), bytes, OpHeader( address ) ) );
  }

  Arch::Arch( ActionNode* actions )
    : path(actions)
    , next_insn_addr(new RIPRead), fcw(new FCWRead), ftop_source(new FTop), ftop(0), mxcsr(new MXCSRRead)
  {
    for (SRegID reg; reg.next();)
      segment_registers[reg.idx()] = newRegRead( reg );

    for (FLAG reg; reg.next();)
      flagvalues[reg.idx()] = newRegRead( reg );

    for (FRegID reg; reg.next();)
      fpuregs[reg.idx()] = newRegRead( reg );
  }

  void
  DMopBase::Repr( std::ostream& sink ) const
  {
    sink << "DMop<" << GetType() << ',' <<  opname() << ',' << prodname() << ">( " << hi << ", " << lo << ", " << ro << ")";
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
    path->updates.insert( Expr( new RIPWrite( next_insn_addr, next_insn_mode ) ) );
    
    // Scalar integer registers
    for (unsigned reg = 0; reg < REGCOUNT; ++reg)
      if (eregdiff(reg))
        path->updates.insert( new GRegWrite( reg, eregread( reg, REGSIZE, 0 ) ) );

    // Vector Registers
    for (unsigned reg = 0; reg < VUConfig::REGCOUNT; ++reg)
      if (vmm_diff(reg))
        path->updates.insert( new VRegWrite( reg, umms[reg].GetConstStorage( &vmm_storage[reg][0], VmmRegister(), VUConfig::BYTECOUNT )->expr ) );
    
    // Flags
    for (FLAG reg; reg.next();)
      if (flagvalues[reg.idx()] != ref.flagvalues[reg.idx()])
        path->updates.insert( newRegWrite( reg, flagvalues[reg.idx()] ) );

    // Segment registers
    for (SRegID reg; reg.next();)
      if (segment_registers[reg.idx()] != ref.segment_registers[reg.idx()])
        path->updates.insert( newRegWrite( reg, segment_registers[reg.idx()] ) );

    // FPU Control Word
    if (fcw != ref.fcw)
      path->updates.insert( new FCWWrite( fcw ) );

    if (mxcsr.expr != ref.mxcsr.expr)
      path->updates.insert( new MXCSRWrite( mxcsr.expr ) );
    
    // FPU registers
    for (FRegID reg; reg.next();)
      if (fpuregs[reg.idx()] != ref.fpuregs[reg.idx()])
        path->updates.insert( new FRegWrite( *this, fpuregs[reg.idx()], Arch::ftop_update(ftop_source, reg.idx()).expr ) );

    if (ftop)
      path->updates.insert( new FTopWrite( Arch::ftop_update(ftop_source, ftop).expr ) );

    for (auto && store : stores)
      path->updates.insert( store );
    
    return complete;
  }

  void
  Arch::setnip( addr_t eip, ipproc_t ipproc )
  {
    next_insn_addr = eip.expr;
    next_insn_mode = ipproc;
  }
  
  bool
  Arch::Cond(unisim::util::symbolic::Expr cond)
  {
    if (unisim::util::symbolic::ConstNodeBase const* cnode = cond.ConstSimplify())
      return cnode->GetBoolean();

    bool predicate = path->proceed( cond );
    path = path->next( predicate );
    return predicate;
  }

  Interface::Interface( Operation const& op )
    : action(std::make_shared<ActionNode>())
  {
    Arch reference( action.get() );

    for (bool end = false; not end;)
      {
        Arch arch( action.get() );
        arch.step( op );
        end = arch.close( reference );
      }
    
    action->simplify();
  }

}
