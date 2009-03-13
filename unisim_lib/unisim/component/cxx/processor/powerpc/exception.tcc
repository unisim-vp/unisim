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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_EXCEPTION_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_EXCEPTION_TCC__

#include <sstream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

template <class CONFIG>
SystemResetException<CONFIG>::SystemResetException()
{
}

template <class CONFIG>
const char * SystemResetException<CONFIG>::what () const throw ()
{
	return "system reset exception";
}

template <class CONFIG>
MachineCheckException<CONFIG>::MachineCheckException()
{
}

template <class CONFIG>
const char * MachineCheckException<CONFIG>::what () const throw ()
{
	return "machine check exception";
}

template <class CONFIG>
DecrementerException<CONFIG>::DecrementerException()
{
}

template <class CONFIG>
const char * DecrementerException<CONFIG>::what () const throw ()
{
	return "decrementer exception";
}

template <class CONFIG>
ExternalInterruptException<CONFIG>::ExternalInterruptException()
{
}

template <class CONFIG>
const char * ExternalInterruptException<CONFIG>::what () const throw ()
{
	return "external interrupt exception";
}

template <class CONFIG>
ISIException<CONFIG>::ISIException(const char *name, address_t addr)
{
	std::stringstream sstr;
	this->addr = addr;
	sstr.setf(std::ios::right | std::ios::hex | std::ios::showbase);
	sstr << "ISI " << name << " exception";
	what_str = sstr.str();
}

template <class CONFIG>
ISIException<CONFIG>::~ISIException() throw()
{
}

template <class CONFIG>
const char * ISIException<CONFIG>::what () const throw ()
{
	return what_str.c_str();
}

template <class CONFIG>
typename CONFIG::address_t ISIException<CONFIG>::GetAddr() const
{
	return addr;
}

template <class CONFIG>
ISIProtectionViolationException<CONFIG>::ISIProtectionViolationException(address_t addr) : ISIException<CONFIG>("protection violation", addr)
{
}

template <class CONFIG>
ISINoExecuteException<CONFIG>::ISINoExecuteException(address_t addr) : ISIException<CONFIG>("no execute", addr)
{
}

template <class CONFIG>
ISIDirectStoreException<CONFIG>::ISIDirectStoreException(address_t addr) : ISIException<CONFIG>("direct store", addr)
{
}

template <class CONFIG>
ISIPageFaultException<CONFIG>::ISIPageFaultException(address_t addr) : ISIException<CONFIG>("page fault", addr)
{
}

template <class CONFIG>
ISIGuardedMemoryException<CONFIG>::ISIGuardedMemoryException(address_t addr) : ISIException<CONFIG>("guarded memory", addr)
{
}

template <class CONFIG>
DSIException<CONFIG>::DSIException(const char *name, address_t addr, typename CONFIG::MemoryAccessType memory_access_type)
{
	this->addr = addr;
	this->memory_access_type = memory_access_type;
	
	std::stringstream sstr;
	sstr.setf(std::ios::right | std::ios::hex | std::ios::showbase);
	sstr << "DSI " << name << " exception";
	what_str = sstr.str();
}

template <class CONFIG>
DSIException<CONFIG>::~DSIException() throw()
{
}

template <class CONFIG>
typename CONFIG::address_t DSIException<CONFIG>::GetAddress() const
{
	return addr;
}

template <class CONFIG>
typename CONFIG::MemoryAccessType DSIException<CONFIG>::GetAccessType() const
{
	return memory_access_type;
}

template <class CONFIG>
const char * DSIException<CONFIG>::what () const throw ()
{
	return what_str.c_str();
}

template <class CONFIG>
DSIDirectStoreException<CONFIG>::DSIDirectStoreException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type) : DSIException<CONFIG>("direct store", addr, memory_access_type)
{
}

template <class CONFIG>
DSIProtectionViolationException<CONFIG>::DSIProtectionViolationException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type) : DSIException<CONFIG>("protection violation", addr, memory_access_type)
{
}

template <class CONFIG>
DSIPageFaultException<CONFIG>::DSIPageFaultException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type) : DSIException<CONFIG>("page fault", addr, memory_access_type)
{
}

template <class CONFIG>
DSIDataAddressBreakpointException<CONFIG>::DSIDataAddressBreakpointException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type) : DSIException<CONFIG>("data address breakpoint", addr, memory_access_type)
{
}

template <class CONFIG>
DSIExternalAccessDisabledException<CONFIG>::DSIExternalAccessDisabledException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type) : DSIException<CONFIG>("external access disabled", addr, memory_access_type)
{
}

template <class CONFIG>
DSIWriteThroughLinkedLoadStore<CONFIG>::DSIWriteThroughLinkedLoadStore(address_t addr, typename CONFIG::MemoryAccessType memory_access_type) : DSIException<CONFIG>("write through linked load-store", addr, memory_access_type)
{
}

template <class CONFIG>
FloatingPointAssistException<CONFIG>::FloatingPointAssistException()
{
}

template <class CONFIG>
const char * FloatingPointAssistException<CONFIG>::what () const throw ()
{
	return "Floating point assist exception";
}

template <class CONFIG>
AlignmentException<CONFIG>::AlignmentException(address_t addr)
{
	this->addr = addr;
}

template <class CONFIG>
typename CONFIG::address_t AlignmentException<CONFIG>::GetAddress() const
{
	return addr;
}

