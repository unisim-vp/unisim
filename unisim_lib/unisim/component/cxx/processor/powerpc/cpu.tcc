/*
 *  Copyright (c) 2007-2017,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_TCC__

#include <unisim/component/cxx/processor/powerpc/cpu.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/util/debug/simple_register.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

////////////////////////////// TypeForBitSize<> ///////////////////////////////

template <unsigned int SIZE> const unsigned int TypeForBitSize<SIZE>::BYTE_SIZE;
template <unsigned int SIZE> const typename TypeForBitSize<SIZE>::TYPE TypeForBitSize<SIZE>::MASK;

////////////////////////////// PoolAllocator<> ////////////////////////////////

template <typename OBJECT>
PoolAllocator<OBJECT>::PoolAllocator()
	: free_list()
	, objects()
{
}

template <typename OBJECT>
PoolAllocator<OBJECT>::~PoolAllocator()
{
	typename std::vector<OBJECT *>::iterator it;
	
	for(it = objects.begin(); it != objects.end(); it++)
	{
		OBJECT *object = *it;
		delete object;
	}
}

template <typename OBJECT>
OBJECT *PoolAllocator<OBJECT>::Allocate()
{
	OBJECT *object;

	if(!free_list.empty())
	{
		object = free_list.top();
		free_list.pop();
		return object;
	}

	object = new OBJECT();
	objects.push_back(object);
	return object;
}

template <typename OBJECT>
void PoolAllocator<OBJECT>::Free(OBJECT *object)
{
	free_list.push(object);
}

/////////////////////////////////// CPU<> /////////////////////////////////////

#if 0
template <typename TYPES, typename CONFIG>
template <typename SLR_REGISTER, typename CPU<TYPES, CONFIG>::SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM, typename CPU<TYPES, CONFIG>::SLR_Access_Type _SLR_ACCESS, typename CPU<TYPES, CONFIG>::SLR_Privilege_Type _SLR_PRIVILEGE>
const typename CPU<TYPES, CONFIG>::SLR_Space_Type CPU<TYPES, CONFIG>::SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, _SLR_ACCESS, _SLR_PRIVILEGE>::SLR_SPACE;

template <typename TYPES, typename CONFIG>
template <typename SLR_REGISTER, typename CPU<TYPES, CONFIG>::SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM, typename CPU<TYPES, CONFIG>::SLR_Access_Type _SLR_ACCESS, typename CPU<TYPES, CONFIG>::SLR_Privilege_Type _SLR_PRIVILEGE>
const unsigned int CPU<TYPES, CONFIG>::SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, _SLR_ACCESS, _SLR_PRIVILEGE>::SLR_NUM;

template <typename TYPES, typename CONFIG>
template <typename SLR_REGISTER, typename CPU<TYPES, CONFIG>::SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM, typename CPU<TYPES, CONFIG>::SLR_Access_Type _SLR_ACCESS, typename CPU<TYPES, CONFIG>::SLR_Privilege_Type _SLR_PRIVILEGE>
const typename CPU<TYPES, CONFIG>::SLR_Access_Type CPU<TYPES, CONFIG>::SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, _SLR_ACCESS, _SLR_PRIVILEGE>::SLR_ACCESS;

template <typename TYPES, typename CONFIG>
template <typename SLR_REGISTER, typename CPU<TYPES, CONFIG>::SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM, typename CPU<TYPES, CONFIG>::SLR_Access_Type _SLR_ACCESS, typename CPU<TYPES, CONFIG>::SLR_Privilege_Type _SLR_PRIVILEGE>
const typename CPU<TYPES, CONFIG>::SLR_Privilege_Type CPU<TYPES, CONFIG>::SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, _SLR_ACCESS, _SLR_PRIVILEGE>::SLR_PRIVILEGE;
#endif

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::CPU(const char *name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, unisim::kernel::service::Client<typename unisim::service::interfaces::SymbolTableLookup<typename TYPES::ADDRESS> >(name, parent)
	, unisim::kernel::service::Client<typename unisim::service::interfaces::DebugControl<typename TYPES::ADDRESS> >(name, parent)
	, unisim::kernel::service::Client<typename unisim::service::interfaces::MemoryAccessReporting<typename TYPES::ADDRESS> >(name, parent)
	, unisim::kernel::service::Client<typename unisim::service::interfaces::TrapReporting>(name, parent)
	, unisim::kernel::service::Service<typename unisim::service::interfaces::MemoryAccessReportingControl>(name, parent)
	, unisim::kernel::service::Service<typename unisim::service::interfaces::Registers>(name, parent)
	, unisim::kernel::service::Service<typename unisim::service::interfaces::Synchronizable>(name, parent)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, debug_control_import("debug-control-import", this)
	, memory_access_reporting_import("memory-access-reporting-import", this)
	, trap_reporting_import("trap-reporting-import", this)
	, memory_access_reporting_control_export("memory-access-reporting-control-export", this)
	, registers_export("registers-export", this)
	, synchronizable_export("synchronizable-export", this)
	, logger(*this)
	, requires_memory_access_reporting(false)
	, requires_finished_instruction_reporting(false)
	, instruction_counter(0)
	, param_instruction_counter("instruction-counter", this, instruction_counter, "number of simulated instructions")
	, trap_on_instruction_counter(0xffffffffffffffffULL)
	, param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter, "number of simulated instruction before traping")
	, max_inst(0xffffffffffffffffULL)
	, param_max_inst("max-inst", this, max_inst, "maximum number of instructions to simulate")
	, halt_on_addr(~typename TYPES::ADDRESS(0))
	, halt_on()
	, param_halt_on("halt-on", this, halt_on, "Symbol or address where to stop simulation")
	, verbose_setup(false)
	, param_verbose_setup("verbose-setup", this, verbose_setup, "enable/disable verbosity while setup")
	, verbose_exception(false)
	, param_verbose_exception("verbose-exception", this, verbose_exception, "enable/disable verbosity of exception")
	, verbose_interrupt(false)
	, param_verbose_interrupt("verbose-interrupt", this, verbose_interrupt, "enable/disable verbosity of interrupt")
	, verbose_move_to_slr(false)
	, param_verbose_move_to_slr("verbose-move-to-slr", this, verbose_move_to_slr, "enable/disable verbosity of move to system level registers (SPRs, and so on)")
	, verbose_move_from_slr(false)
	, param_verbose_move_from_slr("verbose-move-from-slr", this, verbose_move_from_slr, "enable/disable verbosity of move from system level registers (SPRs, and so on)")
	, registers_registry()
	, exception_dispatcher(static_cast<typename CONFIG::CPU *>(this))
	, invalid_spr(static_cast<typename CONFIG::CPU *>(this))
	, invalid_tbr(static_cast<typename CONFIG::CPU *>(this))
	, invalid_pmr(static_cast<typename CONFIG::CPU *>(this))
	, invalid_tmr(static_cast<typename CONFIG::CPU *>(this))
	, external_dcr(static_cast<typename CONFIG::CPU *>(this))
	, slr()
	, reset_addr(0x0)
	, param_reset_addr("reset-addr", this, reset_addr, "reset address")
	, cia(0)
	, nia(0)
	, gpr()
	, xer()
	, lr()
	, ctr()
	, cr()
{
	param_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_trap_on_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_max_inst.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	unsigned int i;
	
	for(i = 0; i < 32; i++)
	{
		GPR& ith_gpr = gpr[i];
		ith_gpr.Init(i);
		AddRegisterInterface(ith_gpr.CreateRegisterInterface());
	}
	AddRegisterInterface(xer.CreateRegisterInterface());
	AddRegisterInterface(lr.CreateRegisterInterface());
	AddRegisterInterface(ctr.CreateRegisterInterface());
	AddRegisterInterface(cr.CreateRegisterInterface());

	AddRegisterInterface(new unisim::util::debug::SimpleRegister<uint32_t>("pc", &cia));
}

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::~CPU()
{
	std::map<std::string, unisim::service::interfaces::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		delete reg_iter->second;
	}
	
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::BeginSetup()
{
	unsigned int slr_space_idx;
	for(slr_space_idx = SLR_SPR_SPACE; slr_space_idx <= SLR_TMR_SPACE; slr_space_idx++)
	{
		unsigned int slr_reg_num;
		for(slr_reg_num = 0; slr_reg_num < 1024; slr_reg_num++)
		{
			SLRBase *slr_p = slr[slr_space_idx][slr_reg_num];
			
			if(slr_p)
			{
				slr_p->AddRegisterInterface();
			}
		}
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::EndSetup()
{
	if(!halt_on.empty())
	{
		const unisim::util::debug::Symbol<typename TYPES::ADDRESS> *halt_on_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByName(halt_on.c_str(), unisim::util::debug::Symbol<typename TYPES::ADDRESS>::SYM_FUNC) : 0;
		
		if(halt_on_symbol)
		{
			halt_on_addr = halt_on_symbol->GetAddress();
			if(verbose_setup)
			{
				logger << DebugInfo << "Simulation will halt at '" << halt_on_symbol->GetName() << "' (0x" << std::hex << halt_on_addr << std::dec << ")" << EndDebugInfo;
			}
		}
		else
		{
			std::stringstream sstr(halt_on);
			sstr >> std::hex;
			if(sstr >> halt_on_addr)
			{
				if(verbose_setup)
				{
					logger << DebugInfo <<  "Simulation will halt at 0x" << std::hex << halt_on_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				logger << DebugWarning << "Invalid address (" << halt_on << ") in Parameter " << param_halt_on.GetName() << EndDebugWarning;
				halt_on_addr = (typename TYPES::ADDRESS) -1;
			}
		}
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Reset()
{
	// GPRs and CR are unaffected
	
	unsigned int slr_space_idx;
	for(slr_space_idx = SLR_SPR_SPACE; slr_space_idx <= SLR_TMR_SPACE; slr_space_idx++)
	{
		unsigned int slr_reg_num;
		for(slr_reg_num = 0; slr_reg_num < 1024; slr_reg_num++)
		{
			SLRBase *slr_p = slr[slr_space_idx][slr_reg_num];
			
			if(slr_p)
			{
				slr_p->Reset();
			}
		}
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::RegisterSLR(unsigned int n, SLRBase *slr_p)
{
	//std::cerr << "registering #" << slr_p->GetRegNum() << " of space #" << slr_p->GetSpace() << std::endl;
	SLR_Space_Type slr_space = slr_p->GetSpace();
	assert(slr[slr_space][n] == 0);
	slr[slr_space][n] = slr_p;
}

template <typename TYPES, typename CONFIG>
typename CPU<TYPES, CONFIG>::SPRBase& CPU<TYPES, CONFIG>::GetSPR(unsigned int n)
{
	SLRBase *slr_p = slr[SLR_SPR_SPACE][n];
	
	if(slr_p) return *slr_p;
	
	return invalid_spr;
}

template <typename TYPES, typename CONFIG>
typename CPU<TYPES, CONFIG>::TBRBase& CPU<TYPES, CONFIG>::GetTBR(unsigned int n)
{
	SLRBase *slr_p = slr[SLR_TBR_SPACE][n];
	
	if(slr_p) return *slr_p;
	
	return invalid_tbr;
}

template <typename TYPES, typename CONFIG>
typename CPU<TYPES, CONFIG>::DCRBase& CPU<TYPES, CONFIG>::GetDCR(unsigned int n)
{
	SLRBase *slr_p = slr[SLR_DCR_SPACE][n];

	if(slr_p) return *slr_p;
	
	external_dcr.Attach(n);
	
	return external_dcr;
}

template <typename TYPES, typename CONFIG>
typename CPU<TYPES, CONFIG>::PMRBase& CPU<TYPES, CONFIG>::GetPMR(unsigned int n)
{
	SLRBase *slr_p = slr[SLR_PMR_SPACE][n];

	if(slr_p) return *slr_p;
	
	return invalid_pmr;
}

template <typename TYPES, typename CONFIG>
typename CPU<TYPES, CONFIG>::TMRBase& CPU<TYPES, CONFIG>::GetTMR(unsigned int n)
{
	SLRBase *slr_p = slr[SLR_TMR_SPACE][n];

	if(slr_p) return *slr_p;
	
	return invalid_tmr;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveFromSPR(unsigned int n, uint32_t& value)
{
	switch(n)
	{
		case 1:
			value = xer;
			return true;
		case 8:
			value = lr;
			return true;
		case 9:
			value = ctr;
			return true;
	}
	
	SPRBase& spr = GetSPR(n);
	if(!spr.CheckMoveFromLegality()) return false;
	return spr.MoveFrom(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveToSPR(unsigned int n, uint32_t value)
{
	switch(n)
	{
		case 1:
			xer = value;
			return true;
		case 8:
			lr = value;
			return true;
		case 9:
			ctr = value;
			return true;
	}

	SPRBase& spr = GetSPR(n);
	if(!spr.CheckMoveToLegality()) return false;
	return spr.MoveTo(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveFromTBR(unsigned int n, uint32_t& value)
{
	TBRBase& tbr = GetTBR(n);
	if(!tbr.CheckMoveFromLegality()) return false;
	return tbr.MoveFrom(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveToTBR(unsigned int n, uint32_t value)
{
	TBRBase& tbr = GetTBR(n);
	if(!tbr.CheckMoveToLegality()) return false;
	return tbr.MoveTo(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveFromDCR(unsigned int n, uint32_t& value)
{
	DCRBase& dcr = GetDCR(n);
	if(!dcr.CheckMoveFromLegality()) return false;
	return dcr.MoveFrom(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveToDCR(unsigned int n, uint32_t value)
{
	DCRBase& dcr = GetDCR(n);
	if(!dcr.CheckMoveToLegality()) return false;
	return dcr.MoveTo(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveFromPMR(unsigned int n, uint32_t& value)
{
	PMRBase& pmr = GetPMR(n);
	if(!pmr.CheckMoveFromLegality()) return false;
	return pmr.MoveFrom(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveToPMR(unsigned int n, uint32_t value)
{
	PMRBase& pmr = GetPMR(n);
	if(!pmr.CheckMoveToLegality()) return false;
	return pmr.MoveTo(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveFromTMR(unsigned int n, uint32_t& value)
{
	TMRBase& tmr = GetTMR(n);
	if(!tmr.CheckMoveFromLegality()) return false;
	return tmr.MoveFrom(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveToTMR(unsigned int n, uint32_t value)
{
	TMRBase& tmr = GetTMR(n);
	if(!tmr.CheckMoveToLegality()) return false;
	return tmr.MoveTo(value);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::AddRegisterInterface(unisim::service::interfaces::Register *reg_if)
{
	registers_registry[reg_if->GetName()] = reg_if;
}

/////////////////////////// ExceptionDispatcher<> /////////////////////////////

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::ExceptionDispatcher(typename CONFIG::CPU *_cpu)
	: cpu(_cpu)
	, exc_flags(0)
	, exc_mask(0)
	, interrupts()
{
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::~ExceptionDispatcher()
{
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
void CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::InstallInterrupt(unsigned int priority, InterruptBase *interrupt)
{
	assert(interrupts[priority] == 0);
	interrupts[priority] = interrupt;
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
template <typename EXCEPTION>
typename EXCEPTION::INTERRUPT& CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::ThrowException()
{
	if(cpu->verbose_exception && !(exc_flags & EXCEPTION::template GetMask<TYPE>()))
	{
		cpu->GetDebugInfoStream() << "Throwing " << EXCEPTION::GetName() << std::endl;
	}
	exc_flags = exc_flags | EXCEPTION::template GetMask<TYPE>();
	return *static_cast<typename EXCEPTION::INTERRUPT *>(interrupts[EXCEPTION::PRIORITY]);
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
template <typename INTERRUPT> void CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::AckInterrupt()
{
	if(cpu->verbose_exception && (exc_flags & INTERRUPT::template GetMask<TYPE>()))
	{
		cpu->GetDebugInfoStream() << "Acknowledging " <<INTERRUPT::GetName() << std::endl;
	}
	exc_flags = exc_flags & ~INTERRUPT::template GetMask<TYPE>();
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
template <typename INTERRUPT> void CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::EnableInterrupt()
{
	if(cpu->verbose_interrupt && ((exc_mask & INTERRUPT::template GetMask<TYPE>()) == 0))
	{
		cpu->GetDebugInfoStream() << "Enabling " << INTERRUPT::GetName() << std::endl;
	}
	exc_mask = exc_mask | INTERRUPT::template GetMask<TYPE>();
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
template <typename INTERRUPT> void CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::DisableInterrupt()
{
	if(cpu->verbose_interrupt && ((exc_mask & INTERRUPT::template GetMask<TYPE>()) != 0))
	{
		cpu->GetDebugInfoStream() << "Disabling " << INTERRUPT::GetName() << std::endl;
	}
	exc_mask = exc_mask & ~INTERRUPT::template GetMask<TYPE>();
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
inline void CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::ProcessExceptions()
{
	unsigned int exception_priority;
	if(unlikely(unisim::util::arithmetic::BitScanForward(exception_priority, exc_flags & exc_mask)))
	{
		InterruptBase *interrupt = interrupts[exception_priority];

		assert(interrupt != 0);
		if(unlikely(cpu->verbose_interrupt))
		{
			cpu->GetDebugInfoStream() << interrupt->GetInterruptName() << " (offset 0x" << std::hex << interrupt->GetOffset() << std::dec << ") is interrupting execution at @0x" << std::hex << cpu->GetCIA() << std::dec << std::endl;
		}
		interrupt->ProcessInterrupt(cpu);
	}
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
template <typename EXCEPTION> bool CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::RecognizedException() const
{
	return (exc_flags & exc_mask) & EXCEPTION::template GetMask<TYPE>();
}

template <typename TYPES, typename CONFIG>
inline void CPU<TYPES, CONFIG>::MonitorLoad(typename TYPES::ADDRESS ea, unsigned int size)
{
	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, ea, size);
	}
}

template <typename TYPES, typename CONFIG>
inline void CPU<TYPES, CONFIG>::MonitorStore(typename TYPES::ADDRESS ea, unsigned int size)
{
	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, ea, size);
	}
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int8Load(unsigned int rd, typename TYPES::ADDRESS ea)
{
	uint8_t value;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint8_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) value; // 8-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int16Load(unsigned int rd, typename TYPES::ADDRESS ea)
{
	uint16_t value;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint16_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) value; // 16-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::SInt16Load(unsigned int rd, typename TYPES::ADDRESS ea)
{
	uint16_t value;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint16_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) (int16_t) value; // 16-bit to 32-bit sign extension
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int32Load(unsigned int rd, typename TYPES::ADDRESS ea)
{
	uint32_t value;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint32_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = value;
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int16LoadByteReverse(unsigned int rd, typename TYPES::ADDRESS ea)
{
	uint16_t value;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint16_t, true, false>(value, ea); // reversed
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) value; // 16-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int32LoadByteReverse(unsigned int rd, typename TYPES::ADDRESS ea)
{
	uint32_t value;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint32_t, true, false>(value, ea); // reversed
	if(unlikely(!status)) return false;
	gpr[rd] = value;
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::IntLoadMSBFirst(unsigned int rd, typename TYPES::ADDRESS ea, uint32_t size)
{
	switch(size)
	{
		case 1:
		{
			uint8_t value;
			bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint8_t, false, true>(value, ea);
			if(unlikely(!status)) return false;
			gpr[rd] = (uint32_t) value << 24;
			break;
		}

		case 2:
		{
			uint16_t value;
			bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint16_t, false, true>(value, ea);
			if(unlikely(!status)) return false;
			gpr[rd] = (uint32_t) unisim::util::endian::BigEndian2Host(value) << 16;
			break;
		}

		case 3:
		{
			typedef uint8_t array_uint8_3_t[3];
			uint8_t buffer[3];
			bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<array_uint8_3_t, false, true>(buffer, ea);
			if(unlikely(!status)) return false;
			uint32_t value = ((uint32_t) buffer[0] << 24) | ((uint32_t) buffer[1] << 16) | ((uint32_t) buffer[2] << 8);
			gpr[rd] = value;
			break;
		}

		case 4:
		{
			uint32_t value;
			bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint32_t, false, true>(value, ea);
			if(unlikely(!status)) return false;
			gpr[rd] = unisim::util::endian::BigEndian2Host(value);
			break;
		}
		
		default:
			return false;
	}
	MonitorLoad(ea, size);
	return true;
}

template <typename TYPES, typename CONFIG>
template <typename REGISTER>
bool CPU<TYPES, CONFIG>::SpecialLoad(REGISTER& reg, typename TYPES::ADDRESS ea)
{
	uint32_t value;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint32_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	reg = value;
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int8Store(unsigned int rs, typename TYPES::ADDRESS ea)
{
	uint8_t value = gpr[rs];
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint8_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int16Store(unsigned int rs, typename TYPES::ADDRESS ea)
{
	uint16_t value = (uint16_t) gpr[rs];
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint16_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int32Store(unsigned int rs, typename TYPES::ADDRESS ea)
{
	uint32_t value = gpr[rs];
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint32_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int16StoreByteReverse(unsigned int rs, typename TYPES::ADDRESS ea)
{
	uint16_t value = (uint16_t) gpr[rs];
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint16_t, true, false>(value, ea); // reversed
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int32StoreByteReverse(unsigned int rs, typename TYPES::ADDRESS ea)
{
	uint32_t value = gpr[rs];
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint32_t, true, false>(value, ea); // reversed
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::IntStoreMSBFirst(unsigned int rs, typename TYPES::ADDRESS ea, uint32_t size)
{
	switch(size)
	{
		case 1:
			{
				uint8_t value = gpr[rs] >> 24;
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint8_t, false, true>(value, ea);
				if(unlikely(!status)) return false;
				break;
			}

		case 2:
			{
				uint16_t value = unisim::util::endian::Host2BigEndian((uint16_t)(gpr[rs] >> 16));
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint16_t, false, true>(value, ea);
				if(unlikely(!status)) return false;
				break;
			}

		case 3:
			{
				typedef uint8_t array_uint8_3_t[3];
				uint32_t value = gpr[rs];
				uint8_t buffer[3];
				buffer[0] = value >> 24;
				buffer[1] = value >> 16;
				buffer[2] = value >> 8;
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<array_uint8_3_t, false, true>(buffer, ea);
				if(unlikely(!status)) return false;
				break;
			}

		case 4:
			{
				uint32_t value = unisim::util::endian::Host2BigEndian(gpr[rs]);
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint32_t, false, true>(value, ea);
				if(unlikely(!status)) return false;
				break;
			}
			
		default:
			return false;
	}

	MonitorStore(ea, size);
	return true;
}

template <typename TYPES, typename CONFIG>
template <typename REGISTER>
bool CPU<TYPES, CONFIG>::SpecialStore(const REGISTER& reg, typename TYPES::ADDRESS ea)
{
	uint32_t value = reg;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint32_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <typename TYPES, typename CONFIG>
unisim::service::interfaces::Register *CPU<TYPES, CONFIG>::GetRegister(const char *name)
{
	std::map<std::string, unisim::service::interfaces::Register *>::iterator reg_iter = registers_registry.find(name);
	if(reg_iter != registers_registry.end())
	{
		return (*reg_iter).second;
	}

	return 0;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	std::map<std::string, unisim::service::interfaces::Register *>::iterator reg_iter;
	
	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		unisim::service::interfaces::Register *reg_if = (*reg_iter).second;
		scanner.Append(reg_if);
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Synchronize()
{
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::RequiresMemoryAccessReporting(bool report)
{
	requires_memory_access_reporting = report;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::RequiresFinishedInstructionReporting(bool report)
{
	requires_finished_instruction_reporting = report;
}

template <typename TYPES, typename CONFIG>
std::string CPU<TYPES, CONFIG>::GetObjectFriendlyName(typename TYPES::ADDRESS addr)
{
	std::stringstream sstr;
	
	const unisim::util::debug::Symbol<typename TYPES::ADDRESS> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(addr, unisim::util::debug::Symbol<typename TYPES::ADDRESS>::SYM_OBJECT) : 0;
	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return sstr.str();
}

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_TCC__


