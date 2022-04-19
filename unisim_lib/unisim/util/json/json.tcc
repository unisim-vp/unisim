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
#include <unisim/util/dictionary/dictionary.tcc>
#include <ctype.h>
#include <stdlib.h>

namespace unisim {
namespace util {
namespace json {

template <typename VISITOR>
void JSON_Value::Scan(VISITOR& visitor) const
{
	switch(type)
	{
		case JSON_UNDEFINED: break;
		case JSON_STRING   : break;
		case JSON_INT      : break;
		case JSON_FLOAT    : break;
		case JSON_OBJECT   : dynamic_cast<JSON_Object const *>(this)->Scan(visitor); break;
		case JSON_ARRAY    : dynamic_cast<JSON_Array const *>(this)->Scan(visitor); break;
		case JSON_BOOLEAN  : break;
		case JSON_NULL     : break;
	}
}

template <typename VISITOR>
bool JSON_Value::Visit(VISITOR& visitor) const
{
	switch(type)
	{
		case JSON_UNDEFINED: return visitor.Visit(*static_cast<JSON_Value const *>(this));
		case JSON_STRING   : return visitor.Visit(*static_cast<JSON_String const *>(this));
		case JSON_INT      : return visitor.Visit(*static_cast<JSON_Integer const *>(this));
		case JSON_FLOAT    : return visitor.Visit(*static_cast<JSON_Float const *>(this));
		case JSON_OBJECT   : return visitor.Visit(*static_cast<JSON_Object const *>(this));
		case JSON_ARRAY    : return visitor.Visit(*static_cast<JSON_Array const *>(this));
		case JSON_BOOLEAN  : return visitor.Visit(*static_cast<JSON_Boolean const *>(this));
		case JSON_NULL     : return visitor.Visit(*static_cast<JSON_Null const *>(this));
	}
	
	return false;
}

template <typename VISITOR> void JSON_Object::Scan(VISITOR& visitor) const
{
	for(typename Members::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		const JSON_Member& member = (*it).second;
		if(!visitor.Visit(member)) break;
	}
}

template <typename VISITOR> void JSON_Array::Scan(VISITOR& visitor) const
{
	for(typename Elements::const_iterator it = elements.begin(); it != elements.end(); ++it)
	{
		const JSON_Value& element = *it;
		if(!element.Visit(visitor)) break;
	}
}

template <typename VISITOR>
JSON_Lexer<VISITOR>::JSON_Lexer()
	: enable_c_comments(false)
	, enable_cpp_comments(false)
	, eof(false)
	, error(false)
	, token(TOK_VOID)
	, look_ahead(TOK_VOID)
	, line()
	, text()
	, str_value()
	, int_value()
	, float_value(0.0)
	, loc(1, 1)
	, token_loc(1, 1)
	, token_dictionary(std::cout, std::cerr, std::cerr)
{
	token_dictionary.Add("true", TOK_TRUE);
	token_dictionary.Add("false", TOK_FALSE);
	token_dictionary.Add("null", TOK_NULL);
	token_dictionary.Add("{", TOK_LEFT_BRACE);
	token_dictionary.Add("}", TOK_RIGHT_BRACE);
	token_dictionary.Add("[", TOK_LEFT_BRACKET);
	token_dictionary.Add("]", TOK_RIGHT_BRACKET);
	token_dictionary.Add(",", TOK_COMMA);
	token_dictionary.Add(":", TOK_COLON);
}

template <typename VISITOR>
JSON_Lexer<VISITOR>::~JSON_Lexer()
{
}

template <typename VISITOR>
void JSON_Lexer<VISITOR>::Reset()
{
	eof = false;
	token = TOK_VOID;
	look_ahead = TOK_VOID;
	line.clear();
	text.clear();
	str_value.clear();
	int_value.sign = false;
	int_value.abs_value = 0;
	float_value = 0.0;
	loc.lineno = 1;
	loc.colno = 1;
}

template <typename VISITOR>
bool JSON_Lexer<VISITOR>::SetOption(JSON_OPTION option, bool value)
{
	switch(option)
	{
		case OPT_ENABLE_C_COMMENTS  : enable_c_comments = value; return true;
		case OPT_ENABLE_CPP_COMMENTS: enable_cpp_comments = value; return true;
	}
	return false;
}

template <typename VISITOR>
bool JSON_Lexer<VISITOR>::GetOption(JSON_OPTION option, bool& value) const
{
	switch(option)
	{
		case OPT_ENABLE_C_COMMENTS  : value = enable_c_comments; return true;
		case OPT_ENABLE_CPP_COMMENTS: value = enable_cpp_comments; return true;
	}
	return false;
}

template <typename VISITOR>
bool JSON_Lexer<VISITOR>::GetOptionFlag(JSON_OPTION option) const
{
	switch(option)
	{
		case OPT_ENABLE_C_COMMENTS  :return enable_c_comments;
		case OPT_ENABLE_CPP_COMMENTS: return enable_cpp_comments;
	}
	return false;
}

template <typename VISITOR>
Token JSON_Lexer<VISITOR>::Next(std::istream& stream, VISITOR& visitor)
{
	if(look_ahead)
	{
		token = look_ahead;
		look_ahead = TOK_VOID;
	}
	else if(token >= 0)
	{
		Scan(stream, visitor);
	}
	
	return token;
}

template <typename VISITOR>
void JSON_Lexer<VISITOR>::Back()
{
	look_ahead = token;
}

template <typename VISITOR>
void JSON_Lexer<VISITOR>::Append(char c)
{
	text += c;
	line += c;
}

template <typename VISITOR>
void JSON_Lexer<VISITOR>::Scan(std::istream& stream, VISITOR& visitor)
{
	if(eof)
	{
		token = TOK_EOF;
		return;
	}
	
	char c;
	int state = 0;
	bool sign = false;
	unsigned int code_point_digits = 0;
	uint32_t code_point = 0;
	text.clear();
	token = TOK_VOID;
	
	do
	{
		switch(state)
		{
			case 0:
			{
				unisim::util::dictionary::DictionaryEntry<Token> *entry = token_dictionary.FindDictionaryEntry(&stream, text);
				if(entry)
				{
					token_loc = loc;
					line += text;
					loc.colno += text.length();
					token = entry->GetValue();
					break;
				}
				
				if(stream.get(c).eof())
				{
					eof = true;
					token_loc = loc;
					token = TOK_EOF;
					break;
				}
				if(stream.bad())
				{
					token_loc = loc;
					token = TOK_IO_ERROR;
					break;
				}
				
				if((c == ' ') || (c == '\t')) { line += c; text.clear(); loc.colno++; break; }
				if(c == '\r') break;
				if(c == '\n') { loc.lineno++; loc.colno = 1; line.clear(); text.clear(); break; }
				if(c == '"')
				{
					Append(c);
					token_loc = loc;
					loc.colno++;
					str_value.clear();
					state = 10;
					break;
				}
				if(c == '-')
				{
					Append(c);
					token_loc = loc;
					loc.colno++;
					sign = true;
					state = 20;
					break;
				}
				if(c == '0')
				{
					Append(c);
					token_loc = loc;
					loc.colno++;
					state = 22;
					break;
				}
				if(isdigit(c))
				{
					Append(c);
					token_loc = loc;
					loc.colno++;
					state = 21;
					break;
				}
				if((enable_c_comments || enable_cpp_comments) && (c == '/'))
				{
					text.clear();
					line += c;
					loc.colno++;
					state = 30;
					break;
				}

				stream.putback(c);
				token_loc = loc;
				token = TOK_ERROR;
				break;
			}
				
			case 10: // string content
				if(stream.get(c).good())
				{
					if(c == '"')
					{
						Append(c);
						loc.colno++;
						state = 0;
						token = TOK_STRING;
						break;
					}
					if(c == '\\')
					{
						Append(c);
						loc.colno++;
						state = 14;
						break;
					}
					if(((uint8_t) c >= 0x20) && ((uint8_t) c <= 0x7f))
					{
						// ASCII/1-byte UTF-8 character (excluding control characters)
						// Code points from U+0020 to U+007F
						Append(c);
						loc.colno++;
						str_value += c;
						break;
					}
					if(((uint8_t) c & 0xe0) == (0xc0))
					{
						// 2-byte UTF-8 character
						// Code points from U+0080 to U+07FF
						Append(c);
						loc.colno++;
						str_value += c;
						state = 13;
						break;
					}
					if(((uint8_t) c & 0xf0) == 0xe0)
					{
						// 3-byte UTF-8 character
						// Code points from U+0800 to U+FFFF
						Append(c);
						str_value += c;
						state = 12;
						break;
					}
					if(((uint8_t) c & 0xf8) == 0xf0)
					{
						// 4-byte UTF-8 character
						// Code points from U+10000 to U+10FFFF
						Append(c);
						str_value += c;
						state = 11;
						break;
					}
					
					stream.putback(c);
				}
				token = TOK_ERROR;
				break;
				
			case 11: // UTF-8 character: 3 bytes remaining
				if(stream.get(c).good())
				{
					if(((uint8_t) c & 0xc0) == 0x80)
					{
						Append(c);
						loc.colno++;
						str_value += c;
						state = 12;
						break;
					}
					
					stream.putback(c);
				}
				
				token = TOK_ERROR;
				break;
				
			case 12: // UTF-8 character: 2 bytes remaining
				if(stream.get(c).good())
				{
					if(((uint8_t) c & 0xc0) == 0x80)
					{
						Append(c);
						loc.colno++;
						str_value += c;
						state = 13;
						break;
					}
					
					stream.putback(c);
				}
				
				token = TOK_ERROR;
				break;
				
			case 13: // UTF-8 character: 1 byte remaining
				if(stream.get(c).good())
				{
					if(((uint8_t) c & 0xc0) == 0x80)
					{
						Append(c);
						loc.colno++;
						str_value += c;
						state = 10;
						break;
					}
					
					stream.putback(c);
				}
				
				token = TOK_ERROR;
				break;
				
			case 14: // escape
				if(stream.get(c).good())
				{
					if((c == '"') || (c == '\\') || (c == '/'))
					{
						Append(c);
						loc.colno++;
						str_value += c;
						state = 10;
						break;
					}
					if(c == 'b')
					{
						Append(c);
						loc.colno++;
						str_value += '\b';
						state = 10;
						break;
					}
					if(c == 'n')
					{
						Append(c);
						loc.colno++;
						str_value += '\n';
						state = 10;
						break;
					}
					if(c == 'r')
					{
						Append(c);
						loc.colno++;
						str_value += '\r';
						state = 10;
						break;
					}
					if(c == 't')
					{
						Append(c);
						loc.colno++;
						str_value += '\t';
						state = 10;
						break;
					}
					if(c == 'u')
					{
						Append(c);
						loc.colno++;
						state = 15;
						code_point_digits = 4;
						code_point = 0;
						break;
					}
					
					stream.putback(c);
				}
				
				token = TOK_ERROR;
				break;
				
			case 15: // unicode code point n-th hex digit of 4 hex digits
				if(stream.get(c).good())
				{
					if(isxdigit(c))
					{
						loc.colno++;
						Append(c);
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
								char c0 = (char)(code_point & 0xff);
								str_value += c0;
							}
							else if(code_point <= 0x7ff)
							{
								// 2-byte UTF-8 character: xxxxxyyyyyy => 110xxxxx  10yyyyyy
								uint16_t v = 0xc080 | ((code_point << 2) & 0x1f00) | (code_point & 0x3f);
								char c0 = (char)((v >> 8) & 0xff);
								char c1 = (char)(v & 0xff);
								str_value += c0;
								str_value += c1;
							}
							else if(code_point <= 0xffff)
							{
								// 3-byte UTF-8 character: xxxxyyyyyyzzzzzz => 1110xxxx 10yyyyyy 10zzzzzz
								uint32_t v = 0xe08080 | ((code_point << 4) & 0x0f0000) | ((code_point << 2) & 0x3f00) | (code_point & 0x3f);
								char c0 = (char)((v >> 16) & 0xff);
								char c1 = (char)((v >> 8) & 0xff);
								char c2 = (char)(v & 0xff);
								str_value += c0;
								str_value += c1;
								str_value += c2;
							}
							state = 10;
						}
						break;
					}
					
					stream.putback(c);
				}
				
				token = TOK_ERROR;
				break;
				
			case 20: // minus
				if(stream.get(c).good())
				{
					if(c == '0')
					{
						Append(c);
						loc.colno++;
						state = 22;
						break;
					}
					if(isdigit(c))
					{
						Append(c);
						loc.colno++;
						state = 21;
						break;
					}
					
					stream.putback(c);
				}
				
				token = TOK_ERROR;
				break;
				
			case 21: // digits
			
				if(stream.get(c).good())
				{
					if(isdigit(c))
					{
						Append(c);
						loc.colno++;
						break;
					}
					if(c == '.')
					{
						Append(c);
						loc.colno++;
						state = 23;
						break;
					}
					if((c == 'e') || (c == 'E'))
					{
						Append(c);
						loc.colno++;
						state = 24;
						break;
					}
					
					stream.putback(c);
				}
				
				{
					int_value.sign = sign;
					const char *nptr = text.c_str();
					char *endptr = 0;
					
					if(sign)
					{
						int_value.abs_value = -::strtoll(nptr, &endptr, 10);
					}
					else
					{
						int_value.abs_value = ::strtoull(nptr, &endptr, 10);
					}
					
					assert((uintptr_t)(endptr - nptr) == text.length());
					token = TOK_INT;
				}
				break;
			
			case 22: // zero
				if(stream.get(c).good())
				{
					if(c == '.')
					{
						Append(c);
						loc.colno++;
						state = 23;
						break;
					}
					
					stream.putback(c);
				}
				
				int_value.sign = sign;
				int_value.abs_value = 0;
				token = TOK_INT;
				break;
			
			case 23: // frac
				if(stream.get(c).good())
				{
					if(isdigit(c))
					{
						Append(c);
						loc.colno++;
						break;
					}
					
					if((c == 'e') || (c == 'E'))
					{
						Append(c);
						loc.colno++;
						state = 24;
						break;
					}
					
					stream.putback(c);
				}

				{
					const char *nptr = text.c_str();
					char *endptr = 0;
					float_value = ::strtod(nptr, &endptr);
					assert((uintptr_t)(endptr - nptr) == text.length());
				}
				token = TOK_FLOAT;
				break;
				
			case 24: // exp with optional signal
				if(stream.get(c).good())
				{
					if((c == '-') || (c == '+') || isdigit(c))
					{
						Append(c);
						loc.colno++;
						state = 25;
						break;
					}
					
					stream.putback(c);
				}
				
				token = TOK_ERROR;
				break;
				
			case 25: // exp
				if(stream.get(c).good())
				{
					if(isdigit(c))
					{
						Append(c);
						loc.colno++;
						break;
					}
					
					stream.putback(c);
				}
				
				{
					const char *nptr = text.c_str();
					char *endptr = 0;
					float_value = ::strtod(nptr, &endptr);
					assert((uintptr_t)(endptr - nptr) == text.length());
				}
				token = TOK_FLOAT;
				break;
				
			case 30: // slash
				if(stream.get(c).good())
				{
					if(enable_c_comments && (c == '*')) // begin of C comment?
					{
						line += c;
						loc.colno++;
						state = 31;
						break;
					}
					if(enable_cpp_comments && (c == '/')) // begin of C++ comment?
					{
						line += c;
						loc.colno++;
						state = 33;
						break;
					}
					break;
				}
				token = TOK_ERROR;
				break;
				
			case 31: // C comment
				if(stream.get(c).good())
				{
					if(c == '*') // star in C comment?
					{
						line += c;
						loc.colno++;
						state = 32; 
						break;
					}
					if(c == '\\') // escape?
					{
						line += c;
						loc.colno++;
						state = 34;
						break;
					}
					if(c == '\r') break;
					if(c == '\n') { loc.lineno++; token_loc.lineno = loc.lineno; token_loc.colno = loc.colno = 1; line.clear(); break; }
					line += c;
					loc.colno++;
					break;
				}
				token = TOK_ERROR;
				break;
				
			case 32: // star in C comment
				if(stream.get(c).good())
				{
					if(c == '/') // end of comment?
					{
						line += c;
						loc.colno++;
						state = 0;
						break;
					}
					if(c == '\r') break;
					if(c == '\n') { loc.lineno++; token_loc.lineno = loc.lineno; token_loc.colno = loc.colno = 1; line.clear(); state = 31; break; }
					line += c;
					loc.colno++;
					state = 31;
					break;
				}
				token = TOK_ERROR;
				break;
			case 33: // C++ comment
				if(stream.get(c).good())
				{
					if(c == '\\') // escape?
					{
						line += c;
						loc.colno++;
						state = 35;
						break;
					}
					if(c == '\r') break;
					if(c == '\n') { loc.lineno++; token_loc.lineno = loc.lineno; token_loc.colno = loc.colno = 1; line.clear(); state = 0; break; }
					line += c;
					loc.colno++;
					break;
				}
				token = TOK_ERROR;
				break;
			case 34: // escape in C comment
				if(stream.get(c).good())
				{
					if(c == '\r') break;
					if(c == '\n') { loc.lineno++; token_loc.lineno = loc.lineno; token_loc.colno = loc.colno = 1; line.clear(); break; }
					line += c;
					loc.colno++;
					state = 31;
					break;
				}
				token = TOK_ERROR;
				break;
			case 35: // escape in C++ comment
				if(stream.get(c).good())
				{
					if(c == '\r') break;
					if(c == '\n') { loc.lineno++; token_loc.lineno = loc.lineno; token_loc.colno = loc.colno = 1; line.clear();  break; }
					line += c;
					loc.colno++;
					state = 33;
					break;
				}
				token = TOK_ERROR;
				break;
		}
	}
	while(!token);
	
