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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_EXCEPTION_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_EXCEPTION_TCC__

#include <unisim/component/cxx/processor/powerpc/exception.tcc>
#include <sstream>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace mpc7447a {

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
ISIException<CONFIG>::ISIException(Type _type, address_t _addr)
	: type(_type)
	, addr(_addr)
{
	std::stringstream sstr;
	sstr.setf(std::ios::right | std::ios::hex | std::ios::showbase);
	sstr << "ISI ";
	switch(type)
	{
		case ISI_PROTECTION_VIOLATION:
			sstr << "protection violation";
			break;
		case ISI_NO_EXECUTE:
			sstr << "no execute";
			break;
		case ISI_DIRECT_STORE:
			sstr << "direct store";
			break;
		case ISI_PAGE_FAULT:
			sstr << "page fault";
			break;
		case ISI_GUARDED_MEMORY:
			sstr << "guarded memory";
			break;
	}
	sstr << " exception";
	what_str = sstr.str();
}

template <class CONFIG>
ISIException<CONFIG>::~ISIException() throw()
{
}

template <class CONFIG>
typename ISIException<CONFIG>::Type ISIException<CONFIG>::GetType() const
{
	return type;
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
ISIProtectionViolationException<CONFIG>::ISIProtectionViolationException(address_t addr) : ISIException<CONFIG>(ISIException<CONFIG>::ISI_PROTECTION_VIOLATION, addr)
{
}

template <class CONFIG>
ISINoExecuteException<CONFIG>::ISINoExecuteException(address_t addr) : ISIException<CONFIG>(ISIException<CONFIG>::ISI_NO_EXECUTE, addr)
{
}

template <class CONFIG>
ISIDirectStoreException<CONFIG>::ISIDirectStoreException(address_t addr) : ISIException<CONFIG>(ISIException<CONFIG>::ISI_DIRECT_STORE, addr)
{
}

template <class CONFIG>
ISIPageFaultException<CONFIG>::ISIPageFaultException(address_t addr) : ISIException<CONFIG>(ISIException<CONFIG>::ISI_PAGE_FAULT, addr)
{
}

template <class CONFIG>
ISIGuardedMemoryException<CONFIG>::ISIGuardedMemoryException(address_t addr) : ISIException<CONFIG>(ISIException<CONFIG>::ISI_GUARDED_MEMORY, addr)
{
}

template <class CONFIG>
DSIException<CONFIG>::DSIException(Type _type, address_t _addr, typename CONFIG::MemoryAccessType _memory_access_type)
	: type(_type)
	, addr(_addr)
	, memory_access_type(_memory_access_type)
	, what_str()
{
	std::stringstream sstr;
	sstr.setf(std::ios::right | std::ios::hex | std::ios::showbase);
	sstr << "DSI ";
	switch(type)
	{
		case DSI_DIRECT_STORE:
			sstr << "direct store";
			break;
		case DSI_PROTECTION_VIOLATION:
			sstr << "protection violation";
			break;
		case DSI_PAGE_FAULT:
			sstr << "page fault";
			break;
		case DSI_DATA_ADDRESS_BREAKPOINT:
			sstr << "data address breakpoint";
			break;
		case DSI_EXTERNAL_ACCESS_DISABLED:
			sstr << "external access disabled";
			break;
		case DSI_WRITE_THROUGH_LINKED_LOAD_STORE:
			sstr << "write through linked load/store";
			break;
	}
	sstr << " exception";
	what_str = sstr.str();
}

template <class CONFIG>
DSIException<CONFIG>::~DSIException() throw()
{
}

template <class CONFIG>
typename DSIException<CONFIG>::Type DSIException<CONFIG>::GetType() const
{
	return type;
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
DSIDirectStoreException<CONFIG>::DSIDirectStoreException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type)
	: DSIException<CONFIG>(DSIException<CONFIG>::DSI_DIRECT_STORE, addr, memory_access_type)
{
}

template <class CONFIG>
DSIProtectionViolationException<CONFIG>::DSIProtectionViolationException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type)
	: DSIException<CONFIG>(DSIException<CONFIG>::DSI_PROTECTION_VIOLATION, addr, memory_access_type)
{
}

template <class CONFIG>
DSIPageFaultException<CONFIG>::DSIPageFaultException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type)
	: DSIException<CONFIG>(DSIException<CONFIG>::DSI_PAGE_FAULT, addr, memory_access_type)
{
}

template <class CONFIG>
DSIDataAddressBreakpointException<CONFIG>::DSIDataAddressBreakpointException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type)
	: DSIException<CONFIG>(DSIException<CONFIG>::DSI_DATA_ADDRESS_BREAKPOINT, addr, memory_access_type)
{
}

template <class CONFIG>
DSIExternalAccessDisabledException<CONFIG>::DSIExternalAccessDisabledException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type)
	: DSIException<CONFIG>(DSIException<CONFIG>::DSI_EXTERNAL_ACCESS_DISABLED, addr, memory_access_type)
{
}

template <class CONFIG>
DSIWriteThroughLinkedLoadStore<CONFIG>::DSIWriteThroughLinkedLoadStore(address_t addr, typename CONFIG::MemoryAccessType memory_access_type)
	: DSIException<CONFIG>(DSIException<CONFIG>::DSI_WRITE_THROUGH_LINKED_LOAD_STORE, addr, memory_access_type)
{
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

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
