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

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
