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

#ifndef __UNISIM_UTIL_JSON_JSON_HH__
#define __UNISIM_UTIL_JSON_JSON_HH__

#include <iostream>
#include <sstream>
#include <stdint.h>

namespace unisim {
namespace util {
namespace json {

// Utility functions

template <typename T>
std::string ToString(const T& v)
{
	std::ostringstream sstr;
	sstr << v;
	return sstr.str();
}

std::string Escape(const std::string& s);

// JSON Parser visitor
struct JSON_Visitor
{
	void BeginObject() {}
	void EndObject() {}
	void BeginArray() {}
	void EndArray() {}
	void Comma() {}
	void Member(const std::string& name) {}
	void Value(const std::string& value) {}
	void Value(uint64_t value) {}
	void Value(int64_t value) {}
	void Value(double value) {}
	void Value(bool value) {}
	void Value() {}
	void Error(const std::string& msg) {}
};

// JSON Parser visitor for printing
struct JSON_Printer : JSON_Visitor
{
	JSON_Printer(std::ostream& out = std::cout, std::ostream& err = std::cerr);
	
	void BeginObject();
	void EndObject();
	void BeginArray();
	void EndArray();
	void Comma();
	void Member(const std::string& name);
	void Value(const std::string& value);
	void Value(uint64_t value);
	void Value(int64_t value);
	void Value(double value);
	void Value(bool value);
	void Value();
	void Error(const std::string& msg);
private:
	std::ostream& out;
	std::ostream& err;
};

// Token
enum Token
{
	TOK_IO_ERROR      = -2,
	TOK_ERROR         = -1,
	TOK_VOID          = 0,
	TOK_STRING        = 1,
	TOK_INT           = 2,
	TOK_UINT          = 3,
	TOK_FLOAT         = 4,
	TOK_TRUE          = 5,
	TOK_FALSE         = 6,
	TOK_NULL          = 7,
	TOK_LEFT_BRACE    = '{',
	TOK_RIGHT_BRACE   = '}',
	TOK_LEFT_BRACKET  = '[',
	TOK_RIGHT_BRACKET = ']',
	TOK_COMMA         = ',',
	TOK_COLON         = ':',
	TOK_EOF           = 127
};

std::ostream& operator << (std::ostream& os, const Token& token);

std::string PrettyString(const Token& token, const std::string& text);

// JSON Lexer
template <class VISITOR>
struct JSON_Lexer
{
	JSON_Lexer();
	~JSON_Lexer();
	
	void Reset();
protected:
	Token Next(std::istream& stream, VISITOR& visitor);
	void Back();
	const std::string& GetText() const { return text; }
	const std::string& GetStringValue() const { return str_value; }
	int64_t GetIntValue() const { return int_value; }
	uint64_t GetUIntValue() const { return uint_value; }
	double GetFloatValue() const { return float_value; }
	unsigned int GetLineNo() const { return lineno; }
	unsigned int GetColNo() const { return colno; }
	unsigned int GetTokenLineNo() const { return token_lineno; }
	unsigned int GetTokenColNo() const { return token_colno; }
	void ParseError(std::istream& stream, VISITOR& visitor, const std::string& msg);
private:
	void Scan(std::istream& stream, VISITOR& visitor);
	void ScanError(std::istream& stream, VISITOR& visitor, const std::string& msg);
	void Error(std::istream& stream, VISITOR& visitor, const std::string& msg, bool parse_error);
	
	Token token;
	Token look_ahead;
	std::string line;
	std::string text;
	std::string str_value;
	int64_t int_value;
	uint64_t uint_value;
	double float_value;
	unsigned int lineno;
	unsigned int colno;
	unsigned int token_lineno;
	unsigned int token_colno;
};

// JSON Parser
template <class VISITOR>
struct JSON_Parser : JSON_Lexer<VISITOR>
{
	typedef JSON_Lexer<VISITOR> Lexer;
	
	JSON_Parser();
	~JSON_Parser();
	
	bool Parse(std::istream& stream, VISITOR& visitor);
private:
	bool ParseValue(std::istream& stream, VISITOR& visitor);
	bool ParseObject(std::istream& stream, VISITOR& visitor);
	bool ParseMembers(std::istream& stream, VISITOR& visitor);
	bool ParseMember(std::istream& stream, VISITOR& visitor);
	bool ParseArray(std::istream& stream, VISITOR& visitor);
	bool ParseElements(std::istream& stream, VISITOR& visitor);
};

} // end of namespace json
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_JSON_JSON_HH__
