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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_EXCEPTION_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_EXCEPTION_TCC__

#include <unisim/component/cxx/processor/powerpc/exception.tcc>
#include <sstream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

template <class CONFIG>
MachineCheckException<CONFIG>::MachineCheckException(Type _type)
	: type(_type)
{
	std::stringstream sstr;
	sstr.setf(std::ios::right | std::ios::hex | std::ios::showbase);
	switch(type)
	{
		case MCP_INSTRUCTION_SYNCHRONOUS:
			sstr << "Instruction synchronous";
			break;
		case MCP_INSTRUCTION_ASYNCHRONOUS:
			sstr << "Instruction asynchronous";
			break;
		case MCP_DATA_ASYNCHRONOUS:
			sstr << "Data asynchronous";
			break;
		case MCP_TLB_ASYNCHRONOUS:
			sstr << "TLB asynchronous";
			break;
	}
	sstr << "machine check exception ";
	what_str = sstr.str();
}

template <class CONFIG>
MachineCheckException<CONFIG>::~MachineCheckException() throw()
{
}

template <class CONFIG>
typename MachineCheckException<CONFIG>::Type MachineCheckException<CONFIG>::GetType() const
{
	return type;
}

template <class CONFIG>
const char * MachineCheckException<CONFIG>::what () const throw ()
{
	return what_str.c_str();
}

template <class CONFIG>
InstructionAsynchronousMachineCheckException<CONFIG>::InstructionAsynchronousMachineCheckException() : MachineCheckException<CONFIG>(MachineCheckException<CONFIG>::MCP_INSTRUCTION_ASYNCHRONOUS)
{
}

template <class CONFIG>
InstructionSynchronousMachineCheckException<CONFIG>::InstructionSynchronousMachineCheckException() : MachineCheckException<CONFIG>(MachineCheckException<CONFIG>::MCP_INSTRUCTION_SYNCHRONOUS)
{
}

template <class CONFIG>
DataAsynchronousMachineCheckException<CONFIG>::DataAsynchronousMachineCheckException() : MachineCheckException<CONFIG>(MachineCheckException<CONFIG>::MCP_DATA_ASYNCHRONOUS)
{
}

template <class CONFIG>
TLBAsynchronousMachineCheckException<CONFIG>::TLBAsynchronousMachineCheckException() : MachineCheckException<CONFIG>(MachineCheckException<CONFIG>::MCP_TLB_ASYNCHRONOUS)
{
}

template <class CONFIG>
DecrementerInterruptException<CONFIG>::DecrementerInterruptException()
{
}

template <class CONFIG>
const char * DecrementerInterruptException<CONFIG>::what () const throw ()
{
	return "decrementer interrupt exception";
}

template <class CONFIG>
ExternalInputInterruptException<CONFIG>::ExternalInputInterruptException()
{
}

template <class CONFIG>
const char * ExternalInputInterruptException<CONFIG>::what () const throw ()
{
	return "external input interrupt exception";
}

template <class CONFIG>
CriticalInputInterruptException<CONFIG>::CriticalInputInterruptException()
{
}

template <class CONFIG>
const char * CriticalInputInterruptException<CONFIG>::what () const throw ()
{
	return "critical interrupt exception";
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
		case DSI_READ_ACCESS_CONTROL:
			sstr << "read access control";
			break;
		case DSI_WRITE_ACCESS_CONTROL:
			sstr << "write access control";
			break;
		case DSI_BYTE_ORDERING:
			sstr << "byte ordering";
			break;
		case DSI_CACHE_LOCKING:
			sstr << "cache locking";
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
DSIReadAccessControlException<CONFIG>::DSIReadAccessControlException(address_t addr)
	: DSIException<CONFIG>(DSIException<CONFIG>::DSI_READ_ACCESS_CONTROL, addr, CONFIG::MAT_READ)
{
}

template <class CONFIG>
DSIWriteAccessControlException<CONFIG>::DSIWriteAccessControlException(address_t addr)
	: DSIException<CONFIG>(DSIException<CONFIG>::DSI_WRITE_ACCESS_CONTROL, addr, CONFIG::MAT_WRITE)
{
}

template <class CONFIG>
DSIByteOrderingException<CONFIG>::DSIByteOrderingException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type)
	: DSIException<CONFIG>(DSIException<CONFIG>::DSI_BYTE_ORDERING, addr, memory_access_type)
{
}

template <class CONFIG>
DSICacheLockingException<CONFIG>::DSICacheLockingException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type)
	: DSIException<CONFIG>(DSIException<CONFIG>::DSI_CACHE_LOCKING, addr, memory_access_type)
{
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
		case ISI_EXECUTE_ACCESS_CONTROL:
			sstr << "execute access control";
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
ISIExecuteAccessControlException<CONFIG>::ISIExecuteAccessControlException(address_t addr) : ISIException<CONFIG>(ISIException<CONFIG>::ISI_EXECUTE_ACCESS_CONTROL, addr)
{
}

template <class CONFIG>
DataTLBErrorException<CONFIG>::DataTLBErrorException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type)
{
	this->memory_access_type = memory_access_type;
	this->addr = addr;
}

template <class CONFIG>
typename CONFIG::MemoryAccessType DataTLBErrorException<CONFIG>::GetMemoryAccessType() const
{
	return memory_access_type;
}

template <class CONFIG>
typename CONFIG::address_t DataTLBErrorException<CONFIG>::GetAddress() const
{
	return addr;
}

template <class CONFIG>
const char * DataTLBErrorException<CONFIG>::what () const throw ()
{
	return (memory_access_type & CONFIG::MAT_WRITE) ? "Data TLB error on store exception" : "Data TLB error on load exception";
}

template <class CONFIG>
InstructionTLBErrorException<CONFIG>::InstructionTLBErrorException(address_t addr)
{
	this->addr = addr;
}

template <class CONFIG>
typename CONFIG::address_t InstructionTLBErrorException<CONFIG>::GetAddress() const
{
	return addr;
}

template <class CONFIG>
const char * InstructionTLBErrorException<CONFIG>::what () const throw ()
{
	return "Instruction TLB exception";
}

template <class CONFIG>
AuxiliaryProcessorUnavailableException<CONFIG>::AuxiliaryProcessorUnavailableException()
{
}

template <class CONFIG>
const char * AuxiliaryProcessorUnavailableException<CONFIG>::what () const throw ()
{
	return "Auxiliary processor unavailable exception";
}

template <class CONFIG>
FixedIntervalTimerInterruptException<CONFIG>::FixedIntervalTimerInterruptException()
{
}

template <class CONFIG>
const char * FixedIntervalTimerInterruptException<CONFIG>::what () const throw ()
{
	return "Fixed interval timer interrupt exception";
}

template <class CONFIG>
WatchDogTimerInterruptException<CONFIG>::WatchDogTimerInterruptException()
{
}

template <class CONFIG>
const char * WatchDogTimerInterruptException<CONFIG>::what () const throw ()
{
	return "Watchdog timer interrupt exception";
}

template <class CONFIG>
DebugInterruptException<CONFIG>::DebugInterruptException()
{
}

template <class CONFIG>
const char * DebugInterruptException<CONFIG>::what () const throw ()
{
	return "Debug interrupt exception";
}

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
