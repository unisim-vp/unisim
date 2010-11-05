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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_EXCEPTION_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_EXCEPTION_HH__

#include <unisim/component/cxx/processor/powerpc/exception.hh>
#include <sstream>
#include <stdexcept>
#include <string>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

template <class CONFIG>
class MachineCheckException : public Exception
{
public:
	typedef enum
	{
		MCP_INSTRUCTION_SYNCHRONOUS,
		MCP_INSTRUCTION_ASYNCHRONOUS,
		MCP_DATA_ASYNCHRONOUS,
		MCP_TLB_ASYNCHRONOUS
	} Type;
	MachineCheckException(Type type);
	virtual ~MachineCheckException() throw();
	Type GetType() const;
	virtual const char * what () const throw ();
private:
	Type type;
	string what_str;
};

template <class CONFIG>
class InstructionSynchronousMachineCheckException : public MachineCheckException<CONFIG>
{
public:
	InstructionSynchronousMachineCheckException();
};

template <class CONFIG>
class InstructionAsynchronousMachineCheckException : public MachineCheckException<CONFIG>
{
public:
	InstructionAsynchronousMachineCheckException();
};

template <class CONFIG>
class DataAsynchronousMachineCheckException : public MachineCheckException<CONFIG>
{
public:
	DataAsynchronousMachineCheckException();
};

template <class CONFIG>
class TLBAsynchronousMachineCheckException : public MachineCheckException<CONFIG>
{
public:
	TLBAsynchronousMachineCheckException();
};

template <class CONFIG>
class DecrementerInterruptException : public Exception
{
public:
	DecrementerInterruptException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class ExternalInputInterruptException : public Exception
{
public:
	ExternalInputInterruptException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class CriticalInputInterruptException : public Exception
{
public:
	CriticalInputInterruptException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class DSIException : public Exception
{
public:
	typedef enum
	{
		DSI_READ_ACCESS_CONTROL,
		DSI_WRITE_ACCESS_CONTROL,
		DSI_BYTE_ORDERING,
		DSI_CACHE_LOCKING
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
class DSIReadAccessControlException : public DSIException<CONFIG>
{
public:
	typedef typename DSIException<CONFIG>::address_t address_t;
	DSIReadAccessControlException(address_t addr);
};

template <class CONFIG>
class DSIWriteAccessControlException : public DSIException<CONFIG>
{
public:
	typedef typename DSIException<CONFIG>::address_t address_t;
	DSIWriteAccessControlException(address_t addr);
};

template <class CONFIG>
class DSIByteOrderingException : public DSIException<CONFIG>
{
public:
	typedef typename DSIException<CONFIG>::address_t address_t;
	DSIByteOrderingException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type);
};

template <class CONFIG>
class DSICacheLockingException : public DSIException<CONFIG>
{
public:
	typedef typename DSIException<CONFIG>::address_t address_t;
	DSICacheLockingException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type);
};

template <class CONFIG>
class ISIException : public Exception
{
public:
	typedef enum
	{
		ISI_EXECUTE_ACCESS_CONTROL
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
class ISIExecuteAccessControlException : public ISIException<CONFIG>
{
public:
	typedef typename ISIException<CONFIG>::address_t address_t;
	ISIExecuteAccessControlException(address_t addr);
};

template <class CONFIG>
class DataTLBErrorException : public Exception
{
public:
	typedef typename CONFIG::address_t address_t;
	DataTLBErrorException(address_t addr, typename CONFIG::MemoryAccessType memory_access_type);
	virtual const char * what () const throw ();
	typename CONFIG::MemoryAccessType GetMemoryAccessType() const;
	address_t GetAddress() const;
private:
	address_t addr;
	typename CONFIG::MemoryAccessType memory_access_type;
};

template <class CONFIG>
class InstructionTLBErrorException : public Exception
{
public:
	typedef typename CONFIG::address_t address_t;
	InstructionTLBErrorException(address_t addr);
	virtual const char * what () const throw ();
	address_t GetAddress() const;
private:
	address_t addr;
};

template <class CONFIG>
class AuxiliaryProcessorUnavailableException : public Exception
{
public:
	AuxiliaryProcessorUnavailableException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class FixedIntervalTimerInterruptException : public Exception
{
public:
	FixedIntervalTimerInterruptException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class WatchDogTimerInterruptException : public Exception
{
public:
	WatchDogTimerInterruptException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class DebugInterruptException : public Exception
{
public:
	DebugInterruptException();
	virtual const char * what () const throw ();
};


} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
