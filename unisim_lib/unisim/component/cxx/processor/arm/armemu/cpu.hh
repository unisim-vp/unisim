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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARMEMU_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARMEMU_CPU_HH__

#include <unisim/component/cxx/processor/arm/cpu.hh>
#include <unisim/component/cxx/processor/arm/isa_arm32.hh>
#include <unisim/component/cxx/processor/arm/isa_thumb.hh>
#include <unisim/component/cxx/processor/arm/hostfloat.hh>
#include <unisim/component/cxx/processor/arm/models.hh>
#include <unisim/component/cxx/processor/arm/memory_op.hh>
#include <unisim/component/cxx/processor/arm/models.hh>
#include <unisim/component/cxx/processor/arm/memory_op.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/register.hh>
#include <unisim/util/queue/queue.hh>
#include <unisim/util/queue/queue.tcc>
#include <string>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace armemu {

struct CPU;

struct ARMv7emu
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
  static bool const     hasVFP  = true;
  static bool const     hasAdvSIMD = false;
};

struct CPU
  : public unisim::component::cxx::processor::arm::CPU<ARMv7emu>
  , public unisim::kernel::service::Service<unisim::service::interfaces::MemoryAccessReportingControl>
  , public unisim::kernel::service::Client<unisim::service::interfaces::MemoryAccessReporting<uint32_t> >
  , public unisim::kernel::service::Client<unisim::service::interfaces::LinuxOS>
  , public unisim::kernel::service::Service<unisim::service::interfaces::MemoryInjection<uint32_t> >
  , public unisim::kernel::service::Client<unisim::service::interfaces::DebugControl<uint32_t> >
  , public unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>
  , public unisim::kernel::service::Service<unisim::service::interfaces::Disassembly<uint32_t> >
  , public unisim::kernel::service::Service<unisim::service::interfaces::Registers >
  , public unisim::kernel::service::Service<unisim::service::interfaces::Memory<uint32_t> >
{
  typedef CPU this_type;
  typedef ARMv7emu CONFIG;
  //=====================================================================
  //=                  public service imports/exports                   =
  //=====================================================================
		
  /** Memory access reporting control service export. */
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_export;
  /** Memory access reporting service import. */
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::MemoryAccessReporting<uint32_t> > memory_access_reporting_import;
  /** Disassembly service export. */
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Disassembly<uint32_t> >  disasm_export;
  /** Registers service export. */
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Registers>  registers_export;
  /** Memory injection service export. */
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::MemoryInjection<uint32_t> >  memory_injection_export;
  /** Memory service export. */
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Memory<uint32_t> >  memory_export;
  /** Debug control service import. */
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::DebugControl<uint32_t> >  debug_control_import;
  /** Symbol table lookup service import. */
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::SymbolTableLookup<uint32_t> > symbol_table_lookup_import;
  /** Linux OS service import. */
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::LinuxOS> linux_os_import;
  /** Trap reporting service import. */
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::TrapReporting>  instruction_counter_trap_reporting_import;

  /** Indicates if the finished instructions require to be reported. */
  bool requires_finished_instruction_reporting;
  /** Indicates if the memory accesses require to be reported. */
  bool requires_memory_access_reporting;
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
  //=                       Logger                                      =
  //=====================================================================
  
  /** Unisim logging services. */
  unisim::kernel::logger::Logger logger;
  
  //=====================================================================
  //=                  Client/Service setup methods                     =
  //=====================================================================

  /** Object setup method.
   * This method is required for all UNISIM objects and will be called during
   *   the setup phase.
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
  virtual bool InjectReadMemory( uint32_t addr, void* buffer, uint32_t size );
  /** Inject an intrusive write memory operation
   *
   * @param addr the target address to write to
   * @param buffer a buffer containing the data to write
   * @param size the amount of bytes to write
   *
   * @return true on success, false otherwise
   */
  virtual bool InjectWriteMemory( uint32_t addr, void const* buffer, uint32_t size );

  //=====================================================================
  //=             memory access reporting control interface methods     =
  //=====================================================================

  /** Set/unset the reporting of finishing instructions.
   * 
   * @param report if true set the reporting of finishing
   *   instructions, unset otherwise
   */
  virtual void RequiresFinishedInstructionReporting( bool report );
  /** Set/unset the reporting of memory accesses.
   * 
   * @param report if true set the reporting of memory accesses, unset
   *   otherwise
   */
  virtual void RequiresMemoryAccessReporting( bool report );

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
  virtual bool ReadMemory( uint32_t addr, void* buffer, uint32_t size );
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
  virtual bool WriteMemory( uint32_t addr, void const* buffer, uint32_t size );
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
  virtual bool ExternalReadMemory( uint32_t addr, void* buffer, uint32_t size ) = 0;
  /** Perform a non intrusive external write access.
   * This method is called to write non intrusively external memory locations.
   *
   * @param addr the address to write to
   * @param buffer the data to write
   * @param size the amount of data to write
   *
   * @return true on success, false otherwise
   */
  virtual bool ExternalWriteMemory( uint32_t addr, void const* buffer, uint32_t size ) = 0;

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
  virtual unisim::util::debug::Register* GetRegister( const char* name );
		
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
  virtual std::string Disasm(uint32_t addr, uint32_t &next_addr);
		
  //=====================================================================
  //=                   LinuxOSInterface methods                        =
  //=====================================================================
	
  /** Exit system call.
   * The LinuxOS service calls this method when the program has finished.
   *
   * @param ret the exit cade sent by the exit system call.
   */
  virtual void PerformExit(int ret);
	
  /**************************************************************/
  /* Memory access methods       START                          */
  /**************************************************************/
	
  /** Refill the Instruction Prefetch Buffer from the memory system
   * (through the instruction cache if present).
   *
   * This method allows the user to prefetch instructions from the memory
   * system, that is, it tries to read from the pertinent caches and if
   * failed from the external memory system.
   * 
   * @param address the address of the requiered instruction that the
   *                prefetch instruction buffer should encompass, once
   *                the refill is complete.
   */
  void RefillInsnPrefetchBuffer(uint32_t base_address);
  
  /** Reads ARM32 instructions from the memory system
   * This method allows the user to read instructions from the memory system,
   *   that is, it tries to read from the pertinent caches and if failed from
   *   the external memory system.
   * 
   * @param address the address to read data from
   * @param insn the resulting instruction word (output reference)
   */
  void ReadInsn( uint32_t address, unisim::component::cxx::processor::arm::isa::arm32::CodeType& insn);
  
  /** Reads THUMB instructions from the memory system
   * This method allows the user to read instructions from the memory system,
   *   that is, it tries to read from the pertinent caches and if failed from
   *   the external memory system.
   * 
   * @param address the address to read data from
   * @param insn the resulting instruction word (output reference)
   */
  void ReadInsn( uint32_t address, unisim::component::cxx::processor::arm::isa::thumb2::CodeType& insn);
  
  void ReportMemoryAccess( unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mtp, uint32_t addr, uint32_t size )
  {
    if (requires_memory_access_reporting and memory_access_reporting_import)
      memory_access_reporting_import->ReportMemoryAccess(mat, mtp, addr, size);
  }
  /**************************************************************/
  /* Memory access methods       END                            */
  /**************************************************************/

  /**************************************************************/
  /* Coprocessor methods          START                         */
  /* NOTE: These methods are only declared because the isa      */
  /*       requires those methods, but they are not implemented */
  /**************************************************************/

  /** Coprocessor Load
   * This method is called each time a coprocessor load (for whatever 
   *   coprocessor) is found in the instruction set.
   * 
   * @param cp_num the coprocessor for which the load is requested
   * @param address the address for the load
   * 
   * @return the target coprocessor decides if the loads have finished by 
   *   returning true, false if it has not finished
   */ 
  bool CoprocessorLoad( uint32_t cp_num, uint32_t address);
  /** Coprocessor Load with options
   * This method is called each time a coprocessor load (for whatever 
   *   coprocessor) is found in the instruction set. This method has an extra 
   *   field to pass options to the coprocessor.
   * 
   * @param cp_num the coprocessor for which the load is requested
   * @param address the address for the load
   * @param option options for the target coprocessor
   * 
   * @return the target coprocessor decides if the loads have finished by 
   *   returning true, false if it has not finished
   */ 
  bool CoprocessorLoad( uint32_t cp_num, uint32_t address, uint32_t option);
  /** Coprocessor Store
   * This method is called each time a coprocessor store (for whatever 
   *   coprocessor) is found in the instruction set.
   * 
   * @param cp_num the coprocessor for which the store is requested
   * @param address the address for the store
   * 
   * @return the target coprocessor decides if the stores have finished by 
   *   returning true, false if it has not finished
   */ 
  bool CoprocessorStore( uint32_t cp_num, uint32_t address);
  /** Coprocessor Store with options
   * This method is called each time a coprocessor store (for whatever 
   *   coprocessor) is found in the instruction set. This method has an extra 
   *   field to pass options to the coprocessor.
   * 
   * @param cp_num the coprocessor for which the store is requested
   * @param address the address for the store
   * @param option options for the target coprocessor
   * 
   * @return the target coprocessor decides if the stores have finished by 
   *   returning true, false if it has not finished
   */ 
  bool CoprocessorStore( uint32_t cp_num, uint32_t address, uint32_t option);
  /** Coprocessor Data Processing method
   * 
   * @param cp_num the coprocessor for which the data processing is requested
   * @param op1 first opcode for the coprocessor
   * @param op2 second opcode for the coprocessor
   * @param crd coprocessor destination register
   * @param crn coprocessor first operand register
   * @param crm coprocessor second operand register
   */
  void CoprocessorDataProcess( uint32_t cp_num, uint32_t op1, uint32_t op2,
                              uint32_t crd, uint32_t crn, uint32_t crm);
  /** Move to Coprocessor from ARM register
   * Copies the value from the rd ARM register to the coprocessor cp_num 
   *   register crn (and depending on the opcodes to crm)
   * 
   * @param cp_num the coprocessor for which the move operation is requested
   * @param op1 first opcode for the coprocessor
   * @param op2 second opcode for the coprocessor
   * @param rd the index of the ARM register whose value will be copied
   * @param crn the destination coprocessor register
   * @param crm the additional destination or source coprocessor register
   */
  void MoveToCoprocessor( uint32_t cp_num, uint32_t op1, uint32_t op2, 
                         uint32_t rd, uint32_t crn, uint32_t crm);
  /** Move to ARM register from Coprocessor.
   * Transfers a value from the coprocessor cp_num to an ARM register or to 
   *   the condition flags
   * 
   * @param cp_num the coprocessor for the which the move operation is 
   *   requested
   * @param op1 first opcode for the coprocessor
   * @param op2 second opcode for the coprocessor
   * @param rd the destination ARM register
   * @param crn specifies the coprocessor register that contains the first 
   *   operand for the instruction
   * @param crm additional coprocessor source or destination register 
   */
  void MoveFromCoprocessor( uint32_t cp_num, uint32_t op1, uint32_t op2, 
                           uint32_t rd, uint32_t crn, uint32_t crm);
  
  uint32_t m_tls_reg;
  
  uint32_t Coproc_GetOneWord( unsigned code, unsigned cp_num, unsigned op1, unsigned op2, unsigned crn, unsigned crm );
  
  /**************************************************************/
  /* Coprocessor methods          END                           */
  /**************************************************************/
	
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
protected:
  /** Decoder for the arm32 instruction set. */
  unisim::component::cxx::processor::arm::isa::arm32::Decoder<ARMv7emu> arm32_decoder;
  /** Decoder for the thumb instruction set. */
  unisim::component::cxx::processor::arm::isa::thumb2::Decoder<ARMv7emu> thumb_decoder;

  /** The registers interface for debugging purpose */
  typedef std::map<std::string, unisim::util::debug::Register *> RegistersRegistry;
  RegistersRegistry registers_registry;
		
  /** Number of SPSR registers.
   * Privileged modes have private SPSR registers, the following is
   *   the organization per running mode:
   * - user:           --
   * - system:         --
   * - supervisor:     0
   * - abort:          1
   * - undefined:      2
   * - interrupt:      3
   * - fast interrupt: 4
   */
  const static uint32_t num_phys_spsrs = 5;
  /** The SPSR registers storage.
   */
  PSR spsr[5];
  
  /** Instruction counter */
  uint64_t instruction_counter;
	
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
  /** String describing the endianness of the processor.
   */
  std::string default_endianness_string;
	
  /************************************************************************/
  /* UNISIM parameters, statistics and registers                    START */
  /************************************************************************/

  /** UNISIM Parameter to set the default endianness.
   */
  unisim::kernel::service::Parameter<std::string> param_default_endianness;
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
	
  /** UNISIM Statistic of the number of instructions executed.
   */
  unisim::kernel::service::Statistic<uint64_t> stat_instruction_counter;

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
  
  // TODO: provide UNISIM registers for SPSRS (will be possible once
  // UNISIM registers go back in base class).
  //
  // /** UNISIM registers for the SPRS registers.  */
  // unisim::kernel::service::Register<uint32_t> *reg_spsr[5];

  /************************************************************************/
  /* UNISIM parameters, statistics and registers                      END */
  /************************************************************************/

  /************************************************************************/
  /* Memory access handling                                         START */
  /************************************************************************/
	
  /** Performs a prefetch access.
   *
   * @param addr the address of the memory prefetch access
   */
  void PerformPrefetchAccess( uint32_t addr );
  /** Performs a write access.
   * @param addr the address of the memory write access
   * @param size the size of the memory write access
   * @param value the value of the memory write access
   */
  void PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value );
  /** Performs a read access.
   * @param addr the address of the memory read access
   * @param size the size of the memory read access
   * @param _signed the nature of the memory read access (signed or unsigned)
   */
  uint32_t PerformReadAccess( uint32_t addr, uint32_t size, bool _signed );

  //=====================================================================
  //=               Instruction prefetch buffer                         =
  //=====================================================================
  
  uint8_t ipb_bytes[Cache::LINE_SIZE];                       //!< The instruction prefetch buffer
  uint32_t ipb_base_address;                                 //!< base address of IPB content (cache line size aligned if valid)
};

} // end of namespace armemu
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARMEMU_CPU_HH__
