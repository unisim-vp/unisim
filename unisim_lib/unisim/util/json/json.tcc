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

namespace unisim {
namespace util {
namespace json {

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
	Token token = Next(stream);
	if(token < 0) return false;
	
	switch(token)
	{
		case TOK_STRING:
			visitor.Value(GetStringValue());
			return true;
			
		case TOK_INT:
			visitor.Value(GetIntValue());
			return true;
			
		case TOK_UINT:
			visitor.Value(GetUIntValue());
			return true;

		case TOK_FLOAT:
			visitor.Value(GetFloatValue());
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
			Back();
			return ParseObject(stream, visitor);
			
		case TOK_LEFT_BRACKET:
			Back();
			return ParseArray(stream, visitor);
	}
	
	visitor.Error(std::string("unexpected '") + GetText() + "'");
	
	return false;
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::ParseObject(std::istream& stream, VISITOR& visitor)
{
	Token token = Next(stream);
	if(token < 0) return false;
	
	if(token != TOK_LEFT_BRACE) return false;
	
	visitor.BeginObject();
	
	token = Next(stream);
	if(token < 0) return false;
	
	if(token == TOK_RIGHT_BRACE)
	{
		visitor.EndObject();
		return true;
	}
	
	Back();
	
	if(!ParseMembers(stream, visitor)) return false;
	
	token = Next(stream);
	
	if(token == TOK_RIGHT_BRACE)
	{
		visitor.EndObject();
		return true;
	}

	return false;
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::ParseMembers(std::istream& stream, VISITOR& visitor)
{
	if(!ParseMember(stream, visitor)) return false;
	
	Token token = Next(stream);
	if(token < 0) return false;
	
	if(token == TOK_COMMA)
	{
		visitor.Comma();
		return ParseMembers(stream, visitor);
	}

	Back();
	
	return true;
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::ParseMember(std::istream& stream, VISITOR& visitor)
{
	Token token = Next(stream);
	if(token < 0) return false;
	
	if(token != TOK_STRING) return false;
	
	visitor.Member(GetStringValue());
	
	token = Next(stream);
	if(token < 0) return false;
	
	if(token != TOK_COLON) return false;
	
	return ParseValue(stream, visitor);
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::ParseArray(std::istream& stream, VISITOR& visitor)
{
	Token token = Next(stream);
	if(token < 0) return false;
	
	if(token != TOK_LEFT_BRACKET) return false;

	visitor.BeginArray();
	
	token = Next(stream);
	if(token < 0) return false;
	
	if(token == TOK_RIGHT_BRACKET)
	{
		visitor.EndArray();
		return true;
	}
	
	Back();
	
	if(!ParseElements(stream, visitor)) return false;
	
	token = Next(stream);
	
	if(token == TOK_RIGHT_BRACKET)
	{
		visitor.EndArray();
		return true;
	}

	return false;
}

template <class VISITOR>
bool JSON_Parser<VISITOR>::ParseElements(std::istream& stream, VISITOR& visitor)
{
	if(!ParseValue(stream, visitor)) return false;
	
	Token token = Next(stream);
	if(token < 0) return false;
	
	if(token == TOK_COMMA)
	{
		visitor.Comma();
		return ParseElements(stream, visitor);
	}
	
	Back();
	
	return true;
}

} // end of namespace json
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_JSON_JSON_TCC__
