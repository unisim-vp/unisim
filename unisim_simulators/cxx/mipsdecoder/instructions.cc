#include "instructions.hh"
#include "architecture.hh"
#include <unisim/component/cxx/processor/mips/isa/mipsel.tcc>
#include <unisim/util/endian/endian.hh>
#include <sstream>
#include <memory>
#include <string>

namespace Mips
{
  void flushout() { std::cout.flush(); }
  const char* print_instr(const Instruction& instr)
    { std::ostringstream out;
      instr.disasm(out);
      static std::string res;
      res = out.str();
      return res.c_str();
    }

  Decoder::Decoder()
  {
  }
  
  typedef unsigned LocalLabel; // persistent unique id among the brothers of an ActionNode ("fratrie" in french)

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
          auto* next = coderoot->getnext(alt);
          if (next->updates.size() == 0)
            continue;
          if (next->updates.size() > 2) // an update may exist after the branch
            throw BadCFG();
          if (auto b = dynamic_cast<Interpreter::Goto const*>(next->updates.begin()->node))
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

    bool next_addresses(std::set<uint32_t>& addresses,
                        Interpreter::FCMemoryState& mem, DomainElementFunctions* def) const override
    {
      if (not branch.cond.good() and not branch.dest.good())
        {
          addresses.insert(operation->GetAddr() + size);
        }
      else if (not branch.cond.good()) // branch.dest.good()
        {
          if (auto cnb = branch.dest->AsConstNode())
            {
              addresses.insert(cnb->Get(uint32_t()));
            }
          else
            {
              Interpreter::ContractComputationResult addr(mem, def);
              Interpreter::INode::ComputeForward(branch.dest, addr);
              return addr.res.retrieve_constant_values(addresses);
            }
        }
      else // branch.cond.good()
        {
          assert(branch.dest.good());
          auto cnb = branch.dest->AsConstNode();
          assert(cnb);
          uint64_t elseAddress = cnb->Get(uint32_t());
          uint64_t thenAddress = operation->GetAddr() + 4;

          Interpreter::ContractComputationResult addr(mem, def);
          Interpreter::INode::ComputeForward(branch.cond, addr);
          if (addr.res.mayBeDifferentZero())
             addresses.insert(thenAddress);
          if (addr.res.mayBeZero())
             addresses.insert(elseAddress);
        }
      return true;
    }

