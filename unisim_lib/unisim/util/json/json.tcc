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

#ifndef __UNISIM_UTIL_JSON_JSON_TCC__
#define __UNISIM_UTIL_JSON_JSON_TCC__

#include <unisim/util/json/json.hh>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

namespace unisim {
namespace util {
namespace json {

template <class VISITOR>
JSON_Lexer<VISITOR>::JSON_Lexer()
	: token(TOK_VOID)
	, look_ahead(TOK_VOID)
	, line()
	, text()
	, str_value()
	, int_value(0)
	, uint_value(0)
	, float_value(0.0)
	, lineno(1)
	, colno(1)
	, token_lineno(1)
	, token_colno(1)
{
}

template <class VISITOR>
JSON_Lexer<VISITOR>::~JSON_Lexer()
{
}

template <class VISITOR>
void JSON_Lexer<VISITOR>::Reset()
{
	token = TOK_VOID;
	look_ahead = TOK_VOID;
	line.clear();
	text.clear();
	str_value.clear();
	int_value = 0;
	uint_value = 0;
	float_value = 0.0;
	lineno = 1;
	colno = 1;
	token_lineno = 1;
	token_colno = 1;
}

template <class VISITOR>
Token JSON_Lexer<VISITOR>::Next(std::istream& stream, VISITOR& visitor)
{
	if(look_ahead)
	{
		token = look_ahead;
		look_ahead = TOK_VOID;
	}
	else
	{
		Scan(stream, visitor);
	}
	
	return token;
}

template <class VISITOR>
void JSON_Lexer<VISITOR>::Back()
{
	look_ahead = token;
}

template <class VISITOR>
void JSON_Lexer<VISITOR>::Scan(std::istream& stream, VISITOR& visitor)
{
	char c;
	int state = 0;
	bool sign = false;
	bool back = false;
	unsigned int code_point_digits = 0;
	uint32_t code_point = 0;
	text.clear();
	token = TOK_VOID;
	token_lineno = lineno;
	token_colno = colno;
	
	do
	{
		if(stream.get(c).eof())
		{
			token = TOK_EOF;
			break;
		}
		
		if(stream.fail())
		{
			token = TOK_IO_ERROR;
			break;
		}
		
		switch(state)
		{
			case 0:
				if((c == ' ') || (c == '\t') || (c == '\r')) break;
				if(c == '\n') { lineno++; token_lineno = lineno; colno = 1; token_colno = colno; line.clear(); break; }
				if(c == '{') { token = TOK_LEFT_BRACE; break; }
				if(c == '}') { token = TOK_RIGHT_BRACE; break; }
				if(c == '[') { token = TOK_LEFT_BRACKET; break; }
				if(c == ']') { token = TOK_RIGHT_BRACKET; break; }
				if(c == ',') { token = TOK_COMMA; break; }
				if(c == ':') { token = TOK_COLON; break; }
				if(c == '"')
				{
					str_value.clear();
					state = 10;
					break;
				}
				if(c == '-')
				{
					sign = true;
					state = 20;
					break;
				}
				if(c == '0')
				{
					state = 22;
					break;
				}
				if(isdigit(c))
				{
					state = 21;
					break;
				}
				if(c == 't')
				{
					state = 30;
					break;
				}
				if(c == 'f')
				{
					state = 40;
					break;
				}
				if(c == 'n')
				{
					state = 50;
					break;
				}

				token = TOK_ERROR;
				break;
				
			case 10: // string content
				if(c == '"')
				{
					state = 0;
					token = TOK_STRING;
					break;
				}
				if(c == '\\')
				{
					state = 14;
					break;
				}
				if(((uint8_t) c >= 0x20) && ((uint8_t) c <= 0x7f))
				{
					// ASCII/1-byte UTF-8 character (excluding control characters)
					// Code points from U+0020 to U+007F
					str_value.append(1, c);
					break;
				}
				if(((uint8_t) c & 0xe0) == (0xc0))
				{
					// 2-byte UTF-8 character
					// Code points from U+0080 to U+07FF
					str_value.append(1, c);
					state = 13;
					break;
				}
				if(((uint8_t) c & 0xf0) == 0xe0)
				{
					// 3-byte UTF-8 character
					// Code points from U+0800 to U+FFFF
					str_value.append(1, c);
					state = 12;
					break;
				}
				if(((uint8_t) c & 0xf8) == 0xf0)
				{
					// 4-byte UTF-8 character
					// Code points from U+10000 to U+10FFFF
					str_value.append(1, c);
					state = 11;
					break;
				}
				token = TOK_ERROR;
				break;
				
			case 11: // 2nd byte a 4-byte UTF-8 character
				if(((uint8_t) c & 0xc0) == 0x80)
				{
					str_value.append(1, c);
					state = 12;
					break;
				}
				
				token = TOK_ERROR;
				break;
				
			case 12: // (n-1)-th byte a n-byte UTF-8 character
				if(((uint8_t) c & 0xc0) == 0x80)
				{
					str_value.append(1, c);
					state = 12;
					break;
				}
				
				token = TOK_ERROR;
				break;
				
			case 13: // n-th byte a n-byte UTF-8 character
				if(((uint8_t) c & 0xc0) == 0x80)
				{
					str_value.append(1, c);
					state = 10;
					break;
				}
				
				token = TOK_ERROR;
				break;
				
			case 14: // escape
				if((c == '"') || (c == '\\') || (c == '/'))
				{
					str_value.append(1, c);
					state = 10;
					break;
				}
				if(c == 'b')
				{
					str_value.append(1, '\b');
					state = 10;
					break;
				}
				if(c == 'n')
				{
					str_value.append(1, '\n');
					state = 10;
					break;
				}
				if(c == 'r')
				{
					str_value.append(1, '\r');
					state = 10;
					break;
				}
				if(c == 't')
				{
					str_value.append(1, '\t');
					state = 10;
					break;
				}
				if(c == 'u')
				{
					state = 15;
					code_point_digits = 4;
					code_point = 0;
					break;
				}
				
				token = TOK_ERROR;
				break;
				
			case 15: // unicode code point n-th hex digit of 4 hex digits
				if(isxdigit(c))
				{
					if(isdigit(c))
					{
						code_point = (code_point << 4) | (c - '0');
					}
					else if((c >= 'a') && (c <= 'f'))
					{
						code_point = (code_point << 4) | (c - 'a' + 10);
					}
					if((c >= 'A') && (c <= 'F'))
					{
						code_point = (code_point << 4) | (c - 'A' + 10);
					}
					if(--code_point_digits == 0)
					{
						if(code_point <= 0x7f)
						{
							// 1-byte UTF-8 character
							str_value.append(1, (char)(code_point & 0xff));
						}
						else if(code_point <= 0x7ff)
						{
							// 2-byte UTF-8 character: xxxxxyyyyyy => 110xxxxx  10yyyyyy
							uint16_t v = 0xc080 | ((code_point << 2) & 0x1f00) | (code_point & 0x3f);
							str_value.append(1, (char)((v >> 8) & 0xff));
							str_value.append(1, (char)(v & 0xff));
						}
						else if(code_point <= 0xffff)
						{
							// 3-byte UTF-8 character: xxxxyyyyyyzzzzzz => 1110xxxx 10yyyyyy 10zzzzzz
							uint32_t v = 0xe08080 | ((code_point << 4) & 0x1f0000) | ((code_point << 2) & 0x3f00) | (code_point & 0x3f);
							str_value.append(1, (char)((v >> 16) & 0xff));
							str_value.append(1, (char)((v >> 8) & 0xff));
							str_value.append(1, (char)(v & 0xff));
						}
						state = 10;
					}
					break;
				}
				
				token = TOK_ERROR;
				break;
				
			case 20: // minus
				if(c == '0')
				{
					state = 22;
					break;
				}
				if(isdigit(c))
				{
					state = 21;
					break;
				}
				
				token = TOK_ERROR;
				break;
				
			case 21: // digits
				if(isdigit(c))
				{
					break;
				}
				if(c == '.')
				{
					state = 23;
					break;
				}
				if((c == 'e') || (c == 'E'))
				{
					state = 24;
					break;
				}
				
				back = true;
				
				if(sign)
				{
					const char *nptr = text.c_str();
					char *endptr = 0;
					int_value = ::strtoll(nptr, &endptr, 10);
					assert((endptr - nptr) == text.length());
					token = TOK_INT;
					break;
				}
				else
				{
					const char *nptr = text.c_str();
					char *endptr = 0;
					uint_value = ::strtoull(nptr, &endptr, 10);
					assert((endptr - nptr) == text.length());
					token = TOK_UINT;
					break;
				}
				break;
				
			case 22: // zero
				if(c == '.')
				{
					state = 23;
					break;
				}
				
				back = true;
				
				if(sign)
				{
					int_value = 0;
					token = TOK_INT;
				}
				else
				{
					uint_value = 0;
					token = TOK_UINT;
				}
				break;
			
			case 23: // frac
				if(isdigit(c))
				{
					break;
				}
				
				if((c == 'e') || (c == 'E'))
				{
					state = 24;
					break;
				}
				
				back = true;

				{
					const char *nptr = text.c_str();
					char *endptr = 0;
					float_value = ::strtod(nptr, &endptr);
					assert((endptr - nptr) == text.length());
				}
				token = TOK_FLOAT;
				break;
				
			case 24: // exp with optional signal
				if((c == '-') || (c == '+') || isdigit(c))
				{
					state = 25;
					break;
				}
				
				back = true;
				token = TOK_ERROR;
				break;
				
			case 25: // exp
				if(isdigit(c))
				{
					break;
				}
				
				back = true;
				
				{
					const char *nptr = text.c_str();
					char *endptr = 0;
					float_value = ::strtod(nptr, &endptr);
					assert((endptr - nptr) == text.length());
				}
				token = TOK_FLOAT;
				break;
				
			case 30:
				if(c == 'r')
				{
					state = 31;
					break;
				}
				token = TOK_ERROR;
				break;
			case 31:
				if(c == 'u')
				{
					state = 32;
					break;
				}
				token = TOK_ERROR;
				break;
			case 32:
				if(c == 'e')
				{
					token = TOK_TRUE;
					break;
				}
				token = TOK_ERROR;
				break;
			case 40:
				if(c == 'a')
				{
					state = 41;
					break;
				}
				token = TOK_ERROR;
				break;
			case 41:
				if(c == 'l')
				{
					state = 42;
					break;
				}
				token = TOK_ERROR;
				break;
			case 42:
				if(c == 's')
				{
					state = 43;
					break;
				}
				token = TOK_ERROR;
				break;
			case 43:
				if(c == 'e')
				{
					token = TOK_FALSE;
					break;
				}
				token = TOK_ERROR;
				break;
			case 50:
				if(c == 'u')
				{
					state = 51;
					break;
				}
				token = TOK_ERROR;
				break;
			case 51:
				if(c == 'l')
				{
					state = 52;
					break;
				}
				token = TOK_ERROR;
				break;
			case 52:
				if(c == 'l')
				{
					token = TOK_NULL;
					break;
				}
				token = TOK_ERROR;
				break;
		}
		
		if(back || (token == TOK_ERROR))
		{
			stream.putback(c);
		}
		else if((c != '\r') && (c != '\n'))
		{
			line.append(1, c);
			
			if(token >= 0)
			{
				colno++;
			}
			
			if((c == ' ') || (c == '\t'))
			{
				token_colno = colno;
			}

			if((c != ' ') && (c != '\t'))
			{
				text.append(1, c);
			}
		}
	}
	while(!token);
	
	if(token < 0)
	{
		if(token == TOK_ERROR)
		{
			ScanError(stream, visitor, std::string("unexpected character '") + Escape(std::string(1, c)) + "'");
		}
		else if(token == TOK_IO_ERROR)
		{
			visitor.Error("I/O error\r\n");
		}
		else
		{
			visitor.Error("Internal error\r\n");
		}
	}
}

template <class VISITOR>
void JSON_Lexer<VISITOR>::ParseError(std::istream& stream, VISITOR& visitor, const std::string& msg)
{
	Error(stream, visitor, msg, true);
}

template <class VISITOR>
void JSON_Lexer<VISITOR>::ScanError(std::istream& stream, VISITOR& visitor, const std::string& msg)
{
	Error(stream, visitor, msg, false);
}

template <class VISITOR>
void JSON_Lexer<VISITOR>::Error(std::istream& stream, VISITOR& visitor, const std::string& msg, bool parse_error)
{
	std::ostringstream sstr;
	sstr << "At line #" << (parse_error ? token_lineno : lineno) << ", column #" << (parse_error ? token_colno : colno) << ", " << msg << std::endl;
	do
	{
		char c;
		
		if(stream.get(c).eof()) break;
		
		if(stream.fail()) break;
		
		if(c == '\n') break;
		
		if(c == '\r') continue;
			
		line.append(1, c);
	}
	while(1);
	
	for(unsigned int i = 0; i < line.length(); i++)
	{
		if(line[i] == '\t') sstr << ' '; else sstr << line[i];
	}
	sstr << std::endl;
	for(unsigned int n = (parse_error ? token_colno : colno); n > 1; --n) sstr << ' ';
	sstr << '^' << std::endl;
	visitor.Error(sstr.str());
}

template <class VISITOR>
JSON_Parser<VISITOR>::JSON_Parser()
{
}

template <class VISITOR>
JSON_Parser<VISITOR>::~JSON_Parser()
{
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::Parse(std::istream& stream, VISITOR& visitor)
{
	return ParseValue(stream, visitor);
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::ParseValue(std::istream& stream, VISITOR& visitor)
{
	Token token = Lexer::Next(stream, visitor);
	if(token < 0) return false;
	
	switch(token)
	{
		case TOK_STRING:
			visitor.Value(Lexer::GetStringValue());
			return true;
			
		case TOK_INT:
			visitor.Value(Lexer::GetIntValue());
			return true;
			
		case TOK_UINT:
			visitor.Value(Lexer::GetUIntValue());
			return true;

		case TOK_FLOAT:
			visitor.Value(Lexer::GetFloatValue());
			return true;

		case TOK_TRUE:
			visitor.Value(true);
			return true;

		case TOK_FALSE:
			visitor.Value(false);
			return true;
			
		case TOK_NULL:
			visitor.Value();
			return true;
			
		case TOK_LEFT_BRACE:
			return ParseObject(stream, visitor);
			
		case TOK_LEFT_BRACKET:
			return ParseArray(stream, visitor);
			
		default:
			Lexer::ParseError(stream, visitor, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting a value");
			return false;
	}
	
	return false;
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::ParseObject(std::istream& stream, VISITOR& visitor)
{
	visitor.BeginObject();
	
	Token token = Lexer::Next(stream, visitor);
	if(token < 0) return false;
	
	if(token == TOK_RIGHT_BRACE)
	{
		visitor.EndObject();
		return true;
	}
	
	Lexer::Back();
	
	if(!ParseMembers(stream, visitor)) return false;
	
	token = Lexer::Next(stream, visitor);
	
	if(token != TOK_RIGHT_BRACE)
	{
		Lexer::ParseError(stream, visitor, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(TOK_RIGHT_BRACE));
		return false;
	}
	
	visitor.EndObject();
	return true;
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::ParseMembers(std::istream& stream, VISITOR& visitor)
{
	if(!ParseMember(stream, visitor)) return false;
	
	Token token = Lexer::Next(stream, visitor);
	if(token < 0) return false;
	
	if(token == TOK_COMMA)
	{
		visitor.Comma();
		return ParseMembers(stream, visitor);
	}

	Lexer::Back();
	
	return true;
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::ParseMember(std::istream& stream, VISITOR& visitor)
{
	Token token = Lexer::Next(stream, visitor);
	if(token < 0) return false;
	
	if(token != TOK_STRING)
	{
		Lexer::ParseError(stream, visitor, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(TOK_STRING));
		return false;
	}
	
	visitor.Member(Lexer::GetStringValue());
	
	token = Lexer::Next(stream, visitor);
	if(token < 0) return false;
	
	if(token != TOK_COLON)
	{
		Lexer::ParseError(stream, visitor, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(TOK_COLON));
		return false;
	}
	
	return ParseValue(stream, visitor);
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::ParseArray(std::istream& stream, VISITOR& visitor)
{
	visitor.BeginArray();
	
	Token token = Lexer::Next(stream, visitor);
	if(token < 0) return false;
	
	if(token == TOK_RIGHT_BRACKET)
	{
		visitor.EndArray();
		return true;
	}
	
	Lexer::Back();
	
	if(!ParseElements(stream, visitor)) return false;
	
	token = Lexer::Next(stream, visitor);
	
	if(token != TOK_RIGHT_BRACKET)
	{
		Lexer::ParseError(stream, visitor, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(TOK_RIGHT_BRACKET));
		return false;
	}
	
	visitor.EndArray();
	return true;
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::ParseElements(std::istream& stream, VISITOR& visitor)
{
	if(!ParseValue(stream, visitor)) return false;
	
	Token token = Lexer::Next(stream, visitor);
	if(token < 0) return false;
	
	if(token == TOK_COMMA)
	{
		visitor.Comma();
		return ParseElements(stream, visitor);
	}
	
	Lexer::Back();
	
	return true;
}

} // end of namespace json
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_JSON_JSON_TCC__