template <class CONFIG>
const char * AlignmentException<CONFIG>::what () const throw ()
{
	return "Alignment exception";
}

template <class CONFIG>
ProgramException<CONFIG>::ProgramException(const char *name)
{
	std::stringstream sstr;
	sstr << "Program " << name << " exception";
	what_str = sstr.str();
}

template <class CONFIG>
ProgramException<CONFIG>::~ProgramException() throw()
{
}

template <class CONFIG>
const char * ProgramException<CONFIG>::what () const throw ()
{
	return what_str.c_str();
}

template <class CONFIG>
IllegalInstructionException<CONFIG>::IllegalInstructionException() : ProgramException<CONFIG>("illegal instruction")
{
}

template <class CONFIG>
PrivilegeViolationException<CONFIG>::PrivilegeViolationException() : ProgramException<CONFIG>("privilege violation")
{
}

template <class CONFIG>
TrapException<CONFIG>::TrapException() : ProgramException<CONFIG>("trap")
{
}

template <class CONFIG>
FloatingPointException<CONFIG>::FloatingPointException() : ProgramException<CONFIG>("floating point")
{
}

template <class CONFIG>
SystemCallException<CONFIG>::SystemCallException()
{
}

template <class CONFIG>
const char * SystemCallException<CONFIG>::what () const throw ()
{
	return "system call exception";
}

template <class CONFIG>
FloatingPointUnavailableException<CONFIG>::FloatingPointUnavailableException()
{
}

template <class CONFIG>
const char * FloatingPointUnavailableException<CONFIG>::what () const throw ()
{
	return "Floating point unavailable exception";
}

template <class CONFIG>
TraceException<CONFIG>::TraceException()
{
}

template <class CONFIG>
const char * TraceException<CONFIG>::what () const throw ()
{
	return "Trace exception";
}

template <class CONFIG>
InstructionAddressBreakpointException<CONFIG>::InstructionAddressBreakpointException()
{
}

template <class CONFIG>
const char * InstructionAddressBreakpointException<CONFIG>::what () const throw ()
{
	return "Instruction address breakpoint exception";
}

template <class CONFIG>
TLBMissException<CONFIG>::TLBMissException(typename CONFIG::MemoryAccessType memory_access_type, typename CONFIG::MemoryType memory_type, address_t addr, uint32_t way, uint32_t key, uint32_t vsid, uint32_t api, physical_address_t primary_pteg, physical_address_t secondary_pteg)
{
	this->memory_access_type = memory_access_type;
	this->memory_type = memory_type;
	this->addr = addr;
	this->vsid = vsid;
	this->api = api;
	this->way = way;
	this->key = key;
	this->primary_pteg = primary_pteg;
	this->secondary_pteg = secondary_pteg;
}

template <class CONFIG>
typename CONFIG::MemoryAccessType TLBMissException<CONFIG>::GetMemoryAccessType() const
{
	return memory_access_type;
}

template <class CONFIG>
typename CONFIG::MemoryType TLBMissException<CONFIG>::GetMemoryType() const
{
	return memory_type;
}

template <class CONFIG>
typename CONFIG::address_t TLBMissException<CONFIG>::GetAddress() const
{
	return addr;
}

template <class CONFIG>
uint32_t TLBMissException<CONFIG>::GetVSID() const
{
	return vsid;
}

template <class CONFIG>
uint32_t TLBMissException<CONFIG>::GetAPI() const
{
	return api;
}

template <class CONFIG>
uint32_t TLBMissException<CONFIG>::GetWay() const
{
	return way;
}

template <class CONFIG>
uint32_t TLBMissException<CONFIG>::GetKey() const
{
	return key;
}

template <class CONFIG>
uint32_t TLBMissException<CONFIG>::GetPrimaryPTEG() const
{
	return primary_pteg;
}

template <class CONFIG>
uint32_t TLBMissException<CONFIG>::GetSecondaryPTEG() const
{
	return secondary_pteg;
}

template <class CONFIG>
const char * TLBMissException<CONFIG>::what () const throw ()
{
	if(memory_type & CONFIG::MT_INSN)
	{
		return "ITLB miss exception";
	}
	
	if(memory_access_type & CONFIG::MAT_WRITE)
	{
		return "DTLB store miss exception";
	}
	
	if(memory_access_type & CONFIG::MAT_READ)
	{
		return "DTLB load miss exception";
	}
	
	return "TLB Miss";
}

template <class CONFIG>
PerformanceMonitorInterruptException<CONFIG>::PerformanceMonitorInterruptException()
{
}

template <class CONFIG>
const char * PerformanceMonitorInterruptException<CONFIG>::what () const throw ()
{
	return "Performance monitor interrupt exception";
}

template <class CONFIG>
SystemManagementInterruptException<CONFIG>::SystemManagementInterruptException()
{
}

template <class CONFIG>
const char * SystemManagementInterruptException<CONFIG>::what () const throw ()
{
	return "System management interrupt exception";
}

template <class CONFIG>
ThermalManagementInterruptException<CONFIG>::ThermalManagementInterruptException()
{
}

template <class CONFIG>
const char * ThermalManagementInterruptException<CONFIG>::what () const throw ()
{
	return "Thermal management interrupt exception";
}


template <class CONFIG>
AltivecUnavailableException<CONFIG>::AltivecUnavailableException()
{
}

template <class CONFIG>
const char * AltivecUnavailableException<CONFIG>::what () const throw ()
{
	return "Altivec unavailable exception";
}


