/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CPU_HH__

#include <unisim/component/cxx/processor/arm/models.hh>
#include <unisim/component/cxx/processor/arm/cpu.hh>
#include <unisim/component/cxx/processor/arm/hostfloat.hh>
#include <unisim/component/cxx/processor/arm/memory_op.hh>
#include <unisim/component/cxx/processor/arm/cache.hh>
#include <unisim/component/cxx/processor/arm/arm926ejs/tlb.hh>
#include <unisim/component/cxx/processor/arm/arm926ejs/lockdown_tlb.hh>
#include <unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.hh>
#include <unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/component/cxx/processor/arm/exception.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/register.hh>
#include <string>
#include <queue>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace arm926ejs {

struct CPU;

struct ARM926ejs
{
  typedef unisim::component::cxx::processor::arm::hostfloat::FPSCR FPSCR;
  typedef double   F64;
  typedef float    F32;
  typedef uint8_t U8;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef uint64_t U64;
  typedef int8_t   S8;
  typedef int16_t  S16;
  typedef int32_t  S32;
  typedef int64_t  S64;
  
  typedef CPU      Arch;

  //=====================================================================
  //=                  ARM architecture model description               =
  //=====================================================================
  
  // Following a standard armv7 configuration
  static uint32_t const MODEL = unisim::component::cxx::processor::arm::ARMEMU;
  static bool const     insns4T = true;
  static bool const     insns5E = true;
  static bool const     insns5J = true;
  static bool const     insns5T = true;
  static bool const     insns6  = true;
  static bool const     insnsRM = false;
  static bool const     insnsT2 = true;
  static bool const     insns7  = true;
};


