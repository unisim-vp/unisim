/*
 *  Copyright (c) 2007-2020,
 *  Commissariat a l'Energie Atomique (CEA),
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Franck Vedrine (franck.vedrine@cea.fr)
 */

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

  Interpreter::FCDomainValue
  Interpreter::INode::Compute(Expr const& expr, FCMemoryState& memory, DomainElementFunctions* def)
  {
    unsigned subcount = expr->SubCount();
    
    if (auto node = expr->AsConstNode())
      {
        ScalarType valtype( node->GetType() );

        if (valtype.is_integer)
          {
            if (valtype.bitsize == 1)
              return Interpreter::FCDomainValue( def->bit_create_constant(node->Get( bool() )), def, memory.env() );
            DomainIntegerConstant divalue{int(valtype.bitsize),valtype.is_signed,node->Get(uint64_t())};
            return Interpreter::FCDomainValue( def->multibit_create_constant(divalue), def, memory.env() );
          }
        //typedef long double float_container;
        typedef double float_container;
        DomainFloatingPointConstant dfpvalue{int(valtype.bitsize), node->Get(float_container())};
        return Interpreter::FCDomainValue( def->multifloat_create_constant(dfpvalue), def, memory.env() );
      }
    else if (auto node = expr->AsOpNode())
      {
        using unisim::util::symbolic::Op;
        
        Interpreter::FCDomainValue res = Interpreter::INode::Compute(expr->GetSub(0), memory, def);
        
        if (subcount == 1)
          {
            switch (node->op.code)
              {                
              case Op::Not:
                {
                  if (res.getSizeInBits() == 1) (*def->bit_unary_apply_assign)(&res.svalue(),     DBUONegate, memory.env());
                  else                     (*def->multibit_unary_apply_assign)(&res.svalue(), DMBUOBitNegate, memory.env());
                  return res;
                }
              case Op::Neg:
                {
                  (*def->multibit_unary_apply_assign)(&res.svalue(), DMBUOOppositeSigned, memory.env());
                  return res;
                }
              case Op::Cast:
                {
                  auto const& cnb = dynamic_cast<unisim::util::symbolic::CastNodeBase const&>( *expr.node );
                  ScalarType src( cnb.GetSrcType() ), dst( cnb.GetType() );
                  if (src.bitsize == 1)
                    res.assign( (*def->bit_create_cast_multibit)(res.value(), dst.bitsize, memory.env()) );
                  else if (dst.bitsize == 1)
                    res.assign( (*def->multibit_create_cast_bit)(res.value(), memory.env()) );
                  else if (src.is_integer and dst.is_integer)
                    res.assign( (*def->multibit_create_cast_multibit)(res.value(), dst.bitsize, dst.is_signed, memory.env()) );
                  else
                    break;
                  return res;
                }
                
              default:
                break;
              }
            
          }
        else if (subcount == 2)
          {
      //       // Interpreter::INode::ComputeForward(expr->GetSub(0), res, memory, dest, flags);
      //       FDScalarElement arg;
      //       Interpreter::INode::ComputeForward(expr->GetSub(1), arg, memory, dest, flags);
            
      //       switch (node->op.code)
      //         {
      //         case Op::Xor: res.applyAssign(MultiBitOperation().setBitExclusiveOr(), arg, flags.scalarPart()); return;
      //         case Op::And: res.applyAssign(MultiBitOperation().setBitAnd(), arg, flags.scalarPart()); return;
      //         case Op::Or:  res.applyAssign(MultiBitOperation().setBitOr(), arg, flags.scalarPart()); return;
      //         case Op::Lsl: res.applyAssign(MultiBitOperation().setLeftShift(), arg, flags.scalarPart()); return;
      //         case Op::Asr: res.applyAssign(MultiBitOperation().setArithmeticRightShift(), arg, flags.scalarPart()); return;
      //         case Op::Lsr: res.applyAssign(MultiBitOperation().setLogicalRightShift(), arg, flags.scalarPart()); return;
      //         case Op::Add: res.applyAssign(MultiBitOperation().setPlusSigned(), arg, flags.scalarPart()); return;
      //         case Op::Sub: res.applyAssign(MultiBitOperation().setMinusSigned(), arg, flags.scalarPart()); return;
      //         case Op::Teq: res.applyAssign(MultiBitOperation().setCompareEqual(), arg, flags.scalarPart()); return;
      //         case Op::Tne: res.applyAssign(MultiBitOperation().setCompareDifferent(), arg, flags.scalarPart()); return;
      //         case Op::Tge: res.applyAssign(MultiBitOperation().setCompareGreaterOrEqualSigned(), arg, flags.scalarPart()); return;
      //         case Op::Tgt: res.applyAssign(MultiBitOperation().setCompareGreaterSigned(), arg, flags.scalarPart()); return;
      //         case Op::Tle: res.applyAssign(MultiBitOperation().setCompareLessOrEqualSigned(), arg, flags.scalarPart()); return;
      //         case Op::Tlt: res.applyAssign(MultiBitOperation().setCompareLessSigned(), arg, flags.scalarPart()); return;
      //         case Op::Tgeu: res.applyAssign(MultiBitOperation().setCompareGreaterOrEqualUnsigned(), arg, flags.scalarPart()); return;
      //         case Op::Tgtu: res.applyAssign(MultiBitOperation().setCompareGreaterUnsigned(), arg, flags.scalarPart()); return;
      //         case Op::Tleu: res.applyAssign(MultiBitOperation().setCompareLessOrEqualUnsigned(), arg, flags.scalarPart()); return;
      //         case Op::Tltu: res.applyAssign(MultiBitOperation().setCompareLessUnsigned(), arg, flags.scalarPart()); return;
            
      //         default: break;
      //         }
          }
            
        { struct NotYet {}; throw NotYet(); }
      }
    else if (auto node = dynamic_cast<Interpreter::INode const*>(expr.node))
      {
        return node->Compute(memory, def);
      }
    return Interpreter::FCDomainValue();
  }

  void
  Interpreter::Load::Repr(std::ostream& sink) const
  {
    sink << "Load( ";
    for (unsigned idx = 0; idx < SubCount(); ++idx)
      { GetSub(idx)->Repr(sink); sink << ", "; }
    sink << bytes << " )";
  }

  void
  Interpreter::Load::ComputeForward(FDScalarElement& elem, FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const
  {
    FDScalarElement address;
    INode::ComputeForward(addr, address, memory, dest, flags);
    elem = memory.loadValue(address, bytes*8, flags);
  }
  
  Interpreter::FCDomainValue
  Interpreter::Load::Compute(Mips::Interpreter::FCMemoryState& memory, DomainElementFunctions* def) const
  {
    Interpreter::FCDomainValue dv_addr = Interpreter::INode::Compute(addr, memory, def);
    
    return Interpreter::FCDomainValue( memory.loadMultiBit(dv_addr, bytes), def, memory.env() );
  }
  
  std::unique_ptr<Interpreter::FDSideEffect>
  Interpreter::Store::InterpretForward(FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const
  {
    struct DoStore : public Interpreter::FDSideEffect
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
  
  std::unique_ptr<Interpreter::FCSideEffect>
  Interpreter::Store::Interpret(FCMemoryState& memory, DomainElementFunctions* def) const
  {
    struct DoStore : public Interpreter::FCSideEffect
    {
      DoStore(unsigned _bytes, Expr _addr, Expr _value, FCMemoryState& memory, DomainElementFunctions* def)
        : addr(INode::Compute(_addr, memory, def)), value(INode::Compute(_value, memory, def)), bytes(_bytes)
      {}
      virtual void Commit(FCMemoryState& memory, DomainElementFunctions* def) override
      {
        memory.valueStore(addr, value);
      }
      FCDomainValue addr, value;
      unsigned bytes;
    };

    return std::make_unique<DoStore>(bytes, addr, value, memory, def);
  }
  
  void
  Interpreter::RegRead::Repr(std::ostream& sink) const
  {
    sink << "RegRead( " << reg.c_str() << " )";
  }

  Interpreter::FCDomainValue
  Interpreter::RegRead::Compute(Mips::Interpreter::FCMemoryState& memory, DomainElementFunctions* def) const
  {
    return Interpreter::FCDomainValue( memory.getRegisterValueAsElement(reg.idx()), def, memory.env() );
  }
  
  void
  Interpreter::RegRead::ComputeForward(FDScalarElement& elem, FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const
  {
    elem = memory.getRegisterValue(reg.idx(), flags, unisim::util::forbint::debug::TIMultiBit);
  }
  
  std::unique_ptr<Interpreter::FDSideEffect>
  Interpreter::RegWrite::InterpretForward(FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const
  {
    struct DoRegWrite : public Interpreter::FDSideEffect
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
  
  std::unique_ptr<Interpreter::FCSideEffect>
  Interpreter::RegWrite::Interpret(FCMemoryState& memory, DomainElementFunctions* def) const
  {
    struct DoRegWrite : public Interpreter::FCSideEffect
    {
      DoRegWrite(RegisterIndex _reg, Expr _value, FCMemoryState& memory, DomainElementFunctions* def)
        : value(INode::Compute(_value, memory, def)), reg(_reg)
      {}
      virtual void Commit(FCMemoryState& memory, DomainElementFunctions* def) override
      {
        memory.setRegisterValue(reg.idx(), value);
      }
      FCDomainValue value;
      RegisterIndex reg;
    };
    
    return std::make_unique<DoRegWrite>(reg, value, memory, def);
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
            /* Indirect branch are not handled yet */
            struct TODO {}; throw TODO();
          }
      }
  }

  void
  Interpreter::Goto::Repr(std::ostream& sink) const
  {
    sink << "Goto( ";
    for (unsigned idx = 0; idx < SubCount(); ++idx)
      { GetSub(idx)->Repr(sink); sink << ", "; }
    sink << unsigned(btype) << " )";
  }

  void
  Interpreter::Goto::next_addresses(std::set<unsigned int>& addresses, FCMemoryState& memory, DomainElementFunctions* def) const
  {
    FCDomainValue addr = INode::Compute(target , memory, def);
    uint32_t addr_as_uint32;
    if (addr.is_constant(addr_as_uint32))
      addresses.insert(addr_as_uint32);
  }


  void
  Interpreter::CancelDS::next_addresses(std::set<unsigned int>& addresses, FCMemoryState& memory, DomainElementFunctions* def) const
  {
    /* We should never get here since delay slots should be merge with
       their previous instruction. */
    struct InternalError {}; throw InternalError();
  }
  
  void
  Interpreter::CancelDS::Repr(std::ostream& sink) const
  {
    sink << "CancelDS";
  }

  
}
