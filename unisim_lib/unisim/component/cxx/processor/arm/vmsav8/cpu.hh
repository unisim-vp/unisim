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
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_HH__

#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/inlining/inlining.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/register.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/memory_injection.hh>
// #include <unisim/service/interfaces/memory_access_reporting.hh>
// #include <unisim/service/interfaces/debug_control.hh>
// #include <unisim/service/interfaces/trap_reporting.hh>
#include <map>
#include <set>
#include <stdexcept>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace vmsav8 {

/** Armv8 cpu
 *
 * Defines and implements architectural state of armv8 processors.
 */

template <typename CONFIG>
struct CPU
  : public virtual unisim::kernel::service::Object
  , public unisim::kernel::service::Service<unisim::service::interfaces::Registers>
  // , public unisim::kernel::service::Service<unisim::service::interfaces::MemoryAccessReportingControl>
  // , public unisim::kernel::service::Client<unisim::service::interfaces::MemoryAccessReporting<uint64_t> >
  // , public unisim::kernel::service::Service<unisim::service::interfaces::MemoryInjection<uint64_t> >
  // , public unisim::kernel::service::Client<unisim::service::interfaces::DebugControl<uint64_t> >
  // , public unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>
  // , public unisim::kernel::service::Service<unisim::service::interfaces::Disassembly<uint64_t> >
  // , public unisim::kernel::service::Service<unisim::service::interfaces::Memory<uint64_t> >
  // , public unisim::kernel::service::Client<unisim::service::interfaces::Memory<uint64_t> >
  // , public unisim::kernel::service::Client<unisim::service::interfaces::LinuxOS>
  // , public unisim::kernel::service::Client<unisim::service::interfaces::SymbolTableLookup<uint64_t> >
{
  typedef CONFIG Config;
  // typedef simfloat::FP FP;
  // typedef FP::F64  F64;
  // typedef FP::F32  F32;
  typedef uint8_t  U8;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef uint64_t U64;
  typedef int8_t   S8;
  typedef int16_t  S16;
  typedef int32_t  S32;
  typedef int64_t  S64;
  typedef bool     BOOL;
  
  /*
   * ARM architecture constants
   */
    
  CPU(const char* name, Object* parent);
  ~CPU();
  
  //=====================================================================
  //=                       Logger                                      =
  //=====================================================================
  
  /** Unisim logging services. */
  unisim::kernel::logger::Logger logger;
  /** Verbosity of the CPU implementation */
  bool verbose;

  virtual unisim::service::interfaces::Register* GetRegister( const char* name );
  virtual void ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner );
		
  //=====================================================================
  //=                  public service imports/exports                   =
  //=====================================================================

  unisim::kernel::service::ServiceImport<unisim::service::interfaces::DebugControl<uint64_t> >          debug_control_import;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::TrapReporting>                    instruction_counter_trap_reporting_import;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::SymbolTableLookup<uint64_t> >     symbol_table_lookup_import;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::Memory<uint64_t> >                memory_import;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::MemoryAccessReporting<uint64_t> > memory_access_reporting_import;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::LinuxOS>                          linux_os_import;
  // unisim::kernel::service::ServiceImport<unisim::service::interfaces::TrapReporting>                    exception_trap_reporting_import;

  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Registers>                        registers_export;
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Memory<uint64_t> >                memory_export;
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Disassembly<uint64_t> >           disasm_export;
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::MemoryAccessReportingControl>     memory_access_reporting_control_export;
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::MemoryInjection<uint64_t> >       memory_injection_export;


};

} // end of namespace vmsav8
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_HH__
