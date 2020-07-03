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
    
    FullInstruction( std::shared_ptr<Operation>& op, unsigned _size )
      : operation(op), actions(), cfg(), size(_size)
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
      
      if (not coderoot->cond.good())
        {
          cfg = coderoot->cfg;
          return;
        }
      if (coderoot->getnext(false)->cond.good()    or coderoot->getnext(true)->cond.good() or
          coderoot->getnext(false)->updates.size() or coderoot->getnext(true)->updates.size())
        { struct BadCond {}; throw BadCond(); }

      struct Fork : Interpreter::Ctrl
      {
        Fork(Interpreter::ActionNode& root) : cond(root.cond), nexts{root.getnext(false)->cfg,root.getnext(true)->cfg} {}

        virtual void retrieveFamily( unisim::util::forbint::debug::Iteration::FamilyInstruction& f, uint32_t origin ) const override
        {
          nexts[0]->retrieveFamily( f, origin );
          nexts[1]->retrieveFamily( f, origin );
        }
        virtual void retrieveTargets( unisim::util::forbint::debug::Iteration& iteration ) const override
        {
        }
        
        unisim::util::symbolic::Expr cond;
        std::shared_ptr<Ctrl> nexts[2];
      };

      cfg = std::make_shared<Fork>(*coderoot);
    }
    
    virtual void disasm(std::ostream& sink) const override { operation->disasm(sink); }

    virtual bool match( uint32_t const* words ) const override { return operation->GetEncoding() == words[0]; }

    unsigned get_family() const override
    {
      unisim::util::forbint::debug::Iteration::FamilyInstruction result;
      if (not cfg)
        result.setSequential();
      else
        cfg->retrieveFamily(result, operation->GetAddr());

      return result.getComputedFamily();
    }
    
    Instruction* clone() const override { return new FullInstruction(*this); }

    virtual void next_addresses(std::set<unsigned int>& addressed, void* mem_model, void* mem_functions, void* parameters) const override
    {
      struct TODO {}; throw TODO();
    }

    virtual void retrieveTargets(unisim::util::forbint::debug::Iteration& iteration) const override
    {
      if (not cfg)
        iteration.addNextTarget();
      else
        cfg->retrieveTargets(iteration);

      // TODO

      if (iteration.isFamilyRequired())
        {
          unisim::util::forbint::debug::Iteration::FamilyInstruction family;
          if (cfg) cfg->retrieveFamily(family, operation->GetAddr());
          else     family.setSequential();
          iteration.setFamily(family);
        }
    }
    virtual unsigned getSize() const override { return size; }
    virtual void interpretForward(uint32_t addr,
                                  unisim::util::forbint::debug::MemoryState& ms,
                                  unisim::util::forbint::debug::Target& tg,
                                  unisim::util::forbint::debug::MemoryFlags& mf) const override
    {
      std::vector<std::unique_ptr<Interpreter::SideEffect>> side_effects;
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
    std::shared_ptr<Interpreter::Ctrl> cfg;
    unsigned size;
  };
    
  Instruction*
  Decoder::decode(uint32_t const* words, uint32_t addr)
  {
    static unisim::component::cxx::processor::mips::isa::Decoder<Mips::Interpreter> idecoder;
    std::shared_ptr<FullInstruction::Operation> op( idecoder.NCDecode(addr, unisim::util::endian::Host2LittleEndian(words[0])) );
    std::unique_ptr<FullInstruction> insn = std::make_unique<FullInstruction>( op, 4 );

    if (insn->cfg)
      {
        struct DBOp : public FullInstruction::Operation
        {
          DBOp( std::shared_ptr<FullInstruction::Operation> first, std::shared_ptr<FullInstruction::Operation> last )
            : FullInstruction::Operation( first->GetEncoding(), first->GetAddr(), "DBOp" )
            , ops{first, last}
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
          
          std::shared_ptr<Operation> ops[2];
        };
        op.reset( idecoder.NCDecode(addr + 4, unisim::util::endian::Host2LittleEndian(words[1])) );
        op.reset( new DBOp( insn->operation, op ) );
        insn = std::make_unique<FullInstruction>( op, 8 );
      }
    
    return insn.release();
  }

}
