/*
 *  Copyright (c) 2013,
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

#include <unisim/util/debug/type.hh>
#include <iostream>
#include <sstream>
#include <queue>
#include <cassert>
#include <cstring>

namespace unisim {
namespace util {
namespace debug {

enum
{
	T_BASE      = T_CHAR | T_INTEGER | T_FLOAT | T_BOOL,
	T_COMPOSITE = T_STRUCT | T_UNION | T_CLASS | T_INTERFACE,
	T_NAMED     = T_BASE | T_COMPOSITE | T_TYPEDEF | T_ENUM
};

// reorder_qualifiers: rewriting "const volatile" into "volatile const"
std::string reorder_qualifiers(const std::string& s)
{
	std::string r(s);
	std::size_t pos = 0;
	do
	{
		std::size_t fpos = s.find("const volatile", pos);
		if(fpos == std::string::npos) break;
		r.replace(fpos, 14, "volatile const");
		pos = fpos + 14;
	}
	while(pos < s.length());
	return r;
}

// kill_array_subscript: rewriting without array subscripts
std::string kill_array_subscripts(const std::string& s)
{
	std::string r;
	std::size_t pos = 0;
	std::size_t len = s.length();
	while(pos < len)
	{
		char c = s[pos++];
		if(c == '[')
		{
			r += "[]";
			std::size_t right_bracket_pos = s.find_first_of(']', pos);
			assert(right_bracket_pos != std::string::npos);
			pos = right_bracket_pos + 1;
		}
		else
		{
			r += c;
		}
	}
	
	return r;
}

Type::Type()
	: type_class(T_UNKNOWN)
	, decl_location(0)
	, c_decl()
	, ref_count(0)
{
}

Type::Type(const DeclLocation *_decl_location)
	: type_class(T_UNKNOWN)
	, decl_location(_decl_location)
	, c_decl()
	, ref_count(0)
{
	if(decl_location) decl_location->Catch();
}

Type::Type(TYPE_CLASS _type_class)
	: type_class(_type_class)
	, decl_location(0)
	, c_decl()
	, ref_count(0)
{
}

Type::Type(TYPE_CLASS _type_class, const DeclLocation *_decl_location)
	: type_class(_type_class)
	, decl_location(_decl_location)
	, c_decl()
	, ref_count(0)
{
	if(decl_location) decl_location->Catch();
}

Type::~Type()
{
	if(decl_location) decl_location->Release();
	for(int i = 0; i < 2; ++i) if(c_decl[i]) delete c_decl[i];
}

TYPE_CLASS Type::GetClass() const
{
	return type_class;
}

bool Type::IsComposite() const
{
	return type_class & T_COMPOSITE; // IsStructure() || IsUnion() || IsClass() || IsInterface();
}

bool Type::IsStructure() const
{
	return type_class == unisim::util::debug::T_STRUCT;
}

bool Type::IsUnion() const
{
	return type_class == unisim::util::debug::T_UNION;
}

bool Type::IsClass() const
{
	return type_class == unisim::util::debug::T_CLASS;
}

bool Type::IsInterface() const
{
	return type_class == unisim::util::debug::T_INTERFACE;
}

bool Type::IsBase() const
{
	return type_class & T_BASE; // IsInteger() || IsChar() || IsFloat() || IsBoolean();
}

bool Type::IsChar() const
{
	return type_class == unisim::util::debug::T_CHAR;
}

bool Type::IsInteger() const
{
	return type_class == unisim::util::debug::T_INTEGER;
}

bool Type::IsBoolean() const
{
	return type_class == unisim::util::debug::T_BOOL;
}

bool Type::IsFloat() const
{
	return type_class == unisim::util::debug::T_FLOAT;
}

bool Type::IsPointer() const
{
	return type_class == unisim::util::debug::T_POINTER;
}

bool Type::IsTypedef() const
{
	return type_class == unisim::util::debug::T_TYPEDEF;
}

bool Type::IsFunction() const
{
	return type_class == unisim::util::debug::T_FUNCTION;
}

bool Type::IsArray() const
{
	return type_class == unisim::util::debug::T_ARRAY;
}

bool Type::IsConst() const
{
	return type_class == unisim::util::debug::T_CONST;
}

bool Type::IsEnum() const
{
	return type_class == unisim::util::debug::T_ENUM;
}

bool Type::IsUnspecified() const
{
	return type_class == unisim::util::debug::T_VOID;
}

bool Type::IsVolatile() const
{
	return type_class == unisim::util::debug::T_VOLATILE;
}

bool Type::IsNamed() const
{
	return type_class & T_NAMED;// IsBase() || IsComposite() || IsTypedef() || IsEnum();
}

const DeclLocation *Type::GetDeclLocation() const
{
	return decl_location;
}

std::string Type::BuildCDecl(char const **identifier, bool collapsed) const
{
	return std::string("void");
}

const std::string& Type::GetCDecl(bool collapsed) const
{
	std::string *_c_decl = c_decl[collapsed];
	return *(_c_decl ? _c_decl : (c_decl[collapsed] = new std::string(reorder_qualifiers(BuildCDecl(0, collapsed)))));
}

void Type::Catch() const
{
	++ref_count;
}

void Type::Release() const
{
	if(ref_count)
	{
		if(--ref_count == 0)
		{
			delete this;
		}
	}
}

const CharType& Type::AsChar() const                   { assert(IsChar()); return *(const CharType *) this; }
const IntegerType& Type::AsInteger() const             { assert(IsInteger()); return *(const IntegerType *) this; }
const FloatingPointType& Type::AsFloat() const         { assert(IsFloat()); return *(const FloatingPointType *) this; }
const BooleanType& Type::AsBoolean() const             { assert(IsBoolean()); return *(const BooleanType *) this; }
const CompositeType& Type::AsComposite() const         { assert(IsComposite()); return *(const CompositeType *) this; }
const StructureType& Type::AsStructure() const         { assert(IsStructure()); return *(const StructureType *) this; }
const UnionType& Type::AsUnion() const                 { assert(IsUnion()); return *(const UnionType *) this; }
const ClassType& Type::AsClass() const                 { assert(IsClass()); return *(const ClassType *) this; }
const InterfaceType& Type::AsInterface() const         { assert(IsInterface()); return *(const InterfaceType *) this; }
const PointerType& Type::AsPointer() const             { assert(IsPointer()); return *(const PointerType *) this; }
const Typedef& Type::AsTypedef() const                 { assert(IsTypedef()); return *(const Typedef *) this; }
const FunctionType& Type::AsFunction() const           { assert(IsFunction()); return *(const FunctionType *) this; }
const ArrayType& Type::AsArray() const                 { assert(IsArray()); return *(const ArrayType *) this; }
const EnumType& Type::AsEnum() const                   { assert(IsEnum()); return *(const EnumType *) this; }
const UnspecifiedType& Type::AsUnspecifiedType() const { assert(IsUnspecified()); return *(const UnspecifiedType *) this; }
const VolatileType& Type::AsVolatile() const           { assert(IsVolatile()); return *(const VolatileType *) this; }
const ConstType& Type::AsConst() const                 { assert(IsConst()); return *(const ConstType *) this; }

CharType& Type::AsChar()                   { assert(IsChar()); return *(CharType *) this; }
IntegerType& Type::AsInteger()             { assert(IsInteger()); return *(IntegerType *) this; }
FloatingPointType& Type::AsFloat()         { assert(IsFloat()); return *(FloatingPointType *) this; }
BooleanType& Type::AsBoolean()             { assert(IsBoolean()); return *(BooleanType *) this; }
CompositeType& Type::AsComposite()         { assert(IsComposite()); return *(CompositeType *) this; }
StructureType& Type::AsStructure()         { assert(IsStructure()); return *(StructureType *) this; }
UnionType& Type::AsUnion()                 { assert(IsUnion()); return *(UnionType *) this; }
ClassType& Type::AsClass()                 { assert(IsClass()); return *(ClassType *) this; }
InterfaceType& Type::AsInterface()         { assert(IsInterface()); return *(InterfaceType *) this; }
PointerType& Type::AsPointer()             { assert(IsPointer()); return *(PointerType *) this; }
Typedef& Type::AsTypedef()                 { assert(IsTypedef()); return *(Typedef *) this; }
FunctionType& Type::AsFunction()           { assert(IsFunction()); return *(FunctionType *) this; }
ArrayType& Type::AsArray()                 { assert(IsArray()); return *(ArrayType *) this; }
EnumType& Type::AsEnum()                   { assert(IsEnum()); return *(EnumType *) this; }
UnspecifiedType& Type::AsUnspecifiedType() { assert(IsUnspecified()); return *(UnspecifiedType *) this; }
VolatileType& Type::AsVolatile()           { assert(IsVolatile()); return *(VolatileType *) this; }
ConstType& Type::AsConst()                 { assert(IsConst()); return *(ConstType *) this; }

std::ostream& operator << (std::ostream& os, const Type& type)
{
	return os << type.GetCDecl();
}

NamedType::NamedType(TYPE_CLASS _type_class, const char *_name)
	: Type(_type_class)
	, name(_name ? _name : "")
	, has_name(_name != 0)
{
}

NamedType::NamedType(TYPE_CLASS _type_class, const char *_name, const DeclLocation *_decl_location)
	: Type(_type_class, _decl_location)
	, name(_name ? _name : "")
	, has_name(_name != 0)
{
}

const std::string& NamedType::GetName() const
{
	return name;
}

bool NamedType::HasName() const
{
	return has_name;
}

BaseType::BaseType(const char *_name, unsigned int _bit_size)
	: NamedType(T_UNKNOWN, _name)
	, bit_size(_bit_size)
{
}

BaseType::BaseType(const char *_name, unsigned int _bit_size, const DeclLocation *_decl_location)
	: NamedType(T_UNKNOWN, _name, _decl_location)
	, bit_size(_bit_size)
{
}

BaseType::BaseType(TYPE_CLASS _type_class, const char *_name, unsigned int _bit_size)
	: NamedType(_type_class, _name)
	, bit_size(_bit_size)
{
}

BaseType::BaseType(TYPE_CLASS _type_class, const char *_name, unsigned int _bit_size, const DeclLocation *_decl_location)
	: NamedType(_type_class, _name, _decl_location)
	, bit_size(_bit_size)
{
}

BaseType::~BaseType()
{
}

unsigned int BaseType::GetBitSize() const
{
	return bit_size;
}

IntegerType::IntegerType(const char *_name, unsigned int _bit_size, bool _is_signed)
	: BaseType(T_INTEGER, _name, _bit_size)
	, is_signed(_is_signed)
{
}

IntegerType::IntegerType(const char *_name, unsigned int _bit_size, bool _is_signed, const DeclLocation *_decl_location)
	: BaseType(T_INTEGER, _name, _bit_size, _decl_location)
	, is_signed(_is_signed)
{
}

IntegerType::~IntegerType()
{
}

bool IntegerType::IsSigned() const
{
	return is_signed;
}

std::string IntegerType::BuildCDecl(char const **identifier, bool collapsed) const
{
	std::stringstream sstr;
	const std::string& name = GetName();
	if(name.empty())
	{
		sstr << (is_signed ? "s" : "u") << "int";
		uint64_t bit_size = GetBitSize();
		if(bit_size) sstr << bit_size;
	}
	else
	{
		sstr << name;
	}
	return sstr.str();
}

CharType::CharType(const char *_name, unsigned int _bit_size, bool _is_signed)
	: BaseType(T_CHAR, _name, _bit_size)
	, is_signed(_is_signed)
{
}

CharType::CharType(const char *_name, unsigned int _bit_size, bool _is_signed, const DeclLocation *_decl_location)
	: BaseType(T_CHAR, _name, _bit_size, _decl_location)
	, is_signed(_is_signed)
{
}

CharType::~CharType()
{
}

bool CharType::IsSigned() const
{
	return is_signed;
}

std::string CharType::BuildCDecl(char const **identifier, bool collapsed) const
{
	std::stringstream sstr;
	const std::string& name = GetName();
	if(name.empty())
	{
		sstr << (is_signed ? "s" : "u") << "char";
		uint64_t bit_size = GetBitSize();
		
		if(bit_size) sstr << bit_size;
	}
	else
	{
		sstr << name;
	}
	return sstr.str();
}

FloatingPointType::FloatingPointType(const char *_name, unsigned int bit_size)
	: BaseType(T_FLOAT, _name, bit_size)
{
}

FloatingPointType::FloatingPointType(const char *_name, unsigned int bit_size, const DeclLocation *_decl_location)
	: BaseType(T_FLOAT, _name, bit_size, _decl_location)
{
}

FloatingPointType::~FloatingPointType()
{
}

std::string FloatingPointType::BuildCDecl(char const **identifier, bool collapsed) const
{
	std::stringstream sstr;
	const std::string& name = GetName();
	if(name.empty())
	{
		sstr << "float";
		uint64_t bit_size = GetBitSize();
		if(bit_size) sstr << bit_size;
	}
	else
	{
		sstr << name;
	}
	
	return sstr.str();
}

BooleanType::BooleanType(const char *_name, unsigned int _bit_size)
	: BaseType(T_BOOL, _name, _bit_size)
{
}

BooleanType::BooleanType(const char *_name, unsigned int _bit_size, const DeclLocation *_decl_location)
	: BaseType(T_BOOL, _name, _bit_size, _decl_location)
{
}

BooleanType::~BooleanType()
{
}

std::string BooleanType::BuildCDecl(char const **identifier, bool collapsed) const
{
	std::stringstream sstr;
	const std::string& name = GetName();
	if(name.empty())
	{
		sstr << "bool";
		uint64_t bit_size = GetBitSize();
		if(bit_size) sstr << bit_size;
	}
	else
	{
		sstr << name;
	}
	return sstr.str();
}

Member::Member(const char *_name, const Type *_type, uint64_t _bit_size)
	: name(_name ? _name : "")
	, has_name(_name != 0)
	, type(_type)
	, bit_size(_bit_size)
{
	if(type)
	{
		type->Catch();
	}
}

Member::~Member()
{
	if(type)
	{
		type->Release();
	}
}

const char *Member::GetName() const
{
	return name.c_str();
}

bool Member::HasName() const
{
	return has_name;
}

const Type *Member::GetType() const
{
	return type;
}

uint64_t Member::GetBitSize() const
{
	return bit_size;
}

std::string Member::BuildCDecl() const
{
	std::stringstream sstr;
	char const *member_name = name.c_str();
	std::string s(type->BuildCDecl(&member_name, true));
	sstr << s;
	if(member_name)
	{
		if(!s.empty() && (s.back() != ' ') && (s.back() != '*')) sstr << " ";
		sstr << member_name;
	}
	if(bit_size)
	{
		sstr << ":" << bit_size;
	}
	return sstr.str();
}

CompositeType::CompositeType(TYPE_CLASS _type_class, const char *_name, bool _incomplete)
	: NamedType(_type_class, _name)
	, incomplete(_incomplete)
	, members()
{
}

CompositeType::CompositeType(TYPE_CLASS _type_class, const char *_name, bool _incomplete, const DeclLocation *_decl_location)
	: NamedType(_type_class, _name, _decl_location)
	, incomplete(_incomplete)
	, members()
{
}

CompositeType::~CompositeType()
{
	unsigned int member_count = members.size();
	unsigned int i;
	for(i = 0; i < member_count; i++)
	{
		delete members[i];
	}
}

void CompositeType::Add(const Member *_member)
{
	members.push_back(_member);
}

bool CompositeType::IsIncomplete() const
{
	return incomplete;
}

unsigned int CompositeType::GetMemberCount() const
{
	return members.size();
}

const Member *CompositeType::GetMember(unsigned int _idx) const
{
	unsigned int member_count = members.size();
	
	return (_idx < member_count) ? members[_idx] : 0;
}

bool CompositeType::HasMember(const char *member_name) const
{
	unsigned int member_count = members.size();
	unsigned int i;
	for(i = 0; i < member_count; i++)
	{
		const Member *member = members[i];
		if(member->HasName())
		{
			if(::strcmp(member_name, member->GetName()) == 0) return true;
		}
	}
	return false;
}

std::string CompositeType::BuildCDecl(char const **identifier, bool collapsed) const
{
	std::stringstream sstr;
	
	switch(GetClass())
	{
		case T_STRUCT:
			sstr << "struct";
			break;
		case T_UNION:
			sstr << "union";
			break;
		case T_CLASS:
			sstr << "class";
			break;
		case T_INTERFACE:
			sstr << "interface";
			break;
		default:
			assert(false);
	}
	if(HasName())
	{
		sstr << " " << GetName();
	}
	if((!collapsed || !HasName()) && !IsIncomplete())
	{
		sstr << " { ";
		unsigned int member_count = members.size();
		if(member_count)
		{
			unsigned int i;
			for(i = 0; i < member_count; i++)
			{
				Member const *member = members[i];
				Type const *member_type = member->GetType();
				if(member_type->GetClass() == T_ARRAY)
				{
					ArrayType const *array_type = dynamic_cast<ArrayType const *>(member_type);
					if((array_type->GetCount() == 0) && ((GetClass() != T_STRUCT) || (i != (member_count - 1))))
					{
						 continue;
					}
				}
				sstr << member->BuildCDecl() << "; ";
			}
		}
		sstr << "}";
	}
	return sstr.str();
}

ArrayType::ArrayType(const Type *_type_of_element, unsigned int _order, int64_t _lower_bound, int64_t _upper_bound)
	: Type(T_ARRAY)
	, type_of_element(_type_of_element)
	, order(_order)
	, lower_bound(_lower_bound)
	, upper_bound(_upper_bound)
{
	if(type_of_element)
	{
		type_of_element->Catch();
	}
}

ArrayType::ArrayType(const Type *_type_of_element, unsigned int _order, int64_t _lower_bound, int64_t _upper_bound, const DeclLocation *_decl_location)
	: Type(T_ARRAY, _decl_location)
	, type_of_element(_type_of_element)
	, order(_order)
	, lower_bound(_lower_bound)
	, upper_bound(_upper_bound)
{
	if(type_of_element)
	{
		type_of_element->Catch();
	}
}

ArrayType::~ArrayType()
{
	if(type_of_element)
	{
		type_of_element->Release();
	}
}

const Type *ArrayType::GetTypeOfElement() const
{
	return type_of_element;
}

unsigned int ArrayType::GetOrder() const
{
	return order;
}

int64_t ArrayType::GetLowerBound() const
{
	return lower_bound;
}

int64_t ArrayType::GetUpperBound() const
{
	return upper_bound;
}

int64_t ArrayType::GetCount() const
{
	return upper_bound - lower_bound + 1;
}

std::string ArrayType::BuildCDecl(char const **identifier, bool collapsed) const
{
	std::stringstream sstr;
	if(identifier && (*identifier))
	{
		sstr << (*identifier);
		*identifier = 0;
	}
	const ArrayType *a = 0;
	const Type *t = this; 
	do
	{
		a = (const ArrayType *) t;
		if(a->lower_bound <= a->upper_bound)
		{
			if(a->lower_bound)
				sstr << '[' << a->lower_bound << ".." << a->upper_bound << ']';
			else
				sstr << '[' << a->upper_bound + 1 << ']';
		}
		else
		{
			sstr << "[]";
		}
	}
	while((t = a->type_of_element)->GetClass() == T_ARRAY);
	std::string s(sstr.str());
	char const *s_cstr = s.c_str();
	
	std::stringstream sstr2;
	sstr2 << t->BuildCDecl(&s_cstr, true);
	if(s_cstr)
	{
		sstr2 << " " << s;
	}
	
	return sstr2.str();
}

PointerType::PointerType(const Type *_type_of_dereferenced_object)
	: Type(T_POINTER)
	, type_of_dereferenced_object(_type_of_dereferenced_object)
{
	if(type_of_dereferenced_object)
	{
		type_of_dereferenced_object->Catch();
	}
}

PointerType::PointerType(const Type *_type_of_dereferenced_object, const DeclLocation *_decl_location)
	: Type(T_POINTER, _decl_location)
	, type_of_dereferenced_object(_type_of_dereferenced_object)
{
	if(type_of_dereferenced_object)
	{
		type_of_dereferenced_object->Catch();
	}
}

PointerType::~PointerType()
{
	if(type_of_dereferenced_object)
	{
		type_of_dereferenced_object->Release();
	}
}

const Type *PointerType::GetTypeOfDereferencedObject() const
{
	return type_of_dereferenced_object;
}

std::string PointerType::BuildCDecl(char const **identifier, bool collapsed) const
{
	const Type *nested_type = type_of_dereferenced_object;
	do
	{
		if(nested_type->GetClass() == T_FUNCTION) break;
		if(nested_type->GetClass() == T_ARRAY) break;
		if(nested_type->GetClass() == T_CONST)
		{
			nested_type = static_cast<const ConstType *>(nested_type)->GetType();
		}
		if(nested_type->GetClass() == T_VOLATILE)
		{
			nested_type = static_cast<const VolatileType *>(nested_type)->GetType();
		}
		else
		{
			break;
		}
	}
	while(1);
	
	std::stringstream sstr;
	if((nested_type->GetClass() == T_FUNCTION) || (nested_type->GetClass() == T_ARRAY))
	{
		std::string s("(*");
		if(identifier && (*identifier))
		{
			s += (*identifier);
		}
		s += ')';
		char const *s_cstr = s.c_str();
		sstr << type_of_dereferenced_object->BuildCDecl(&s_cstr);
		if(identifier && (*identifier) && !s_cstr)
		{
			*identifier = 0;
		}
	}
	else
	{
		std::string s(type_of_dereferenced_object->GetCDecl(true));
		sstr << s;
		if(!s.empty() && (s.back() != '*') && (s.back() != ' ')) sstr << " ";
		sstr << "*";
	}
	return sstr.str();
}

Typedef::Typedef(const Type *_type, const char *_name)
	: NamedType(T_TYPEDEF, _name)
	, type(_type)
{
	if(type)
	{
		type->Catch();
	}
}

Typedef::Typedef(const Type *_type, const char *_name, const DeclLocation *_decl_location)
	: NamedType(T_TYPEDEF, _name, _decl_location)
	, type(_type)
{
	if(type)
	{
		type->Catch();
	}
}

Typedef::~Typedef()
{
	if(type)
	{
		type->Release();
	}
}

const Type *Typedef::GetType() const
{
	return type;
}

std::string Typedef::BuildCDecl(char const **identifier, bool collapsed) const
{
	if(collapsed)
	{
		return std::string(GetName());
	}
	else
	{
		std::stringstream sstr;
		sstr << "typedef ";
		char const *typedef_name = GetName().c_str();
		sstr << type->BuildCDecl(&typedef_name, true);
		if(typedef_name)
		{
			sstr << " " << typedef_name;
		}
		return sstr.str();
	}
}

FormalParameter::FormalParameter(const char *_name, const Type *_type)
	: name(_name)
	, type(_type)
{
	if(type)
	{
		type->Catch();
	}
}

FormalParameter::~FormalParameter()
{
	if(type)
	{
		type->Release();
	}
}

const char *FormalParameter::GetName() const
{
	return name.c_str();
}

const Type *FormalParameter::GetType() const
{
	return type;
}

FunctionType::FunctionType(const Type *_return_type)
	: Type(T_FUNCTION)
	, return_type(_return_type)
	, formal_params()
{
	if(return_type)
	{
		return_type->Catch();
	}
}

FunctionType::FunctionType(const Type *_return_type, const DeclLocation *_decl_location)
	: Type(T_FUNCTION, _decl_location)
	, return_type(_return_type)
	, formal_params()
{
	if(return_type)
	{
		return_type->Catch();
	}
}

FunctionType::~FunctionType()
{
	unsigned int formal_param_count = formal_params.size();
	unsigned int i;
	for(i = 0; i < formal_param_count; i++)
	{
		delete formal_params[i];
	}
	if(return_type)
	{
		return_type->Release();
	}
}

void FunctionType::Add(const FormalParameter *formal_param)
{
	formal_params.push_back(formal_param);
}

const Type *FunctionType::GetReturnType() const
{
	return return_type;
}

std::string FunctionType::BuildCDecl(char const **identifier, bool collapsed) const
{
	std::stringstream sstr;
	
	if(return_type)
	{
		std::string s(return_type->GetCDecl(true));
		sstr << s;
		if(!s.empty() && (s.back() != ' ') && (s.back() != '*')) sstr << " ";
	}
	else
	{
		sstr << "void ";
	}
	if(identifier && (*identifier))
	{
		sstr << (*identifier);
		*identifier = 0;
	}
	sstr << "(";
	unsigned int formal_param_count = formal_params.size();
	if(formal_param_count)
	{
		unsigned int i;
		for(i = 0; i < formal_param_count; i++)
		{
			sstr << formal_params[i]->GetType()->GetCDecl(true);
			if(i != (formal_param_count - 1)) sstr << ", ";
		}
	}
	sstr << ")";
	return sstr.str();
}

ConstType::ConstType(const Type *_type)
	: Type(T_CONST)
	, type(_type)
{
	if(type)
	{
		type->Catch();
	}
}

ConstType::ConstType(const Type *_type, const DeclLocation *_decl_location)
	: Type(T_CONST, _decl_location)
	, type(_type)
{
	if(type)
	{
		type->Catch();
	}
}

ConstType::~ConstType()
{
	if(type)
	{
		type->Release();
	}
}

const Type *ConstType::GetType() const
{
	return type;
}

std::string ConstType::BuildCDecl(char const **identifier, bool collapsed) const
{
	if(type->GetClass() != T_ARRAY)
	{
		std::stringstream sstr;
	
		if(identifier && (*identifier))
		{
			std::string s("const");
			s += ' ';
			s += (*identifier);
			char const *s_cstr = s.c_str();
			sstr << type->BuildCDecl(&s_cstr, collapsed);
			if(!s_cstr)
			{
				*identifier = 0;
			}
		}
		else
		{
			sstr << type->GetCDecl(collapsed);
			sstr << " const";
		}
		return sstr.str();
	}

	// Discarding const for arrays
	return type->BuildCDecl(identifier, collapsed);
}

Enumerator::Enumerator(const char *_name, int64_t _value)
	: name(_name)
	, sign(_value < 0)
	, value(sign ? (-_value) : _value)
{
}

Enumerator::Enumerator(const char *_name, uint64_t _value)
	: name(_name)
	, sign(false)
	, value(_value)
{
}

Enumerator::~Enumerator()
{
}

const char *Enumerator::GetName() const
{
	return name.c_str();
}

bool Enumerator::IsSigned() const
{
	return sign;
}

int64_t Enumerator::GetSignedValue() const
{
	return sign ? -value : value;
}

uint64_t Enumerator::GetUnsignedValue() const
{
	return value;
}

std::ostream& operator << (std::ostream& os, const Enumerator& enumerator)
{
	os << enumerator.name << " = ";
	if(enumerator.sign)
	{
		os << (int64_t)(-enumerator.value);
	}
	else
	{
		os << enumerator.value;
	}
	return os;
}

EnumType::EnumType(const char *_name)
	: NamedType(T_ENUM, _name)
	, enumerators()
{
}

EnumType::EnumType(const char *_name, const DeclLocation *_decl_location)
	: NamedType(T_ENUM, _name, _decl_location)
	, enumerators()
{
}

EnumType::~EnumType()
{
	unsigned int enumerator_count = enumerators.size();
	unsigned int i;
	for(i = 0; i < enumerator_count; i++)
	{
		delete enumerators[i];
	}
}

void EnumType::Add(const Enumerator *enumerator)
{
	enumerators.push_back(enumerator);
}

std::string EnumType::BuildCDecl(char const **identifier, bool collapsed) const
{
	std::stringstream sstr;
	
	sstr << "enum";
	if(HasName())
	{
		sstr << " " << GetName();
	}
	unsigned int enumerator_count = enumerators.size();
	if(enumerator_count && (!collapsed || !HasName()))
	{
		sstr << " { ";
		unsigned int i;
		for(i = 0; i < enumerator_count; i++)
		{
			if(i != 0) sstr << ", ";
			const Enumerator *enumerator = enumerators[i];
			sstr << (*enumerator);
		}
		sstr << " }";
	}
	return sstr.str();
}

UnspecifiedType::UnspecifiedType()
	: Type(T_VOID)
{
}

UnspecifiedType::UnspecifiedType(const DeclLocation *_decl_location)
	: Type(T_VOID, _decl_location)
{
}

UnspecifiedType::~UnspecifiedType()
{
}

std::string UnspecifiedType::BuildCDecl(char const **identifier, bool collapsed) const
{
	return std::string("void");
}

VolatileType::VolatileType(const Type *_type)
	: Type(T_VOLATILE)
	, type(_type)
{
	if(type)
	{
		type->Catch();
	}
}

VolatileType::VolatileType(const Type *_type, const DeclLocation *_decl_location)
	: Type(T_VOLATILE, _decl_location)
	, type(_type)
{
	if(type)
	{
		type->Catch();
	}
}

VolatileType::~VolatileType()
{
	if(type)
	{
		type->Release();
	}
}

const Type *VolatileType::GetType() const
{
	return type;
}

std::string VolatileType::BuildCDecl(char const **identifier, bool collapsed) const
{
	if(type->GetClass() != T_ARRAY)
	{
		std::stringstream sstr;
	
		if(identifier && (*identifier))
		{
			std::string s("volatile");
			s += ' ';
			s += (*identifier);
			char const *s_cstr = s.c_str();
			sstr << type->BuildCDecl(&s_cstr, collapsed);
			if(!s_cstr)
			{
				*identifier = 0;
			}
		}
		else
		{
			sstr << type->GetCDecl(collapsed);
			sstr << " volatile";
		}
		return sstr.str();
	}
	
	// Discarding volatile for arrays
	return type->BuildCDecl(identifier, collapsed);
}

Type const *TypeResolver::Resolve(Type const *type)
{
	if(!type) return 0;
	Visitor visitor;
	return type->Visit<Visitor, Type const *>(visitor);
}

Type const *TypeResolver::Visitor::Visit(CharType const *type)
{
	return type;
}

Type const *TypeResolver::Visitor::Visit(IntegerType const *type)
{
	return type;
}

Type const *TypeResolver::Visitor::Visit(FloatingPointType const *type)
{
	return type;
}

Type const *TypeResolver::Visitor::Visit(BooleanType const *type)
{
	return type;
}

Type const *TypeResolver::Visitor::Visit(CompositeType const *type)
{
	return type;
}

Type const *TypeResolver::Visitor::Visit(ArrayType const *type)
{
	return type;
}

Type const *TypeResolver::Visitor::Visit(PointerType const *type)
{
	return type;
}

Type const *TypeResolver::Visitor::Visit(Typedef const *type)
{
	return type->Scan<Visitor, Type const *>(*this);
}

Type const *TypeResolver::Visitor::Visit(FunctionType const *type)
{
	return type;
}

Type const *TypeResolver::Visitor::Visit(ConstType const *type)
{
	return type->Scan<Visitor, Type const *>(*this);
}

Type const *TypeResolver::Visitor::Visit(EnumType const *type)
{
	return type;
}

Type const *TypeResolver::Visitor::Visit(UnspecifiedType const *type)
{
	return type;
}

Type const *TypeResolver::Visitor::Visit(VolatileType const *type)
{
	return type->Scan<Visitor, Type const *>(*this);
}

Type const *TypeResolver::Visitor::Visit(Member const *member)
{
	return 0;
}

Type const *TypeResolver::Visitor::Visit(FormalParameter const *formal_param)
{
	return 0;
}

TypeIsCharPointer::Visitor::Visitor()
	: state(0)
	, is_char_pointer(false)
{
}

bool TypeIsCharPointer::Visitor::Visit(Type const *type)
{
	return true;
}

bool TypeIsCharPointer::Visitor::Visit(CharType const *type)
{
	if(state == 1)
	{
		is_char_pointer = true;
	}
	return true;
}

bool TypeIsCharPointer::Visitor::Visit(PointerType const *type)
{
	if(state == 0)
	{
		state = 1;
		type->Scan(*this);
	}
	return true;
}

bool TypeIsCharPointer::Visitor::Visit(Typedef const *type)
{
	type->Scan(*this);
	return false;
}

bool TypeIsCharPointer::Visitor::Visit(ConstType const *type)
{
	type->Scan(*this);
	return false;
}

bool TypeIsCharPointer::Visitor::Visit(VolatileType const *type)
{
	type->Scan(*this);
	return false;
}

bool TypeIsCharPointer::Visitor::Visit(Member const *member)
{
	return true;
}

bool TypeIsCharPointer::Visitor::Visit(FormalParameter const *formal_param)
{
	return true;
}

bool TypeIsCharPointer::Test(Type const *type)
{
	if(!type) return false;
	Visitor visitor;
	type->Visit(visitor);
	return visitor.is_char_pointer;
}

TypeIsSigned::Visitor::Visitor()
	: is_signed(false)
{
}

bool TypeIsSigned::Visitor::Visit(Type const *type)
{
	return true;
}

bool TypeIsSigned::Visitor::Visit(CharType const *type)
{
	is_signed = type->IsSigned();
	return true;
}

bool TypeIsSigned::Visitor::Visit(IntegerType const *type)
{
	is_signed = type->IsSigned();
	return true;
}

bool TypeIsSigned::Test(Type const *type)
{
	Visitor visitor;
	type->Visit(visitor);
	return visitor.is_signed;
}

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
