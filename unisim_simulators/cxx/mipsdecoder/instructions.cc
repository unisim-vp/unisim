#include "instructions.hh"
#include "architecture.hh"
#include <unisim/component/cxx/processor/mips/isa/mipsel.tcc>
#include <unisim/util/endian/endian.hh>
#include <sstream>
#include <memory>
#include <string>

namespace Mips
{
  Decoder::Decoder()
  {
  }
  
  struct FullInstruction : public Instruction
  {
    typedef unisim::component::cxx::processor::mips::isa::Operation<Mips::Interpreter> Operation;
    
    FullInstruction( std::shared_ptr<Operation>&& op, unsigned _size )
      : operation(std::move(op)), actions(), branch(), size(_size)
    {
      // Get actions
      std::unique_ptr<Interpreter::ActionNode> coderoot = std::make_unique<Interpreter::ActionNode>();
      Interpreter reference(operation->GetAddr(), coderoot.get());
      for (bool end = false; not end;)
        {
          Interpreter state(operation->GetAddr(), coderoot.get());
          operation->execute( state );
          end = state.close( reference );
        }
      
      coderoot->simplify();
      std::swap(actions, coderoot->updates);

      struct BadCFG {};
      //uint32_t cont = operation->GetAddr() + size;

      if (not coderoot->cond.good())
        {
          for (auto itr = actions.begin(), end = actions.end(); itr != end;)
            {
              if (auto b = dynamic_cast<Interpreter::Goto const*>(itr->node))
                {
                  if (branch.dest.good())
                    throw BadCFG();
                  branch.dest = b->target;
                  itr = actions.erase(itr);
                }
              else
                { ++itr; }
            }
          return;
        }

      for (unsigned alt = 0; alt < 2; ++alt)
        {
          if (coderoot->getnext(alt)->cond.good())
            throw BadCFG();
          if (coderoot->updates.size() == 0)
            continue;
          if (coderoot->updates.size() != 1)
            throw BadCFG();
          if (auto b = dynamic_cast<Interpreter::Goto const*>(coderoot->updates.begin()->node))
            {
              if (branch.dest.good())
                throw BadCFG();
              branch.dest = b->target;
            }
          else
            throw BadCFG();
        }
      
      if (not branch.dest.good())
        throw BadCFG();
    }
    
    virtual void disasm(std::ostream& sink) const override { operation->disasm(sink); }

    virtual bool match( uint32_t const* words ) const override { return operation->GetEncoding() == words[0]; }

    void get_family(Interpreter::FDIteration::FamilyInstruction& family) const
    {
      if (not branch.dest.good())
        family.setSequential();
      else
        {
          switch (branch.type)
            {
            case Interpreter::B_JMP:
              {
                unisim::util::symbolic::Expr mt(branch.dest);
                if (auto addr = mt.ConstSimplify())
                  {
                    uint32_t destination = addr->Get( uint32_t() ), origin = operation->GetAddr();
                    if (destination > origin)
                      family.setForwardJump();
                    else
                      family.setBackwardJump();
                  }
                else
                  family.setComputedJump();
              } break;
            case Interpreter::B_CALL: family.setCall(); break;
            case Interpreter::B_RET:  family.setReturn(); break;
            case Interpreter::B_EXC:  family.setBranch(); break;
            case Interpreter::B_DBG:  family.setBranch(); break;
            case Interpreter::B_RFE:  family.setReturn(); break;
            }
              
        }
    }

    unsigned get_family() const override
    {
      Interpreter::FDIteration::FamilyInstruction result;
      get_family(result);
      return result.getComputedFamily();
    }
    
    Instruction* clone() const override { return new FullInstruction(*this); }

    bool compute_branch_target( uint32_t& address, Interpreter::FCMemoryState& mem,
          DomainElementFunctions* def) const
    {
      Interpreter::ContractComputationResult addr(mem, def);
      Interpreter::INode::ComputeForward(branch.dest, addr);
      return addr.res.is_constant(address);
    }

    void next_addresses(std::set<unsigned int>& addresses,
                        Interpreter::FCMemoryState& mem, DomainElementFunctions* def) const override
    {
      if (branch.cond.good() or not branch.dest.good())
        {
          addresses.insert(operation->GetAddr() + size);
        }
      
      if (branch.dest.good())
        {
          uint32_t addr_as_uint32 = 0;
          if (compute_branch_target(addr_as_uint32, mem, def))
            addresses.insert(addr_as_uint32);
          else
            {
              /* Should inform that a target wasn't computable in current
               * memory state.
               */
            }
        }
    }

