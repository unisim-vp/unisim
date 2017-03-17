/*
 *  Copyright (c) 2017,
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
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <unisim/util/reg/core/register.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace util {
namespace reg {
namespace core {

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// DECLARATIONS //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// DEFINITIONS /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////// Access ////////////////////////////////////

std::ostream& operator << (std::ostream& os, const Access& access)
{
	switch(access & SW_RW1)
	{
		case HW_RO: os << "hardware read-only/no software access"; break;
		case HW_RW: os << "hardware read-write/no software access"; break;
		case SW_R: os << "hardware read/software read access"; break;
		case SW_W: os << "hardware write/software write access"; break;
		case SW_RW: os << "hardware read-write/software read-write access"; break;
		case SW_W1: os << "hardware write/software writeOnce access"; break;
		case SW_RW1: os << "hardware read-write/software read-writeOnce access"; break;
	}
	
	return os;
};

//////////////////////////////// WarningStatus ////////////////////////////////

std::ostream& operator << (std::ostream& os, const WarningStatus& ws)
{
	bool warn = false;
	
	if(ws == WS_OK)
	{
		os << "OK";
	}
	else
	{
		if(ws & WS_WOORV)
		{
			os << "writing out-of-range value";
			warn = true;
		}

		if(ws & WS_WROR)
		{
			os << (warn ? "and" : "") << "writing read-only register";
			warn = true;
		}

		if(ws & WS_WROB)
		{
			os << (warn ? "and" : "") << "writing read-only bits";
			warn = true;
		}

		if(ws & WS_RWOR)
		{
			os << (warn ? "and" : "") << "reading write-only register";
			warn = true;
		}

		if(ws & WS_RWOB)
		{
			os << (warn ? "and" : "") << "reading write-only bits";
			warn = true;
		}
	}
	
	return os;
}

///////////////////////////// PropertyRegistry ////////////////////////////////

std::map<std::string, std::string> PropertyRegistry::string_prop_registry;
std::map<std::string, uint64_t> PropertyRegistry::uint_prop_registry;

void PropertyRegistry::SetStringProperty(const std::string& unique_id, const std::string& string_value)
{
	if(!string_value.empty())
	{
		std::map<std::string, std::string>::const_iterator iter = string_prop_registry.find(unique_id);
		
		if(iter != string_prop_registry.end())
		{
			return; // already exists
		}

		string_prop_registry[unique_id] = string_value;
	}
}

void PropertyRegistry::SetUnsignedIntProperty(const std::string& unique_id, uint64_t uint_value)
{
	std::map<std::string, uint64_t>::const_iterator iter = uint_prop_registry.find(unique_id);
	
	if(iter != uint_prop_registry.end())
	{
		return; // already exists
	}

	uint_prop_registry[unique_id] = uint_value;
}

const std::string& PropertyRegistry::GetStringProperty(const std::string& unique_id)
{
	std::map<std::string, std::string>::const_iterator iter = string_prop_registry.find(unique_id);
	
	if(iter != string_prop_registry.end())
	{
		return (*iter).second;
	}
	
	static std::string null_string;
	
	return null_string;
}

uint64_t PropertyRegistry::GetUnsignedIntegerProperty(const std::string& unique_id)
{
	std::map<std::string, uint64_t>::const_iterator iter = uint_prop_registry.find(unique_id);
	
	if(iter != uint_prop_registry.end())
	{
		return (*iter).second;
	}
	
	return 0;
}

std::string PropertyRegistry::GenUniqueInstanceName(const void *instance_ptr, const std::string& name)
{
	std::stringstream sstr;
	sstr << name << '_' << instance_ptr;
	return sstr.str();
}

///////////////////////////// LongPrettyPrinter ///////////////////////////////

const unsigned int LongPrettyPrinter::COLUMN_SPACING_PRINT_WIDTH;
const unsigned int LongPrettyPrinter::KIND_PRINT_WIDTH;
const unsigned int LongPrettyPrinter::RANGE_PRINT_WIDTH;
const unsigned int LongPrettyPrinter::NAME_PRINT_WIDTH;
const unsigned int LongPrettyPrinter::VALUE_PRINT_WIDTH;
const unsigned int LongPrettyPrinter::ACCESS_PRINT_WIDTH;

void LongPrettyPrinter::Print(std::ostream& os, const std::string& s, unsigned int column_width)
{
	std::size_t i;
	std::string truncated_string = s;
	if(s.length() > column_width)
	{
		truncated_string.resize(column_width - 3);
		truncated_string.append("...");
	}
	os << truncated_string;
	
	column_width += COLUMN_SPACING_PRINT_WIDTH;
	
	for(i = truncated_string.length(); i < column_width; i++)
	{
		os << ' ';
	}
}

void LongPrettyPrinter::PrintHeader(std::ostream& os)
{
	Print(os, "kind", KIND_PRINT_WIDTH);
	Print(os, "range", RANGE_PRINT_WIDTH);
	Print(os, "name", NAME_PRINT_WIDTH);
	Print(os, "value", VALUE_PRINT_WIDTH);
	Print(os, "access", ACCESS_PRINT_WIDTH);
	os << "description" << std::endl;
}

void LongPrettyPrinter::PrintKind(std::ostream& os, const std::string& kind)
{
	Print(os, kind, KIND_PRINT_WIDTH);
}

void LongPrettyPrinter::PrintRegisterSize(std::ostream& os, unsigned int size)
{
	std::stringstream sstr_register_size;
	
	sstr_register_size << size << "-bit";

	std::string str_register_size(sstr_register_size.str());
	
	Print(os, str_register_size, RANGE_PRINT_WIDTH);
}

void LongPrettyPrinter::PrintFieldRange(std::ostream& os, unsigned int bitoffset, unsigned int bitwidth)
{
	std::stringstream sstr_field_range;
	
	sstr_field_range << '[';
	if(bitwidth != 1)
	{
		sstr_field_range << (bitoffset + bitwidth - 1);
		sstr_field_range << "..";
	}
	sstr_field_range << bitoffset;
	sstr_field_range << ']';
	
	std::string str_field_range(sstr_field_range.str());
	
	Print(os, str_field_range, RANGE_PRINT_WIDTH);
}

void LongPrettyPrinter::PrintDisplayName(std::ostream& os, const std::string& display_name)
{
	Print(os, display_name, NAME_PRINT_WIDTH);
}

void LongPrettyPrinter::PrintAccess(std::ostream& os, const Access& access)
{
	std::stringstream sstr_access;
	
	sstr_access << access;
	
	std::string str_access(sstr_access.str());
	
	Print(os, str_access, ACCESS_PRINT_WIDTH);
}

void LongPrettyPrinter::PrintDescription(std::ostream& os, const std::string& s)
{
	os << s;
}

/////////////////////////////////// Field /////////////////////////////////////

/////////////////////////////// NullField //////////////////////////////////

void NullField::SetName(const std::string& name)
{
}

const std::string& NullField::GetName()
{
	static std::string null_string;
	return null_string;
}

//////////////////////////////// FieldSet //////////////////////////////////


/////////////////////////////////// Register //////////////////////////////////


} // end of namespace core
} // end of namespace reg
} // end of namespace util
} // end of namespace unisim
