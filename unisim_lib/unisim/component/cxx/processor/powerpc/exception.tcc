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
ProgramException<CONFIG>::ProgramException(Type _type)
	: type(_type)
{
	std::stringstream sstr;
	sstr << "Program ";
	switch(type)
	{
		case PX_ILLEGAL_INSTRUCTION:
			sstr << "illegal instruction";
			break;
		case PX_PRIVILEGE_VIOLATION:
			sstr << "privilege violation";
			break;
		case PX_TRAP:
			sstr << "trap";
			break;
		case PX_FLOATING_POINT:
			sstr << "floating-point";
			break;
		case PX_UNIMPLEMENTED_INSTRUCTION:
			sstr << "unimplemented instruction";
			break;
	}
	sstr << " exception";
	what_str = sstr.str();
}

template <class CONFIG>
ProgramException<CONFIG>::~ProgramException() throw()
{
}

template <class CONFIG>
typename ProgramException<CONFIG>::Type ProgramException<CONFIG>::GetType() const
{
	return type;
}

template <class CONFIG>
const char * ProgramException<CONFIG>::what () const throw ()
{
	return what_str.c_str();
}

template <class CONFIG>
IllegalInstructionException<CONFIG>::IllegalInstructionException() : ProgramException<CONFIG>(ProgramException<CONFIG>::PX_ILLEGAL_INSTRUCTION)
{
}

template <class CONFIG>
PrivilegeViolationException<CONFIG>::PrivilegeViolationException() : ProgramException<CONFIG>(ProgramException<CONFIG>::PX_PRIVILEGE_VIOLATION)
{
}

template <class CONFIG>
TrapException<CONFIG>::TrapException() : ProgramException<CONFIG>(ProgramException<CONFIG>::PX_TRAP)
{
}

template <class CONFIG>
FloatingPointException<CONFIG>::FloatingPointException() : ProgramException<CONFIG>(ProgramException<CONFIG>::PX_FLOATING_POINT)
{
}

template <class CONFIG>
UnimplementedInstructionException<CONFIG>::UnimplementedInstructionException() : ProgramException<CONFIG>(ProgramException<CONFIG>::PX_UNIMPLEMENTED_INSTRUCTION)
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

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
