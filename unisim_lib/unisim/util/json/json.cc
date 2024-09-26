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

#include <unisim/util/json/json.tcc>

namespace unisim {
namespace util {
namespace json {

JSON_Value undefined = JSON_Value(Location(0, 0));

JSON_Value null = JSON_Null(Location(0, 0));

std::string Escape(const std::string& s)
{
	std::string r;
	for(std::size_t i = 0; i < s.length(); ++i)
	{
		char c = s[i];
		switch(c)
		{
			case '\"': r += "\\\""; break;
			case '\\': r += "\\\\"; break;
			case '\r': r += "\\r"; break;
			case '\n': r += "\\n"; break;
			case '\t': r += "\\t"; break;
			case '\b': r += "\\b"; break;
			default: r += c; break;
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
		case TOK_INT          : os << "an integer number"; break;
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
	
	return os;
}

std::string PrettyString(const Token& token, const std::string& text)
{
	switch(token)
	{
		case TOK_IO_ERROR     : break;
		case TOK_ERROR        : break;
		case TOK_VOID         : break;
		case TOK_STRING       : return std::string("string \"") + Escape(text) + "\""; break;
		case TOK_INT          : return std::string("number ") + text; break;
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

std::ostream& operator << (std::ostream& os, const JSON_ValueType& type)
{
	switch(type)
	{
		case JSON_UNDEFINED: os << "undefined"; break;
		case JSON_STRING   : os << "string"; break;
		case JSON_INT      : os << "integer"; break;
		case JSON_FLOAT    : os << "float"; break;
		case JSON_OBJECT   : os << "object"; break;
		case JSON_ARRAY    : os << "array"; break;
		case JSON_BOOLEAN  : os << "boolean"; break;
		case JSON_NULL     : os << "null"; break;
		default            : os << "value of unknown type"; break;
	}
	return os;
}

std::ostream& operator << (std::ostream& os, const JSON_Value& value)
{
	switch(value.GetType())
	{
		case JSON_UNDEFINED: os << "undefined"; break;
		case JSON_STRING   : os << value.AsString(); break;
		case JSON_INT      : os << value.AsInteger(); break;
		case JSON_FLOAT    : os << value.AsFloat(); break;
		case JSON_OBJECT   : os << value.AsObject(); break;
		case JSON_ARRAY    : os << value.AsArray(); break;
		case JSON_BOOLEAN  : os << value.AsBoolean(); break;
		case JSON_NULL     : os << value.AsNull(); break;
		default            : break;
	}
	return os;
}

std::ostream& operator << (std::ostream& stream, const Location& loc)
{
	if(loc.lineno)
	{
		stream << "line #" << loc.lineno;
		if(loc.colno) stream << ", column #" << loc.colno;
	}
	else
	{
		stream << "undefined location";
	}
	return stream;
}

JSON_Value *JSON_Object::Clone() const
{
	JSON_Object *obj = new JSON_Object(GetLocation());
	for(Members::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		obj->Add(new JSON_Member((*it).second.GetName(), (*it).second.Clone(), (*it).second.GetLocation()));
	}
	return obj;
}

JSON_Object::KeysType JSON_Object::Keys() const
{
	KeysType keys;
	keys.reserve(members.size());
	for(typename Members::const_iterator it = members.begin(); it != members.end(); ++it) keys.push_back((*it).second.GetName());
	return keys;
}

JSON_MemberRef JSON_Object::operator [] (const char *member_name)
{
	JSON_Member *member = FindMember(member_name);
	if(member) return JSON_MemberRef(member);
	member = new JSON_Member(member_name);
	Add(member);
	return JSON_MemberRef(member);
}

JSON_MemberConstRef JSON_Object::operator [] (const char *member_name) const
{
	const JSON_Member *member = FindMember(member_name);
	return member ? JSON_MemberConstRef(member) : JSON_MemberConstRef(new JSON_Member(member_name, &undefined));
}

JSON_Member *JSON_Object::FindMember(const char *member_name)
{
	JSON_Object::Members::iterator it = members.find(member_name);
	return (it != members.end()) ? &(JSON_Member&)(*it).second : 0;
}

const JSON_Member *JSON_Object::FindMember(const char *member_name) const
{
	JSON_Object::Members::const_iterator it = members.find(member_name);
	return (it != members.end()) ? &(const JSON_Member&)(*it).second : 0;
}

JSON_Object& JSON_Object::Add(JSON_Member *member)
{
	JSON_Object::Members::iterator it = members.find(member->GetName());
	if(it != members.end())
	{
		(*it).second = member;
	}
	else
	{
		members.insert(JSON_Object::Members::value_type(member->GetName(), member));
	}
	return *this;
}

std::ostream& operator << (std::ostream& os, const JSON_Object& object)
{
	os << '{';
	const JSON_Object::Members& members = object.GetMembers();
	JSON_Object::Members::const_iterator it = members.begin();
	while(it != members.end())
	{
		const JSON_Member& member = (*it).second;
		os << member;
		if(++it != members.end()) os << ',';
	}
	return os << '}';
}

JSON_Value *JSON_Array::Clone() const
{
	JSON_Array *array = new JSON_Array(GetLocation());
	for(Elements::const_iterator it = elements.begin(); it != elements.end(); ++it) array->Push((*it).Clone());
	return array;
}

std::ostream& operator << (std::ostream& os, const JSON_Array& array)
{
	os << '[';
	const JSON_Array::Elements& elements = array.GetElements();
	JSON_Array::Elements::const_iterator it = elements.begin();
	while(it != elements.end())
	{
		const JSON_Value& value = *it;
		os << value;
		if(++it != elements.end()) os << ',';
	}
	return os << ']';
}

JSON_AST_Printer::Visitor::Visitor(std::ostream& _stream, const JSON_Value& _root)
	: stream(_stream)
	, root(_root)
	, stack()
{
}

bool JSON_AST_Printer::Visitor::Visit(const JSON_Value& value)
{
	if(GetIndex() != 0) stream << ",";
	stream << value;
	NextIndex();
	return false;
}

bool JSON_AST_Printer::Visitor::Visit(const JSON_Object& object)
{
	if(GetIndex() != 0) stream << ",";
	stream << '{';
	Push();
	object.Scan(*this);
	Pop();
	stream << '}';
	NextIndex();
	return false;
}

bool JSON_AST_Printer::Visitor::Visit(const JSON_Member& member)
{
	if(GetIndex() != 0) stream << ",";
	stream << '"' << Escape(member.GetName()) << "\":";
	Push();
	member.Scan(*this);
	Pop();
	NextIndex();
	return false;
}

bool JSON_AST_Printer::Visitor::Visit(const JSON_Array& array)
{
	if(GetIndex() != 0) stream << ",";
	stream << '[';
	Push();
	array.Scan(*this);
	Pop();
	std::cout << ']';
	NextIndex();
	return false;
}

void JSON_AST_Printer::Visitor::Push()
{
	stack.push_back(0);
}

void JSON_AST_Printer::Visitor::Pop()
{
	stack.pop_back();
}

unsigned int JSON_AST_Printer::Visitor::GetIndex() const
{
	return stack.back();
}

void JSON_AST_Printer::Visitor::NextIndex()
{
	++stack.back();
}

void JSON_AST_Printer::Print(std::ostream& stream, const JSON_Value& root)
{
	Visitor visitor(stream, root);
	visitor.Push();
	root.Visit(visitor);
}

} // end of namespace json
} // end of namespace util
} // end of namespace unisim
