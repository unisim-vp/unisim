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
#include <unisim/component/cxx/processor/arm/models.hh>
#include <unisim/component/cxx/processor/arm/exception.hh>
#include <unisim/component/cxx/processor/arm/hostfloat.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/util/endian/endian.hh>
#include <string>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace armemu {

struct ARMv7emu
{
  //=====================================================================
  //=                  ARM architecture model description               =
  //=====================================================================
  
  // Following a standard armv7 configuration
  static uint32_t const model = unisim::component::cxx::processor::arm::ARMEMU;
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
  , public unisim::kernel::service::Service<unisim::service::interfaces::MemoryInjection<uint32_t> >
  , public unisim::kernel::service::Client<unisim::service::interfaces::DebugControl<uint32_t> >
  , public unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>
  , public unisim::kernel::service::Service<unisim::service::interfaces::Disassembly<uint32_t> >
  , public unisim::kernel::service::Service<unisim::service::interfaces::Memory<uint32_t> >
  , public unisim::kernel::service::Client<unisim::service::interfaces::LinuxOS>
{
  typedef CPU this_type;
  typedef unisim::component::cxx::processor::arm::CPU<ARMv7emu> BaseCpu;
  typedef typename BaseCpu::CP15Reg CP15Reg;

  //=====================================================================
  //=                  public service imports/exports                   =
  //=====================================================================
		
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_export;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::MemoryAccessReporting<uint32_t> > memory_access_reporting_import;
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Disassembly<uint32_t> > disasm_export;
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::MemoryInjection<uint32_t> > memory_injection_export;
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Memory<uint32_t> > memory_export;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::DebugControl<uint32_t> > debug_control_import;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::SymbolTableLookup<uint32_t> > symbol_table_lookup_import;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::TrapReporting> exception_trap_reporting_import;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::TrapReporting> instruction_counter_trap_reporting_import;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::LinuxOS> linux_os_import;

  /** Indicates if the finished instructions require to be reported. */
  bool requires_finished_instruction_reporting;
  /** Indicates if the memory accesses require to be reported. */
  bool requires_memory_access_reporting;
  //=====================================================================
  //=                    Constructor/Destructor                         =
  //=====================================================================

  CPU( const char *name, Object *parent = 0 );
  ~CPU();

  //=====================================================================
  //=                  Client/Service setup methods                     =
  //=====================================================================

  virtual bool EndSetup();
  virtual void OnDisconnect();

  //=====================================================================
  //=                    execution handling methods                     =
  //=====================================================================

  void StepInstruction();

  //=====================================================================
  //=             memory injection interface methods                    =
  //=====================================================================

  virtual bool InjectReadMemory( uint32_t addr, void* buffer, uint32_t size );
  virtual bool InjectWriteMemory( uint32_t addr, void const* buffer, uint32_t size );

  //=====================================================================
  //=             memory access reporting control interface methods     =
  //=====================================================================

  virtual void RequiresFinishedInstructionReporting( bool report );
  virtual void RequiresMemoryAccessReporting( bool report );

  //=====================================================================
  //=             non intrusive memory interface methods                =
  //=====================================================================

  virtual bool ReadMemory( uint32_t addr, void* buffer, uint32_t size );
  virtual bool WriteMemory( uint32_t addr, void const* buffer, uint32_t size );
  virtual bool ExternalReadMemory( uint32_t addr, void* buffer, uint32_t size ) = 0;
  virtual bool ExternalWriteMemory( uint32_t addr, void const* buffer, uint32_t size ) = 0;

  //=====================================================================
  //=                   DebugDisasmInterface methods                    =
  //=====================================================================

  virtual std::string Disasm(uint32_t addr, uint32_t &next_addr);
		
  //=====================================================================
  //=                   LinuxOSInterface methods                        =
  //=====================================================================
	
  virtual void PerformExit(int ret);
	
  /**************************************************************/
  /* Memory access methods       START                          */
  /**************************************************************/
	
  virtual void PrWrite( uint32_t addr, uint8_t const* buffer, uint32_t size ) = 0;
  virtual void PrRead( uint32_t addr, uint8_t* buffer, uint32_t size ) = 0;
  
  uint32_t MemRead32( uint32_t address ) { return PerformReadAccess( address, 4, false ); }
  uint32_t MemRead16( uint32_t address ) { return PerformReadAccess( address, 2, false ); }
  uint32_t MemReadS16( uint32_t address ) { return PerformReadAccess( address, 2, true ); }
  uint32_t MemRead8( uint32_t address ) { return PerformReadAccess( address, 1, false ); }
  uint32_t MemReadS8( uint32_t address ) { return PerformReadAccess( address, 1, true ); }
  void     MemWrite32( uint32_t address, uint32_t value ) { PerformWriteAccess( address, 4, value ); }
  void     MemWrite16( uint32_t address, uint16_t value ) { PerformWriteAccess( address, 2, value ); }
  void     MemWrite8( uint32_t address, uint8_t value ) { PerformWriteAccess( address, 1, value ); }

  void     PerformPrefetchAccess( uint32_t addr );
  void     PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value );
  uint32_t PerformReadAccess( uint32_t addr, uint32_t size, bool _signed );
  