    virtual void interpret( uint32_t addr, uint32_t next_addr,
                            Interpreter::FCMemoryState& mem, DomainElementFunctions* def) const override
    {
      uint32_t addrs[2] = {operation->GetAddr() + size, 0};
      bool bt_known = false;
      if (branch.dest.good())
        bt_known = compute_branch_target(addrs[1], mem, def);

      struct Ouch {};
      
      if (branch.cond.good())
        {
          bool cond = false;
          if (bt_known and next_addr == addrs[1]) cond = true;
          else if (        next_addr == addrs[0]) cond = false;
          else throw Ouch();

          if (auto comp = branch.cond->AsOpNode())
            {
              using unisim::util::symbolic::Op;

              if (comp->SubCount() != 2)
                throw Ouch();
              
              /* Getting compared registers (note 0 is effectively r0
               * which is a zeroed register)*/
              unsigned regs[2] = {0,0};
              for (unsigned alt = 0; alt < 2; ++alt)
                {
                  auto const& op = comp->GetSub(alt);
                  if (auto z = op->AsConstNode())
                    {
                      if (z->Get(uint32_t()) != 0) throw Ouch(); /* in mips comparisons, constant is always zero */
                      regs[alt] = 0;
                    }
                  else if (Interpreter::RegRead const* reg = dynamic_cast<Interpreter::RegRead const*>( op.node ))
                    {
                      regs[alt] = reg->reg.idx();
                    }
                  else
                    throw Ouch();
                }

              /* TODO: need to signal constraint on register operands (0 is the zero constant) */
              switch (comp->op.code)
                {
                default:
                  throw Ouch();
                case Op::Teq: /* operands are equals if cond is true */
                  std::cout << regs[0] << " ~~ " << regs[1] << cond << std::endl;
                  break;
                case Op::Tne: /* operands are not equals if cond is true */
                  std::cout << regs[0] << " ~~ " << regs[1] << cond << std::endl;
                  break;

                  /*in the following, one operand should be 0 (most probably the second) */
                case Op::Tgt: /* operand0 is greater than operand1 (signed) */
                  std::cout << regs[0] << " ~~ " << regs[1] << cond << std::endl;
                  break;
                case Op::Tge: /* operand0 is greater or equal to operand1 (signed) */
                  std::cout << regs[0] << " ~~ " << regs[1] << cond << std::endl;
                  break;
                case Op::Tlt: /* operand0 is less than operand1 (signed) */
                  std::cout << regs[0] << " ~~ " << regs[1] << cond << std::endl;
                  break;
                case Op::Tle: /* operand0 is less or equal to operand1 (signed) */
                  std::cout << regs[0] << " ~~ " << regs[1] << cond << std::endl;
                  break;
                  
                }
            }
          else
            {
              throw Ouch();
            }
        }      
      else if (branch.dest.good())
        {
          if (not bt_known or next_addr != addrs[1])
            throw Ouch();
        }
      else
        {
          if (next_addr != addrs[0])
            throw Ouch();
        }
      
      Interpreter::ContractComputationResult computationUnit(mem, def);
      std::vector<std::unique_ptr<Interpreter::SideEffect>> side_effects;
      side_effects.reserve(actions.size());
      for (auto const& action : actions)
        if (auto update = dynamic_cast<Interpreter::Update const*>(action.node))
          side_effects.push_back(update->InterpretForward(computationUnit));
        else
          { struct NotAnUpdate {}; throw NotAnUpdate(); }
      for (auto const& side_effect : side_effects)
        side_effect->Commit(computationUnit);
    }

    virtual void retrieveTargets(Interpreter::FDIteration& iteration) const override
    {
      if (branch.cond.good() or not branch.dest.good())
        iteration.addNextTarget();
      else if (not branch.cond.good())
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
              if (auto cnb = branch.dest->AsConstNode())
                {
                  uint64_t address = cnb->Get(uint32_t());
                  iteration.addTarget(&address, 1);
                }
              else
                {
                  /* Indirect branch are not handled yet */
                  typedef Interpreter::FDIteration Iteration;
                  typedef Interpreter::FDScalarElement ScalarElement;
                  typedef Interpreter::FDMemoryFlags Flags;
                  Interpreter::DebugIterationComputationResult destination(iteration);
                  Interpreter::INode::ComputeForward(branch.dest, destination);

                  struct TODO {}; throw TODO();
                }
            }
        }
      else
        {
          typedef Interpreter::FDIteration Iteration;
          typedef Interpreter::FDScalarElement ScalarElement;
          Iteration::Target firstTarget, thenTarget, elseTarget;
          if (iteration.mayFollowGraph())
            {
              if (/*_isMultitarget*/false)
                {
                  if (!(firstTarget = iteration.getLocalTarget()).isValid())
                    {
                      Iteration::TargetCursor targetCursor = iteration.newCursorOnExistingTargets();
                      while (targetCursor.setToNext())
                        {
                          bool isFirst = false, isThen = false, isElse = false;
                          Iteration::Target target = targetCursor.elementAt(isFirst, isThen, isElse);
                          if (isFirst)
                            firstTarget = target;
                          if (isThen)
                            thenTarget = target;
                          if (isElse)
                            elseTarget = target;
                        }
                      if (/*_doesStoreNext*/false and not iteration.advanceOnCallInstruction() and not iteration.advanceOnNextGraphInstruction())
                           {
                          iteration.assumeAcceptEmptyDestination();
                        }
                    }
                }
              else
                {
                  if (!iteration.localAdvanceOnInstruction())
                    {
                      if (/*_doesStoreNext*/false and not iteration.advanceOnCallInstruction() and not iteration.advanceOnNextGraphInstruction())
                        {
                          iteration.assumeAcceptEmptyDestination();
                        }
                    }
                }
            }

          // ComputeForward currently works with an Interpret
          // architecture, but here we work with a Iteration
          // architecture. We need to work on a common abstraction
          // layer... (most probably a virtual ScalarElement Computer)
          struct TODO {}; throw TODO();
          
          ScalarElement se_cond/* = Interpreter::INode::ComputeForward(cond, iteration)*/;
          // ScalarElement target;
  
          // if (se_cond.queryZeroResult().mayBeDifferentZero())
          //   {
          //     uint64_t elseAddress = 0;
          //     if (/*_isAbsoluteTarget*/false)
          //       elseAddress = _target;
          //     else
          //       {
          //         iteration.getCurrentInstruction(&elseAddress, 1);
          //         if (elseAddress + _target == 0x10000000)
          //           return;
          //         elseAddress += _target;
          //       }
          //     if (firstTarget.isValid() && firstTarget.hasAddress(&elseAddress, 1))
          //       elseTarget = firstTarget;
          //     if (elseTarget.isValid())
          //       iteration.addElseTarget(elseTarget);
          //     else
          //       iteration.addElseTarget(&elseAddress, 1);
          //   }
          // if (se_cond.queryZeroResult().mayBeZero())
          //   {
          //     uint64_t thenAddress = 0;
          //     iteration.getCurrentInstruction(&thenAddress, 1);
          //     // iteration.retrieveCurrentAddress(&thenAddress, 1);
          //     thenAddress += 4;
          //     if (!thenTarget.isValid() && firstTarget.isValid() && firstTarget.hasAddress(&thenAddress, 1))
          //       thenTarget = firstTarget;
          //     if (thenTarget.isValid())
          //       iteration.addThenTarget(thenTarget);
          //     else
          //       iteration.addThenTarget(&thenAddress, 1);
          //   }
        }

