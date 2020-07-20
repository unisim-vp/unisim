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

  bool
  Interpreter::INode::requiresOriginValue(unisim::util::symbolic::Expr const& expr,
        FDIteration& iteration, RequirementLevel requirementLevel)
  {
    unsigned subcount = expr->SubCount();
    if (auto node = expr->AsConstNode())
      return true;
    else if (auto node = expr->AsOpNode())
      {
        using unisim::util::symbolic::Op;
        bool first = Interpreter::INode::requiresOriginValue(expr->GetSub(0), iteration, requirementLevel);
        if (subcount == 1)
          return first;
        else if (subcount == 2)
          {
            if (!first)
              return false;
            return Interpreter::INode::requiresOriginValue(expr->GetSub(1), iteration, requirementLevel);
          }
        
        { struct NotYet {}; throw NotYet(); }
      }
    else if (auto node = dynamic_cast<Interpreter::INode const*>(expr.node))
      {
        return node->requiresOriginValue(iteration, requirementLevel);
      }
    return true;
  }

  void
  Interpreter::INode::ComputeForward(unisim::util::symbolic::Expr const& expr,
                                     ComputationResult& res)
  {
    unsigned subcount = expr->SubCount();
    if (auto node = expr->AsConstNode())
      {
        ScalarType valtype( node->GetType() );
        if (valtype.is_integer)
          {
            if (valtype.bitsize == 1) {
              res.setBoolResultFromConstant(node->Get( bool() ));
              return;
            }
            uint64_t val = node->Get( uint64_t() );
            res.setMultiBitResultFromConstant(&val, valtype.bitsize, valtype.is_signed);
            return;
          }
        typedef long double float_container;
        res.setMultiFloatResultFromConstant(node->Get(float_container()), int(valtype.bitsize));
        return;
      }
    else if (auto node = expr->AsOpNode())
      {
        using unisim::util::symbolic::Op;
        Interpreter::INode::ComputeForward(expr->GetSub(0), res);
        if (subcount == 1)
          {
            switch (node->op.code)
              {
              case Op::Not:
                  if (ScalarType(expr.node->GetType()).bitsize == 1)
                     res.applyBitNegate();
                  else
                     res.applyMultiBitNegate();
                  return;
              case Op::Neg:
                  res.applyMultiBitOpposite();
              case Op::Cast:
                {
                  auto const& cnb = dynamic_cast<unisim::util::symbolic::CastNodeBase const&>( *expr.node );
                  ScalarType dst( cnb.GetType() ), src ( cnb.GetSrcType() );
                  if (dst.is_integer && src.is_integer)
                    res.applyMultiBitCast(src.bitsize > 1, dst.bitsize > 1, dst.is_signed, dst.bitsize);
                  else
                    { struct NotYet {}; throw NotYet(); }
                }
                return;
              default:
                break;
              }
            
          }
        else if (subcount == 2)
          {
            std::unique_ptr<ComputationResult> arg(res.Mutate());
            Interpreter::INode::ComputeForward(expr->GetSub(1), *arg);
            
            ScalarType valtype( node->GetType() );
            switch (node->op.code)
              {
              case Op::Xor:
                if (valtype.bitsize == 1)
                  res.applyBitExclusiveOr(*arg);
                else
                  res.applyMultiBitExclusiveOr(*arg);
                return;
              case Op::And:
                if (valtype.bitsize == 1)
                  res.applyBitAnd(*arg);
                else
                  res.applyMultiBitAnd(*arg);
                return;
              case Op::Or:
                if (valtype.bitsize == 1)
                  res.applyBitOr(*arg);
                else
                  res.applyMultiBitOr(*arg);
                return;
              case Op::Lsl: res.applyMultiBitLeftShift(*arg); return;
              case Op::Asr: res.applyMultiBitArithmeticRightShift(*arg); return;
              case Op::Lsr: res.applyMultiBitLogicalRightShift(*arg); return;
              case Op::Add:
                if (valtype.is_integer)
                  res.applyMultiBitPlusSigned(*arg);
                else
                  res.applyMultiFloatPlus(*arg);
                return;
              case Op::Sub:
                if (valtype.is_integer)
                  res.applyMultiBitMinusSigned(*arg);
                else
                  res.applyMultiFloatMinus(*arg);
                return;
              case Op::Teq:
                if (valtype.is_integer) {
                  if (valtype.bitsize == 1)
                    res.applyBitCompareEqual(*arg);
                  else
                    res.applyMultiBitCompareEqual(*arg);
                }
                else
                   res.applyMultiFloatCompareEqual(*arg);
                return;
              case Op::Tne:
                if (valtype.is_integer) {
                  if (valtype.bitsize == 1)
                    res.applyBitCompareDifferent(*arg);
                  else
                    res.applyMultiBitCompareDifferent(*arg);
                }
                else
                   res.applyMultiFloatCompareDifferent(*arg);
                return;
              case Op::Tge:
                if (valtype.is_integer)
                   res.applyMultiBitCompareGreaterOrEqualSigned(*arg);
                else
                   res.applyMultiFloatCompareGreaterOrEqual(*arg);
                return;
              case Op::Tgt:
                if (valtype.is_integer)
                   res.applyMultiBitCompareGreaterSigned(*arg);
                else
                   res.applyMultiFloatCompareGreater(*arg);
                return;
              case Op::Tle:
                if (valtype.is_integer)
                   res.applyMultiBitCompareLessOrEqualSigned(*arg);
                else
                   res.applyMultiFloatCompareLessOrEqual(*arg);
                return;
              case Op::Tlt:
                if (valtype.is_integer)
                   res.applyMultiBitCompareLessSigned(*arg);
                else
                   res.applyMultiFloatCompareLess(*arg);
                return;
              case Op::Tgeu: res.applyMultiBitCompareGreaterOrEqualUnsigned(*arg); return;
              case Op::Tgtu: res.applyMultiBitCompareGreaterUnsigned(*arg); return;
              case Op::Tleu: res.applyMultiBitCompareLessOrEqualUnsigned(*arg); return;
              case Op::Tltu: res.applyMultiBitCompareLessUnsigned(*arg); return;

              default: break;
              }
          }
        
        { struct NotYet {}; throw NotYet(); }
      }
    else if (auto node = dynamic_cast<Interpreter::INode const*>(expr.node))
      {
        node->ComputeForward(res);
        return;
      }

  }

  void
  Interpreter::Load::Repr(std::ostream& sink) const
  {
    sink << "Load( ";
    for (unsigned idx = 0; idx < SubCount(); ++idx)
      { GetSub(idx)->Repr(sink); sink << ", "; }
    sink << bytes << " )";
  }

  bool
  Interpreter::Load::requiresOriginValue(FDIteration& iteration, RequirementLevel requirementLevel) const
  {
    if (not INode::requiresOriginValue(addr, iteration, RLStore))
      return false;
    Interpreter::DebugIterationComputationResult address(iteration);
    INode::ComputeForward(addr, address);

    int sizeInBits = 0;
    int numberOfElements = 0;
    bool isConstantAddress = iteration.isConstant(address.res, sizeInBits);
    if (isConstantAddress
          || iteration.isConstantDisjunction(address.res, numberOfElements, sizeInBits))
      {
        FDScalarElement expressionValue(iteration.loadDisjunctiveValue(address.res, bytes*8));
        assert(expressionValue.getTypeInfo() == unisim::util::forbint::debug::TIMultiBit);
        sizeInBits = 0;
        numberOfElements = 0;
        if (!iteration.isConstant(expressionValue, sizeInBits)
              && !iteration.isConstantDisjunction(expressionValue, numberOfElements, sizeInBits))
           return iteration.requiresAddressValue(expressionValue, bytes*8, requirementLevel);
        else if (isConstantAddress && !iteration.isConstant(expressionValue, sizeInBits))
          iteration.storeValue(address.res, expressionValue);
      }
    return true;
  }

  void
  Interpreter::Load::ComputeForward(ComputationResult& res) const
  {
    INode::ComputeForward(addr, res);
    res.loadValue(bytes*8);
  }
  
  std::unique_ptr<Interpreter::SideEffect>
  Interpreter::Store::InterpretForward(ComputationResult& computationUnit) const
  {
    struct DoStore : public Interpreter::SideEffect
    {
      DoStore(unsigned _bytes, Expr _addr, Expr _value, ComputationResult& computationUnit)
        : addr(), value(), bytes(_bytes)
      {
        addr = std::unique_ptr<ComputationResult>(computationUnit.Mutate());
        value = std::unique_ptr<ComputationResult>(computationUnit.Mutate());
        INode::ComputeForward( _addr, *addr);
        INode::ComputeForward(_value, *value);
      }
      virtual void Commit(ComputationResult& res) override
      {
        res.storeValue(*addr, *value);
      }
      std::unique_ptr<ComputationResult> addr, value;
      unsigned bytes;
    };
    
    return std::make_unique<DoStore>(bytes,addr,value,computationUnit);
  }
  
  void
  Interpreter::RegRead::Repr(std::ostream& sink) const
  {
    sink << "RegRead( " << reg.c_str() << " )";
  }

  bool
  Interpreter::RegRead::requiresOriginValue(FDIteration& iteration, RequirementLevel requirementLevel) const
  {
    iteration.requiresRegisterValue(reg.idx(), requirementLevel);
  }

  void
  Interpreter::RegRead::ComputeForward(ComputationResult& res) const
  {
    res.getRegisterValue(reg.idx());
  }
  
  std::unique_ptr<Interpreter::SideEffect>
  Interpreter::RegWrite::InterpretForward(ComputationResult& computationUnit) const
  {
    struct DoRegWrite : public Interpreter::SideEffect
    {
      DoRegWrite(RegisterIndex _reg, Expr _value, ComputationResult& computationUnit)
        : value(), reg(_reg)
      {
        value = std::unique_ptr<ComputationResult>(computationUnit.Mutate());
        INode::ComputeForward(_value, *value);
      }
      virtual void Commit(ComputationResult& res) override
      {
        res.setRegisterValue(reg.idx(), *value);
      }
      std::unique_ptr<ComputationResult> value;
      RegisterIndex reg;
    };
    
    return std::make_unique<DoRegWrite>(reg, value, computationUnit);
  }
  
  void
  Interpreter::Goto::Repr(std::ostream& sink) const
  {
    sink << "Goto( ";
    for (unsigned idx = 0; idx < SubCount(); ++idx)
      { GetSub(idx)->Repr(sink); sink << ", "; }
    sink << unsigned(btype) << " )";
  }

}
