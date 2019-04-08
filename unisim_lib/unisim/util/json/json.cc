/*
 *  Copyright (c) 2019,
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

#include <unisim/util/json/json.hh>

namespace unisim {
namespace util {
namespace json {

std::string Escape(const std::string& s)
{
	std::string r;
	for(std::size_t i = 0; i < s.length(); ++i)
	{
		char c = s[i];
		switch(c)
		{
			case '\\': r += "\\"; break;
			case '\r': r += "\\r"; break;
			case '\n': r += "\\n"; break;
			case '\t': r += "\\t"; break;
			case '\b': r += "\\b"; break;
			default: r.append(1, c); break;
		}
	}
	return r;
}

std::ostream& operator << (std::ostream& os, const Token& token)
{
	switch(token)
	{
		case TOK_IO_ERROR     : os << "an I/O error"; break;
		case TOK_ERROR        : os << "an error"; break;
		case TOK_VOID         : os << "nothing"; break;
		case TOK_STRING       : os << "a string"; break;
		case TOK_INT          : os << "a signed integer number"; break;
		case TOK_UINT         : os << "a unsigned integer number"; break;
		case TOK_FLOAT        : os << "a floating point number"; break;
		case TOK_TRUE         : os << "'true'"; break;
		case TOK_FALSE        : os << "'false'"; break;
		case TOK_NULL         : os << "'null'"; break;
		case TOK_LEFT_BRACE   : os << "'{'"; break;
		case TOK_RIGHT_BRACE  : os << "'}'"; break;
		case TOK_LEFT_BRACKET : os << "'['"; break;
		case TOK_RIGHT_BRACKET: os << "']'"; break;
		case TOK_COMMA        : os << "','"; break;
		case TOK_COLON        : os << "':'"; break;
		case TOK_EOF          : os << "an end-of-file"; break;
		default               : os << "an unknown token"; break;
	}
}

std::string PrettyString(const Token& token, const std::string& text)
{
	switch(token)
	{
		case TOK_IO_ERROR     : break;
		case TOK_ERROR        : break;
		case TOK_VOID         : break;
		case TOK_STRING       : return std::string("string ") + Escape(text); break;
		case TOK_INT          : return std::string("number ") + text; break;
		case TOK_UINT         : return std::string("number ") + text; break;
		case TOK_FLOAT        : return std::string("number ") + text; break;
		case TOK_TRUE         :
		case TOK_FALSE        :
		case TOK_NULL         :
		case TOK_LEFT_BRACE   :
		case TOK_RIGHT_BRACE  :
		case TOK_LEFT_BRACKET :
		case TOK_RIGHT_BRACKET:
		case TOK_COMMA        :
		case TOK_COLON        :
		case TOK_EOF          :
			return ToString(token);
	}
	return std::string();
}

/////////////////////////////// JSON_Printer //////////////////////////////////

JSON_Printer::JSON_Printer(std::ostream& _out, std::ostream& _err)
	: out(_out)
	, err(_err)
{
}

void JSON_Printer::BeginObject()
{
	out << "{";
}

void JSON_Printer::EndObject()
{
	out << "}";
}
void JSON_Printer::BeginArray()
{
	out << "[";
}

void JSON_Printer::EndArray()
{
	out << "]";
}

void JSON_Printer::Comma()
{
	out << ",";
}

void JSON_Printer::Member(const std::string& name)
{
	out << "\"" << name << "\":";
}

void JSON_Printer::Value(const std::string& value)
{
	out << "\"";
	for(std::size_t i = 0; i < value.length(); ++i)
	{
		char c = value[i];
		switch(c)
		{
			case '\\': out << "\\\\";break;
			case '\r': out << "\\r"; break;
			case '\n': out << "\\n"; break;
			case '\t': out << "\\t"; break;
			case '\b': out << "\\b"; break;
			default: out << c; break;
		}
	}
	out << "\"";
}

void JSON_Printer::Value(uint64_t value)
{
	out << value;
}

void JSON_Printer::Value(int64_t value)
{
	out << value;
}

void JSON_Printer::Value(double value)
{
	out << value;
}

void JSON_Printer::Value(bool value)
{
	out << (value ? "true" : "false");
}

void JSON_Printer::Value()
{
	out << "null";
}

void JSON_Printer::Error(const std::string& msg)
{
	err << std::endl << msg;
}

} // end of namespace json
} // end of namespace util
} // end of namespace unisim
