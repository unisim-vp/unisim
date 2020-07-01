#include "instructions.hh"
#include <unisim/util/forbint/debug/debug.hh>
#include <unisim/util/endian/endian.hh>
#include <sstream>

#if defined(_WIN32) && defined(_USRDLL)
#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif // APE_EXPORTS
#else
#define DLL_API
#endif // _USRDLL

extern "C"
{

  DLL_API struct _Decoder*
  decoder_create(int default_decoder_sate)
  {
    return reinterpret_cast<struct _Decoder*>(new Mips::Decoder);
  }

  DLL_API void
  decoder_free(struct _Decoder* adecoder)
  {
    Mips::Decoder* decoder = reinterpret_cast<Mips::Decoder*>(adecoder);
    delete decoder;
  }

  DLL_API int
  get_max_size_address_in_bytes(struct _Decoder* decoder)
  {  return 4; }

  DLL_API unisim::util::forbint::debug::DefaultTargetEndianness
  get_default_endianness(struct _Decoder* decoder)
  {
    return unisim::util::forbint::debug::DTELittleEndian;
  }
   
  DLL_API struct _Instruction*
  decode_instruction(struct _Decoder* drp, const char* bytes, size_t* insn_size, uint64_t* addr, size_t addr_size, int external_state)
  {
    uint32_t insn_addr = unisim::util::endian::Host2LittleEndian(*addr);
    if (Mips::Decoder* decoder = reinterpret_cast<Mips::Decoder*>(drp))
      {
        if (Mips::Instruction* instruction = decoder->decode((uint32_t*)bytes, insn_addr))
          {
            *insn_size = instruction->getSize();
            return reinterpret_cast<struct _Instruction*>(instruction);
          }
      }
  
    { struct Issue {}; throw Issue(); }
  }

  DLL_API void
  initialize_memory(struct _Decoder* decoder,
                    struct _MemoryState* amemory, struct _MemoryStateFunctions* memoryFunctions,
                    struct _InterpretParameters* parameters,
                    struct _InterpretParametersFunctions* parametersFunctions)
  {
    unisim::util::forbint::debug::MemoryState memory(amemory, memoryFunctions);
    unisim::util::forbint::debug::MemoryFlags flags(parameters, parametersFunctions);
    memory.setNumberOfRegisters(Mips::RegisterIndex::end);
    for (Mips::RegisterIndex reg; reg.next();)
      {
        unisim::util::forbint::debug::ScalarElement value = flags.newTop(32);
        memory.setRegisterValue(reg.idx(), value, flags);
      }
  }

  DLL_API void
  retrieve_targets(struct _Decoder* adecoder, struct _Instruction* ainstruction,
                   struct _Iteration* aiteration, bool may_follow_graph, bool is_family_required,
                   struct _IterationFunctions* iteration_functions)
  {
    if (Mips::Instruction* instruction = reinterpret_cast<Mips::Instruction*>(ainstruction))
      {
        typedef unisim::util::forbint::debug::Iteration Iteration;
        Iteration iteration(aiteration, (bool) may_follow_graph, (bool) is_family_required, iteration_functions);
        instruction->retrieveTargets(iteration);
        return;
      }
  
    { struct Issue {}; throw Issue(); }
  }

  DLL_API void
  interpret_forward(struct _Decoder* decoder, struct _Instruction* ainstruction,
                    uint64_t* address, struct _MemoryState* amemory, struct _MemoryStateFunctions* memoryFunctions,
                    uint64_t* adestination, struct _InterpretParameters* parameters,
                    struct _InterpretParametersFunctions* parametersFunctions)
  {
    if (Mips::Instruction* instruction = reinterpret_cast<Mips::Instruction*>(ainstruction))
      {
        uint32_t insn_addr = unisim::util::endian::Host2LittleEndian(*address);
        typedef unisim::util::forbint::debug::MemoryState MemoryState;
        typedef unisim::util::forbint::debug::Target Target;
        typedef unisim::util::forbint::debug::MemoryFlags MemoryFlags;
        MemoryState memory(amemory, memoryFunctions);
        Target destination(adestination);
        MemoryFlags flags(parameters, parametersFunctions);
        instruction->interpretForward(insn_addr, memory, destination, flags);
        return;
      }

    { struct Issue {}; throw Issue(); }
  }

  DLL_API void
  interpret_backward(struct _Decoder* decoder, struct _Instruction* ainstruction, uint64_t* address,
                     struct _MemoryState* after_memory, struct _MemoryState* before_memory_to_constraint,
                     struct _MemoryStateFunctions* memoryFunctions, uint64_t* after_memory_address,
                     struct _InterpretParameters* parameters,
                     struct _InterpretParametersFunctions* parametersFunctions)
  {
  }

  DLL_API FamilyInstruction
  get_family_instruction(struct _Decoder* decoder, struct _Instruction* ainstruction, uint64_t* target_address_validation)
  {
    if (Mips::Instruction* instruction = reinterpret_cast<Mips::Instruction*>(ainstruction))
      return instruction->get_family();
  
    struct Issue {}; throw Issue();
  }

  DLL_API const char*
  print_instruction(struct _Instruction* ainstruction, uint64_t* address)
  {
    //uint32_t addr = address[0] <<
    if (Mips::Instruction* instruction = reinterpret_cast<Mips::Instruction*>(ainstruction))
      {
        static std::string res;
        std::ostringstream buf;
        instruction->disasm(buf);
        res = buf.str();
        return res.c_str();
      }
      
    struct Issue {}; throw Issue();
  }

  DLL_API unsigned int
  get_opcode_as_unsigned(struct _Instruction* ainstruction) {
    Mips::Instruction* instruction = reinterpret_cast<Mips::Instruction*>(ainstruction);
    assert(instruction);
    // return instruction->queryOpcodeAsUnsigned();
    return 0;
  }

  DLL_API void
  free_instruction(struct _Instruction* ainstruction)
  {
    if (Mips::Instruction* instruction = reinterpret_cast<Mips::Instruction*>(ainstruction))
      { delete instruction; return; }
    struct Issue {}; throw Issue();
  }

  DLL_API struct _Instruction*
  clone_instruction(struct _Instruction* ainstruction)
  {
    if (Mips::Instruction* instruction = reinterpret_cast<Mips::Instruction*>(ainstruction))
      return reinterpret_cast<struct _Instruction*>(instruction->clone());
    struct Issue {}; throw Issue();
  }

  DLL_API int
  get_registers_number(struct _Decoder* adecoder)
  {
    return 32;
  }

  DLL_API int
  get_size_register(struct _Decoder* adecoder, int register_index)
  {
    return 32;
  }

  DLL_API char const*
  get_register_name(struct _Decoder* adecoder, int register_index)
  {
    return Mips::RegisterIndex(Mips::RegisterIndex::Code(register_index)).c_str();
  }

  DLL_API int
  get_register_index(struct _Decoder* adecoder, char const* register_name, unsigned* size, unsigned* low_bit, unsigned* high_bit, bool* is_floating_register)
  {
    Mips::RegisterIndex reg(register_name);

    return reg.code != reg.end ? reg.idx() : -1;
  }

  DLL_API void
  set_support_syntactic_condition(struct _Decoder* adecoder) {

  }

} // end of extern "C"