	if(token < 0)
	{
		if(token == TOK_ERROR)
		{
			if(eof)
			{
				ScanError(stream, visitor, std::string("unexpected end-of-file"));
			}
			else
			{
				ScanError(stream, visitor, std::string("unexpected character '") + Escape(std::string(1, c)) + "'");
			}
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

template <typename VISITOR>
void JSON_Lexer<VISITOR>::ParseError(std::istream& stream, VISITOR& visitor, const std::string& msg)
{
	Error(stream, visitor, msg, true);
}

template <typename VISITOR>
void JSON_Lexer<VISITOR>::ScanError(std::istream& stream, VISITOR& visitor, const std::string& msg)
{
	Error(stream, visitor, msg, false);
}

template <typename VISITOR>
void JSON_Lexer<VISITOR>::Error(std::istream& stream, VISITOR& visitor, const std::string& msg, bool parse_error)
{
	if(error) return;
	error = true;
	
	std::ostringstream sstr;
	sstr << "At line #" << (parse_error ? token_loc.lineno : loc.lineno) << ", column #" << (parse_error ? token_loc.colno : loc.colno) << ", " << msg << std::endl;
	do
	{
		char c;
		
		if(stream.get(c).eof()) break;
		
		if(stream.bad()) break;
		
		if(c == '\n') break;
		
		if(c == '\r') continue;
			
		line += c;
	}
	while(1);
	look_ahead = TOK_ERROR;
	
	for(unsigned int i = 0; i < line.length(); i++)
	{
		if(line[i] == '\t') sstr << ' '; else sstr << line[i];
	}
	sstr << std::endl;
	for(unsigned int n = (parse_error ? token_loc.colno : loc.colno); n > 1; --n) sstr << ' ';
	sstr << '^' << std::endl;
	visitor.Error(sstr.str());
}

template <typename VISITOR>
JSON_Parser<VISITOR>::JSON_Parser()
{
}

template <typename VISITOR>
JSON_Parser<VISITOR>::~JSON_Parser()
{
}

template <typename VISITOR>
typename VISITOR::JSON_VALUE_TYPE JSON_Parser<VISITOR>::Parse(std::istream& stream, VISITOR& visitor)
{
	JSON_VALUE_TYPE value = ParseValue(stream, visitor);
	if(!value) return JSON_VALUE_TYPE();
	
	Token token = Lexer::Next(stream, visitor);
	if(token != TOK_EOF)
	{
		Lexer::ParseError(stream, visitor, std::string("At ") + ToString(Lexer::GetTokenLocation()) + ", expecting end-of-file, got " + PrettyString(token, Lexer::GetText()));
		visitor.Release(value);
		return JSON_VALUE_TYPE();
	}
	
	return value;
}

template <typename VISITOR>
typename VISITOR::JSON_VALUE_TYPE JSON_Parser<VISITOR>::ParseValue(std::istream& stream, VISITOR& visitor)
{
	Token token = Lexer::Next(stream, visitor);
	if(token < 0) return JSON_VALUE_TYPE();
	
	switch(token)
	{
		case TOK_STRING:
			return visitor.Value(Lexer::GetStringValue(), Lexer::GetTokenLocation());
			
		case TOK_INT:
			return visitor.Value(Lexer::GetIntValue(), Lexer::GetTokenLocation());
			
		case TOK_FLOAT:
			return visitor.Value(Lexer::GetFloatValue(), Lexer::GetTokenLocation());

		case TOK_TRUE:
			return visitor.Value(true, Lexer::GetTokenLocation());

		case TOK_FALSE:
			return visitor.Value(false, Lexer::GetTokenLocation());
			
		case TOK_NULL:
			return visitor.Value(Lexer::GetTokenLocation());
			
		case TOK_LEFT_BRACE:
			return ParseObject(stream, visitor);
			
		case TOK_LEFT_BRACKET:
			return ParseArray(stream, visitor);
			
		default:
			Lexer::ParseError(stream, visitor, std::string("At ") + ToString(Lexer::GetTokenLocation()) + ", unexpected " + PrettyString(token, Lexer::GetText()) + ", expecting a value");
			return JSON_VALUE_TYPE();
	}
	
	return JSON_VALUE_TYPE();
}

template <typename VISITOR>
typename VISITOR::JSON_OBJECT_TYPE JSON_Parser<VISITOR>::ParseObject(std::istream& stream, VISITOR& visitor)
{
	Location object_loc = Lexer::GetTokenLocation();
	visitor.BeginObject();
	
	Token token = Lexer::Next(stream, visitor);
	if(token < 0) return JSON_OBJECT_TYPE();
	
	if(token == TOK_RIGHT_BRACE)
	{
		visitor.EndObject();
		return visitor.Object(object_loc);
	}
	
	Lexer::Back();
	
	Location members_loc = Lexer::GetTokenLocation();
	JSON_MEMBERS_TYPE members = ParseMembers(stream, visitor);
	if(!members)
	{
		Lexer::ParseError(stream, visitor, std::string("At ") + ToString(members_loc) + ", expecting object members");
		return JSON_OBJECT_TYPE();
	}
	
	token = Lexer::Next(stream, visitor);
	
	if(token != TOK_RIGHT_BRACE)
	{
		Lexer::ParseError(stream, visitor, std::string("At ") + ToString(Lexer::GetTokenLocation()) + ", unexpected " + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(TOK_COMMA) + " or " + ToString(TOK_RIGHT_BRACE));
		visitor.Release(members);
		return JSON_OBJECT_TYPE();
	}
	
	visitor.EndObject();
	
	return visitor.Object(members, object_loc);
}

template <typename VISITOR>
typename VISITOR::JSON_MEMBERS_TYPE JSON_Parser<VISITOR>::ParseMembers(std::istream& stream, VISITOR& visitor, JSON_MEMBERS_TYPE _members)
{
	Location member_loc = Lexer::GetTokenLocation();
	JSON_MEMBER_TYPE member = ParseMember(stream, visitor);
	if(!member)
	{
		Lexer::ParseError(stream, visitor, std::string("At ") + ToString(member_loc) + ", expecting an object member");
		visitor.Release(_members);
		return JSON_MEMBERS_TYPE();
	}
	
	JSON_MEMBERS_TYPE members = visitor.Members(_members, member);
	if(!members)
	{
		visitor.Release(member);
		visitor.Release(_members);
		return JSON_MEMBERS_TYPE();
	}
	
	Token token = Lexer::Next(stream, visitor);
	if(token < 0)
	{
		visitor.Release(members);
		return JSON_MEMBERS_TYPE();
	}
	
	if(token == TOK_COMMA)
	{
		visitor.Comma();
		return ParseMembers(stream, visitor, members);
	}

	Lexer::Back();
	
	return members;
}

template <typename VISITOR>
typename VISITOR::JSON_MEMBER_TYPE JSON_Parser<VISITOR>::ParseMember(std::istream& stream, VISITOR& visitor)
{
	Token token = Lexer::Next(stream, visitor);
	if(token < 0) return JSON_MEMBER_TYPE();
	
	if(token != TOK_STRING)
	{
		Lexer::ParseError(stream, visitor, std::string("At ") + ToString(Lexer::GetTokenLocation()) + ", unexpected " + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(TOK_STRING));
		return JSON_MEMBER_TYPE();
	}
	
	Location member_loc = Lexer::GetTokenLocation();
	JSON_STRING_TYPE member_name = visitor.MemberName(Lexer::GetStringValue(), member_loc);
	if(!member_name) return JSON_MEMBER_TYPE();
	
	token = Lexer::Next(stream, visitor);
	if(token < 0)
	{
		visitor.Release(member_name);
		return JSON_MEMBER_TYPE();
	}
	
	if(token != TOK_COLON)
	{
		Lexer::ParseError(stream, visitor, std::string("At ") + ToString(Lexer::GetTokenLocation()) + ", unexpected " + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(TOK_COLON));
		visitor.Release(member_name);
		return JSON_MEMBER_TYPE();
	}
	
	JSON_VALUE_TYPE member_value = ParseValue(stream, visitor);
	if(!member_value)
	{
		Lexer::ParseError(stream, visitor, std::string("At ") + ToString(Lexer::GetTokenLocation()) + ", expecting a member value");
		visitor.Release(member_name);
		return JSON_MEMBER_TYPE();
	}
	
	JSON_MEMBER_TYPE member = visitor.Member(member_name, member_value, member_loc);
	if(!member)
	{
		visitor.Release(member_name);
		visitor.Release(member_value);
		return JSON_MEMBER_TYPE();
	}
	
	return member;
}

template <typename VISITOR>
typename VISITOR::JSON_ARRAY_TYPE JSON_Parser<VISITOR>::ParseArray(std::istream& stream, VISITOR& visitor)
{
	Location array_loc = Lexer::GetTokenLocation();
	visitor.BeginArray();
	
	Token token = Lexer::Next(stream, visitor);
	if(token < 0) return JSON_ARRAY_TYPE();
	
	if(token == TOK_RIGHT_BRACKET)
	{
		visitor.EndArray();
		return visitor.Array(array_loc);
	}
	
	Lexer::Back();
	
	Location elements_loc = Lexer::GetTokenLocation();
	JSON_ELEMENTS_TYPE elements = ParseElements(stream, visitor);
	if(!elements)
	{
		Lexer::ParseError(stream, visitor, std::string("At ") + ToString(elements_loc) + ", expecting array elements");
		return JSON_ARRAY_TYPE();
	}
	
	token = Lexer::Next(stream, visitor);
	
	if(token != TOK_RIGHT_BRACKET)
	{
		Lexer::ParseError(stream, visitor, std::string("At ") + ToString(Lexer::GetTokenLocation()) + ", unexpected " + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_COMMA) + " or " + ToString(unisim::util::json::TOK_RIGHT_BRACKET));
		visitor.Release(elements);
		return JSON_ARRAY_TYPE();
	}
	
	visitor.EndArray();
	
	return visitor.Array(elements, array_loc);
}

template <typename VISITOR>
typename VISITOR::JSON_ELEMENTS_TYPE JSON_Parser<VISITOR>::ParseElements(std::istream& stream, VISITOR& visitor, JSON_ELEMENTS_TYPE _elements)
{
	Location value_loc = Lexer::GetTokenLocation();
	JSON_VALUE_TYPE value = ParseValue(stream, visitor);
	if(!value)
	{
		Lexer::ParseError(stream, visitor, std::string("At ") + ToString(value_loc) + ", expecting an array element");
		visitor.Release(_elements);
		return JSON_ELEMENTS_TYPE();
	}
	
	JSON_ELEMENTS_TYPE elements = visitor.Elements(_elements, value);
	if(!elements)
	{
		visitor.Release(value);
		visitor.Release(_elements);
		return JSON_ELEMENTS_TYPE();
	}
	
	Token token = Lexer::Next(stream, visitor);
	if(token < 0)
	{
		visitor.Release(elements);
		return JSON_ELEMENTS_TYPE();
	}
	
	if(token == TOK_COMMA)
	{
		visitor.Comma();
		return ParseElements(stream, visitor, elements);
	}
	
	Lexer::Back();
	
	return elements;
}

} // end of namespace json
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_JSON_JSON_TCC__
