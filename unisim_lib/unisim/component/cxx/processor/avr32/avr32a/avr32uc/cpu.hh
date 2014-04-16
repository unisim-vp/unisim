/*
 *  Copyright (c) 2014,
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
 * Authors: Julien Lisita (julien.lisita@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_AVR32A_AVR32UC_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_AVR32A_AVR32UC_CPU_HH__

#include <unisim/kernel/logger/logger.hh>
#include <unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.hh>
#include <unisim/component/cxx/processor/avr32/avr32a/avr32uc/config.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/synchronizable.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/avr32_t2h_syscalls.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/inlining/inlining.hh>
#include <map>
#include <iosfwd>

#include <unisim/kernel/debug/debug.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace avr32 {
namespace avr32a {
namespace avr32uc {

using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::AVR32_T2H_Syscalls;
using namespace unisim::util::endian;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Object;
using unisim::service::interfaces::Loader;
using unisim::util::debug::Symbol;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Synchronizable;
using unisim::service::interfaces::TrapReporting;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Statistic;
using unisim::kernel::service::ParameterArray;
using unisim::kernel::service::Formula;
using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using namespace std;

template <class CONFIG>
class CPU :
	public unisim::component::cxx::processor::avr32::avr32a::avr32uc::Decoder<CONFIG>,
	public Client<Loader>,
	public Client<SymbolTableLookup<typename CONFIG::address_t> >,
	public Client<DebugControl<typename CONFIG::address_t> >,
	public Client<MemoryAccessReporting<typename CONFIG::address_t> >,
	public Client<TrapReporting>,
	public Service<MemoryAccessReportingControl>,
	public Service<Disassembly<typename CONFIG::address_t> >,
	public Service<unisim::service::interfaces::Registers>,
	public Service<Memory<typename CONFIG::address_t> >,
	public Service<MemoryInjection<typename CONFIG::address_t> >,
	public Client<Memory<typename CONFIG::physical_address_t> >,
	public Service<Synchronizable>,
	public Client<AVR32_T2H_Syscalls>
{
public:
	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================
	
	ServiceExport<Disassembly<typename CONFIG::address_t> > disasm_export;
	ServiceExport<unisim::service::interfaces::Registers> registers_export;
	ServiceExport<Memory<typename CONFIG::address_t> > memory_export;
	ServiceExport<MemoryInjection<typename CONFIG::address_t> > memory_injection_export;
	ServiceExport<Synchronizable> synchronizable_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

	ServiceImport<Loader> loader_import;
	ServiceImport<DebugControl<typename CONFIG::address_t> > debug_control_import;
	ServiceImport<MemoryAccessReporting<typename CONFIG::address_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<typename CONFIG::address_t> > symbol_table_lookup_import;
	ServiceImport<Memory<typename CONFIG::physical_address_t> > memory_import;
	ServiceImport<TrapReporting> trap_reporting_import;
	ServiceImport<AVR32_T2H_Syscalls> avr32_t2h_syscalls_import;
	
	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	CPU(const char *name, Object *parent = 0);
	virtual ~CPU();
	
        //=====================================================================
	//=                        Getters//Setters                           =
	//=====================================================================    

        virtual uint32_t getPC();    //return adress of currently instruction
        virtual uint32_t getNPC();

	virtual void setPC(uint32_t pc);
        virtual void setNPC(uint32_t npc);

	
	virtual uint32_t getSP();
	virtual uint32_t getSR();
	
	

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================
	
	virtual bool BeginSetup();
	virtual bool EndSetup();
	virtual void OnDisconnect();
	
	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================

	void StepOneInstruction();
	virtual void Synchronize();
	virtual void Reset();
	virtual void Idle();

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	virtual void RequiresMemoryAccessReporting(bool report);
	virtual void RequiresFinishedInstructionReporting(bool report) ;

	//=====================================================================
	//=               Programmer view memory access methods               =
	//=====================================================================
	
	virtual bool ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size);
	virtual bool InjectReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	virtual bool InjectWriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=                        Debugging stuffs                           =
	//=====================================================================

	virtual unisim::util::debug::Register *GetRegister(const char *name);
	virtual string Disasm(typename CONFIG::address_t addr, typename CONFIG::address_t& next_addr);
	string GetObjectFriendlyName(typename CONFIG::address_t addr);
	string GetFunctionFriendlyName(typename CONFIG::address_t addr);

	//=====================================================================
	//=               Hardware check/acknowledgement methods              =
	//=====================================================================
	
	void SetNMIREQ();
	void ResetNMIREQ();
	void SetIRQ(unsigned int irq);
	void ResetIRQ(unsigned int irq);

	//=====================================================================
	//=                     Memory access methods                         =
	//=====================================================================
	
	bool Fetch(typename CONFIG::address_t addr,void *buffer, uint32_t size);
protected:

	//=====================================================================
	//=                         Timers handling                           =
	//=====================================================================
	
	uint64_t GetMaxIdleTime() const
	{
		// TODO: compute the maximum idle time, i.e. the time without any internal timer interrupts
		return 0xffffffffffffffffULL;
	}

	uint64_t GetTimersDeadline() const
	{
		// TODO: compute the timers deadline, i.e. the time until an internal timer expires
		return 0xffffffffffffffffULL;
	}

	inline void RunTimers(uint64_t delta) ALWAYS_INLINE
	{
		// TODO: run timers
	}
	
	virtual void RunInternalTimers();
	
	//=====================================================================
	//=                        Debugging stuff                            =
	//=====================================================================

	Logger logger;
    /** indicates if the memory accesses require to be reported */
    bool requires_memory_access_reporting;
    /** indicates if the finished instructions require to be reported */
    bool requires_finished_instruction_reporting;
	
	inline bool IsVerboseSetup() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SETUP_ENABLE && (verbose_all || verbose_setup); }
	inline bool IsVerboseInterrupt() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_INTERRUPT_ENABLE && (verbose_all || verbose_interrupt); }
	inline bool IsVerboseStep() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_STEP_ENABLE && (verbose_all || verbose_step); }
	
	//=====================================================================
	//=                      Bus access methods                           =
	//=====================================================================

	virtual bool IHSBRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size);
	virtual bool DHSBRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size);
	virtual bool DHSBWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size);