  void ReportMemoryAccess( unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mtp, uint32_t addr, uint32_t size )
  {
    if (requires_memory_access_reporting and memory_access_reporting_import)
      memory_access_reporting_import->ReportMemoryAccess(mat, mtp, addr, size);
  }
  
  void RefillInsnPrefetchBuffer( uint32_t base_address );
  
  void ReadInsn( uint32_t address, unisim::component::cxx::processor::arm::isa::arm32::CodeType& insn );
  void ReadInsn( uint32_t address, unisim::component::cxx::processor::arm::isa::thumb2::CodeType& insn );
  
  /**************************************************************/
  /* Memory access methods       END                            */
  /**************************************************************/
	
  /**************************************************/
  /* Software Exceptions                     START  */
  /**************************************************/
	
  void SWI( uint32_t imm );
  void BKPT( uint32_t imm );
	
  /**************************************************/
  /* Software Exceptions                      END   */
  /**************************************************/
  
  /** Instruction cache */
  Cache icache;
  /** Data cache */
  Cache dcache;
  
protected:
  /** Decoder for the ARM32 instruction set. */
  unisim::component::cxx::processor::arm::isa::arm32::Decoder<CPU> arm32_decoder;
  /** Decoder for the THUMB instruction set. */
  unisim::component::cxx::processor::arm::isa::thumb2::Decoder<CPU> thumb_decoder;

  /**************************/
  /* CP15 Interface   START */
  /**************************/

  virtual CP15Reg& CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 );
    
  /**************************/
  /* CP15 Interface    END */
  /**************************/

  /** Instruction counter */
  uint64_t instruction_counter;
	
  /** CPU cycle time in picoseconds. */
  uint64_t cpu_cycle_time_ps;
  /** CPU voltage in mV, required to compute cache power consumption. */
  uint64_t voltage;
  // uint64_t cpu_cycle;      //!< Number of cpu cycles
  // uint64_t bus_cycle;      //!< Number of front side bus cycles

  /** Verbose level of the emulator */
  uint32_t verbose;
  /** Trap when reaching the number of instructions indicated. */
  uint64_t trap_on_instruction_counter;
  /** String describing the endianness of the processor. */
  std::string default_endianness_string;
	
  /**********************************************************/
  /* UNISIM parameters, statistics                    START */
  /**********************************************************/

  /** UNISIM Parameter to set the default endianness. */
  unisim::kernel::service::Parameter<std::string> param_default_endianness;
  /** UNISIM Parameter to set the CPU cycle time. */
  unisim::kernel::service::Parameter<uint64_t> param_cpu_cycle_time_ps;
  /** UNISIM Parameter to set the CPU voltage. */
  unisim::kernel::service::Parameter<uint64_t> param_voltage;
  /** UNISIM Parameter to set/unset verbose mode. */
  unisim::kernel::service::Parameter<uint32_t> param_verbose;
  /** UNISIM Parameter to set traps on instruction counter. */
  unisim::kernel::service::Parameter<uint64_t> param_trap_on_instruction_counter;
  /** UNISIM Statistic of the number of instructions executed. */
  unisim::kernel::service::Statistic<uint64_t> stat_instruction_counter;

  /**********************************************************/
  /* UNISIM parameters, statistics                      END */
  /**********************************************************/

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
