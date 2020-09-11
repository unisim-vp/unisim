/*
 *  Copyright (c) 2009-2020,
 *  Commissariat a l'Energie Atomique (CEA)
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include "instructions.hh"
//#include "mipssec_decoder.hh"
#include <unisim/util/forbint/contract/contract.hh>
#include <unisim/util/identifier/identifier.hh>
#include <unisim/util/endian/endian.hh>
#include <functional>
#include <iostream>
#include <memory>
#include <map>
#include <set>
#include <cmath>
#include <cassert>

  
namespace
{
  struct Processor
  {
    Processor() {}
    ~Processor() {}

    void setVerbose() { verbose = true; }

    
    Mips::Instruction* decode(uint8_t const* bytes, size_t length, uint32_t addr)
    {
      if (length < 8)
        std::cerr << "need to have at least 8 byte in instruction buffer to decode";
      uint32_t words[2];
      std::copy(&bytes[0], &bytes[8], reinterpret_cast<uint8_t*>(&words[0]));
      if (unisim::util::endian::IsHostBigEndian())
        unisim::util::endian::BSwap(words, 2);

      auto itr = instructions.lower_bound(addr);
      if (itr != instructions.end() and itr->first == addr)
        {
          if (itr->second->match(&words[0]))
            return itr->second.get();
          itr = instructions.erase(itr);
        }

      std::unique_ptr<Mips::Instruction> insn( decoder.decode(words, addr) );
      itr = instructions.emplace_hint(itr, std::piecewise_construct, std::make_tuple(addr), std::forward_as_tuple(std::move(insn)));
      return itr->second.get();
    }
    
    Mips::Decoder decoder;
    std::map< uint32_t, std::unique_ptr<Mips::Instruction> > instructions;
    DomainElementFunctions dftable;
    bool verbose;
  };

  struct DecisionVector
  {
  };


  struct _Processor* mips_create_processor()
  {
    return reinterpret_cast<struct _Processor*>(new Processor());
    return 0;
  }

  void mips_set_domain_functions(struct _Processor* aprocessor, struct _DomainElementFunctions* functionTable)
  {
    auto* processor = reinterpret_cast<Processor*>(aprocessor);
    processor->dftable = *functionTable;
  }
  
  struct _DomainElementFunctions* mips_get_domain_functions(struct _Processor* aprocessor)
  {
    auto* processor = reinterpret_cast<Processor*>(aprocessor);
    return &processor->dftable;
  }

  void mips_initialize_memory(struct _Processor* aprocessor, MemoryModel* memory,
                              MemoryModelFunctions* memory_functions, InterpretParameters* parameters)
  {
    auto* processor = reinterpret_cast<Processor*>(aprocessor);
    
    // for (Mips::RegisterIndex reg; reg.next();)
    //   {
    //     unisim::util::forbint::debug::ScalarElement value = flags.newTop(32);
    //     memory.setRegisterValue(reg.idx(), value, flags);
    //   }
    auto de = (*processor->dftable.multibit_create_constant)(DomainIntegerConstant{32,false,0});
    (*memory_functions->set_register_value)(memory, 0, &de, 0, 0);
  }

  void mips_set_verbose(struct _Processor* processor)
  {
    if (processor) reinterpret_cast<Processor*>(processor)->setVerbose();
  }

  void mips_free_processor(struct _Processor* processor)
  {
    delete reinterpret_cast<Processor*>(processor);
  }

  int mips_get_registers_number(struct _Processor* processor)
  {
    return Mips::RegisterIndex::end;
  }


  int mips_get_register_index(struct _Processor* processor, const char* name)
  {
    Mips::RegisterIndex reg(name);

    return reg.code != reg.end ? reg.idx() : -1;
  }
  
  const char* mips_get_register_name(struct _Processor* processor, int register_index)
  {
    return Mips::RegisterIndex(Mips::RegisterIndex::Code(register_index)).c_str();
  }
  
  struct _DecisionVector* mips_create_decision_vector(struct _Processor* processor)
  {
    return reinterpret_cast<struct _DecisionVector*>(new DecisionVector());
  }

  struct _DecisionVector* mips_clone_decision_vector(struct _DecisionVector* decision_vector)
  {
    return reinterpret_cast<struct _DecisionVector*>(new DecisionVector(*reinterpret_cast<DecisionVector*>(decision_vector)));
  }

  void mips_free_decision_vector(struct _DecisionVector* decision_vector)
  {
    delete reinterpret_cast<DecisionVector*>(decision_vector);
  }

  void mips_filter_decision_vector(struct _DecisionVector* decision_vector, uint64_t target)
  {
    // No decision vector used for now...
    // struct TODO {}; throw TODO();
    // auto* decisionVector = reinterpret_cast<DecisionVector*>(decision_vector);
    // decisionVector->setToLastInstruction();
    // decisionVector->filter(target);
  }
  
  bool mips_next_targets(struct _Processor* processor, char* instruction_buffer,
                         size_t buffer_size, uint64_t address, struct _TargetAddresses* atarget_addresses,
                         MemoryModel* memory, MemoryModelFunctions* memory_functions,
                         struct _DecisionVector* decision_vector, InterpretParameters* parameters)

  {
    struct TargetAddresses
    {
      uint64_t* addresses;
      int addresses_array_size;
      int addresses_length;

      uint64_t* (*realloc_addresses)(uint64_t* old_addresses, int old_size, int* new_size, void* address_container);
      void* address_container;
    };

    Processor* proc = reinterpret_cast<Processor*>(processor);
    TargetAddresses* target_addresses = reinterpret_cast<TargetAddresses*>(atarget_addresses);

    auto insn = proc->decode((uint8_t const*)instruction_buffer, buffer_size, address);

    //auto& dv = *reinterpret_cast<DecisionVector*>(decision_vector);

    std::set<uint32_t> addresses;
    unisim::util::forbint::contract::MemoryState memstate(memory, memory_functions, parameters);
    DomainEvaluationEnvironment env;
    memstate.setEvaluationEnvironment(env);
    if (!insn->next_addresses(addresses, memstate, &proc->dftable))
      return false;

    // realloc target_addresses to the needed size
    for (int needed = addresses.size(); target_addresses->addresses_array_size < needed;)
      {
        target_addresses->addresses =
          (*target_addresses->realloc_addresses)(target_addresses->addresses, target_addresses->addresses_array_size,
                                                &target_addresses->addresses_array_size, target_addresses->address_container);
      }
    
    target_addresses->addresses_length = addresses.size();
    std::copy(addresses.begin(), addresses.end(), &target_addresses->addresses[0] );
    // update the memory if addresses.size() == 1
    uint32_t addr = address;
    if (addresses.size() == 1)
       insn->interpret(addr, *addresses.begin(), memstate, &proc->dftable);
    
    return true;
  }

  bool mips_interpret(struct _Processor* processor, char* instruction_buffer,
                                   size_t buffer_size, uint64_t* address, uint64_t target_address,
                                   MemoryModel* memory, MemoryModelFunctions* memory_functions,
                                   struct _DecisionVector* decision_vector, InterpretParameters* parameters)
  {
    Processor* proc = reinterpret_cast<Processor*>(processor);

    uint32_t addr = unisim::util::endian::Host2LittleEndian(*address);

    auto insn = proc->decode((uint8_t const*)instruction_buffer, buffer_size, addr);

    unisim::util::forbint::contract::MemoryState memstate(memory, memory_functions, parameters);
    DomainEvaluationEnvironment env;
    memstate.setEvaluationEnvironment(env);
    bool result = insn->interpret(addr, target_address, memstate, &proc->dftable);
    *address = unisim::util::endian::Host2LittleEndian(addr);
    return result;
  }

}

extern "C"
{
  DLL_API uint64_t init_processor_functions(struct _ProcessorFunctions* functions)
  {
    functions->create_processor = mips_create_processor;
    functions->set_domain_functions = mips_set_domain_functions;
    functions->get_domain_functions = mips_get_domain_functions;
    functions->initialize_memory = mips_initialize_memory;
    functions->set_verbose = mips_set_verbose;
    functions->free_processor = mips_free_processor;
    functions->get_registers_number = mips_get_registers_number;
    functions->get_register_index = mips_get_register_index;
    functions->get_register_name = mips_get_register_name;
    functions->create_decision_vector = mips_create_decision_vector;
    functions->clone_decision_vector = mips_clone_decision_vector;
    functions->free_decision_vector = mips_free_decision_vector;
    functions->filter_decision_vector = mips_filter_decision_vector;
    functions->processor_next_targets = mips_next_targets;
    functions->processor_interpret = mips_interpret;

    return 1;
  }
  
}  
