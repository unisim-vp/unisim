/*
 *  Copyright (c) 2009,
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

#include <unisim/component/cxx/processor/tms320/cpu.hh>
#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320 {

ExtendedPrecisionRegisterDebugInterface::ExtendedPrecisionRegisterDebugInterface(const char *_name, reg_t *_reg)
	: name(_name)
	, reg(_reg)
{
}

ExtendedPrecisionRegisterDebugInterface::~ExtendedPrecisionRegisterDebugInterface()
{
}

const char *ExtendedPrecisionRegisterDebugInterface::GetName() const
{
	return name.c_str();
}

void ExtendedPrecisionRegisterDebugInterface::GetValue(void *buffer) const
{
	// Packing the structure into the buffer
#if BYTE_ORDER == LITTLE_ENDIAN
	memcpy(buffer, &reg->lo, sizeof(reg->lo));
	memcpy((char *) buffer + sizeof(reg->lo), &reg->hi, sizeof(reg->hi));
#else
	memcpy(buffer, &reg->hi, sizeof(reg->hi));
	memcpy((char *) buffer + sizeof(reg->hi), &reg->lo, sizeof(reg->lo));
#endif
}

void ExtendedPrecisionRegisterDebugInterface::SetValue(const void *buffer)
{
	// Unpacking the buffer into the structure
#if BYTE_ORDER == LITTLE_ENDIAN
	memcpy(&reg->lo, buffer, sizeof(reg->lo));
	memcpy(&reg->hi, (char *) buffer + sizeof(reg->lo), sizeof(reg->hi));
#else
	memcpy(&reg->hi, buffer, sizeof(reg->hi));
	memcpy(&req->lo, (char *) buffer + sizeof(reg->hi), sizeof(reg->lo));
#endif
}

int ExtendedPrecisionRegisterDebugInterface::GetSize() const
{
	return sizeof(reg->lo) + sizeof(reg->hi);
}


} // end of namespace tms320
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