struct CPU
  : public unisim::component::cxx::processor::arm::CPU<ARM926ejs>
  , public unisim::kernel::service::Service<unisim::service::interfaces::MemoryInjection<uint64_t> >
  , public unisim::kernel::service::Client<unisim::service::interfaces::DebugControl<uint64_t> >
  , public unisim::kernel::service::Client<unisim::service::interfaces::MemoryAccessReporting<uint64_t> >
  , public unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>
  , public unisim::kernel::service::Service<unisim::service::interfaces::MemoryAccessReportingControl>
  , public unisim::kernel::service::Service<unisim::service::interfaces::Disassembly<uint64_t> >
  , public unisim::kernel::service::Service<unisim::service::interfaces::Registers >
  , public unisim::kernel::service::Service<unisim::service::interfaces::Memory<uint64_t> >
{
  typedef CPU this_type;
  typedef ARM926ejs CONFIG;
  //=====================================================================
  //=                  ARM architecture model description               =
  //=====================================================================
  
  static uint32_t const MODEL = unisim::component::cxx::processor::arm::ARMV5TEJ;
  static bool const     insns4T = true;
  static bool const     insns5E = true;
  static bool const     insns5J = true;
  static bool const     insns5T = true;
  static bool const     insns6  = false;
  static bool const     insnsRM = false;
  static bool const     insnsT2 = false;

  //=====================================================================
  //=                       Logger                                      =
  //=====================================================================
  
  /** Unisim logging services. */
  unisim::kernel::logger::Logger logger;
  
  //=====================================================================
  //=                  public service imports/exports                   =
  //=====================================================================
		
  /** Disassembly service export. */
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Disassembly<uint64_t> >  disasm_export;
  /** Registers service export. */
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Registers>  registers_export;
  /** Memory injection service export. */
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::MemoryInjection<uint64_t> >   memory_injection_export;
  /** Memory service export. */
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Memory<uint64_t> >   memory_export;
  /** Memory access reporting control service export. */
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::MemoryAccessReportingControl>   memory_access_reporting_control_export;
  /** Debug control service import. */
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::DebugControl<uint64_t> >  debug_control_import;
  /** Memory access reporting service import. */
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::MemoryAccessReporting<uint64_t> >   memory_access_reporting_import;
  /** Symbol table lookup service import. */
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::SymbolTableLookup<uint64_t> >   symbol_table_lookup_import;
  /** Instruction counter trap reporting service import. */
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::TrapReporting>   instruction_counter_trap_reporting_import;
  /** Exception trap reporting service import. */
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::TrapReporting>  exception_trap_reporting_import;

  //=====================================================================
  //=                    Constructor/Destructor                         =
  //=====================================================================

  /** Constructor.
   *
   * @param name the name that will be used by the UNISIM service 
   *   infrastructure and will identify this object
   * @param parent the parent object of this object
   */
  CPU(const char *name, Object *parent = 0);
  /** Destructor.
   */
  ~CPU();

  //=====================================================================
  //=                  Client/Service setup methods                     =
  //=====================================================================

  /** Object begin setup method.
   * This method is required for all UNISIM objects and will be called during
   *   the begin setup phase.
   * 
   * @return true on success, false otherwise
   */
  virtual bool BeginSetup();
  /** Object end setup method.
   * This method is required for all UNISIM objects and will be called during
   *   the end setup phase.
   * 
   * @return true on success, false otherwise
   */
  virtual bool EndSetup();
  /** Object disconnect method.
   * This method is called when this UNISIM object is disconnected from other
   *   UNISIM objects.
   */
  virtual void OnDisconnect();

  //=====================================================================
  //=                    execution handling methods                     =
  //=====================================================================

  /** Execute one complete instruction.
   */
  void StepInstruction();

  //=====================================================================
  //=                      memory methods                               =
  //=====================================================================

  /** Processor external memory write.
   * Perform an external write memory access, that is an access that is not
   *   in cache (or cache absent).
   *
   * @param addr the address of the access
   * @param buffer byte buffer with the data to write
   * @param size the size of the access
   */
  virtual void PrWrite(uint32_t addr, 
                       const uint8_t *buffer, 
                       uint32_t size) = 0;
  /** Processor external memory read.
   * Perform an external read memory access, that is an access that is not
   *   in cache (or cache absent).
   *
   * @param addr the address of the access
   * @param buffer byte buffer that will be filled with the read data
   * @param size the size of the access
   */
  virtual void PrRead(uint32_t addr,
                      uint8_t *buffer,
                      uint32_t size) = 0;

  //=====================================================================
  //=             memory injection interface methods                    =
  //=====================================================================

  /** Inject an intrusive read memory operation.
   *
   * @param addr the target address to read from
   * @param buffer the buffer to fill with the read memory
   * @param size the amount of bytes to read from starting from the target 
   *   address
   *
   * @return true on success, false otherwise
   */
  virtual bool InjectReadMemory(uint64_t addr, 
                                void *buffer,
                                uint32_t size);
  /** Inject an intrusive write memory operation
   *
   * @param addr the target address to write to
   * @param buffer a buffer containing the data to write
   * @param size the amount of bytes to write
   *
   * @return true on success, false otherwise
   */
  virtual bool InjectWriteMemory(uint64_t addr, 
                                 const void *buffer, 
                                 uint32_t size);

  //=====================================================================
  //=             memory access reporting control interface methods     =
  //=====================================================================

  /** Set/unset the reporting of memory accesses.
   *
   * @param report if true set the reporting of memory acesses, unset 
   *   otherwise
   */
  virtual void RequiresMemoryAccessReporting(bool report);
  /** Set/unset the reporting of finishing instructions.
   * 
   * @param report if true set the reporting of finishing instructions, 
   *   unset otherwise
   */
  virtual void RequiresFinishedInstructionReporting(bool report);

  //=====================================================================
  //=             non intrusive memory interface methods                =
  //=====================================================================

  /** Perform a non intrusive read access.
   * This method performs a normal read, but it does not change the state
   *   of the caches. It calls ExternalReadMemory if data not in cache.
   *
   * @param addr the address to read from
   * @param buffer the buffer to fill with the read data (must be allocated
   *   by the caller)
   * @param size the amount of data to read
   *
   * @return true on success, false otherwise
   */
  virtual bool ReadMemory(uint64_t addr, 
                          void *buffer, 
                          uint32_t size);
  /** Perform a non intrusive write access.
   * This method perform a normal write, but it does not change the state
   *   of the caches. It calls ExternalWriteMemory in order to modify external
   *   ocurrences of the address.
   *
   * @param addr the address to write to
   * @param buffer the data to write
   * @param size the amount of data to write
   *
   * @return true on success, false otherwise
   */
  virtual bool WriteMemory(uint64_t addr, 
                           const void *buffer, 
                           uint32_t size);
  /** Perform a non intrusive external read access.
   * This method is called when a ReadAccess does not find the data in the
   *   cache.
   *
   * @param addr the address to read from
   * @param buffer the buffer to fill with the read data (must be allocated
   *   by the caller)
   * @param size the amount of data to read
   *
   * @return true on success, false otherwise
   */
  virtual bool ExternalReadMemory(uint64_t addr, 
                                  void *buffer, 
                                  uint32_t size) = 0;
  /** Perform a non intrusive external write access.
   * This method is called to write non intrusively external memory locations.
   *
   * @param addr the address to write to
   * @param buffer the data to write
   * @param size the amount of data to write
   *
   * @return true on success, false otherwise
   */
  virtual bool ExternalWriteMemory(uint64_t addr, 
                                   const void *buffer, 
                                   uint32_t size) = 0;

  //=====================================================================
  //=             CPURegistersInterface interface methods               =
  //=====================================================================

  /** Get a register by its name.
   * Gets a register interface to the register specified by name.
   *
   * @param name the name of the requested register
   *
   * @return a pointer to the RegisterInterface corresponding to name
   */
  virtual unisim::util::debug::Register *GetRegister(const char *name);
		
  //=====================================================================
  //=                   DebugDisasmInterface methods                    =
  //=====================================================================

  /** Disasm an instruction address.
   * Returns a string with the disassembling of the instruction found 
   *   at address addr.
   * 
   * @param addr the address of the instruction to disassemble
   * @param next_addr the address following the requested instruction
   *
   * @return the disassembling of the requested instruction address
   */
  virtual std::string Disasm(uint64_t addr, uint64_t &next_addr);
		
  /**************************************************************/
  /* Memory access methods       START                          */
  /**************************************************************/
	
  /** Reads 32bits instructions from the memory system
   * This method allows the user to read instructions from the memory system,
   *   that is, it tries to read from the pertinent caches and if failed from
   *   the external memory system.
   * 
   * @param address the address to read data from
   * @param val the buffer to fill with the read data
   */
  void ReadInsn(uint32_t address, unisim::component::cxx::processor::arm::isa::arm32::CodeType& insn);

  /** Reads THUMB instructions from the memory system
   * This method allows the user to read instructions from the memory system,
   *   that is, it tries to read from the pertinent caches and if failed from
   *   the external memory system.
   * 
   * @param address the address to read data from
   * @param insn the resulting instruction word (output reference)
   */
  void ReadInsn(uint32_t address, unisim::component::cxx::processor::arm::isa::thumb::CodeType& insn);

  void ReportMemoryAccess( unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mtp, uint32_t addr, uint32_t size )
  {
    if (requires_memory_access_reporting and memory_access_reporting_import)
      memory_access_reporting_import->ReportMemoryAccess(mat, mtp, addr, size);
  }
  /**************************************************************/
  /* Memory access methods       END                            */
  /**************************************************************/

  /** Determine wether the processor instruction stream is inside an
   * IT block.  Always false before ARMv6T2 architectures.
   */
  bool itblock() const { return false; }
  
  /** Return the current condition associated to the IT state of the
   * processor. Always "AL" before ARMv6T2 architectures.
   */
  uint32_t itcond() const { return this->COND_AL; }

  /** Software Interrupt
   *  This method is called by SWI instructions to handle software interrupts.
   */
  void SWI( uint32_t imm );
  /** Breakpoint
   *  This method is called by BKPT instructions to handle breakpoints.
   */
  void BKPT( uint32_t imm );
	
  /** Unpredictable Instruction Behaviour.
   * This method is just called when an unpredictable behaviour is detected to
   *   notifiy the processor.
   */
  void UnpredictableInsnBehaviour();

  /** Instruction cache */
  Cache icache;
  /** Data cache */
  Cache dcache;

  /** The Lockdown TLB */
  LockdownTLB ltlb;
  /** The TLB */
  TLB tlb;

protected:
  /** The registers interface for debugging purpose */
  typedef std::map<std::string, unisim::util::debug::Register *> RegistersRegistry;
  RegistersRegistry registers_registry;
		
  /** Decoder for the arm32 instruction set. */
  unisim::component::cxx::processor::arm::isa::arm32::Decoder<ARM926ejs>  arm32_decoder;
  /** Decoder for the thumb instruction set. */
  unisim::component::cxx::processor::arm::isa::thumb::Decoder<ARM926ejs>  thumb_decoder;

  /** The exceptions that have occured */
  uint32_t exception;

  /** Instruction counter */
  uint64_t instruction_counter;
  /** Current instruction address */
  uint32_t cur_instruction_address;
	
  /** CPU cycle time in picoseconds.
   */
  uint64_t cpu_cycle_time_ps;
  /** CPU voltage in mV, required to compute cache power consumption.
   */
  uint64_t voltage;
  // uint64_t cpu_cycle;      //!< Number of cpu cycles
  // uint64_t bus_cycle;      //!< Number of front side bus cycles

  /** Indicates if the emulator is running in verbose mode.
   * 0 indicates no verbose.
   * Different than 0 indicates verbose.
   */
  uint32_t verbose;
  /** Trap when reaching the number of instructions indicated.
   */
  uint64_t trap_on_instruction_counter;
  /** Trap when handling an exception
   */
  bool trap_on_exception;
	
  /** Indicates if the memory accesses require to be reported.
   */
  bool requires_memory_access_reporting;
  /** Indicates if the finished instructions require to be reported.
   */
  bool requires_finished_instruction_reporting;

  /************************************************************************/
  /* Exception handling                                             START */
  /************************************************************************/

  /** Process exceptions
   *
   * Returns true if there is an exception to handle.
   * @return true if an exception handling begins
   */
  bool HandleException();

  /************************************************************************/
  /* Exception handling                                               END */
  /************************************************************************/

  /************************************************************************/
  /* UNISIM parameters, statistics and registers                    START */
  /************************************************************************/

  /** UNISIM Parameter to set the CPU cycle time.
   */
  unisim::kernel::service::Parameter<uint64_t> param_cpu_cycle_time_ps;
  /** UNISIM Parameter to set the CPU voltage.
   */
  unisim::kernel::service::Parameter<uint64_t> param_voltage;
  /** UNISIM Parameter to set/unset verbose mode.
   */
  unisim::kernel::service::Parameter<uint32_t> param_verbose;
  /** UNISIM Parameter to set traps on instruction counter.
   */
  unisim::kernel::service::Parameter<uint64_t> 
  param_trap_on_instruction_counter;
  /** UNISIM Parameter to set traps on exception.
   */
  unisim::kernel::service::Parameter<bool> 
  param_trap_on_exception;
	
	
  /** UNISIM Statistic of the number of instructions executed.
   */
  unisim::kernel::service::Statistic<uint64_t> stat_instruction_counter;
  /** UNISIM Statistic with the address of the current instruction.
   */
  unisim::kernel::service::Statistic<uint32_t> stat_cur_instruction_address;

  /** UNISIM registers for the physical registers.
   */
  unisim::kernel::service::Register<uint32_t> *reg_phys_gpr[32];
  /** UNISIM registers for the logical registers.
   */
  unisim::kernel::service::Register<uint32_t> *reg_gpr[16];
  /** UNISIM register for the stack pointer register (gpr 13).
   */
  unisim::kernel::service::Register<uint32_t> reg_sp;
  /** UNISIM register for the link register (gpr 14).
   */
  unisim::kernel::service::Register<uint32_t> reg_lr;
  /** UNISIM register for the program counter register (gpr 15).
   */
  unisim::kernel::service::Register<uint32_t> reg_pc;
  /** UNISIM register for the CPSR register.
   */
  unisim::kernel::service::Register<uint32_t> reg_cpsr;
  /** UNISIM registers for the SPRS registers.
   */
  unisim::kernel::service::Register<uint32_t> *reg_spsr[5];

  /************************************************************************/
  /* UNISIM parameters, statistics and registers                      END */
  /************************************************************************/

  /************************************************************************/
  /* Memory statistics                                              START */
  /************************************************************************/
	
  uint64_t num_data_prefetches;
  uint64_t num_data_reads;
  uint64_t num_data_writes;
  uint64_t num_insn_reads;
  unisim::kernel::service::Statistic<uint64_t> stat_num_data_prefetches;
  unisim::kernel::service::Statistic<uint64_t> stat_num_data_reads;
  unisim::kernel::service::Statistic<uint64_t> stat_num_data_writes;
  unisim::kernel::service::Statistic<uint64_t> stat_num_insn_reads;

  /************************************************************************/
  /* Memory statistics                                                END */
  /************************************************************************/

};

} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CPU_HH__