/* System reset exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const SystemResetException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetNIA()); // save NIA
	
	// clear SRR1[0-5], SRR1[6]=MSR[6], clear SSR1[7-15], SRR1[16-31]=MSR[16-31]
	SetSRR1(GetMSR() & 0x0200ffffUL);
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	if(HasHardReset())
	{
		if(CONFIG::HAS_HID0_NHR) ResetHID0_NHR(); // reset HID0[NHR]
		AckHardReset();
	}
	else if(HasSoftReset())
	{
		if(CONFIG::HAS_HID0_NHR) SetHID0_NHR(); // set HID0[NHR]
		AckSoftReset();
	}
	
	SetNIA(EXC_SYSTEM_RESET_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
		(*logger_import) << DebugInfo << exc.what() << Endl << EndDebugInfo;
}

/* Machine check exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const MachineCheckException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetNIA()); // save NIA
		
	// clear SRR1[0-15], SRR1[16-31]=MSR[16-31]
	SetSRR1(GetMSR() & 0x0000ffffUL);
	
	if(HasMCP())
	{
		// set SRR1[12]
		SetSRR1(GetSRR1() | 0x00080000UL);
		AckMCP();
	}
	
	if(HasTEA())
	{
		// set SRR1[13]
		SetSRR1(GetSRR1() | 0x00040000UL);
		AckTEA();
	}
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));

	SetNIA(EXC_MACHINE_CHECK_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
		(*logger_import) << DebugInfo << exc.what() << Endl << EndDebugInfo;
}

/* Decrementer exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const DecrementerException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
//	cerr << "before jumping to exception handler r9=0x" << hex << GetGPR(9) << std::dec << endl;
	SetSRR0(GetNIA()); // save NIA
	
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	SetNIA(EXC_DECREMENTER_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
		(*logger_import) << DebugInfo << "bus cycle " << bus_cycle << ": " << exc.what() << Endl << EndDebugInfo;
	
	AckDecrementerOverflow();
}

/* External interrupt exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const ExternalInterruptException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetNIA()); // save NIA
	
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	SetNIA(EXC_EXTERNAL_INTERRUPT_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
		(*logger_import) << DebugInfo << exc.what() << Endl << EndDebugInfo;

	AckExternalInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const PerformanceMonitorInterruptException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetNIA()); // save NIA
	
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	SetNIA(EXC_PERFORMANCE_MONITOR_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	AckPerformanceMonitorInterrupt();
	
	if(IsVerboseException())
		(*logger_import) << DebugInfo << exc.what() << Endl << EndDebugInfo;
}

/* System management interrupt exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const SystemManagementInterruptException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetNIA()); // save NIA
	
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	SetNIA(EXC_SYSTEM_MANAGEMENT_INTERRUPT_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	AckSMI();

	if(IsVerboseException())
		(*logger_import) << DebugInfo << exc.what() << Endl << EndDebugInfo;
}

/* Thermal management interrupt exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const ThermalManagementInterruptException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetNIA()); // save NIA
	
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	SetNIA(EXC_THERMAL_MANAGEMENT_INTERRUPT_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	AckThermalManagementInterrupt();
	
	if(IsVerboseException())
		(*logger_import) << DebugInfo << exc.what() << Endl << EndDebugInfo;
}

/* ISI exception */
// SRR1 indicates to the operating system the cause of the ISI exception:
//
//   0   1   2   3   4   
// +---+---+---+---+---+--
// | ? |   | 0 |   |   | ...
// +-+-+-+-+---+-+-+-+-+--
//       |       |   |
//       |       |   +-----> protection violation
//       |       |
//       |       +---------> either direct store exception (SR[T]=1)
//       |                       or no execute segment (SR[NOEXECUTE]=1)
//       |                       or guarded memory (PTE[G]=1 or DBAT[G]=1)
//       |
//       +-----------------> page fault

