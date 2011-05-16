/*
 *  Copyright (c) 2007,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_EXCEPTION_HANDLING_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_EXCEPTION_HANDLING_TCC__

#include <sstream>
#include <unisim/component/cxx/processor/powerpc/exception.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

/* System call exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const SystemCallException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	if(linux_os_import)
	{
		// Do Linux ABI translation
		linux_os_import->ExecuteSystemCall(GetGPR(0));
	}
	else
	{
		// Invalidate shadow TLBs
		InvalidateITLB();
		InvalidateDTLB();
		
		SetSRR0(GetNIA()); // effective address of the instruction after the system call instruction
		
		SetSRR1(GetMSR()); // content of MSR
		
		SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
		
		SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_SYSTEM_CALL));

		if(unlikely(IsVerboseException() || enable_trap_on_exception))
		{
			std::stringstream sstr;
			sstr << "At 0x" << std::hex << GetCIA() << std::dec;
			const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
			if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
			sstr << ", got a " << exc.what() << " (syscall #" << GetGPR(0) << ")";
			std::string msg = sstr.str();

			if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
			if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const MachineCheckException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	switch(exc.GetType())
	{
		case MachineCheckException<CONFIG>::MCP_INSTRUCTION_SYNCHRONOUS:
			break;
			SetMCSRR0(GetCIA()); // effective address of the instruction presenting the exception
		case MachineCheckException<CONFIG>::MCP_INSTRUCTION_ASYNCHRONOUS:
		case MachineCheckException<CONFIG>::MCP_DATA_ASYNCHRONOUS:
		case MachineCheckException<CONFIG>::MCP_TLB_ASYNCHRONOUS:
			SetMCSRR0(GetNIA()); // effective address of the next instruction to be executed
			break;
	}
	
	SetMCSRR1(GetMSR()); // content of MSR
	
	SetMSR(0); //  all MSR bits set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_MACHINE_CHECK));

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DecrementerInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetNIA()); // effective address of the next instruction to be executed
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_DECREMENTER));
	
	//ResetIRQ(CONFIG::IRQ_DECREMENTER_INTERRUPT);

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const ExternalInputInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetNIA()); // effective address of the next instruction to be executed
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_EXTERNAL_INPUT));

	//ResetIRQ(CONFIG::IRQ_EXTERNAL_INPUT_INTERRUPT);

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const CriticalInputInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetCSRR0(GetNIA()); // effective address of the next instruction to be executed
	
	SetCSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & CONFIG::MSR_ME_MASK); // MSR[ME] unchanged, all other MSR bits set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_CRITICAL_INPUT));

	//ResetIRQ(CONFIG::IRQ_CRITICAL_INPUT_INTERRUPT);

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DSIException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetDEAR(exc.GetAddress()); // effective address causing the esception

	SetESR(GetESR() & CONFIG::ESR_MCI_MASK); // leave ESR[MCI] unmodified, other ESR bits set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_DATA_STORAGE));

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(func_symbol) sstr << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what() << " while accessing data at 0x" << std::hex << exc.GetAddress() << std::dec;
		const Symbol<typename CONFIG::address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) sstr << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const ISIException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetESR(GetESR() & CONFIG::ESR_MCI_MASK); // ESR[MCI] unchanged, all other bits are set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_INSTRUCTION_STORAGE));

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DataTLBErrorException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetDEAR(exc.GetAddress()); // effective address causing the esception

	uint32_t esr_value = (GetESR() & CONFIG::ESR_MCI_MASK) | (operation->get_esr() & (CONFIG::ESR_FP_MASK | CONFIG::ESR_ST_MASK | CONFIG::ESR_AP_MASK));
	SetESR(esr_value);
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_DATA_TLB_ERROR));

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(func_symbol) sstr << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what() << " while accessing data at 0x" << std::hex << exc.GetAddress() << std::dec;
		const Symbol<typename CONFIG::address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) sstr << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const InstructionTLBErrorException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_INSTRUCTION_TLB_ERROR));

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const AlignmentException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetDEAR(exc.GetAddress()); // effective address causing the esception

	uint32_t esr_value = operation->get_esr() & (CONFIG::ESR_FP_MASK | CONFIG::ESR_ST_MASK | CONFIG::ESR_AP_MASK);
	SetESR(esr_value);
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_ALIGNMENT));

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const ProgramException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	uint32_t esr_value = operation->get_esr() & (CONFIG::ESR_PIL_MASK | CONFIG::ESR_PPR_MASK | CONFIG::ESR_PTR_MASK | CONFIG::ESR_PUO_MASK
	                                           | CONFIG::ESR_FP_MASK | CONFIG::ESR_AP_MASK | CONFIG::ESR_PIE_MASK | CONFIG::ESR_PCRE_MASK
	                                           | CONFIG::ESR_PCMP_MASK | CONFIG::ESR_PCRF_MASK);
	switch(exc.GetType())
	{
		case ProgramException<CONFIG>::PX_ILLEGAL_INSTRUCTION:
			esr_value |= CONFIG::ESR_PIL_MASK;
			break;
		case ProgramException<CONFIG>::PX_PRIVILEGE_VIOLATION:
			esr_value |= CONFIG::ESR_PPR_MASK;
			break;
		case ProgramException<CONFIG>::PX_TRAP:
			esr_value |= CONFIG::ESR_PTR_MASK;
			break;
		case ProgramException<CONFIG>::PX_FLOATING_POINT:
			break;
		case ProgramException<CONFIG>::PX_UNIMPLEMENTED_INSTRUCTION:
			esr_value |= CONFIG::ESR_PUO_MASK;
			break;
	}
	
	SetESR(esr_value);
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_PROGRAM));
	
	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const FloatingPointUnavailableException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_FLOATING_POINT_UNAVAILABLE));

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const AuxiliaryProcessorUnavailableException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_AUXILIARY_PROCESSOR_UNAVAILABLE));

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const FixedIntervalTimerInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetNIA()); // effective address of the next instruction to be executed
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_FIXED_INTERVAL_TIMER));

	//ResetIRQ(CONFIG::IRQ_FIXED_INTERVAL_TIMER_INTERRUPT);

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const WatchDogTimerInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetCSRR0(GetNIA()); // effective address of the next instruction to be executed
	
	SetCSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & CONFIG::MSR_ME_MASK); // MSR[ME] unchanged, all other MSR bits set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_WATCHDOG_TIMER));

	ResetIRQ(CONFIG::IRQ_WATCHDOG_TIMER_INTERRUPT);

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DebugInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetCSRR0(GetCIA()); // CHECKME
	
	SetCSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & CONFIG::MSR_ME_MASK); // MSR[ME] unchanged, all other MSR bits set to 0
	
	SetNIA(GetIVPR() | GetIVOR(CONFIG::IVOR_DEBUG));

	if(unlikely(IsVerboseException() || enable_trap_on_exception))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}
	if(linux_os_import)
	{
		Object::Stop(-1);
	}
}

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
