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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

#include <sstream>

using std::stringstream;

template <class CONFIG>
ProgramException<CONFIG>::
ProgramException(const char *name) {
	stringstream sstr;
	sstr << "Program " << name << " exception";
	what_str = sstr.str();
}

template <class CONFIG>
ProgramException<CONFIG>::
~ProgramException() throw() {
}

template <class CONFIG>
const char * 
ProgramException<CONFIG>::
what () const throw () {
	return what_str.c_str();
}

template <class CONFIG>
IllegalInstructionException<CONFIG>::
IllegalInstructionException() : 
	ProgramException<CONFIG>("illegal instruction") {
}

template <class CONFIG>
ResetException<CONFIG>::
ResetException() {}

template <class CONFIG>
const char *
ResetException<CONFIG>::
what() const throw() {
	return "reset exception";
}

template <class CONFIG>
UndefinedInstructionException<CONFIG>::
UndefinedInstructionException() {}

template <class CONFIG>
const char *
UndefinedInstructionException<CONFIG>::
what() const throw() {
	return "undefined instruction exception";
}

template <class CONFIG>
SoftwareInterruptException<CONFIG>::
SoftwareInterruptException() {}

template <class CONFIG>
const char *
SoftwareInterruptException<CONFIG>::
what() const throw() {
	return "software interrupt exception";
}

template <class CONFIG>
PrefetchAbortException<CONFIG>::
PrefetchAbortException() {}

template <class CONFIG>
const char *
PrefetchAbortException<CONFIG>::
what() const throw() {
	return "prefetch abort exception";
}

template <class CONFIG>
DataAbortException<CONFIG>::
DataAbortException() {}

template <class CONFIG>
const char *
DataAbortException<CONFIG>::
what() const throw() {
	return "data abort exception";
}

template <class CONFIG>
IRQException<CONFIG>::
IRQException() {}

template <class CONFIG>
const char *
IRQException<CONFIG>::
what() const throw() {
	return "irq exception";
}

template <class CONFIG>
FIQException<CONFIG>::
FIQException() {}

template <class CONFIG>
const char * 
FIQException<CONFIG>::
what() const throw() {
	return "fiq exception";
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_TCC__
