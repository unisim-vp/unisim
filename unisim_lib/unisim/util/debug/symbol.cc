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
 
#include <unisim/util/debug/symbol.hh>
#include <iostream>
#include <sstream>
#include <string.h>

namespace unisim {
namespace util {
namespace debug {

SymbolBase::Type SymbolBase::Types[9] =
{
	SYM_NOTYPE ,
	SYM_OBJECT ,
	SYM_FUNC   ,
	SYM_SECTION,
	SYM_FILE   ,
	SYM_COMMON ,
	SYM_TLS    ,
	SYM_OS     ,
	SYM_PROC
};

SymbolBase::SymbolBase(const char *_name, Type _type)
	: name(_name)
	, type(_type)
{
}

} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

std::ostream& operator << (std::ostream& stream, unisim::util::debug::SymbolBase::Type sym_type)
{
	switch(sym_type)
	{
		case unisim::util::debug::SymbolBase::SYM_NOTYPE : stream << "unspecified-type"; break;
		case unisim::util::debug::SymbolBase::SYM_OBJECT : stream << "data-object"; break;
		case unisim::util::debug::SymbolBase::SYM_FUNC   : stream << "function"; break;
		case unisim::util::debug::SymbolBase::SYM_SECTION: stream << "section"; break;
		case unisim::util::debug::SymbolBase::SYM_FILE   : stream << "source-file"; break;
		case unisim::util::debug::SymbolBase::SYM_COMMON : stream << "uninitialized-common-block"; break;
		case unisim::util::debug::SymbolBase::SYM_TLS    : stream << "tls-object"; break;
		case unisim::util::debug::SymbolBase::SYM_OS     : stream << "operating-system-specific-type"; break;
		case unisim::util::debug::SymbolBase::SYM_PROC   : stream << "processor-specific-type"; break;
	}
	return stream;
}

std::istream& operator >> (std::istream& stream, unisim::util::debug::SymbolBase::Type& sym_type)
{
	std::string str;
	if(stream >> str)
	{
		if(str == "unspecified-type") sym_type = unisim::util::debug::SymbolBase::SYM_NOTYPE;
		else if(str == "data-object") sym_type = unisim::util::debug::SymbolBase::SYM_OBJECT;
		else if(str == "function") sym_type = unisim::util::debug::SymbolBase::SYM_FUNC;
		else if(str == "section") sym_type = unisim::util::debug::SymbolBase::SYM_SECTION;
		else if(str == "source-file") sym_type = unisim::util::debug::SymbolBase::SYM_FILE;
		else if(str == "uninitialized-common-block") sym_type = unisim::util::debug::SymbolBase::SYM_COMMON;
		else if(str == "tls-object") sym_type = unisim::util::debug::SymbolBase::SYM_TLS;
		else if(str == "operating-system-specific-type") sym_type = unisim::util::debug::SymbolBase::SYM_OS;
		else if(str == "processor-specific-type") sym_type = unisim::util::debug::SymbolBase::SYM_PROC;
		else stream.setstate(std::ios::failbit);
	}
	return stream;
}
