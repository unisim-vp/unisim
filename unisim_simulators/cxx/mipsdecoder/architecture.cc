#include <architecture.hh>

namespace Mips
{
  namespace {
    typedef unisim::util::symbolic::Expr Expr;
    struct UpdatesMerger { void operator () ( std::set<Expr>& updates, Expr const& l, Expr const& r ) { updates.insert( l ); } };
  }

  void
  Interpreter::ActionNode::simplify()
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

  void
  Interpreter::INode::ComputeForward(unisim::util::symbolic::Expr const& expr,
                                     FDScalarElement& res, FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags)
  {
    unsigned subcount = expr->SubCount();
    typedef unisim::util::forbint::debug::MultiBit::Operation MultiBitOperation;
    typedef unisim::util::forbint::debug::MultiBit::CastMultiBitOperation CastMultiBitOperation;
    
    if (auto node = expr->AsConstNode())
      {
        ScalarType dst_type( node->GetType() );
        res = flags.newMultiBitConstant(node->Get( uint64_t() ), dst_type.bitsize);
        return;
      }
    else if (auto node = expr->AsOpNode())
      {
        using unisim::util::symbolic::Op;
        
        Interpreter::INode::ComputeForward(expr->GetSub(0), res, memory, dest, flags);
        
        if (subcount == 1)
          {
            switch (node->op.code)
              {
              case Op::Not: res.applyAssign(MultiBitOperation().setBitNegate(), flags.scalarPart()); return;
              case Op::Neg: res.applyAssign(MultiBitOperation().setOppositeSigned(), flags.scalarPart()); return;
              case Op::Cast:
                {
                  auto const& cnb = dynamic_cast<unisim::util::symbolic::CastNodeBase const&>( *expr.node );
                  ScalarType dst( cnb.GetType() );
                  res.applyAssign(CastMultiBitOperation().setSize(dst.bitsize), flags.scalarPart());
                }
                return;
                
              default:
                break;
              }
            
          }
        else if (subcount == 2)
          {
            // Interpreter::INode::ComputeForward(expr->GetSub(0), res, memory, dest, flags);
            FDScalarElement arg;
            Interpreter::INode::ComputeForward(expr->GetSub(1), arg, memory, dest, flags);
            
            switch (node->op.code)
              {
              case Op::Xor: res.applyAssign(MultiBitOperation().setBitExclusiveOr(), arg, flags.scalarPart()); return;
              case Op::And: res.applyAssign(MultiBitOperation().setBitAnd(), arg, flags.scalarPart()); return;
              case Op::Or:  res.applyAssign(MultiBitOperation().setBitOr(), arg, flags.scalarPart()); return;
              case Op::Lsl: res.applyAssign(MultiBitOperation().setLeftShift(), arg, flags.scalarPart()); return;
              case Op::Asr: res.applyAssign(MultiBitOperation().setArithmeticRightShift(), arg, flags.scalarPart()); return;
              case Op::Lsr: res.applyAssign(MultiBitOperation().setLogicalRightShift(), arg, flags.scalarPart()); return;
              case Op::Add: res.applyAssign(MultiBitOperation().setPlusSigned(), arg, flags.scalarPart()); return;
              case Op::Sub: res.applyAssign(MultiBitOperation().setMinusSigned(), arg, flags.scalarPart()); return;
              case Op::Teq: res.applyAssign(MultiBitOperation().setCompareEqual(), arg, flags.scalarPart()); return;
              case Op::Tne: res.applyAssign(MultiBitOperation().setCompareDifferent(), arg, flags.scalarPart()); return;
              case Op::Tge: res.applyAssign(MultiBitOperation().setCompareGreaterOrEqualSigned(), arg, flags.scalarPart()); return;
              case Op::Tgt: res.applyAssign(MultiBitOperation().setCompareGreaterSigned(), arg, flags.scalarPart()); return;
              case Op::Tle: res.applyAssign(MultiBitOperation().setCompareLessOrEqualSigned(), arg, flags.scalarPart()); return;
              case Op::Tlt: res.applyAssign(MultiBitOperation().setCompareLessSigned(), arg, flags.scalarPart()); return;
              case Op::Tgeu: res.applyAssign(MultiBitOperation().setCompareGreaterOrEqualUnsigned(), arg, flags.scalarPart()); return;
              case Op::Tgtu: res.applyAssign(MultiBitOperation().setCompareGreaterUnsigned(), arg, flags.scalarPart()); return;
              case Op::Tleu: res.applyAssign(MultiBitOperation().setCompareLessOrEqualUnsigned(), arg, flags.scalarPart()); return;
              case Op::Tltu: res.applyAssign(MultiBitOperation().setCompareLessUnsigned(), arg, flags.scalarPart()); return;

              default: break;
              }
          }
        
        { struct NotYet {}; throw NotYet(); }
      }
    else if (auto node = dynamic_cast<Interpreter::INode const*>(expr.node))
      {
        node->ComputeForward(res, memory, dest, flags);
        return;
      }

  }

