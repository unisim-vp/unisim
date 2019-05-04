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
  
  Arch::Expr
  Arch::eregread( unsigned reg, unsigned size, unsigned pos )
  {
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

    for (EIRegID reg; reg.next();)
      regvalues[reg.idx()][0] = newRegRead( reg );

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
    struct { bool operator()( Expr const* a, Expr const* b )
      { for (unsigned ipos = 0; ipos < REGSIZE; ++ipos) { if (a[ipos] != b[ipos]) return true; } return false; }
    } regdiff;
    for (EIRegID reg; reg.next();)
      if (regdiff( regvalues[reg.idx()], ref.regvalues[reg.idx()] ))
        path->updates.insert( newRegWrite( reg, eregread( reg.idx(), 4, 0 ) ) );
    
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
