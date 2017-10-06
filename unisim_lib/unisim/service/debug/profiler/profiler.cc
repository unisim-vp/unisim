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
 */

#include <unisim/service/debug/profiler/profiler.hh>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <io.h>     // for function access() and mkdir()
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace unisim {
namespace service {
namespace debug {
namespace profiler {

//////////////////////////////// Format ///////////////////////////////////////

std::ostream& operator << (std::ostream& os, OutputFormat o_fmt)
{
	switch(o_fmt)
	{
		case O_FMT_TEXT: os << "text"; break;
		case O_FMT_HTML: os << "html"; break;
		case O_FMT_CSV: os << "csv"; break;
		default: os << "?"; break;
	}
	
	return os;
}

std::string OutputFormatSuffix(OutputFormat o_fmt)
{
	switch(o_fmt)
	{
		case O_FMT_TEXT: return std::string(".txt");
		case O_FMT_HTML: return std::string(".html");
		case O_FMT_CSV: return std::string(".csv");
	}
	
	return std::string();
}

/////////////////////////// c_string_to_HTML //////////////////////////////////

std::string c_string_to_HTML(const char *s)
{
	std::stringstream sstr;
	char c = *s;

	if(c)
	{
		do
		{
			switch(c)
			{
				case '\n':
					sstr << "<br>";
					break;
				case '<':
					sstr << "&lt;";
					break;
				case '>':
					sstr << "&gt;";
					break;
				case '&':
					sstr << "&amp;";
					break;
				case '"':
					sstr << "&quot;";
					break;
				case '\'':
					sstr << "&apos;";
					break;
				case ' ':
					sstr << "&nbsp;";
					break;
				case '\t':
					sstr << "&nbsp;&nbsp;&nbsp;&nbsp;";
				default:
					sstr << c;
			}
		}
		while((c = *(++s)));
	}
	
	return sstr.str();
}

///////////////////////////// c_string_to_URL /////////////////////////////////

std::string c_string_to_URL(const char *s)
{
	static const char reserved_char[] = { ';', '|', '?', ':', '@', '&', '=', '+', '$', ',' };
	static const char hex_digit[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	
	std::stringstream sstr;
	char c = *s;

	if(c)
	{
		do
		{
			unsigned int i;
			bool is_reserved_char = false;
			for(i = 0; i < sizeof(reserved_char) / sizeof(reserved_char[0]); i++)
			{
				if(c == reserved_char[i])
				{
					sstr << '%' << hex_digit[(c >> 4) & 0xf] << hex_digit[c & 0xf];
					is_reserved_char = true;
					break;
				}
			}
			
			if(!is_reserved_char)
			{
				if((c >= 32) && (c < 127) && (c != '/'))
				{
					sstr << c;
				}
				else
				{
					sstr << '_';
				}
			}
		}
		while((c = *(++s)));
	}
	
	return sstr.str();
}

///////////////////////////// c_string_to_CSV /////////////////////////////////

std::string c_string_to_CSV(const char *s)
{
	std::stringstream sstr;
	
	sstr << "\"";
	
	char c = *s;

	if(c)
	{
		do
		{
			switch(c)
			{
				case '"':
					sstr << "\"\"";
					break;
				default:
					sstr << c;
			}
		}
		while((c = *(++s)));
	}
	
	sstr << "\"";
	
	return sstr.str();
}

//////////////////////////////// MakeDir //////////////////////////////////////

void MakeDir(const char *dirname)
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	mkdir(dirname);
#else
	mkdir(dirname, S_IRWXU);
#endif
}

} // end of namespace profiler
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

namespace unisim {
namespace kernel {
namespace service {

using unisim::service::debug::profiler::OutputFormat;
using unisim::service::debug::profiler::O_FMT_TEXT;
using unisim::service::debug::profiler::O_FMT_HTML;
using unisim::service::debug::profiler::O_FMT_CSV;

template <> Variable<OutputFormat>::Variable(const char *_name, Object *_object, OutputFormat& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
	AddEnumeratedValue("text");
	AddEnumeratedValue("html");
}

template <>
const char *Variable<OutputFormat>::GetDataTypeName() const
{
	return "output-format";
}

template <>
unsigned int Variable<OutputFormat>::GetBitSize() const
{
	return 1;
}

template <> Variable<OutputFormat>::operator bool () const { return *storage != O_FMT_TEXT; }
template <> Variable<OutputFormat>::operator long long () const { return *storage; }
template <> Variable<OutputFormat>::operator unsigned long long () const { return *storage; }
template <> Variable<OutputFormat>::operator double () const { return (double)(*storage); }
template <> Variable<OutputFormat>::operator string () const
{
	switch(*storage)
	{
		case O_FMT_TEXT: return std::string("text");
		case O_FMT_HTML: return std::string("html");
		case O_FMT_CSV: return std::string("csv");
	}
	return std::string("?");
}

template <> VariableBase& Variable<OutputFormat>::operator = (bool value)
{
	if(IsMutable())
	{
		OutputFormat tmp = *storage;
		switch((unsigned int) value)
		{
			case O_FMT_TEXT:
			case O_FMT_HTML:
			case O_FMT_CSV:
				tmp = (OutputFormat)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<OutputFormat>::operator = (long long value)
{
	if(IsMutable())
	{
		OutputFormat tmp = *storage;
		switch(value)
		{
			case O_FMT_TEXT:
			case O_FMT_HTML:
			case O_FMT_CSV:
				tmp = (OutputFormat) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<OutputFormat>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		OutputFormat tmp = *storage;
		switch(value)
		{
			case O_FMT_TEXT:
			case O_FMT_HTML:
			case O_FMT_CSV:
				tmp = (OutputFormat) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<OutputFormat>::operator = (double value)
{
	if(IsMutable())
	{
		OutputFormat tmp = *storage;
		switch((unsigned int) value)
		{
			case O_FMT_TEXT:
			case O_FMT_HTML:
			case O_FMT_CSV:
				tmp = (OutputFormat)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<OutputFormat>::operator = (const char *value)
{
	if(IsMutable())
	{
		OutputFormat tmp = *storage;
		if(std::string(value) == std::string("text")) tmp = O_FMT_TEXT;
		else if(std::string(value) == std::string("html")) tmp = O_FMT_HTML;
		else if(std::string(value) == std::string("csv")) tmp = O_FMT_CSV;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<OutputFormat>;

} // end of service namespace
} // end of kernel namespace
} // end of unisim namespace
