/*
 *  Copyright (c) 2007-2016,
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
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 *  SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_TCC__

#include <unisim/component/cxx/processor/arm/isa/arm64/disasm.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/simple_register.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace vmsav8 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;

/** Constructor.
 * Initializes CPU
 *
 * @param name the name that will be used by the UNISIM service 
 *   infrastructure and will identify this object
 * @param parent the parent object of this object
 */
template <class CONFIG>
CPU<CONFIG>::CPU(const char *name, Object *parent)
  : unisim::kernel::service::Object(name, parent)
  , Service<Registers>(name, parent)
  , logger(*this)
  , verbose(false)
    /*Imports*/
  , debug_control_import("debug-control-import", this)
  , instruction_counter_trap_reporting_import("instruction-counter-trap-reporting-import", this)
  , symbol_table_lookup_import("symbol-table-lookup-import", this)
  , memory_import("memory-import", this)
  , linux_os_import("linux-os-import", this)
  , memory_access_reporting_import("memory-access-reporting-import", this)
    /*Exports*/
  , registers_export("registers-export", this)
  , memory_export("memory-export", this)
  , disasm_export("disasm-export", this)
  , memory_access_reporting_control_export("memory-access-reporting-control-export", this)
  , memory_injection_export("memory-injection-export", this)
  // , Service<MemoryAccessReportingControl>(name, parent)
  // , Client<MemoryAccessReporting<uint32_t> >(name, parent)
  // , Service<MemoryInjection<uint32_t> >(name, parent)
  // , Client<unisim::service::interfaces::DebugControl<uint32_t> >(name, parent)
  // , Client<TrapReporting>(name, parent)
  // , Service<Disassembly<uint32_t> >(name, parent)
  // , Service< Memory<uint32_t> >(name, parent)
  // , Client< Memory<uint32_t> >(name, parent)
  // , Client<LinuxOS>(name, parent)
  // , Client<SymbolTableLookup<uint32_t> >(name, parent)
  // , exception_trap_reporting_import("exception-trap-reporting-import", this)
  // , requires_finished_instruction_reporting(false)
  // , requires_memory_access_reporting(false)
  // // , icache("icache", this)
  // // , dcache("dcache", this)
  // , arm32_decoder()
  // , thumb_decoder()
  // , csselr(0)
  // , DFSR()
  // , IFSR()
  // , DFAR()
  // , IFAR()
  // , mmu()
  // , instruction_counter(0)
  // , trap_on_instruction_counter(0)
  // , ipb_base_address( -1 )
  // , linux_printk_buf_addr( 0 )
  // , linux_printk_buf_size( 0 )
  // , linux_printk_snooping( false )
  // , halt_on_addr( uint32_t(-1) )
  // , halt_on_location()
  // , param_verbose("verbose", this, this->PCPU::verbose, "Activate the verbose system.")
  // , param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter,
  //                                     "Produce a trap when the given instruction count is reached.")
  // , param_linux_printk_snooping( "linux-printk-snooping", this, linux_printk_snooping, "Activate the printk snooping" )
  // , param_halt_on_location( "halt-on-location", this, halt_on_location,
  //                           "Tell the CPU to halt simulation on a specific instruction (address or symbol)." )
  // , stat_instruction_counter("instruction-counter", this, instruction_counter, "Number of instructions executed.")
{
}

/** Destructor.
 *
 * Takes care of releasing:
 *  - Debug Registers
 */
template <class CONFIG>
CPU<CONFIG>::~CPU()
{
}

/** Get a register by its name.
 * Gets a register interface to the register specified by name.
 *
 * @param name the name of the requested register
 *
 * @return a pointer to the RegisterInterface corresponding to name
 */
template <class CONFIG>
unisim::service::interfaces::Register*
CPU<CONFIG>::GetRegister(const char *name)
{
  // RegistersRegistry::iterator itr = registers_registry.find( name );
  // if (itr != registers_registry.end())
  //   return itr->second;
  // else
    return 0;
}

/** Scan available registers for the Registers interface
 * 
 *  Allows clients of the Registers interface to scan available
 * register by providing a suitable RegisterScanner interface.
 */
template <class CONFIG>
void
CPU<CONFIG>::ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner )
{
  scanner.Append( this->GetRegister( "r0" ) );
  scanner.Append( this->GetRegister( "r1" ) );
  scanner.Append( this->GetRegister( "r2" ) );
  scanner.Append( this->GetRegister( "r3" ) );
  scanner.Append( this->GetRegister( "r4" ) );
  scanner.Append( this->GetRegister( "r5" ) );
  scanner.Append( this->GetRegister( "r6" ) );
  scanner.Append( this->GetRegister( "r7" ) );
  scanner.Append( this->GetRegister( "r8" ) );
  scanner.Append( this->GetRegister( "r9" ) );
  scanner.Append( this->GetRegister( "r10" ) );
  scanner.Append( this->GetRegister( "r11" ) );
  scanner.Append( this->GetRegister( "r12" ) );
  scanner.Append( this->GetRegister( "sp" ) );
  scanner.Append( this->GetRegister( "lr" ) );
  scanner.Append( this->GetRegister( "pc" ) );
  // TODO: should expose CPSR (and most probably the APSR view)
  // scanner.Append( this->GetRegister( "cpsr" ) );
}


} // end of namespace vmsav8
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_TCC__