template <class CONFIG>
void CPU<CONFIG>::HandleException(const ISIProtectionViolationException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-3], set SRR1[4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x080000000UL); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	SetNIA(EXC_ISI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));

	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const ISINoExecuteException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-2], set SRR1[3], clear SRR1[4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x10000000UL); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	SetNIA(EXC_ISI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const ISIDirectStoreException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-2], set SRR1[3], clear SRR1[4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x10000000UL); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	SetNIA(EXC_ISI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const ISIPageFaultException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], set SRR1[1], clear SRR1[2-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x40000000UL); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	SetNIA(EXC_ISI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const ISIGuardedMemoryException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-2], set SRR1[3], clear SRR1[4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x10000000UL); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	SetNIA(EXC_ISI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

/* DSI exception */
// DSISR indicates to the operating system the cause of the DSI exception:
//
//   0   1   2   3   4   5   6   7   8   9   10  11  12 .... 31
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   | 0 | 0 |   |   |   | 0 | 0 |   | 0 |   | 0  .... 0 |
// +-+-+-+-+---+---+-+-+-+-+-+-+---+---+-+-+---+-+-+---+---+---+
//   |   |           |   |   |           |       |
//   |   |           |   |   |           |       +----> eciwx/ecowx and EAR[E]=0
//   |   |           |   |   |           |
//   |   |           |   |   |           +------------> DABR match
//   |   |           |   |   |
//   |   |           |   |   +------------------------> 1 for a store, 0 for a load
//   |   |           |   |
//   |   |           |   +----------------------------> direct store exception (eciwx, ecowx, lwarx, stwcx.)
//   |   |           |
//   |   |           +--------------------------------> protection violation
//   |   |
//   |   +--------------------------------------------> page fault
//   |
//   +------------------------------------------------> direct store exception (load/store)
template <class CONFIG>
void CPU<CONFIG>::HandleException(const DSIDirectStoreException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetDAR(exc.GetAddress()); // DAR=effective address
	
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	// FIXME: for eciwx, ecowx, lwarx or stwcx. DSISR[5] should be set
	if(exc.GetAccessType() == CONFIG::MAT_WRITE)
		SetDSISR(0x82000000UL); // set DSISR[0], DSISR[6], clear other bits
	else
		SetDSISR(0x80000000UL); // set DSISR[0], clear other bits
		
	SetNIA(EXC_DSI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(func_symbol) (*logger_import) << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << " while " << (exc.GetAccessType() == CONFIG::MAT_WRITE ? "writing" : "reading")
				<< " data at 0x" << Hex << exc.GetAddress() << Dec;
		const Symbol<address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) (*logger_import) << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		(*logger_import) << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DSIProtectionViolationException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetDAR(exc.GetAddress()); // DAR=effective address
	
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	if(exc.GetAccessType() == CONFIG::MAT_WRITE)
		SetDSISR(0x0a000000UL); // set DSISR[4], set DSISR[6], clear other bits
	else
		SetDSISR(0x08000000UL); // set DSISR[4], clear other bits
		
	SetNIA(EXC_DSI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(func_symbol) (*logger_import) << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << " while " << (exc.GetAccessType() == CONFIG::MAT_WRITE ? "writing" : "reading")
				<< " data at 0x" << Hex << exc.GetAddress() << Dec;
		const Symbol<address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) (*logger_import) << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		(*logger_import) << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DSIPageFaultException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetDAR(exc.GetAddress()); // DAR=effective address
	
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	if(exc.GetAccessType() == CONFIG::MAT_WRITE)
		SetDSISR(0x42000000UL); // set DSISR[1], set DSISR[6], clear other bits
	else
		SetDSISR(0x40000000UL); // set DSISR[1], clear other bits
		
	SetNIA(EXC_DSI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(func_symbol) (*logger_import) << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << " while " << (exc.GetAccessType() == CONFIG::MAT_WRITE ? "writing" : "reading")
				<< " data at 0x" << Hex << exc.GetAddress() << Dec;
		const Symbol<address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) (*logger_import) << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		(*logger_import) << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DSIDataAddressBreakpointException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetDAR(exc.GetAddress()); // DAR=effective address
	
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	if(exc.GetAccessType() == CONFIG::MAT_WRITE)
		SetDSISR(0x02400000UL); // set DSISR[6], set DSISR[9], clear other bits
	else
		SetDSISR(0x00400000UL); // set DSISR[9], clear other bits
		
	SetNIA(EXC_DSI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << ":" << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(func_symbol) (*logger_import) << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << exc.what() << " while " << (exc.GetAccessType() == CONFIG::MAT_WRITE ? "writing" : "reading")
				<< " data at 0x" << Hex << exc.GetAddress() << Dec;
		const Symbol<address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) (*logger_import) << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		(*logger_import) << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DSIExternalAccessDisabledException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetDAR(exc.GetAddress()); // DAR=effective address
	
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	if(exc.GetAccessType() == CONFIG::MAT_WRITE)
		SetDSISR(0x02100000UL); // set DSISR[6], DSISR[11], clear other bits
	else
		SetDSISR(0x02100000UL); // set DSISR[11], clear other bits
		
	SetNIA(EXC_DSI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(func_symbol) (*logger_import) << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << " while " << (exc.GetAccessType() == CONFIG::MAT_WRITE ? "writing" : "reading")
				<< " data at 0x" << Hex << exc.GetAddress() << Dec;
		const Symbol<address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) (*logger_import) << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		(*logger_import) << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DSIWriteThroughLinkedLoadStore<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetDAR(exc.GetAddress()); // DAR=effective address
	
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	if(exc.GetAccessType() == CONFIG::MAT_WRITE)
		SetDSISR(0x06000000UL); // set DSISR[5], DSISR[6], clear other bits
	else
		SetDSISR(0x04000000UL); // set DSISR[5], clear other bits
		
	SetNIA(EXC_DSI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(func_symbol) (*logger_import) << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << " while " << (exc.GetAccessType() == CONFIG::MAT_WRITE ? "writing" : "reading")
				<< " data at 0x" << Hex << exc.GetAddress() << Dec;
		const Symbol<address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) (*logger_import) << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		(*logger_import) << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

/* Alignment exception */
// DSISR helps the operating system to implement misaligned memory accesses not handled in the hardware:
//
//   0 ..... 14  15  16  17  18  29  20  21  22  23  24  25  26  27  28  29  30  31
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// | 0 ..... 0 |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+-+-+-+-+-+-+-+-+---+---+-+-+-+-+---+---+---+-+-+-+-+---+---+---+-+-+
//               |   |   |   |           |   |               |   |               |
//               +-+-+   |   +-----+-----+   +-------+-------+   +-------+-------+
//                 |     |         |                 |                   |
//                 |     |         |                 |                   +-----------> bits 11-15 of instruction encoding
//                 |     |         |                 |                                 for update forms, and lmw, lswi, lswx
//                 |     |         |                 |
//                 |     |         |                 +-------------------------------> bits 6-10 of instruction encoding. undefined for dcbz
//                 |     |         |
//                 |     |         +-------------------------------------------------> index addressing: bits 21-24 of instruction encoding
//                 |     |                                                             immediate index addressing: bits 1-4 of instruction encoding
//                 |     |
//                 |     +-----------------------------------------------------------> index addressing: bits 25 of instruction encoding
//                 |                                                                   immediate index addressing: bits 5 of instruction encoding
//                 |
//                 +-----------------------------------------------------------------> index addressing: bits 29-30 of instruction encoding
//                                                                                     immediate index addressing: cleared
template <class CONFIG>
void CPU<CONFIG>::HandleException(const AlignmentException<CONFIG>& exc, uint32_t instruction_encoding)
{
	if(linux_os_import) Stop(-1);
	SetDAR(exc.GetAddress()); // DAR=effective address
	
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	// clear DSISR[0-14]
	SetDSISR(GetDSISR() & 0x0001ffffUL);
	
	// Look at primary opcode (bits 0-5)
	bool xform = ((instruction_encoding >> 26) & 0x3fUL) == 31;
		
	if(xform)
	{
		// X-Form: index addressing
		
		// copy bits 29-30 of instruction encoding to DSISR[15-16]
		SetDSISR((GetDSISR() & 0xfffe7fffUL) | ((instruction_encoding << 14) & 0x00018000UL));
		
		 // copy bit 25 of instruction encoding to bits 17 of DSISR
		SetDSISR((GetDSISR() & 0xffffbfffUL) | ((instruction_encoding << 8) & 0x00004000UL));
		
		 // copy bits 21-24 of instruction encoding to bits 18-21 of DSISR
		SetDSISR((GetDSISR() & 0xffffc3ffUL) | ((instruction_encoding << 3) & 0x00003c00UL));
	}
	else
	{
		// immediate index addressing
		
		// clear DSISR[15-16]
		SetDSISR(GetDSISR() & 0xfffe7fffUL);
		
		// copy bit 5 of instruction encoding to DSISR[17]
		SetDSISR((GetDSISR() & 0xffffbfffUL) | ((instruction_encoding >> 12) & 0x00004000UL));
		
		// copy bits 1-4 of instruction encoding to DSISR[18-21]
		SetDSISR((GetDSISR() & 0xffffc3ffUL) | ((instruction_encoding >> 17) & 0x00003c00UL));
	}
		
	// copy bits 6-10 of instruction encoding to DSISR[22-26]
	SetDSISR((GetDSISR() & 0xfffffc1fUL) | ((instruction_encoding >> 16) & 0x000003e0UL));
		
	// copy bits 11-15 of instruction encoding into DSISR[27-31]
	SetDSISR((GetDSISR() & 0xffffffe0UL) | ((instruction_encoding >> 16) & 0x0000001fUL));
		
	SetNIA(EXC_ALIGNMENT_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(func_symbol) (*logger_import) << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << " while accessing data at 0x" << Hex << exc.GetAddress() << Dec;
		const Symbol<address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) (*logger_import) << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		(*logger_import) << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

/* Program exceptions */
// SRR1 indicates to the operating system the cause of the program exception:
//
//   0   1 ....   4   5 ....  9  10  11  12  13  14  15                             
// +---+---+---+---+---+---+---+---+---+---+---+---+---+--
// | ? | 0 ....  0 | ?  .... ? | 0 |   |   |   |   |   | ...
// +---+---+---+-+-+-+-+-+-+-+-+---+-+-+-+-+-+-+-+-+-+-+--
//                                   |   |   |   |   |
//                                   |   |   |   |   +----> cleared if SSR0=address of faulting instruction
//                                   |   |   |   |          set if subsequent instructions
//                                   |   |   |   |
//                                   |   |   |   +--------> trap
//                                   |   |   |
//                                   |   |   +------------> privilege violation
//                                   |   | 
//                                   |   +----------------> illegal instruction
//                                   |
//                                   +--------------------> IEEE floating point exception
template <class CONFIG>
void CPU<CONFIG>::HandleException(const IllegalInstructionException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x00080000UL); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	SetNIA(EXC_PROGRAM_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const PrivilegeViolationException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-12], set SRR1[13], clear SRR1[14-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x00040000UL); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	SetNIA(EXC_PROGRAM_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const TrapException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-13], set SRR1[14], clear SRR1[15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x00020000UL); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	SetNIA(EXC_PROGRAM_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const FloatingPointException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10], set SRR1[11], clear SRR1[12-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x00100000UL); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	SetNIA(EXC_PROGRAM_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

/* Floating point unavailable exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const FloatingPointUnavailableException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
	
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); // clear SRR1[1-4], SRR1[10-15], copy MSR[16-23], MSR[25-27], and MSR[30-31]
	
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1)); // copy MSR[ILE] into MSR[LE], clear MSR[POW], MSR[EE], MSR[PR], MSR[FP], MSR[FE0], MSR[SE], MSR[BE], MSR[FE1], MSR[IR], MSR[DR] and MSR[RI]
	
	SetNIA(EXC_FLOATING_POINT_UNAVAILABLE_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}


template <class CONFIG>
int CPU<CONFIG>::StringLength(address_t addr)
{
	int len = 0;
	char buffer;

	while(1)
	{
		ReadMemory(addr, &buffer, 1);
		if(buffer == 0) return len;
		len++;
		addr += 1;
	}
}


/* System call exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const SystemCallException<CONFIG>& exc)
{
	if(linux_os_import)
	{
//   cerr << "Syscall #" << GetGPR(0) << endl;
		linux_os_import->ExecuteSystemCall(GetGPR(0));
/*   if(GetCR() & (1 << 28))
   {
     cerr << "got an error" << endl;
   }*/
	}
	else
	{
#if 1
		if(IsVerboseException())
		{
			switch(GetGPR(0))
			{
				case 1:
					(*logger_import) << DebugInfo << "exit(0x" << Hex << GetGPR(3) << Dec << ");" << Endl << EndDebugInfo;
					break;
					
				case 2:
					(*logger_import) << DebugInfo << "fork()" << Endl << EndDebugInfo;
					break;
			
				case 3:
					(*logger_import) << DebugInfo << "read(fd=" << (signed) GetGPR(3) << ", buf=0x" << Hex << GetGPR(4) << Dec
							<< ", count=" << GetGPR(5) << ");" << Endl << EndDebugInfo;
					break;
			
				case 4:
					{
						int fd; 
		
						fd = GetGPR(3);
#if 0
						if(fd == 1 || fd == 2)
						{
							size_t count;
							void *buf;
							address_t buf_addr;
							
							buf_addr = GetGPR(4);
							count = (size_t) GetGPR(5);
							buf = malloc(count);
							ReadMemory(buf_addr, buf, count);
								
							char *tbuf = new char[count + 1];
							memcpy(tbuf, buf, count);
							tbuf[count] = '\0';
							string *str = new string(tbuf);
							cout << (*str);
							cout << flush;
							delete str;
						}
#endif
						
						(*logger_import) << DebugInfo << "write(fd=" << (signed) GetGPR(3) << ", buf=0x" << Hex << GetGPR(4) << Dec
								<< ", count=" << GetGPR(5) << ");" << Endl << EndDebugInfo;
					}
					break;
					
				case 5:
					{
						address_t addr;
						int pathnamelen;
						char *pathname;
						int flags;
						mode_t mode;
						
						addr = GetGPR(3);
						pathnamelen = StringLength(addr);
						pathname = (char *) malloc(pathnamelen + 1);
						ReadMemory(addr, pathname, pathnamelen + 1);
						flags = GetGPR(4);
						mode = GetGPR(5);
							
						(*logger_import) << DebugInfo << "open(pathname=\"" << pathname << "\", flags=0x" << Hex << flags 
								<< ", mode=0x" << mode << Dec << ");" << Endl << EndDebugInfo;
							
						free(pathname);
					}
					break;
				
				case 6:
					(*logger_import) << DebugInfo << "close(fd=" << (signed) GetGPR(3) << ");" << Endl << EndDebugInfo;
					break;
					
				case 11:
					{
						address_t filename_addr = GetGPR(3);
						int filenamelen = StringLength(filename_addr);
						char *filename = (char *) malloc(filenamelen + 1);
						ReadMemory(filename_addr, filename, filenamelen + 1);
						address_t argv_addr = GetGPR(4);
						address_t envp_addr = GetGPR(5);
						(*logger_import) << DebugInfo << "execve(filename=\"" << filename << "\", argv=0x" << Hex << argv_addr << ", envp=0x" << envp_addr << Dec << ");" << Endl << EndDebugInfo;
						free(filename);
					}
					break;
					
				case 14:
					{
						address_t path_addr = GetGPR(3);
						int pathlen = StringLength(path_addr);
						char *path = (char *) malloc(pathlen + 1);
						ReadMemory(path_addr, path, pathlen + 1);
						
						uint32_t mode = GetGPR(4);
						uint32_t dev = GetGPR(5);
						
						(*logger_import) << DebugInfo << "mknod(pathname=\"" << path << "\", mode=0x" << Hex << mode << ", dev=0x" << dev << Dec << ");" << Endl << EndDebugInfo;
						free(path);
					}
					break;
					
				case 15:
					{
						address_t path_addr = GetGPR(3);
						int pathlen = StringLength(path_addr);
						char *path = (char *) malloc(pathlen + 1);
						ReadMemory(path_addr, path, pathlen + 1);
							
						uint32_t mode = GetGPR(4);
							
						(*logger_import) << DebugInfo << "chmod(pathname=\"" << path << "\", mode=0x" << Hex << mode << Dec << ");" << Endl << EndDebugInfo;
						free(path);
					}
					break;
					
				case 20:
					(*logger_import) << DebugInfo << "getpid();" << Endl << EndDebugInfo;
					break;
					
				case 21:
					{
						address_t source_addr = GetGPR(3);
						int sourcelen = StringLength(source_addr);
						char *source = (char *) malloc(sourcelen + 1);
						ReadMemory(source_addr, source, sourcelen + 1);
							
						address_t target_addr = GetGPR(4);
						int targetlen = StringLength(target_addr);
						char *target = (char *) malloc(targetlen + 1);
						ReadMemory(target_addr, target, targetlen + 1);
							
						address_t filesystemtype_addr = GetGPR(5);
						int filesystemtypelen = StringLength(filesystemtype_addr);
						char *filesystemtype = (char *) malloc(filesystemtypelen + 1);
						ReadMemory(filesystemtype_addr, filesystemtype, filesystemtypelen + 1);
							
						unsigned long mountflags = GetGPR(6);
						address_t data_addr = GetGPR(7);
							
						(*logger_import) << DebugInfo << "mount(source=\"" << source << "\", target=\"" << target << "\", filesystemtype=\""
								<< filesystemtype << "\", mountflags=0x" << Hex << mountflags << ", data=0x"
								<< data_addr << Dec << ");" << Endl << EndDebugInfo;
						free(source);
						free(target);
						free(filesystemtype);
					}
					break;
				
				case 22:
					{
						address_t target_addr = GetGPR(4);
						int targetlen = StringLength(target_addr);
						char *target = (char *) malloc(targetlen + 1);
						ReadMemory(target_addr, target, targetlen + 1);
								
						(*logger_import) << DebugInfo << "umount(target=\"" << target << "\");" << Endl << EndDebugInfo;
						free(target);
					}
					break;
					
				case 23:
					(*logger_import) << DebugInfo << "setuid(uid=0x" << Hex << GetGPR(3) << Dec << ");" << Endl << EndDebugInfo;
					break;
					
				case 24:
					(*logger_import) << DebugInfo << "getuid();" << Endl << EndDebugInfo;
					break;
				
				case 33:
					{
						address_t addr;
						int pathnamelen;
						char *pathname;
						mode_t mode;
		
						addr = GetGPR(3);
						pathnamelen = StringLength(addr);
						pathname = (char *) malloc(pathnamelen + 1);
						ReadMemory(addr, pathname, pathnamelen + 1);
						mode = GetGPR(4);
						(*logger_import) << DebugInfo << "access(pathname=\"" << pathname 
							<< "\", mode=0x" << Hex << mode << Dec << ");" << Endl << EndDebugInfo;
						free(pathname);
					}
					break;
					
				case 39:
					{
						address_t path_addr = GetGPR(3);
						int pathlen = StringLength(path_addr);
						char *path = (char *) malloc(pathlen + 1);
						ReadMemory(path_addr, path, pathlen + 1);
					
						uint32_t mode = GetGPR(4);
					
						(*logger_import) << DebugInfo << "mkdir(pathname=\"" << path << "\", mode=0x" << Hex << mode << Dec << ");" << Endl << EndDebugInfo;
						free(path);
					}
					break;
					
				case 45:
					(*logger_import) << DebugInfo << "brk(end_data_segment=0x" << Hex << GetGPR(3) << Dec << ");" << Endl << EndDebugInfo;
					break;
					
				case 46:
					(*logger_import) << DebugInfo << "setgid(gid=0x" << Hex << GetGPR(3) << Dec << ");" << Endl << EndDebugInfo;
					break;
				
				case 47:
					(*logger_import) << DebugInfo << "getgid();" << Endl << EndDebugInfo;
					break;
					
				case 49:
					(*logger_import) << DebugInfo << "geteuid();" << Endl << EndDebugInfo;
					break;
					
				case 50:
					(*logger_import) << DebugInfo << "getegid();" << Endl << EndDebugInfo;
					break;
				
				case 60:
					(*logger_import) << DebugInfo << "umask(mask=0x" << Hex << GetGPR(3) << Dec << ");" << Endl << EndDebugInfo;
					break;
					
				case 63:
					(*logger_import) << DebugInfo << "dup2(old=" << GetGPR(3) << ", new=" << GetGPR(4) << ");" << Endl << EndDebugInfo;
					break;
					
				case 64:
					(*logger_import) << DebugInfo << "getppid();" << Endl << EndDebugInfo;
					break;
					
				case 90:
					(*logger_import) << DebugInfo << "mmap(start=0x" << Hex << GetGPR(3) << Dec << ", length=" << GetGPR(4) << ", prot=0x"
							<< Hex << GetGPR(5) << ", flags=0x" << GetGPR(6) << Dec << ", fd=" << (signed) GetGPR(7)
							<< ", offset=" << GetGPR(8) << ");" << Endl << EndDebugInfo;
					break;
					
				case 106:
					{
						address_t path_addr = GetGPR(3);
						int pathlen = StringLength(path_addr);
						char *path = (char *) malloc(pathlen + 1);
						ReadMemory(path_addr, path, pathlen + 1);
						
						address_t buf_addr = GetGPR(4);
						
						(*logger_import) << DebugInfo << "stat(path=\"" << path << "\", buf=0x" << Hex << buf_addr << Dec << ");" << Endl << EndDebugInfo;
						free(path);
					}
					break;
					
				case 114:
					(*logger_import) << DebugInfo << "wait4(pid=" << GetGPR(3) << ", status=0x" << Hex << GetGPR(4) << ", options=0x" << GetGPR(5) << ", rusage=0x" << GetGPR(6) << Dec << ");" << Endl << EndDebugInfo;
					break;
				
				case 120:
					{
						address_t fn_addr = GetGPR(3);
						address_t child_stack = GetGPR(4);
						int flags = GetGPR(5);
						address_t arg = GetGPR(6);
						
						(*logger_import) << DebugInfo << "clone(fn=0x" << Hex << fn_addr << ", child_stack=0x" << child_stack << ", flags=0x" << flags << ", arg=0x" << arg << Dec << ");" << Endl << EndDebugInfo;
					}
					break;
					
				case 122:
					(*logger_import) << DebugInfo << "uname(buf=0x" << Hex << GetGPR(3) << Dec << ");" << Endl << EndDebugInfo;
					break;
					
				case 141:
					(*logger_import) << DebugInfo << "getdents(fd=" << (signed) GetGPR(3) << ", dirp=0x" << Hex << GetGPR(4) << Dec << ", count=" << GetGPR(5) << ");" << Endl << EndDebugInfo;
					break;
					
				case 173:
					(*logger_import) << DebugInfo << "rt_sigaction(" << GetGPR(3) << ", act=0x" << Hex << GetGPR(4) << ", oact=0x" << GetGPR(5) << Dec << ", sigsetsize=" << GetGPR(6) << ");" << Endl << EndDebugInfo;
					break;
					
				case 182:
					(*logger_import) << DebugInfo << "getcwd(buf=0x" << Hex << GetGPR(3) << Dec << ", size=" << GetGPR(4) << ");" << Endl << EndDebugInfo;
					break;
					
				case 204:
					(*logger_import) << DebugInfo << "fcntl64(fd=" << GetGPR(3) << ", cmd=0x" << Hex << GetGPR(4) << ", arg=0x" << GetGPR(5) << ");" << Endl << EndDebugInfo;
					break;
					
				case 234:
					(*logger_import) << DebugInfo << "exit_group(" << (signed) GetGPR(3) << ");" << Endl << EndDebugInfo;
					break;
			}
		}
#endif

		SetSRR0(GetNIA()); // save NIA
			
		SetSRR1((GetSRR1() & 0x87c008cUL) | (GetMSR() & 0x0000ff73UL)); // clear SRR1[1-4], clear SRR1[10-15], save MSR[16-23], MSR[25-27], MSR[30-31]
					
		SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1)); // copy MSR[ILE] into MSR[LE], clear MSR[POW], MSR[EE], MSR[PR], MSR[FP], MSR[FE0], MSR[SE], MSR[BE], MSR[FE1], MSR[IR], MSR[DR] and MSR[RI]
			
		SetNIA(EXC_SYSTEM_CALL_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
		
		if(IsVerboseException())
		{
			(*logger_import) << DebugInfo;
			(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
			const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
			if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
			(*logger_import) << ":" << exc.what() << " (syscall #" << GetGPR(0) << ")" << Endl;
			(*logger_import) << EndDebugInfo;
		}
	}
}

/* Trace exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const TraceException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetNIA()); // save NIA
	
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); // clear SRR1[1-4], SRR1[10-15], copy MSR[16-23], MSR[25-27], and MSR[30-31]
	
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1)); // copy MSR[ILE] into MSR[LE], clear MSR[POW], MSR[EE], MSR[PR], MSR[FP], MSR[FE0], MSR[SE], MSR[BE], MSR[FE1], MSR[IR], MSR[DR] and MSR[RI]
	
	SetNIA(EXC_TRACE_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

/* Instruction Address Breakpoint exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const InstructionAddressBreakpointException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
	
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); // clear SRR1[1-4], SRR1[10-15], copy MSR[16-23], MSR[25-27], and MSR[30-31]
	
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1)); // copy MSR[ILE] into MSR[LE], clear MSR[POW], MSR[EE], MSR[PR], MSR[FP], MSR[FE0], MSR[SE], MSR[BE], MSR[FE1], MSR[IR], MSR[DR] and MSR[RI]
	
	SetNIA(EXC_INSTRUCTION_ADDRESS_BREAKPOINT_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const TLBMissException<CONFIG>& exc)
{
	if(linux_os_import) Stop(-1);
	SetSRR0(GetCIA()); // save CIA
	
	uint32_t pte_hi = ((exc.GetVSID() & 0x00ffffffUL) << 7) | (exc.GetAPI() & 0x3fUL) | 0x80000000UL;
	
	// SRR1[0-3]=CR0, clear SRR1[4-11], SRR1[12]=key, SRR1[13]=0, SRR1[14]=way, SRR1[15]=0, SRR1[16-31]=MSR[16-31]
	SetSRR1((GetCR() & 0xf0000000UL) | ((exc.GetKey() & 1) << 19) | ((exc.GetWay() & 1) << 17) | (GetMSR() & 0x0000ffffUL));
	
	if(exc.GetMemoryType() == CONFIG::MT_INSN)
	{
		SetIMISS(exc.GetAddress());
		SetICMP(pte_hi);
		// SRR1[13]=1
		SetSRR1(GetSRR1() | 0x00040000UL);
		SetNIA(EXC_ITLB_MISS_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	}
	else
	{
		SetDMISS(exc.GetAddress());
		SetDCMP(pte_hi);
		if(exc.GetMemoryAccessType() == CONFIG::MAT_WRITE)
		{
			// SRR1[15]=1
			SetSRR1(GetSRR1() | 0x00010000UL);
			SetNIA(EXC_DTLB_STORE_MISS_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
		}
		else
		{
			SetNIA(EXC_DTLB_LOAD_MISS_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
		}
	}
	SetHASH1(exc.GetPrimaryPTEG());
	SetHASH2(exc.GetSecondaryPTEG());
	
	if(IsVerboseException())
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "At 0x" << Hex << GetCIA() << Dec;
		const Symbol<address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<address_t>::SYM_FUNC) : 0;
		if(symbol) (*logger_import) << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		(*logger_import) << ":" << exc.what() << Endl;
		(*logger_import) << EndDebugInfo;
	}
}

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
