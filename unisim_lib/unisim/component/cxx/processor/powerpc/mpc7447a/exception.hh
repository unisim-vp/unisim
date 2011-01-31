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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_EXCEPTION_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_EXCEPTION_HH__

#include <unisim/component/cxx/processor/powerpc/exception.hh>
#include <sstream>
#include <stdexcept>
#include <string>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace mpc7447a {

using std::string;

template <class CONFIG>
class SystemResetException : public Exception
{
public:
	SystemResetException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class MachineCheckException : public Exception
{
public:
	MachineCheckException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class DecrementerException : public Exception
{
public:
	DecrementerException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class ExternalInterruptException : public Exception
{
public:
	ExternalInterruptException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class ISIException : public Exception
{
public:
	typedef enum
	{
		ISI_PROTECTION_VIOLATION,
		ISI_NO_EXECUTE,
		ISI_DIRECT_STORE,
		ISI_PAGE_FAULT,
		ISI_GUARDED_MEMORY
	} Type;
	typedef typename CONFIG::address_t address_t;
	ISIException(Type type, address_t addr);
	virtual ~ISIException() throw();
	Type GetType() const;
	virtual const char * what () const throw ();
	address_t GetAddr() const;
private:
	Type type;
	address_t addr;
	string what_str;
};

template <class CONFIG>
class ISIProtectionViolationException : public ISIException<CONFIG>
{
public:
	typedef typename ISIException<CONFIG>::address_t address_t;
	ISIProtectionViolationException(address_t addr);
};

template <class CONFIG>
class ISINoExecuteException : public ISIException<CONFIG>
{
public:
	typedef typename ISIException<CONFIG>::address_t address_t;
	ISINoExecuteException(address_t addr);
};

template <class CONFIG>
class ISIDirectStoreException : public ISIException<CONFIG>
{
public:
	typedef typename ISIException<CONFIG>::address_t address_t;
	ISIDirectStoreException(address_t addr);
};

template <class CONFIG>
class ISIPageFaultException : public ISIException<CONFIG>
{
public:
	typedef typename ISIException<CONFIG>::address_t address_t;
	ISIPageFaultException(address_t addr);
};

template <class CONFIG>
class ISIGuardedMemoryException : public ISIException<CONFIG>
{
public:
	typedef typename ISIException<CONFIG>::address_t address_t;
	ISIGuardedMemoryException(address_t addr);
};

template <class CONFIG>
class DSIException : public Exception
{
public:
	typedef enum
	{
		DSI_DIRECT_STORE,
		DSI_PROTECTION_VIOLATION,
		DSI_PAGE_FAULT,
		DSI_DATA_ADDRESS_BREAKPOINT,
		DSI_EXTERNAL_ACCESS_DISABLED,
		DSI_WRITE_THROUGH_LINKED_LOAD_STORE
	} Type;
	typedef typename CONFIG::address_t address_t;
	DSIException(Type type, address_t addr, typename CONFIG::MemoryAccessType memory_access_type);
	virtual ~DSIException() throw();
	Type GetType() const;
	address_t GetAddress() const;
	typename CONFIG::MemoryAccessType GetAccessType() const;
	virtual const char * what () const throw ();
private:
	Type type;
	address_t addr;
	typename CONFIG::MemoryAccessType memory_access_type;
	string what_str;
};

template <class CONFIG>
class DSIDirectStoreException : public DSIException<CONFIG>
{
public:
	typedef typename DSIException<CONFIG>::address_t address_t;
	DSIDirectStoreException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type);
};

template <class CONFIG>
class DSIProtectionViolationException : public DSIException<CONFIG>
{
public:
	typedef typename DSIException<CONFIG>::address_t address_t;
	DSIProtectionViolationException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type);
};

template <class CONFIG>
class DSIPageFaultException : public DSIException<CONFIG>
{
public:
	typedef typename DSIException<CONFIG>::address_t address_t;
	DSIPageFaultException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type);
};

template <class CONFIG>
class DSIDataAddressBreakpointException : public DSIException<CONFIG>
{
public:
	typedef typename DSIException<CONFIG>::address_t address_t;
	DSIDataAddressBreakpointException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type);
};

template <class CONFIG>
class DSIExternalAccessDisabledException : public DSIException<CONFIG>
{
public:
	typedef typename DSIException<CONFIG>::address_t address_t;
	DSIExternalAccessDisabledException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type);
};

template <class CONFIG>
class DSIWriteThroughLinkedLoadStore : public DSIException<CONFIG>
{
public:
	typedef typename DSIException<CONFIG>::address_t address_t;
	DSIWriteThroughLinkedLoadStore(address_t addr, typename CONFIG::MemoryAccessType memory_access_type);
};

template <class CONFIG>
class TraceException : public Exception
{
public:
	TraceException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class InstructionAddressBreakpointException : Exception
{
public:
	InstructionAddressBreakpointException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class TLBMissException : public Exception
{
public:
	typedef typename CONFIG::address_t address_t;
	typedef typename CONFIG::physical_address_t physical_address_t;
	TLBMissException(typename CONFIG::MemoryAccessType memory_access_type, typename CONFIG::MemoryType memory_type, address_t addr, uint32_t key, uint32_t way, uint32_t vsid, uint32_t api, physical_address_t primary_pteg, physical_address_t secondary_pteg);
	virtual const char * what () const throw ();
	typename CONFIG::MemoryAccessType GetMemoryAccessType() const;
	typename CONFIG::MemoryType GetMemoryType() const;
	address_t GetAddress() const;
	uint32_t GetVSID() const;
	uint32_t GetAPI() const;
	uint32_t GetWay() const;
	uint32_t GetKey() const;
	uint32_t GetPrimaryPTEG() const;
	uint32_t GetSecondaryPTEG() const;
private:
	typename CONFIG::MemoryAccessType memory_access_type;
	typename CONFIG::MemoryType memory_type;
	address_t addr;
	uint32_t vsid;
	uint32_t api;
	uint32_t way;
	uint32_t key;
	physical_address_t primary_pteg;
	physical_address_t secondary_pteg;
};

template <class CONFIG>
class PerformanceMonitorInterruptException : Exception
{
public:
	PerformanceMonitorInterruptException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class SystemManagementInterruptException : Exception
{
public:
	SystemManagementInterruptException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class ThermalManagementInterruptException : Exception
{
public:
	ThermalManagementInterruptException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class AltivecUnavailableException : Exception
{
public:
	AltivecUnavailableException();
	virtual const char * what () const throw ();
};

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
