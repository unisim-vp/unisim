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
 *          Reda   NOUACER  (reda.nouacer@cea.fr)
 */

#include <unisim/service/pim/pim_server.hh>

namespace unisim {
namespace service {
namespace pim {

GDBRegister::GDBRegister(const string& reg_name, int reg_size, GDBEndian endian, unsigned int reg_num)
{
	this->name = reg_name;
	this->size = reg_size;
	this->endian = endian;
	this->reg = 0;
	this->reg_num = reg_num;
}

GDBRegister::GDBRegister(unisim::util::debug::Register *reg, GDBEndian endian, unsigned int reg_num)
{
	this->name = reg->GetName();
	this->size = reg->GetSize();
	this->endian = endian;
	this->reg = reg;
	this->reg_num = reg_num;
}

bool GDBRegister::SetValue(const string& hex)
{
	int i;
	unsigned int len = hex.length();
	unsigned int pos = 0;
	uint8_t value[size];

	if(endian == GDB_BIG_ENDIAN)
	{
#if BYTE_ORDER == BIG_ENDIAN
		for(i = 0; i < size && pos < len; i++, pos += 2)
#else
		for(i = size - 1; i >= 0 && pos < len; i--, pos += 2)
#endif
		{
			if(!IsHexChar(hex[pos]) || !IsHexChar(hex[pos + 1])) return false;
	
			value[i] = (HexChar2Nibble(hex[pos]) << 4) | HexChar2Nibble(hex[pos + 1]);
		}
	}
	else
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		for(i = 0; i < size && pos < len; i++, pos += 2)
#else
		for(i = size - 1; i >= 0 && pos < len; i--, pos += 2)
#endif
		{
			if(!IsHexChar(hex[pos]) || !IsHexChar(hex[pos + 1])) return false;

			value[i] = (HexChar2Nibble(hex[pos]) << 4) | HexChar2Nibble(hex[pos + 1]);
		}
	}

	if(reg)	reg->SetValue(&value);

	return true;
}

void GDBRegister::SetValue(const void *buffer)
{
	if(reg) reg->SetValue(buffer);
}

void GDBRegister::GetValue(string& hex) const
{
	int i;
	char c[2];
	c[1] = 0;
	uint8_t value[size];
	memset(value, 0, size);
	if(reg)
		reg->GetValue(&value);

	hex.erase();

	if(endian == GDB_BIG_ENDIAN)
	{
#if BYTE_ORDER == BIG_ENDIAN
		for(i = 0; i < size; i++)
#else
		for(i = size - 1; i >= 0; i--)
#endif
		{
			c[0] = Nibble2HexChar(value[i] >> 4);
			hex += c;
			c[0] = Nibble2HexChar(value[i] & 0xf);
			hex += c;
		}
	}
	else
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		for(i = 0; i < size; i++)
#else
		for(i = size - 1; i >= 0; i--)
#endif
		{
			c[0] = Nibble2HexChar(value[i] >> 4);
			hex += c;
			c[0] = Nibble2HexChar(value[i] & 0xf);
			hex += c;
		}
	}
}

void GDBRegister::GetValue(void *buffer) const
{
	memset(buffer, 0, size);
	if(reg)
		reg->GetValue(buffer);
}

} // end of namespace pim
} // end of namespace service
} // end of namespace unisim
