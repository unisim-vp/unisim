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

#include "mipssec_decoder.hh"
#include <unisim/util/identifier/identifier.hh>
#include <functional>
#include <cmath>

#include <cassert>

  
namespace
{

  struct _Processor* mips_create_processor()
  {
    struct TODO {}; throw TODO();
    // debugPrint((DomainValue*) nullptr);
    // return reinterpret_cast<struct _Processor*>(new Processor());
    return 0;
  }

  void mips_set_domain_functions(struct _Processor* aprocessor, struct _DomainElementFunctions* functionTable)
  {
    struct TODO {}; throw TODO();
    // auto* processor = reinterpret_cast<Processor*>(aprocessor);
    // processor->setDomainFunctions(*functionTable);
  }
  
  struct _DomainElementFunctions* mips_get_domain_functions(struct _Processor* aprocessor)
  {
    struct TODO {}; throw TODO();
    // auto* processor = reinterpret_cast<Processor*>(aprocessor);
    // return &processor->getDomainFunctions();
    return 0;
  }

  void mips_initialize_memory(struct _Processor* aprocessor, MemoryModel* memory,
                              MemoryModelFunctions* memory_functions, InterpretParameters* parameters)
  {
    struct TODO {}; throw TODO();
    // auto* processor = reinterpret_cast<Processor*>(aprocessor);
    // MemoryState memoryState(memory, memory_functions, parameters);
    // memoryState.initializeThumbMemory(processor->getDomainFunctions());
  }

  void mips_set_verbose(struct _Processor* processor)
  {
    struct TODO {}; throw TODO();
    //  if (processor) reinterpret_cast<Processor*>(processor)->setVerbose();
  }

  void mips_free_processor(struct _Processor* processor)
  {
    struct TODO {}; throw TODO();
    // delete reinterpret_cast<Processor*>(processor);
  }

  int mips_get_registers_number(struct _Processor* processor)
  {
    struct TODO {}; throw TODO();
    // return Processor::RLEnd;
  }

  int mips_get_register_index(struct _Processor* processor,
                                           const char* name)
  {
    struct TODO {}; throw TODO();
    // int code = Processor::RegID(name).code;
    // if (code == Processor::RegID::end)
    //   {
    //     code = Processor::SRegID(name).code;
    //     if (code == Processor::SRegID::end)
    //       return -1;
    //     return Processor::RegID::end + code;
    //   }
    // return code;
    return 0;
  }
  
  const char* mips_get_register_name(struct _Processor* processor, int register_index)
  {
    struct TODO {}; throw TODO();
    // if (register_index < 0)
    //   return nullptr;
    // if (register_index >= Processor::RegID::end)
    //   return Processor::SRegID((Processor::SRegID::Code)
    //                            (register_index-Processor::RegID::end)).c_str();
    // return Processor::RegID((Processor::RegID::Code) register_index).c_str();
    return "";
  }
  
  struct _DecisionVector* mips_create_decision_vector(struct _Processor* processor)
  {
    struct TODO {}; throw TODO();
    // return reinterpret_cast<struct _DecisionVector*>(new DecisionVector());
    return 0;
  }

  struct _DecisionVector* mips_clone_decision_vector(struct _DecisionVector* decision_vector)
  {
    struct TODO {}; throw TODO();
    // return reinterpret_cast<struct _DecisionVector*>(new DecisionVector(
    //        *reinterpret_cast<DecisionVector*>(decision_vector)));
  }

  void mips_free_decision_vector(struct _DecisionVector* decision_vector)
  {
    struct TODO {}; throw TODO();
    //  delete reinterpret_cast<DecisionVector*>(decision_vector);
  }

  void mips_filter_decision_vector(struct _DecisionVector* decision_vector,
                                                uint64_t target)
  {
    struct TODO {}; throw TODO();
    // auto* decisionVector = reinterpret_cast<DecisionVector*>(decision_vector);
    // decisionVector->setToLastInstruction();
    // decisionVector->filter(target);
  }

  bool mips_next_targets(struct _Processor* processor, char* instruction_buffer,
                                      size_t buffer_size, uint64_t address, TargetAddresses* target_addresses,
                                      MemoryModel* memory, MemoryModelFunctions* memory_functions,
                                      struct _DecisionVector* decision_vector, InterpretParameters* parameters)

  {
    struct TODO {}; throw TODO();
    // Processor* proc = reinterpret_cast<Processor*>(processor);
    // proc->setMemoryFunctions(*memory_functions);
    // MemoryState memoryState(memory, memory_functions, parameters);
    // proc->setMemoryState(memoryState);
    // proc->setInterpretParameters(*parameters);

    // uint32_t code;
    // memcpy(&code, instruction_buffer, sizeof(code));
    // unisim::util::endian::ByteSwap(code);

    // Translator actset( address, code );
    // Processor::StatusRegister& status = actset.status;
    // status.iset = status.Thumb;
    // DecisionVector* decisionVector = reinterpret_cast<DecisionVector*>(decision_vector);
    // decisionVector->setToNextInstruction();
    // actset.next_targets(*proc, *target_addresses, *decisionVector);
    return true;
  }

  bool mips_interpret(struct _Processor* processor, char* instruction_buffer,
                                   size_t buffer_size, uint64_t* address, uint64_t target_address,
                                   MemoryModel* memory, MemoryModelFunctions* memory_functions,
                                   struct _DecisionVector* decision_vector, InterpretParameters* parameters)
  {
    struct TODO {}; throw TODO();
    // Processor* proc = reinterpret_cast<Processor*>(processor);
    // proc->setMemoryFunctions(*memory_functions);
    // MemoryState memoryState(memory, memory_functions, parameters);
    // proc->setMemoryState(memoryState);
    // proc->setInterpretParameters(*parameters);

    // uint32_t code;
    // memcpy(&code, instruction_buffer, sizeof(code));
    // unisim::util::endian::ByteSwap(code);

    // Translator actset( *address, code );
    // Processor::StatusRegister& status = actset.status;
    // status.iset = status.Thumb;
    // DecisionVector* decisionVector = reinterpret_cast<DecisionVector*>(decision_vector);
    // actset.interpret(*proc, *decisionVector);
    // *address = actset.addr;
    // return decisionVector->isEmpty();
    return false;
  }

}

#include "decsec_callback.h"

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