    virtual bool interpret( uint32_t& addr, uint32_t next_addr,
                            Interpreter::FCMemoryState& mem, DomainElementFunctions* def) const override
    {
      uint32_t addrs[2] = {operation->GetAddr() + size, 0};
      
      struct Ouch {};
      bool result = next_addr == addrs[0];
      addr = addrs[0];
      if (branch.cond.good())
        {
          bool bt_known = false, has_cond_true = false, has_cond_false = false;
          { Interpreter::ContractComputationResult addr2(mem, def);
            Interpreter::INode::ComputeForward(branch.cond, addr2);
            has_cond_true = addr2.res.mayBeDifferentZero();
            has_cond_false = addr2.res.mayBeZero();
          }
          if (branch.dest.good()) {
            Interpreter::ContractComputationResult addr2(mem, def);
            Interpreter::INode::ComputeForward(branch.dest, addr2);
            bt_known = addr2.res.is_constant(addrs[1]);
            if (bt_known && has_cond_false && (!has_cond_true || !result)) {
               result = next_addr == addrs[1];
               addr = addrs[1];
            }
          }

          bool cond = false;
          if (has_cond_true && has_cond_false && bt_known) {
            if (     next_addr == addrs[1]) cond = true;
            else if (next_addr == addrs[0]) cond = false;
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
        }   
      else if (branch.dest.good())
        {
          // if (not bt_known or next_addr != addrs[1])
          //   throw Ouch();
        }
      else
        {
          // only the last instruction of a sequence reaches next_addr
          // if (next_addr != addrs[0])
          //   throw Ouch();
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
      return result;
    }

    void
    addDynamicTarget(Interpreter::branch_type_t type, uint64_t targetValue,
          Interpreter::FDIteration& iteration, Interpreter::FDIteration::TargetCursor& nextCursor,
          Interpreter::FDIteration::CalleeCursor& callCursor,
          Interpreter::FDIteration::Target& target, Interpreter::FDIteration::Exit& exit) const {
       if (type == Interpreter::B_JMP || type == Interpreter::B_EXC
             || type == Interpreter::B_DBG)
         {
           bool hasFound = false;
           if (target.isValid() && target.hasAddress(&targetValue, 1))
             {
               iteration.addIdentifiedTarget(target, 1);
               hasFound = true;
             }
           if (!hasFound && target.isValid() && nextCursor.isValid())
             {
               int index = 0;
               while (!hasFound && nextCursor.setToNext()) {
                 Interpreter::FDIteration::Target nextTarget = nextCursor.elementAt();
                 if (nextTarget.hasAddress(&targetValue, 1)) {
                   iteration.addIdentifiedTarget(nextTarget, index);
                   hasFound = true;
                 };
                 ++index;
               };
             }
           if (!hasFound)
              iteration.addIdentifiedTarget(&targetValue, 1, 1);
         }
       else if (type == Interpreter::B_CALL) {
          bool hasFound = false;
          if (target.isValid() && target.hasAddress(&targetValue, 1)) {
             // _iteration->addIdentifiedTarget(target, index);
             iteration.addTarget(target);
             hasFound = true;
          }
          if (!hasFound && target.isValid() && callCursor.isValid()) {
             while (!hasFound && callCursor.setToNext()) {
                Interpreter::FDIteration::Target callTarget = callCursor.elementAt();
                if (callTarget.hasAddress(&targetValue, 1)) {
                   // _iteration->addIdentifiedTarget(callTarget, index);
                   iteration.addTarget(callTarget);
                   hasFound = true;
                };
             };
          }
          if (!hasFound)
             // _iteration->addIdentifiedTarget(targetValue, 1, index);
             iteration.addTarget(&targetValue, 1);
       }
       else if (type == Interpreter::B_RET || type == Interpreter::B_RFE) {
          bool hasFound = false;
          if (nextCursor.isValid() || exit.isValid()) {
             Interpreter::FDIteration::Exit newExit = exit;
             do {
                if (nextCursor.isValid()) {
                   if (nextCursor.setToNext())
                      newExit = nextCursor.exitAt();
                   else
                      break;
                };
                Interpreter::FDIteration::Target returnTarget = iteration.advanceOnExitReturn(newExit);
                if ((hasFound = returnTarget.isValid()) != false
                      && returnTarget.hasAddress(&targetValue, 1))
                   iteration.addTarget(returnTarget);
             } while (!hasFound && nextCursor.isValid());
          };
          if (!hasFound)
             iteration.addTarget(&targetValue, 1);
       };
    }

    virtual void retrieveTargets(Interpreter::FDIteration& iteration) const override
    {
      if (not branch.cond.good() and not branch.dest.good())
        iteration.addNextTarget();
      else if (not branch.cond.good()) // branch.dest.good()
        {
          typedef Interpreter::FDIteration Iteration;
          typedef Interpreter::FDScalarElement ScalarElement;
          typedef Interpreter::FDMemoryFlags Flags;
          Iteration::TargetCursor nextCursor;
          Iteration::CalleeCursor callCursor;
          Iteration::Target target;
          Iteration::Exit exit;

          if (auto cnb = branch.dest->AsConstNode())
            {
              if (iteration.mayFollowGraph()) {
                if (!iteration.localAdvanceOnInstruction()) {
                  if (!iteration.advanceOnCallInstruction() && !iteration.advanceOnNextGraphInstruction()) {
                    iteration.assumeAcceptEmptyDestination();
                  };
                }
              }
              else {
                uint64_t address = cnb->Get(uint32_t());
                iteration.addTarget(&address, 1);
              }
            }
          else
            {
              if (iteration.mayFollowGraph())
                {
                  switch (branch.type)
                    {
                    case Interpreter::B_JMP: case Interpreter::B_EXC: case Interpreter::B_DBG:
                      if (not (target = iteration.getLocalTarget()).isValid())
                        nextCursor = iteration.newCursorOnExistingTargets();
                      break;
                    case Interpreter::B_CALL:
                      if (not (target = iteration.getLocalTarget()).isValid())
                        callCursor = iteration.newCursorOnExistingCallees();
                      break;
                    case Interpreter::B_RET: case Interpreter::B_RFE:
                      if (!(exit = iteration.advanceOnExit()).isValid()) {
                        if (!(target = iteration.getLocalTarget()).isValid())
                          nextCursor = iteration.newCursorOnExistingTargets();
                      };
                      break;
                    default:
                      assert(false);
                    }
                }

                Interpreter::DebugIterationComputationResult destination(iteration);
                Interpreter::INode::ComputeForward(branch.dest, destination);
                int sizeInBits = 0;
                int numberOfElements = 0;
                if (!iteration.isConstant(destination.res, sizeInBits)
                      && !iteration.isConstantDisjunction(destination.res, numberOfElements, sizeInBits)) {
                  if (not Interpreter::INode::requiresOriginValue(branch.dest, iteration, RLJump)) {
                    iteration.notifyUndefinedTarget();
                    return;
                  }
                  destination.clear();
                  Interpreter::INode::ComputeForward(branch.dest, destination);
                }
                if (iteration.isConstant(destination.res, sizeInBits)) {
                  iteration.notifyDynamicTargets(destination.res);
                  uint64_t targetValue = 0UL;
                  iteration.retrieveConstant(destination.res, &targetValue, 1);
                  if (targetValue != 0U)
                    addDynamicTarget(branch.type, targetValue, iteration,
                          nextCursor, callCursor, target, exit);
                }
                else if (iteration.isConstantDisjunction(destination.res, numberOfElements, sizeInBits))
                {
                  iteration.notifyDynamicTargets(destination.res);
                  assert(numberOfElements > 0 && sizeInBits > 0 /* == 32 */);
                  int sizeTargetInCell = (sizeInBits+sizeof(uint64_t)*8-1)
                        /(sizeof(uint64_t)*8);
                  assert(sizeTargetInCell == 1);
                  uint64_t* targetResult = new uint64_t[numberOfElements*sizeTargetInCell];
                  uint64_t** tab = new uint64_t*[numberOfElements];
                  for (int i = 0; i < numberOfElements; ++i)
                    tab[i] = &targetResult[i*sizeTargetInCell];
                  iteration.retrieveConstantDisjunction(destination.res, tab, sizeTargetInCell,
                        numberOfElements);
                  for (int i = 0; i < numberOfElements; ++i) {
                    if (targetResult[i*sizeTargetInCell]) {
                      addDynamicTarget(branch.type, targetResult[i*sizeTargetInCell], iteration,
                            nextCursor, callCursor, target, exit);
                    };
                  }
                }
                else
                   iteration.notifyUndefinedTarget();
            }
        }
      else // branch.cond.good()
        {
          assert(branch.dest.good());
          auto cnb = branch.dest->AsConstNode();
          assert(cnb);
          uint64_t elseAddress = cnb->Get(uint32_t());
          uint64_t thenAddress = operation->GetAddr() + 4;
          // iteration.getCurrentInstruction(&thenAddress, 1);
          // thenAddress += 4;

          Interpreter::FDIteration::Target firstTarget, thenTarget, elseTarget;
          if (iteration.mayFollowGraph())
            {
              if (!(firstTarget = iteration.getLocalTarget()).isValid())
                {
                  Interpreter::FDIteration::TargetCursor targetCursor
                      = iteration.newCursorOnExistingTargets();
                  while (targetCursor.setToNext())
                    {
                      bool isFirst = false, isThen = false, isElse = false;
                      Interpreter::FDIteration::Target target
                         = targetCursor.elementAt(isFirst, isThen, isElse);
                      if (isFirst)
                        firstTarget = target;
                      if (isThen)
                        thenTarget = target;
                      if (isElse)
                        elseTarget = target;
                    }
                }
            }

          Interpreter::DebugIterationComputationResult condition(iteration);
          Interpreter::INode::ComputeForward(branch.cond, condition);
          if (condition.res.queryZeroResult().mayBeDifferentZero())
            {
              if (firstTarget.isValid() && firstTarget.hasAddress(&elseAddress, 1))
                elseTarget = firstTarget;
              if (elseTarget.isValid())
                iteration.addElseTarget(elseTarget);
              else
                iteration.addElseTarget(&elseAddress, 1);
            }
          if (condition.res.queryZeroResult().mayBeZero())
            {
              if (!thenTarget.isValid() && firstTarget.isValid() && firstTarget.hasAddress(&thenAddress, 1))
                thenTarget = firstTarget;
              if (thenTarget.isValid())
                iteration.addThenTarget(thenTarget);
              else
                iteration.addThenTarget(&thenAddress, 1);
            }
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
    }

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
        // std::cout << print_instr(*insn);
        // flushout();
      }

    return insn.release();
  }
}
