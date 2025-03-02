/*
 *  Copyright (c) 2022,
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

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if HAVE_HLA_RTI1516E

#include <unisim/service/debug/hla/federate.hh>

namespace unisim {
namespace service {
namespace debug {
namespace hla {

///////////////////////////////////////////////////////////////////////////////
//                                Definitions                                //
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////// ObjectClass //////////////////////////////////

ObjectClass::ObjectClass(unisim::util::hla::FederateBase& _federate, const std::string& _name)
	: unisim::util::hla::ObjectClass(_federate, _name)
{
}

ObjectClass::~ObjectClass()
{
	DestroyAttributes();
}

////////////////////////////// InstrumentBase /////////////////////////////////

InstrumentBase::InstrumentBase(const std::string& _expr, bool _read, bool _write)
	: expr(_expr)
	, read(_read)
	, write(_write)
{
}

InstrumentBase::~InstrumentBase()
{
}

bool InstrumentBase::IsRead() const
{
	return read;
}

bool InstrumentBase::IsWrite() const
{
	return write;
}

} // end of namespace hla
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // HAVE_HLA_RTI1516E
