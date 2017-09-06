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

#include <unisim/service/debug/gdb_server/gdb_server.hh>

namespace unisim {
namespace service {
namespace debug {
namespace gdb_server {

bool GDBServerBase::killed = false;

GDBRegister::GDBRegister()
	: name()
	, bitsize(0)
	, reg(0)
	, endian(GDB_LITTLE_ENDIAN)
	, reg_num(0)
	, type()
	, group()
{
}

GDBRegister::GDBRegister(const std::string& reg_name, int reg_bitsize, GDBEndian reg_endian, unsigned int _reg_num, const std::string& _type, const std::string& _group)
	: name(reg_name)
	, bitsize(reg_bitsize)
	, reg(0)
	, endian(reg_endian)
	, reg_num(_reg_num)
	, type(_type)
	, group(_group)
{
}

GDBRegister::GDBRegister(unisim::service::interfaces::Register *_reg, const std::string& reg_name, GDBEndian reg_endian, unsigned int _reg_num, const std::string& _type, const std::string& _group)
	: name(reg_name)
	, bitsize(8 * _reg->GetSize())
	, reg(_reg)
	, endian(reg_endian)
	, reg_num(_reg_num)
	, type(_type)
	, group(_group)
{
}

bool GDBRegister::SetValue(const std::string& hex)
{
	int i;
	unsigned int len = hex.length();
	unsigned int pos = 0;
	int size = bitsize / 8;
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
	return reg != 0;
}

bool GDBRegister::SetValue(const void *buffer)
{
	if(reg) reg->SetValue(buffer);
	return reg != 0;
}

bool GDBRegister::GetValue(std::string& hex) const
{
	int i;
	int size = bitsize / 8;
	uint8_t value[size];
	
	hex.erase();
	
	memset(value, 0, size);
	
	if(reg)
	{
		reg->GetValue(&value);
		
		char c[2];
		c[1] = 0;

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
	else
	{
		for(i = 0; i < size; i++)
		{
			hex += 'x';
			hex += 'x';
		}
	}
	
	return reg != 0;
}

bool GDBRegister::GetValue(void *buffer) const
{
	int size = bitsize / 8;
	memset(buffer, 0, size);
	if(reg)
		reg->GetValue(buffer);
	return reg != 0;
}

std::ostream& GDBRegister::ToXML(std::ostream& os, unsigned int _reg_num) const
{
	os << "<reg name=\"" << name << "\" bitsize=\"" << bitsize << "\"";
	if(reg_num != _reg_num)
	{
		os << " regnum=\"" << reg_num << "\"";
	}
	if(!type.empty())
	{
		os << " type=\"" << type << "\"";
	}
	if(!group.empty())
	{
		os << " group=\"" << group << "\"";
	}
	os << "/>";
	
	return os;
}

GDBFeature::GDBFeature(const std::string& _name, unsigned int _id)
	: name(_name)
	, id(_id)
	, gdb_registers()
{
}
void GDBFeature::AddRegister(const GDBRegister *gdb_register)
{
	gdb_registers.push_back(gdb_register);
}

unsigned int GDBFeature::GetId() const
{
	return id;
}

std::ostream& GDBFeature::ToXML(std::ostream& os, std::string req_filename) const
{
	std::stringstream feature_filename_sstr;
	feature_filename_sstr << "feature" << id << ".xml";
	std::string feature_filename(feature_filename_sstr.str());

	if(req_filename == feature_filename)
	{
		os << "<feature name=\"" << name << "\"";

		unsigned int n = gdb_registers.size();
		if(n > 0)
		{
			os << ">";
			
			unsigned int i;
			unsigned int reg_num = (unsigned int) -1;
			for(i = 0; i < n; i++)
			{
				const GDBRegister *gdb_reg = gdb_registers[i];
				if(gdb_reg)
				{
					gdb_reg->ToXML(os, reg_num);
					
					reg_num = gdb_reg->GetRegNum() + 1;
				}
			}
			
			os << "</feature>";
		}
		else
		{
			os << "/>";
		}
	}
	
	return os;
}

} // end of namespace gdb_server
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim
