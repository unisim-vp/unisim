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
      : operation(std::move(op)), actions(), cfg(), size(_size)
    {
      // Get actions
      std::unique_ptr<Interpreter::ActionNode> coderoot = std::make_unique<Interpreter::ActionNode>();
      Interpreter reference(op->GetAddr(), coderoot.get());
      for (bool end = false; not end;)
        {
          Interpreter state(op->GetAddr(), coderoot.get());
          operation->execute( state );
          end = state.close( reference );
        }
      
      coderoot->simplify();
      std::swap(actions, coderoot->updates);

      struct BadCFG {};
      uint32_t cont = operation->GetAddr() + size;
      struct Cont : Interpreter::Ctrl
      {
        Cont(uint32_t _addr) : addr(_addr) {} uint32_t addr;
        virtual Cont* Mutate() const override { return new Cont(*this); }
        virtual unsigned SubCount() const { return 0; }
        virtual void Repr( std::ostream& sink ) const override { sink << "Cont( " << addr << " )"; }
        virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Cont const&>(rhs) ); }
        int compare( Cont const& rhs ) const { return (addr > rhs.addr) ? 1 : addr < rhs.addr ? -1 : 0; }
        /**DEBUG**/
        virtual void retrieveFamily( Interpreter::FDIteration::FamilyInstruction& f, uint32_t ) const override
        { f.setForwardJump(); }
        virtual void retrieveTargets( Interpreter::FDIteration& iteration ) const override
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
              uint64_t address = addr;
              iteration.addTarget(&address, 1);
            }
          // if (iteration.isFamilyRequired())
          //   {
          //     Interpreter::FDIteration::FamilyInstruction family;
          //     retrieveFamily(family, NULL);
          //     iteration.setFamily(family);
          //   }
        }
        /**CONTRACT**/
        void next_addresses(std::set<unsigned int>& addresses, Interpreter::FCMemoryState&, DomainElementFunctions*) const override
        { addresses.insert(addr); }
      };
      

      if (not coderoot->cond.good())
        {
          for (auto itr = actions.begin(), end = actions.end(); itr != end;)
            {
              if (not dynamic_cast<Interpreter::Ctrl const*>(itr->node))
                { ++itr; continue; }
              if (cfg.good())
                throw BadCFG();
              cfg = *itr;
              itr = actions.erase(itr);
            }
          if (not cfg.good())
            cfg = new Cont(cont);
          return;
        }

      for (unsigned alt = 0; alt < 2; ++alt)
        {
          if (coderoot->getnext(alt)->cond.good())
            throw BadCFG();
          if (coderoot->updates.size() == 0)
            continue;
          if (coderoot->updates.size() != 1 or not dynamic_cast<Interpreter::Ctrl const*>(coderoot->updates.begin()->node))
            throw BadCFG();
        }

      struct Fork : Interpreter::Ctrl
      {
        Fork(Interpreter::ActionNode& root)
          : cond(root.cond), nexts()
        {
          for (unsigned alt = 0; alt < 2; ++alt)
            for (auto const& ctrl : root.getnext(alt)->updates)
              nexts[alt] = ctrl;
        }
        virtual Fork* Mutate() const override { return new Fork(*this); }

        virtual unsigned SubCount() const { return 3; }
        virtual Expr const& GetSub(unsigned idx) const override
        { switch (idx) { case 0: case 1: return nexts[idx]; case 2: return cond; } return ExprNode::GetSub(idx); }
        virtual void Repr( std::ostream& sink ) const override
        {
          sink << "Fork( ";
          char const* sep = "";
          for (unsigned idx = 0; idx < SubCount(); ++idx, sep = ", ")
            GetSub(idx)->Repr(sink << sep);
          sink << " )";
        }
        virtual int cmp( ExprNode const& rhs ) const override { return 0; }
        
        virtual void retrieveFamily( Interpreter::FDIteration::FamilyInstruction& f, uint32_t origin ) const override
        {
          dynamic_cast<Interpreter::Ctrl const&>( *nexts[0] ).retrieveFamily( f, origin );
          dynamic_cast<Interpreter::Ctrl const&>( *nexts[1] ).retrieveFamily( f, origin );
        }
        virtual void retrieveTargets( Interpreter::FDIteration& iteration ) const override
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

          // if (doesSupportSyntacticCondition)
          //   {
          //     if (!_previousInstruction)
          //       _previousInstruction = (BasicInstruction*) iteration.getPreviousInstruction(&_previousInstructionAddress, 1);
          //   }

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
  
          // if (iteration.isFamilyRequired())
          //   {
          //     Iteration::FamilyInstruction family;
          //     retrieveFamilyInstruction(family, NULL);
          //     iteration.setFamily(family);
          //   }
        }
        
        /**CONTRACT**/
        virtual void next_addresses(std::set<unsigned int>& addresses,
                                    Interpreter::FCMemoryState& mem, DomainElementFunctions* def) const override
        {
          Interpreter::FCDomainValue then = Interpreter::INode::Compute(cond, mem, def);
          auto zr = then.queryZeroResult();
          if (zr != ZRZero)           dynamic_cast<Interpreter::Ctrl const&>( *nexts[1] ).next_addresses(addresses, mem, def);
          if (zr != ZRDifferentZero)  dynamic_cast<Interpreter::Ctrl const&>( *nexts[0] ).next_addresses(addresses, mem, def);
        }
        
        unisim::util::symbolic::Expr cond, nexts[2];
      };

      cfg = unisim::util::symbolic::Expr( new Fork( *coderoot ) );
    }
    
    virtual void disasm(std::ostream& sink) const override { operation->disasm(sink); }

    virtual bool match( uint32_t const* words ) const override { return operation->GetEncoding() == words[0]; }

    unsigned get_family() const override
    {
      Interpreter::FDIteration::FamilyInstruction result;
      if (not cfg.good())
        result.setSequential();
      else
        dynamic_cast<Interpreter::Ctrl const&>( *cfg ).retrieveFamily(result, operation->GetAddr());

      return result.getComputedFamily();
    }
    
    Instruction* clone() const override { return new FullInstruction(*this); }

    void next_addresses(std::set<unsigned int>& addresses,
                        Interpreter::FCMemoryState& mem, DomainElementFunctions* def) const override
    {
      if (not cfg.good())
        addresses.insert(operation->GetAddr() + size);
      else
        dynamic_cast<Interpreter::Ctrl const&>(*cfg).next_addresses(addresses, mem, def);
      
      //DomainValue cond = INode::compute();
      //   DomainValue value = 
      //     bool isConstant(bool* value) const
      // {
      //   if (inherited::isValid())
      //     return (*functionTable().bit_is_constant_value)(this->value(), value);
      //   else
      //   {
      //     if (value)
      //       *value = fConstant;
      //     return true;
      //   }
      // }

    }

    virtual void interpret( uint32_t addr, uint32_t next_addr,
                            Interpreter::FCMemoryState& mem, DomainElementFunctions* def) const override
    {
      std::vector<std::unique_ptr<Interpreter::FCSideEffect>> side_effects;
      side_effects.reserve(actions.size());
      for (auto const& action : actions)
        if (auto update = dynamic_cast<Interpreter::Update const*>(action.node))
          side_effects.push_back(update->Interpret(mem, def));
        else
          { struct NotAnUpdate {}; throw NotAnUpdate(); }
      for (auto const& side_effect : side_effects)
        side_effect->Commit(mem, def);
    }

    virtual void retrieveTargets(Interpreter::FDIteration& iteration) const override
    {
      if (not cfg.good())
        iteration.addNextTarget();
      else
        dynamic_cast<Interpreter::Ctrl const&>( *cfg ).retrieveTargets(iteration);

      // TODO

      if (iteration.isFamilyRequired())
        {
          Interpreter::FDIteration::FamilyInstruction family;
          if (cfg.good()) dynamic_cast<Interpreter::Ctrl const&>( *cfg ).retrieveFamily(family, operation->GetAddr());
          else            family.setSequential();
          iteration.setFamily(family);
        }
    }
    virtual unsigned getSize() const override { return size; }
    virtual void interpretForward(uint32_t addr,
                                  Interpreter::FDMemoryState& ms,
                                  Interpreter::FDTarget& tg,
                                  Interpreter::FDMemoryFlags& mf) const override
    {
      std::vector<std::unique_ptr<Interpreter::FDSideEffect>> side_effects;
      side_effects.reserve(actions.size());
      for (auto const& action : actions)
        if (auto update = dynamic_cast<Interpreter::Update const*>(action.node))
          side_effects.push_back(update->InterpretForward(ms, tg, mf));
        else
          { struct NotAnUpdate {}; throw NotAnUpdate(); }
      for (auto const& side_effect : side_effects)
        side_effect->Commit(ms, tg, mf);
    };

    std::shared_ptr<Operation> operation;
    std::set<unisim::util::symbolic::Expr> actions;
    unisim::util::symbolic::Expr cfg;
    unsigned size;
  };
    
  Instruction*
  Decoder::decode(uint32_t const* words, uint32_t addr)
  {
    static unisim::component::cxx::processor::mips::isa::Decoder<Mips::Interpreter> idecoder;

    typedef std::shared_ptr<FullInstruction::Operation> op_ptr;
    FullInstruction::Operation* op = idecoder.NCDecode(addr, unisim::util::endian::Host2LittleEndian(words[0]));
    std::unique_ptr<FullInstruction> insn = std::make_unique<FullInstruction>( op_ptr(op), 4 );

    if (insn->cfg.good())
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
            if (arch.in_delay_slot) return;
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
