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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_EXCEPTION_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_EXCEPTION_HH__

#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <sstream>
#include <stdexcept>
#include <string>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

using std::string;

class Exception : public std::exception {};

template <class CONFIG>
class AlignmentException : public Exception
{
public:
	typedef typename CONFIG::address_t address_t;
	AlignmentException(address_t addr);
	address_t GetAddress() const;
	virtual const char * what () const throw ();
private:
	address_t addr;
};

template <class CONFIG>
class ProgramException : public Exception
{
public:
	typedef enum
	{
		PX_ILLEGAL_INSTRUCTION,
		PX_PRIVILEGE_VIOLATION,
		PX_TRAP,
		PX_FLOATING_POINT,
		PX_UNIMPLEMENTED_INSTRUCTION
	} Type;
	ProgramException(Type type);
	virtual ~ProgramException() throw();
	Type GetType() const;
	virtual const char * what () const throw ();
private:
	Type type;
	string what_str;
};

template <class CONFIG>
class IllegalInstructionException : public ProgramException<CONFIG>
{
public:
	IllegalInstructionException();
};

template <class CONFIG>
class PrivilegeViolationException : public ProgramException<CONFIG>
{
public:
	PrivilegeViolationException();
};

template <class CONFIG>
class TrapException : public ProgramException<CONFIG>
{
public:
	TrapException();
};

template <class CONFIG>
class FloatingPointException : public ProgramException<CONFIG>
{
public:
	FloatingPointException();
};

template <class CONFIG>
class UnimplementedInstructionException : public ProgramException<CONFIG>
{
public:
	UnimplementedInstructionException();
};

template <class CONFIG>
class SystemCallException : public Exception
{
public:
	SystemCallException();
	virtual const char * what () const throw ();
};

template <class CONFIG>
class FloatingPointUnavailableException : public Exception
{
public:
	FloatingPointUnavailableException();
	virtual const char * what () const throw ();
};

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
