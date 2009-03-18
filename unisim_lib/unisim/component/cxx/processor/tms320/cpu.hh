/*
 *  Copyright (c) 2009,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CPU_HH__

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/debug_control.hh"
#include "unisim/service/interfaces/disassembly.hh"
#include "unisim/service/interfaces/memory_access_reporting.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"
#include "unisim/component/cxx/processor/tms320/isa_tms320.hh"
#include "unisim/util/endian/endian.hh"

#include <string>
#include <map>

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320 {

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Parameter;
using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::util::debug::Register;

template<class CONFIG, bool DEBUG = false>
class CPU :
	public Service<MemoryInjection<typename CONFIG::address_t> >,
	public Client<DebugControl<typename CONFIG::address_t> >,
	public Client<MemoryAccessReporting<typename CONFIG::address_t> >,
	public Service<MemoryAccessReportingControl>,
	public Service<Disassembly<typename CONFIG::address_t> >,
	public Service<Registers>,
	public Service<Memory<typename CONFIG::address_t> >,
	public Client<Memory<typename CONFIG::address_t> >
{
private:
	typedef typename CONFIG::address_t address_t;
	
	// the kernel logger
	unisim::kernel::logger::Logger logger;
	
public:
	//===============================================================
	//= Client/Service setup methods                          START =
	//===============================================================
	
	CPU(const char *name, Object *parent = 0);
	virtual ~CPU();
	
	//===============================================================
	//= Client/Service setup methods                           STOP =
	//===============================================================
	
	//===============================================================
	//= Public service imports/exports                        START =
	//===============================================================
	
	ServiceExport<Disassembly<uint64_t> > disasm_export;
	ServiceExport<Registers> registers_export;
	ServiceExport<MemoryInjection<address_t> > memory_injection_export;
	ServiceExport<Memory<uint64_t> > memory_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;
	
	ServiceImport<DebugControl<uint64_t> > debug_control_import;
	ServiceImport<MemoryAccessReporting<uint64_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<uint64_t> > symbol_table_lookup_import;
	ServiceImport<Memory<uint64_t> > memory_import; // TODO: check for removal
	
	//===============================================================
	//= Public service imports/exports                         STOP =
	//===============================================================
	
    //===============================================================
	//= Client/Service setup methods                          START =
	//===============================================================

	virtual bool Setup();
	virtual void OnDisconnect();
	
    //===============================================================
	//= Client/Service setup methods                           STOP =
	//===============================================================

    //===============================================================
	//= Memory injection interface methods                    START =
	//===============================================================
	
	virtual bool InjectReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool InjectWriteMemory(address_t addr, const void *buffer, uint32_t size);

    //===============================================================
	//= Memory injection interface methods                     STOP =
	//===============================================================

    //===============================================================
	//= Memory access reporting control interface methods     START =
	//===============================================================

	virtual void RequiresMemoryAccessReporting(bool report);
	virtual void RequiresFinishedInstructionReporting(bool report);
	
    //===============================================================
	//= Memory access reporting control interface methods      STOP =
	//===============================================================

    //===============================================================
	//= Memory interface methods                              START =
	//===============================================================

	virtual void Reset();
	virtual bool ReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(address_t addr, const void *buffer, uint32_t size);

    //===============================================================
	//= Memory interface methods                               STOP =
	//===============================================================

    //===============================================================
	//= CPURegistersInterface interface methods               START =
	//===============================================================

	/**
	 * Gets a register interface to the register specified by name.
	 * 
	 * @param  name   The name of the requested register.
	 * @return        A pointer to the RegisterInterface corresponding to name.
	 */
	virtual Register *GetRegister(const char *name);

    //===============================================================
	//= CPURegistersInterface interface methods                STOP =
	//===============================================================

    //===============================================================
	//= DebugDisasmInterface interface methods                START =
	//===============================================================
	
	/**
	 * Returns a string with the disassembling for the instruction found
	 *   at address addr.
	 *
	 * @param   addr       The address of the instruction to disassemble.
	 * @param   next_addr  The address following the requested instruction.
	 * @return             The disassembling of the requested instruction address.
	 */
	 virtual std::string Disasm(address_t addr, address_t &next_addr);
	
    //===============================================================
	//= DebugDisasmInterface interface methods                 STOP =
	//===============================================================

    //===============================================================
	//= Execution methods                                     START =
	//===============================================================

	/** Execute one complete instruction
	 */
	void StepInstruction();
	virtual void Stop(int ret);
	
    //===============================================================
	//= Execution methods                                      STOP =
	//===============================================================

private:
	/** The registers interface for debugging purpose */
	std::map<std::string, Register *> registers_registry;
	/** indicates if the memory accesses require to be reported */
	bool requires_memory_access_reporting;
	/** indicates if the finished instructions require to be reported */
	bool requires_finished_instruction_reporting;
  
	//===============================================================
	//= Instruction set decoder variables                     START =
	//===============================================================

	typename isa::tms320::Decoder<CONFIG, DEBUG> decoder;

    //===============================================================
	//= Verbose variables, parameters, and methods            START =
	//===============================================================

	bool verbose_all;
	bool verbose_setup;
	
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_setup;
	
	inline INLINE bool VerboseAll();
	inline INLINE bool VerboseSetup();
}; // end of calss CPU<CONFIG>

} // end of namespace tms320
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#undef INLINE

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CPU_HH__