public:
	inline void ProcessExceptions(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation) ALWAYS_INLINE;



private:	
	//=====================================================================
	//=                      Debugging stuffs                             =
	//=====================================================================
	bool verbose_all;
	bool verbose_setup;
	bool verbose_interrupt;
	bool verbose_step;
	uint64_t trap_on_instruction_counter;
	bool enable_trap_on_exception;
	typename CONFIG::address_t halt_on_addr;
	std::string halt_on;
	uint64_t max_inst;                                         //!< maximum number of instructions to execute

	map<string, unisim::util::debug::Register *> registers_registry;       //!< Every CPU register interfaces
	uint64_t instruction_counter;                              //!< Number of executed instructions

	inline uint64_t GetInstructionCounter() const ALWAYS_INLINE { return instruction_counter; }
	inline void MonitorLoad(typename CONFIG::address_t ea, uint32_t size) ALWAYS_INLINE;
	inline void MonitorStore(typename CONFIG::address_t ea, uint32_t size) ALWAYS_INLINE;

	//=====================================================================
	//=                         AVR32A registers                          =
	//=====================================================================


	uint32_t gpr[16]; // register file gpr[15]=pc,gpr[14]=lr , gpr[13]=sp
	uint32_t npc;     //program counter
	uint32_t sr;      // stack register
	

	
	//=====================================================================
	//=                    CPU run-time parameters                        =
	//=====================================================================
	
	Parameter<uint64_t> param_max_inst;                   //!< linked to member max_inst
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_setup;
	Parameter<bool> param_verbose_interrupt;
	Parameter<bool> param_verbose_step;
	Parameter<uint64_t> param_trap_on_instruction_counter;
	Parameter<bool> param_enable_trap_on_exception;
	Parameter<std::string> param_halt_on;

	//=====================================================================
	//=                    CPU run-time statistics                        =
	//=====================================================================

	Statistic<uint64_t> stat_instruction_counter;
};

} // end of namespace avr32uc
} // end of namespace avr32a
} // end of namespace avr32
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