      if (iteration.isFamilyRequired())
        {
          Interpreter::FDIteration::FamilyInstruction family;
          get_family(family);
          iteration.setFamily(family);
        }
    }
    
    virtual unsigned getSize() const override { return size; }
    virtual void interpretForward(uint32_t addr,
                                  Interpreter::FDMemoryState& ms,
                                  Interpreter::FDTarget& tg,
                                  Interpreter::FDMemoryFlags& mf) const override
    {
      std::vector<std::unique_ptr<Interpreter::SideEffect>> side_effects;
      side_effects.reserve(actions.size());
      Interpreter::DebugMemoryComputationResult computationUnit(ms, mf);
      for (auto const& action : actions)
        if (auto update = dynamic_cast<Interpreter::Update const*>(action.node))
          side_effects.push_back(update->InterpretForward(computationUnit));
        else
          { struct NotAnUpdate {}; throw NotAnUpdate(); }
      for (auto const& side_effect : side_effects)
        side_effect->Commit(computationUnit);
    };

    std::shared_ptr<Operation> operation;
    std::set<unisim::util::symbolic::Expr> actions;
    struct
    {
      unisim::util::symbolic::Expr cond;
      unisim::util::symbolic::Expr dest;
      Interpreter::branch_type_t   type;
    } branch;
    unsigned size;
  };
    
  Instruction*
  Decoder::decode(uint32_t const* words, uint32_t addr)
  {
    static unisim::component::cxx::processor::mips::isa::Decoder<Mips::Interpreter> idecoder;

    typedef std::shared_ptr<FullInstruction::Operation> op_ptr;
    FullInstruction::Operation* op = idecoder.NCDecode(addr, unisim::util::endian::Host2LittleEndian(words[0]));
    std::unique_ptr<FullInstruction> insn = std::make_unique<FullInstruction>( op_ptr(op), 4 );

    if (insn->branch.dest.good())
      {
        struct DBOp : public FullInstruction::Operation
        {
          typedef FullInstruction::Operation Operation;
          DBOp( op_ptr&& first, op_ptr&& last )
            : Operation( first->GetEncoding(), first->GetAddr(), "DBOp" )
            , ops{std::move(first), std::move(last)}
          {}
          virtual void execute( Interpreter& arch ) const override
          {
            ops[0]->execute( arch );
            if (arch.in_delay_slot) /* if true, delay slot has been canceled */
              return;
            arch.in_delay_slot = true;
            arch.current_address = Interpreter::U32(ops[1]->GetAddr());
            ops[1]->execute( arch );
          }
          virtual void disasm(std::ostream& sink) const override
          {
            ops[0]->disasm( sink );
            sink << " ;  ";
            ops[1]->disasm( sink );
          }
          
          op_ptr ops[2];
        };
        op = idecoder.NCDecode(addr + 4, unisim::util::endian::Host2LittleEndian(words[1]));
        op = new DBOp( std::move(insn->operation), op_ptr(op) );
        insn = std::make_unique<FullInstruction>( op_ptr(op), 8 );
      }

    return insn.release();
  }
}