  void
  Interpreter::Load::ComputeForward(FDScalarElement& elem, FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const
  {
    FDScalarElement address;
    INode::ComputeForward(addr, address, memory, dest, flags);
    elem = memory.loadValue(address, bytes*8, flags);
  }
  
  std::unique_ptr<Interpreter::SideEffect>
  Interpreter::Store::InterpretForward(FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const
  {
    struct DoStore : public Interpreter::SideEffect
    {
      DoStore(unsigned _bytes, Expr _addr, Expr _value, FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags)
        : addr(), value(), bytes(_bytes)
      {
        INode::ComputeForward( _addr,  addr, memory, dest, flags);
        INode::ComputeForward(_value, value, memory, dest, flags);
      }
      virtual void Commit(FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) override
      {
        memory.storeValue(addr, value, flags);
      }
      FDScalarElement addr, value;
      unsigned bytes;
    };
    
    return std::make_unique<DoStore>(bytes,addr,value, memory, dest, flags);
  }
  
  void
  Interpreter::RegRead::ComputeForward(FDScalarElement& elem, FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const
  {
    elem = memory.getRegisterValue(reg.idx(), flags, unisim::util::forbint::debug::TIMultiBit);
  }
  
  std::unique_ptr<Interpreter::SideEffect>
  Interpreter::RegWrite::InterpretForward(FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const
  {
    struct DoRegWrite : public Interpreter::SideEffect
    {
      DoRegWrite(RegisterIndex _reg, Expr _value, FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags)
        : value(), reg(_reg)
      {
        INode::ComputeForward(_value, value, memory, dest, flags);
      }
      virtual void Commit(FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) override
      {
        memory.setRegisterValue(reg.idx(), value, flags);
      }
      FDScalarElement value;
      RegisterIndex reg;
    };
    
    return std::make_unique<DoRegWrite>(reg, value, memory, dest, flags);
  }
  
  void
  Interpreter::Goto::retrieveFamily( unisim::util::forbint::debug::Iteration::FamilyInstruction& family, uint32_t origin ) const
  {
    switch (btype)
      {
      case B_JMP:
        {
          Expr mt(target);
          if (auto addr = mt.ConstSimplify())
            {
              uint32_t destination = addr->Get( uint32_t() );
              if (destination > origin)
                family.setForwardJump();
              else
                family.setBackwardJump();
            }
          else
            family.setComputedJump();
        } break;
      case B_CALL: family.setCall(); break;
      case B_RET:  family.setReturn(); break;
      case B_EXC:  family.setBranch(); break;
      case B_DBG:  family.setBranch(); break;
      case B_RFE:  family.setReturn(); break;
      }
  }

  void
  Interpreter::Goto::retrieveTargets( unisim::util::forbint::debug::Iteration& iteration ) const
  {
    if (iteration.mayFollowGraph())
      {
        if (!iteration.localAdvanceOnInstruction())
          {
            if (!iteration.advanceOnCallInstruction() && !iteration.advanceOnNextGraphInstruction()) {
              iteration.assumeAcceptEmptyDestination();
            }
          }
      }
    else
      {
        if (auto cnb = target->AsConstNode())
          {
            uint64_t address = cnb->Get(uint32_t());
            iteration.addTarget(&address, 1);
          }
        else
          {
            struct TODO {}; throw TODO();
          }
      }
  }
  
}
