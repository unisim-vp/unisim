/*
 *  Copyright (c) 2023,
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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_TYPE_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_TYPE_TCC__

#include <unisim/service/debug/nodejs/type.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

//////////////////////////////// TypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *TypeWrapper<CONFIG>::CLASS_NAME = "Type";

template <typename CONFIG>
const uint32_t TypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> TypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = type_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "isComposite"  , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsComposite  > >, 0 },
		{ "isStructure"  , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsStructure  > >, 0 },
		{ "isUnion"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsUnion      > >, 0 },
		{ "isClass"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsClass      > >, 0 },
		{ "isInterface"  , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsInterface  > >, 0 },
		{ "isBase"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsBase       > >, 0 },
		{ "isChar"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsChar       > >, 0 },
		{ "isInteger"    , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsInteger    > >, 0 },
		{ "isBoolean"    , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsBoolean    > >, 0 },
		{ "isFloat"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsFloat      > >, 0 },
		{ "isPointer"    , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsPointer    > >, 0 },
		{ "isTypedef"    , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsTypedef    > >, 0 },
		{ "isFunction"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsFunction   > >, 0 },
		{ "isArray"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsArray      > >, 0 },
		{ "isConst"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsConst      > >, 0 },
		{ "isEnum"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsEnum       > >, 0 },
		{ "isUnspecified", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsUnspecified> >, 0 },
		{ "isVolatile"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsVolatile   > >, 0 },
		{ "isNamed"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::Type::IsNamed      > >, 0 },
		{ "declLoc"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetDeclLoc                                                >, 0 },
		{ "cdecl"        , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetCDecl                                                  >, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}

	return handle_scope.Escape(type_function_template);
}

// Type() => Type
template <typename CONFIG>
void TypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
TypeWrapper<CONFIG> *TypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Type *type)
{
	This *type_wrapper = nodejs.template GetObjectWrapper<This>(type);
	return type_wrapper ? type_wrapper : new TypeWrapper<CONFIG>(nodejs, type);
}

template <typename CONFIG>
v8::Local<v8::Object> TypeWrapper<CONFIG>::MakeObjectFromType(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Type *type)
{
	switch(type->GetClass())
	{
		case unisim::util::debug::T_UNKNOWN  : return (TypeWrapper             <CONFIG>::Wrap(nodejs, type)                      )->MakeObject();
		case unisim::util::debug::T_CHAR     : return (CharTypeWrapper         <CONFIG>::Wrap(nodejs, &type->AsChar           ()))->MakeObject();
		case unisim::util::debug::T_INTEGER  : return (IntegerTypeWrapper      <CONFIG>::Wrap(nodejs, &type->AsInteger        ()))->MakeObject();
		case unisim::util::debug::T_FLOAT    : return (FloatingPointTypeWrapper<CONFIG>::Wrap(nodejs, &type->AsFloat          ()))->MakeObject();
		case unisim::util::debug::T_BOOL     : return (BooleanTypeWrapper      <CONFIG>::Wrap(nodejs, &type->AsBoolean        ()))->MakeObject();
		case unisim::util::debug::T_STRUCT   : return (StructureTypeWrapper    <CONFIG>::Wrap(nodejs, &type->AsStructure      ()))->MakeObject();
		case unisim::util::debug::T_UNION    : return (UnionTypeWrapper        <CONFIG>::Wrap(nodejs, &type->AsUnion          ()))->MakeObject();
		case unisim::util::debug::T_CLASS    : return (ClassTypeWrapper        <CONFIG>::Wrap(nodejs, &type->AsClass          ()))->MakeObject();
		case unisim::util::debug::T_INTERFACE: return (InterfaceTypeWrapper    <CONFIG>::Wrap(nodejs, &type->AsInterface      ()))->MakeObject();
		case unisim::util::debug::T_ARRAY    : return (ArrayTypeWrapper        <CONFIG>::Wrap(nodejs, &type->AsArray          ()))->MakeObject();
		case unisim::util::debug::T_POINTER  : return (PointerTypeWrapper      <CONFIG>::Wrap(nodejs, &type->AsPointer        ()))->MakeObject();
		case unisim::util::debug::T_TYPEDEF  : return (TypedefWrapper          <CONFIG>::Wrap(nodejs, &type->AsTypedef        ()))->MakeObject();
		case unisim::util::debug::T_FUNCTION : return (FunctionTypeWrapper     <CONFIG>::Wrap(nodejs, &type->AsFunction       ()))->MakeObject();
		case unisim::util::debug::T_CONST    : return (ConstTypeWrapper        <CONFIG>::Wrap(nodejs, &type->AsConst          ()))->MakeObject();
		case unisim::util::debug::T_ENUM     : return (EnumTypeWrapper         <CONFIG>::Wrap(nodejs, &type->AsEnum           ()))->MakeObject();
		case unisim::util::debug::T_VOID     : return (UnspecifiedTypeWrapper  <CONFIG>::Wrap(nodejs, &type->AsUnspecifiedType()))->MakeObject();
		case unisim::util::debug::T_VOLATILE : return (VolatileTypeWrapper     <CONFIG>::Wrap(nodejs, &type->AsVolatile       ()))->MakeObject();
	}
	
	struct Bad {};
	throw Bad();
	return v8::Local<v8::Object>();
}

template <typename CONFIG>
TypeWrapper<CONFIG>::TypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::Type *_type, std::size_t size)
	: Super(_nodejs, _type, size ? size : sizeof(*this))
	, type(_type)
{
	if(type) type->Catch();
}

template <typename CONFIG>
TypeWrapper<CONFIG>::~TypeWrapper()
{
	if(type) type->Release();
}

template <typename CONFIG>
const unisim::util::debug::Type *TypeWrapper<CONFIG>::GetType() const
{
	return type;
}

template <typename CONFIG>
template <bool (unisim::util::debug::Type::*METHOD)() const>
void TypeWrapper<CONFIG>::GetIsSomething(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(type)
	{
		info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), (type->*METHOD)()));
	}
}

template <typename CONFIG>
void TypeWrapper<CONFIG>::GetDeclLoc(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(type)
	{
		const unisim::util::debug::DeclLocation *decl_location = type->GetDeclLocation();
		if(decl_location)
		{
			SourceCodeLocationWrapper<CONFIG> *source_code_location_wrapper = new SourceCodeLocationWrapper<CONFIG>(this->nodejs, decl_location->GetSourceCodeLocation());
			info.GetReturnValue().Set(source_code_location_wrapper->MakeObject());
		}
	}
}

template <typename CONFIG>
void TypeWrapper<CONFIG>::GetCDecl(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(type) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), type->GetCDecl().c_str()).ToLocalChecked());
}

template <typename CONFIG>
void TypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/type.h>
	;
}

//////////////////////////////// NamedTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *NamedTypeWrapper<CONFIG>::CLASS_NAME = "NamedType";

template <typename CONFIG>
const uint32_t NamedTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> NamedTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> named_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = named_type_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "name"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetName   >, 0 },
		{ "hasName", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetHasName>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "Type"
	named_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(named_type_function_template);
}

// NamedType() => NamedType
template <typename CONFIG>
void NamedTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
NamedTypeWrapper<CONFIG>::NamedTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::NamedType *_named_type, std::size_t size)
	: Super(_nodejs, _named_type, size ? size : sizeof(*this))
	, named_type(_named_type)
{
}

template <typename CONFIG>
NamedTypeWrapper<CONFIG>::~NamedTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::NamedType *NamedTypeWrapper<CONFIG>::GetNamedType() const
{
	return named_type;
}

template <typename CONFIG>
void NamedTypeWrapper<CONFIG>::GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(named_type) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), named_type->GetName().c_str()).ToLocalChecked());
}

template <typename CONFIG>
void NamedTypeWrapper<CONFIG>::GetHasName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(named_type) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), named_type->HasName()));
}

template <typename CONFIG>
void NamedTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/named_type.h>
	;
}

//////////////////////////////// BaseTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *BaseTypeWrapper<CONFIG>::CLASS_NAME = "BaseType";

template <typename CONFIG>
const uint32_t BaseTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> BaseTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> base_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = base_type_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "bitSize", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetBitSize>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "NamedType"
	base_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(base_type_function_template);
}

// BaseType() => BaseType
template <typename CONFIG>
void BaseTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
BaseTypeWrapper<CONFIG>::BaseTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::BaseType *_base_type, std::size_t size)
	: Super(_nodejs, _base_type, size ? size : sizeof(*this))
	, base_type(_base_type)
{
}

template <typename CONFIG>
BaseTypeWrapper<CONFIG>::~BaseTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::BaseType *BaseTypeWrapper<CONFIG>::GetBaseType() const
{
	return base_type;
}

template <typename CONFIG>
void BaseTypeWrapper<CONFIG>::GetBitSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(base_type) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), base_type->GetBitSize()));
}

template <typename CONFIG>
void BaseTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/base_type.h>
	;
}

//////////////////////////////// IntegerTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *IntegerTypeWrapper<CONFIG>::CLASS_NAME = "IntegerType";

template <typename CONFIG>
const uint32_t IntegerTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> IntegerTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> integer_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = integer_type_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "isSigned", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSigned>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "BaseType"
	integer_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(integer_type_function_template);
}

// IntegerType() => IntegerType
template <typename CONFIG>
void IntegerTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
IntegerTypeWrapper<CONFIG> *IntegerTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::IntegerType *integer_type)
{
	This *integer_type_wrapper = nodejs.template GetObjectWrapper<This>(integer_type);
	return integer_type_wrapper ? integer_type_wrapper : new IntegerTypeWrapper<CONFIG>(nodejs, integer_type);
}

template <typename CONFIG>
IntegerTypeWrapper<CONFIG>::IntegerTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::IntegerType *_integer_type, std::size_t size)
	: Super(_nodejs, _integer_type, size ? size : sizeof(*this))
	, integer_type(_integer_type)
{
}

template <typename CONFIG>
IntegerTypeWrapper<CONFIG>::~IntegerTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::IntegerType *IntegerTypeWrapper<CONFIG>::GetIntegerType() const
{
	return integer_type;
}

template <typename CONFIG>
void IntegerTypeWrapper<CONFIG>::GetIsSigned(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(integer_type) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), integer_type->IsSigned()));
}

template <typename CONFIG>
void IntegerTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/integer_type.h>
	;
}

//////////////////////////////// CharTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *CharTypeWrapper<CONFIG>::CLASS_NAME = "CharType";

template <typename CONFIG>
const uint32_t CharTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> CharTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> char_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = char_type_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "isSigned", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSigned>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "BaseType"
	char_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(char_type_function_template);
}

// CharType() => CharType
template <typename CONFIG>
void CharTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
CharTypeWrapper<CONFIG> *CharTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::CharType *char_type)
{
	This *char_type_wrapper = nodejs.template GetObjectWrapper<This>(char_type);
	return char_type_wrapper ? char_type_wrapper : new CharTypeWrapper<CONFIG>(nodejs, char_type);
}

template <typename CONFIG>
CharTypeWrapper<CONFIG>::CharTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::CharType *_char_type, std::size_t size)
	: Super(_nodejs, _char_type, size ? size : sizeof(*this))
	, char_type(_char_type)
{
}

template <typename CONFIG>
CharTypeWrapper<CONFIG>::~CharTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::CharType *CharTypeWrapper<CONFIG>::GetCharType() const
{
	return char_type;
}

template <typename CONFIG>
void CharTypeWrapper<CONFIG>::GetIsSigned(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(char_type) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), char_type->IsSigned()));
}

template <typename CONFIG>
void CharTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/char_type.h>
	;
}

//////////////////////////////// FloatingPointTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *FloatingPointTypeWrapper<CONFIG>::CLASS_NAME = "FloatingPointType";

template <typename CONFIG>
const uint32_t FloatingPointTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> FloatingPointTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> floating_point_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Inherit from "BaseType"
	floating_point_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(floating_point_type_function_template);
}

// FloatingPointType() => FloatingPointType
template <typename CONFIG>
void FloatingPointTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
FloatingPointTypeWrapper<CONFIG> *FloatingPointTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::FloatingPointType *floating_point_type)
{
	This *floating_point_type_wrapper = nodejs.template GetObjectWrapper<This>(floating_point_type);
	return floating_point_type_wrapper ? floating_point_type_wrapper : new FloatingPointTypeWrapper<CONFIG>(nodejs, floating_point_type);
}

template <typename CONFIG>
FloatingPointTypeWrapper<CONFIG>::FloatingPointTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::FloatingPointType *_floating_point_type, std::size_t size)
	: Super(_nodejs, _floating_point_type, size ? size : sizeof(*this))
	, floating_point_type(_floating_point_type)
{
}

template <typename CONFIG>
FloatingPointTypeWrapper<CONFIG>::~FloatingPointTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::FloatingPointType *FloatingPointTypeWrapper<CONFIG>::GetFloatingPointType() const
{
	return floating_point_type;
}

template <typename CONFIG>
void FloatingPointTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/floating_point_type.h>
	;
}

//////////////////////////////// BooleanTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *BooleanTypeWrapper<CONFIG>::CLASS_NAME = "BooleanType";

template <typename CONFIG>
const uint32_t BooleanTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> BooleanTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> boolean_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Inherit from "BaseType"
	boolean_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(boolean_type_function_template);
}

// BooleanType() => booleanType
template <typename CONFIG>
void BooleanTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
BooleanTypeWrapper<CONFIG> *BooleanTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::BooleanType *boolean_type)
{
	This *boolean_type_wrapper = nodejs.template GetObjectWrapper<This>(boolean_type);
	return boolean_type_wrapper ? boolean_type_wrapper : new BooleanTypeWrapper<CONFIG>(nodejs, boolean_type);
}

template <typename CONFIG>
BooleanTypeWrapper<CONFIG>::BooleanTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::BooleanType *_boolean_type, std::size_t size)
	: Super(_nodejs, _boolean_type, size ? size : sizeof(*this))
	, boolean_type(_boolean_type)
{
}

template <typename CONFIG>
BooleanTypeWrapper<CONFIG>::~BooleanTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::BooleanType *BooleanTypeWrapper<CONFIG>::GetBooleanType() const
{
	return boolean_type;
}

template <typename CONFIG>
void BooleanTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/boolean_type.h>
	;
}

//////////////////////////////// MemberWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *MemberWrapper<CONFIG>::CLASS_NAME = "Member";

template <typename CONFIG>
const uint32_t MemberWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> MemberWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> member_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = member_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "name"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetName   >, 0 },
		{ "hasName", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetHasName>, 0 },
		{ "type"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetType   >, 0 },
		{ "bitSize", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetBitSize>, 0 },
		{ "cdecl"  , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetCDecl  >, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	return handle_scope.Escape(member_function_template);
}

// Member() => Member
template <typename CONFIG>
void MemberWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
MemberWrapper<CONFIG> *MemberWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Member *member)
{
	This *member_wrapper = nodejs.template GetObjectWrapper<This>(member);
	return member_wrapper ? member_wrapper : new MemberWrapper<CONFIG>(nodejs, member);
}

template <typename CONFIG>
MemberWrapper<CONFIG>::MemberWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::Member *_member, std::size_t size)
	: Super(_nodejs, size ? size : sizeof(*this))
	, member(_member)
{
}

template <typename CONFIG>
MemberWrapper<CONFIG>::~MemberWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::Member *MemberWrapper<CONFIG>::GetMember() const
{
	return member;
}

template <typename CONFIG>
void MemberWrapper<CONFIG>::GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(member) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), member->GetName()).ToLocalChecked());
}

template <typename CONFIG>
void MemberWrapper<CONFIG>::GetHasName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(member) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), member->HasName()));
}

template <typename CONFIG>
void MemberWrapper<CONFIG>::GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(member)
	{
		const unisim::util::debug::Type *type = member->GetType();
		if(type)
		{
			info.GetReturnValue().Set(TypeWrapper<CONFIG>::MakeObjectFromType(this->nodejs, type));
		}
	}
}

template <typename CONFIG>
void MemberWrapper<CONFIG>::GetBitSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(member) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), member->GetBitSize()));
}

template <typename CONFIG>
void MemberWrapper<CONFIG>::GetCDecl(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(member) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), member->BuildCDecl().c_str()).ToLocalChecked());
}

template <typename CONFIG>
void MemberWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/member.h>
	;
}

//////////////////////////////// CompositeTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *CompositeTypeWrapper<CONFIG>::CLASS_NAME = "CompositeType";

template <typename CONFIG>
const uint32_t CompositeTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> CompositeTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> composite_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = composite_type_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "isIncomplete", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsIncomplete>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "NamedType"
	composite_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(composite_type_function_template);
}

// CompositeType() => CompositeType
template <typename CONFIG>
void CompositeTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
CompositeTypeWrapper<CONFIG>::CompositeTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::CompositeType *_composite_type, std::size_t size)
	: Super(_nodejs, _composite_type, size ? size : sizeof(*this))
	, composite_type(_composite_type)
{
}

template <typename CONFIG>
CompositeTypeWrapper<CONFIG>::~CompositeTypeWrapper()
{
}

template <typename CONFIG>
v8::Local<v8::Object> CompositeTypeWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	
	v8::Local<v8::Object> composite_type_object = this->ThisObject();
	
	if(composite_type_object.IsEmpty())
	{
		composite_type_object = Super::template MakeObject<This>();
		
		v8::Local<v8::Object> prop_members = v8::Object::New(this->GetIsolate());
		
		struct CompositeTypeScanner
		{
			NodeJS<CONFIG>& nodejs;
			v8::Local<v8::Object>& prop_members;
			
			CompositeTypeScanner(NodeJS<CONFIG>& _nodejs, v8::Local<v8::Object>& _prop_members) : nodejs(_nodejs), prop_members(_prop_members) {}
			
			bool Visit(const unisim::util::debug::Member *member)
			{
				MemberWrapper<CONFIG> *member_wrapper = MemberWrapper<CONFIG>::Wrap(nodejs, member);
				// add "members.<member-name>" read-only property
				prop_members->DefineOwnProperty(
					nodejs.GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8(nodejs.GetIsolate(), member->GetName(), v8::NewStringType::kInternalized).ToLocalChecked(),
					member_wrapper->MakeObject(),
					v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
				).ToChecked();
				return false; // do not stop: continue with next member
			}
		};
		
		CompositeTypeScanner composite_type_scanner(this->nodejs, prop_members);
		this->composite_type->Scan(composite_type_scanner);
		
		// add "members" read-only property
		composite_type_object->DefineOwnProperty(
			this->GetIsolate()->GetCurrentContext(),
			v8::String::NewFromUtf8Literal(this->GetIsolate(), "members", v8::NewStringType::kInternalized),
			prop_members,
			v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
		).ToChecked();
	}
	
	return handle_scope.Escape(composite_type_object);
}

template <typename CONFIG>
const unisim::util::debug::CompositeType *CompositeTypeWrapper<CONFIG>::GetCompositeType() const
{
	return composite_type;
}

template <typename CONFIG>
void CompositeTypeWrapper<CONFIG>::GetIsIncomplete(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(composite_type) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), composite_type->IsIncomplete()));
}

template <typename CONFIG>
void CompositeTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/composite_type.h>
	;
}

//////////////////////////////// StructureTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *StructureTypeWrapper<CONFIG>::CLASS_NAME = "StructureType";

template <typename CONFIG>
const uint32_t StructureTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> StructureTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> structure_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Inherit from "CompositeType"
	structure_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(structure_type_function_template);
}

// StructureType() => StructureType
template <typename CONFIG>
void StructureTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
StructureTypeWrapper<CONFIG> *StructureTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::StructureType *structure_type)
{
	This *structure_type_wrapper = nodejs.template GetObjectWrapper<This>(structure_type);
	return structure_type_wrapper ? structure_type_wrapper : new StructureTypeWrapper<CONFIG>(nodejs, structure_type);
}

template <typename CONFIG>
StructureTypeWrapper<CONFIG>::StructureTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::StructureType *_structure_type, std::size_t size)
	: Super(_nodejs, _structure_type, size ? size : sizeof(*this))
	, structure_type(_structure_type)
{
}

template <typename CONFIG>
StructureTypeWrapper<CONFIG>::~StructureTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::StructureType *StructureTypeWrapper<CONFIG>::GetStructureType() const
{
	return structure_type;
}

template <typename CONFIG>
void StructureTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/structure_type.h>
	;
}

//////////////////////////////// UnionTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *UnionTypeWrapper<CONFIG>::CLASS_NAME = "UnionType";

template <typename CONFIG>
const uint32_t UnionTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> UnionTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> union_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Inherit from "CompositeType"
	union_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(union_type_function_template);
}

// UnionType() => UnionType
template <typename CONFIG>
void UnionTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
UnionTypeWrapper<CONFIG> *UnionTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::UnionType *union_type)
{
	This *union_type_wrapper = nodejs.template GetObjectWrapper<This>(union_type);
	return union_type_wrapper ? union_type_wrapper : new UnionTypeWrapper<CONFIG>(nodejs, union_type);
}

template <typename CONFIG>
UnionTypeWrapper<CONFIG>::UnionTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::UnionType *_union_type, std::size_t size)
	: Super(_nodejs, _union_type, size ? size : sizeof(*this))
	, union_type(_union_type)
{
}

template <typename CONFIG>
UnionTypeWrapper<CONFIG>::~UnionTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::UnionType *UnionTypeWrapper<CONFIG>::GetUnionType() const
{
	return union_type;
}

template <typename CONFIG>
void UnionTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/union_type.h>
	;
}

//////////////////////////////// ClassTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *ClassTypeWrapper<CONFIG>::CLASS_NAME = "ClassType";

template <typename CONFIG>
const uint32_t ClassTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> ClassTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> class_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Inherit from "CompositeType"
	class_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(class_type_function_template);
}

// ClassType() => ClassType
template <typename CONFIG>
void ClassTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
ClassTypeWrapper<CONFIG> *ClassTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::ClassType *class_type)
{
	This *class_type_wrapper = nodejs.template GetObjectWrapper<This>(class_type);
	return class_type_wrapper ? class_type_wrapper : new ClassTypeWrapper<CONFIG>(nodejs, class_type);
}

template <typename CONFIG>
ClassTypeWrapper<CONFIG>::ClassTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::ClassType *_class_type, std::size_t size)
	: Super(_nodejs, _class_type, size ? size : sizeof(*this))
	, class_type(_class_type)
{
}

template <typename CONFIG>
ClassTypeWrapper<CONFIG>::~ClassTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::ClassType *ClassTypeWrapper<CONFIG>::GetClassType() const
{
	return class_type;
}

template <typename CONFIG>
void ClassTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/class_type.h>
	;
}

//////////////////////////////// InterfaceTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *InterfaceTypeWrapper<CONFIG>::CLASS_NAME = "InterfaceType";

template <typename CONFIG>
const uint32_t InterfaceTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> InterfaceTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> interface_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Inherit from "CompositeType"
	interface_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(interface_type_function_template);
}

// InterfaceType() => InterfaceType
template <typename CONFIG>
void InterfaceTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
InterfaceTypeWrapper<CONFIG> *InterfaceTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::InterfaceType *interface_type)
{
	This *interface_type_wrapper = nodejs.template GetObjectWrapper<This>(interface_type);
	return interface_type_wrapper ? interface_type_wrapper : new InterfaceTypeWrapper<CONFIG>(nodejs, interface_type);
}

template <typename CONFIG>
InterfaceTypeWrapper<CONFIG>::InterfaceTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::InterfaceType *_interface_type, std::size_t size)
	: Super(_nodejs, _interface_type, size ? size : sizeof(*this))
	, interface_type(_interface_type)
{
}

template <typename CONFIG>
InterfaceTypeWrapper<CONFIG>::~InterfaceTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::InterfaceType *InterfaceTypeWrapper<CONFIG>::GetInterfaceType() const
{
	return interface_type;
}

template <typename CONFIG>
void InterfaceTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/interface_type.h>
	;
}

//////////////////////////////// ArrayTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *ArrayTypeWrapper<CONFIG>::CLASS_NAME = "ArrayType";

template <typename CONFIG>
const uint32_t ArrayTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> ArrayTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> array_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = array_type_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "typeOfElement", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetTypeOfElement>, 0 },
		{ "order"        , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetOrder        >, 0 },
		{ "lowerBound"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetLowerBound   >, 0 },
		{ "upperBound"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetUpperBound   >, 0 },
		{ "count"        , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetCount        >, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "Type"
	array_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(array_type_function_template);
}

// ArrayType() => ArrayType
template <typename CONFIG>
void ArrayTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
ArrayTypeWrapper<CONFIG> *ArrayTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::ArrayType *array_type)
{
	This *array_type_wrapper = nodejs.template GetObjectWrapper<This>(array_type);
	return array_type_wrapper ? array_type_wrapper : new ArrayTypeWrapper<CONFIG>(nodejs, array_type);
}

template <typename CONFIG>
ArrayTypeWrapper<CONFIG>::ArrayTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::ArrayType *_array_type, std::size_t size)
	: Super(_nodejs, _array_type, size ? size : sizeof(*this))
	, array_type(_array_type)
{
}

template <typename CONFIG>
ArrayTypeWrapper<CONFIG>::~ArrayTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::ArrayType *ArrayTypeWrapper<CONFIG>::GetArrayType() const
{
	return array_type;
}

template <typename CONFIG>
void ArrayTypeWrapper<CONFIG>::GetTypeOfElement(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(array_type)
	{
		const unisim::util::debug::Type *type_of_element = array_type->GetTypeOfElement();
		if(type_of_element)
		{
			info.GetReturnValue().Set(TypeWrapper<CONFIG>::MakeObjectFromType(this->nodejs, type_of_element));
		}
	}
}

template <typename CONFIG>
void ArrayTypeWrapper<CONFIG>::GetOrder(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(array_type) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), array_type->GetOrder()));
}

template <typename CONFIG>
void ArrayTypeWrapper<CONFIG>::GetLowerBound(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(array_type) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), array_type->GetLowerBound()));
}

template <typename CONFIG>
void ArrayTypeWrapper<CONFIG>::GetUpperBound(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(array_type) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), array_type->GetUpperBound()));
}

template <typename CONFIG>
void ArrayTypeWrapper<CONFIG>::GetCount(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(array_type) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), array_type->GetCount()));
}

template <typename CONFIG>
void ArrayTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/array_type.h>
	;
}

//////////////////////////////// PointerTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *PointerTypeWrapper<CONFIG>::CLASS_NAME = "PointerType";

template <typename CONFIG>
const uint32_t PointerTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> PointerTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> pointer_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = pointer_type_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "typeOfDereferencedObject", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetTypeOfDereferencedObject>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "Type"
	pointer_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(pointer_type_function_template);
}

// PointerType() => PointerType
template <typename CONFIG>
void PointerTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
PointerTypeWrapper<CONFIG> *PointerTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::PointerType *pointer_type)
{
	This *pointer_type_wrapper = nodejs.template GetObjectWrapper<This>(pointer_type);
	return pointer_type_wrapper ? pointer_type_wrapper : new PointerTypeWrapper<CONFIG>(nodejs, pointer_type);
}

template <typename CONFIG>
PointerTypeWrapper<CONFIG>::PointerTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::PointerType *_pointer_type, std::size_t size)
	: Super(_nodejs, _pointer_type, size ? size : sizeof(*this))
	, pointer_type(_pointer_type)
{
}

template <typename CONFIG>
PointerTypeWrapper<CONFIG>::~PointerTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::PointerType *PointerTypeWrapper<CONFIG>::GetPointerType() const
{
	return pointer_type;
}

template <typename CONFIG>
void PointerTypeWrapper<CONFIG>::GetTypeOfDereferencedObject(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(pointer_type)
	{
		const unisim::util::debug::Type *type_of_dereferenced_object = pointer_type->GetTypeOfDereferencedObject();
		if(type_of_dereferenced_object)
		{
			info.GetReturnValue().Set(TypeWrapper<CONFIG>::MakeObjectFromType(this->nodejs, type_of_dereferenced_object));
		}
	}
}

template <typename CONFIG>
void PointerTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/pointer_type.h>
	;
}

//////////////////////////////// TypedefWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *TypedefWrapper<CONFIG>::CLASS_NAME = "Typedef";

template <typename CONFIG>
const uint32_t TypedefWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> TypedefWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> typedef_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = typedef_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "type", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetType>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}

	// Inherit from "NamedType"
	typedef_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(typedef_function_template);
}

// Typedef() => Typedef
template <typename CONFIG>
void TypedefWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
TypedefWrapper<CONFIG> *TypedefWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Typedef *typedef_type)
{
	This *typedef_type_wrapper = nodejs.template GetObjectWrapper<This>(typedef_type);
	return typedef_type_wrapper ? typedef_type_wrapper : new TypedefWrapper<CONFIG>(nodejs, typedef_type);
}

template <typename CONFIG>
TypedefWrapper<CONFIG>::TypedefWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::Typedef *_typedef_type, std::size_t size)
	: Super(_nodejs, _typedef_type, size ? size : sizeof(*this))
	, typedef_type(_typedef_type)
{
}

template <typename CONFIG>
TypedefWrapper<CONFIG>::~TypedefWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::Typedef *TypedefWrapper<CONFIG>::GetTypedef() const
{
	return typedef_type;
}

template <typename CONFIG>
void TypedefWrapper<CONFIG>::GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(typedef_type)
	{
		const unisim::util::debug::Type *type = typedef_type->GetType();
		if(type)
		{
			info.GetReturnValue().Set(TypeWrapper<CONFIG>::MakeObjectFromType(this->nodejs, type));
		}
	}
}

template <typename CONFIG>
void TypedefWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/typedef.h>
	;
}

//////////////////////////////// FormalParameterWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *FormalParameterWrapper<CONFIG>::CLASS_NAME = "FormalParameter";

template <typename CONFIG>
const uint32_t FormalParameterWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> FormalParameterWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> formal_parameter_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = formal_parameter_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "name", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetName>, 0 },
		{ "type", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetType>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	return handle_scope.Escape(formal_parameter_function_template);
}

// FormalParameter() => FormalParameter
template <typename CONFIG>
void FormalParameterWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
FormalParameterWrapper<CONFIG> *FormalParameterWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::FormalParameter *formal_parameter)
{
	This *formal_parameter_wrapper = nodejs.template GetObjectWrapper<This>(formal_parameter);
	return formal_parameter_wrapper ? formal_parameter_wrapper : new FormalParameterWrapper<CONFIG>(nodejs, formal_parameter);
}

template <typename CONFIG>
FormalParameterWrapper<CONFIG>::FormalParameterWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::FormalParameter *_formal_parameter, std::size_t size)
	: Super(_nodejs, _formal_parameter, size ? size : sizeof(*this))
	, formal_parameter(_formal_parameter)
{
}

template <typename CONFIG>
FormalParameterWrapper<CONFIG>::~FormalParameterWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::FormalParameter *FormalParameterWrapper<CONFIG>::GetFormalParameter() const
{
	return formal_parameter;
}

template <typename CONFIG>
void FormalParameterWrapper<CONFIG>::GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(formal_parameter) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), formal_parameter->GetName()).ToLocalChecked());
}

template <typename CONFIG>
void FormalParameterWrapper<CONFIG>::GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(formal_parameter)
	{
		const unisim::util::debug::Type *type = formal_parameter->GetType();
		if(type)
		{
			info.GetReturnValue().Set(TypeWrapper<CONFIG>::MakeObjectFromType(this->nodejs, type));
		}
	}
}

template <typename CONFIG>
void FormalParameterWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/formal_parameter.h>
	;
}

//////////////////////////////// FunctionTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *FunctionTypeWrapper<CONFIG>::CLASS_NAME = "FunctionType";

template <typename CONFIG>
const uint32_t FunctionTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> FunctionTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> function_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = function_type_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "returnType", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetReturnType>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "Type"
	function_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(function_type_function_template);
}

// FunctionType() => FunctionType
template <typename CONFIG>
void FunctionTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
FunctionTypeWrapper<CONFIG> *FunctionTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::FunctionType *function_type)
{
	This *function_type_wrapper = nodejs.template GetObjectWrapper<This>(function_type);
	return function_type_wrapper ? function_type_wrapper : new FunctionTypeWrapper<CONFIG>(nodejs, function_type);
}

template <typename CONFIG>
FunctionTypeWrapper<CONFIG>::FunctionTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::FunctionType *_function_type, std::size_t size)
	: Super(_nodejs, _function_type, size ? size : sizeof(*this))
	, function_type(_function_type)
{
}

template <typename CONFIG>
FunctionTypeWrapper<CONFIG>::~FunctionTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::FunctionType *FunctionTypeWrapper<CONFIG>::GetFunctionType() const
{
	return function_type;
}

template <typename CONFIG>
v8::Local<v8::Object> FunctionTypeWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	
	v8::Local<v8::Object> function_type_object = this->ThisObject();
	
	if(function_type_object.IsEmpty())
	{
		function_type_object = Base::template MakeObject<This>();
		
		struct FunctionTypeScanner
		{
			NodeJS<CONFIG>& nodejs;
			std::vector<v8::Local<v8::Value> > formal_parameter_objects;
			
			FunctionTypeScanner(NodeJS<CONFIG>& _nodejs) : nodejs(_nodejs), formal_parameter_objects() {}
			
			bool Visit(const unisim::util::debug::Type *type) { return false; }
			
			bool Visit(const unisim::util::debug::FormalParameter *formal_parameter)
			{
				FormalParameterWrapper<CONFIG> *formal_parameter_wrapper = FormalParameterWrapper<CONFIG>::Wrap(nodejs, formal_parameter);
				formal_parameter_objects.push_back(formal_parameter_wrapper->MakeObject().template As<v8::Value>());
				return false; // do not stop: continue with next member
			}
		};
		
		FunctionTypeScanner function_type_scanner(this->nodejs);
		this->function_type->Scan(function_type_scanner);
		
		// add "formalParameters" read-only property
		function_type_object->DefineOwnProperty(
			this->GetIsolate()->GetCurrentContext(),
			v8::String::NewFromUtf8Literal(this->GetIsolate(), "formalParameters", v8::NewStringType::kInternalized),
			v8::Array::New(this->GetIsolate(), &function_type_scanner.formal_parameter_objects[0], function_type_scanner.formal_parameter_objects.size()),
			v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
		).ToChecked();
	}
	
	return handle_scope.Escape(function_type_object);
}

template <typename CONFIG>
void FunctionTypeWrapper<CONFIG>::GetReturnType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(function_type)
	{
		const unisim::util::debug::Type *return_type = function_type->GetReturnType();
		if(return_type)
		{
			info.GetReturnValue().Set(TypeWrapper<CONFIG>::MakeObjectFromType(this->nodejs, return_type));
		}
	}
}

template <typename CONFIG>
void FunctionTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/function_type.h>
	;
}

//////////////////////////////// ConstTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *ConstTypeWrapper<CONFIG>::CLASS_NAME = "ConstType";

template <typename CONFIG>
const uint32_t ConstTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> ConstTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> const_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = const_type_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "type", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetType>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "Type"
	const_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(const_type_function_template);
}

// ConstType() => ConstType
template <typename CONFIG>
void ConstTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
ConstTypeWrapper<CONFIG> *ConstTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::ConstType *const_type)
{
	This *const_type_wrapper = nodejs.template GetObjectWrapper<This>(const_type);
	return const_type_wrapper ? const_type_wrapper : new ConstTypeWrapper<CONFIG>(nodejs, const_type);
}

template <typename CONFIG>
ConstTypeWrapper<CONFIG>::ConstTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::ConstType *_const_type, std::size_t size)
	: Super(_nodejs, _const_type, size ? size : sizeof(*this))
	, const_type(_const_type)
{
}

template <typename CONFIG>
ConstTypeWrapper<CONFIG>::~ConstTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::ConstType *ConstTypeWrapper<CONFIG>::GetConstType() const
{
	return const_type;
}

template <typename CONFIG>
void ConstTypeWrapper<CONFIG>::GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(const_type)
	{
		const unisim::util::debug::Type *type = const_type->GetType();
		if(type)
		{
			info.GetReturnValue().Set(TypeWrapper<CONFIG>::MakeObjectFromType(this->nodejs, type));
		}
	}
}

template <typename CONFIG>
void ConstTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/const_type.h>
	;
}

//////////////////////////////// EnumeratorWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *EnumeratorWrapper<CONFIG>::CLASS_NAME = "Enumerator";

template <typename CONFIG>
const uint32_t EnumeratorWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> EnumeratorWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> enumerator_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = enumerator_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "name" , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetName> , 0 },
		{ "value", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetValue>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	return handle_scope.Escape(enumerator_function_template);
}

// Enumerator() => Enumerator
template <typename CONFIG>
void EnumeratorWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
EnumeratorWrapper<CONFIG> *EnumeratorWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Enumerator *enumerator)
{
	This *enumerator_wrapper = nodejs.template GetObjectWrapper<This>(enumerator);
	return enumerator_wrapper ? enumerator_wrapper : new EnumeratorWrapper<CONFIG>(nodejs, enumerator);
}

template <typename CONFIG>
EnumeratorWrapper<CONFIG>::EnumeratorWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::Enumerator *_enumerator, std::size_t size)
	: Super(_nodejs, _enumerator, size ? size : sizeof(*this))
	, enumerator(_enumerator)
{
}

template <typename CONFIG>
EnumeratorWrapper<CONFIG>::~EnumeratorWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::Enumerator *EnumeratorWrapper<CONFIG>::GetEnumerator() const
{
	return enumerator;
}

template <typename CONFIG>
void EnumeratorWrapper<CONFIG>::GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(enumerator) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), enumerator->GetName()).ToLocalChecked());
}

template <typename CONFIG>
void EnumeratorWrapper<CONFIG>::GetValue(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(enumerator)
	{
		v8::Local<v8::Value> value = enumerator->IsSigned() ? MakeInteger(this->GetIsolate(), enumerator->GetSignedValue())
		                                                    : MakeInteger(this->GetIsolate(), enumerator->GetUnsignedValue());
		info.GetReturnValue().Set(value);
	}
}

template <typename CONFIG>
void EnumeratorWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/enumerator.h>
	;
}

//////////////////////////////// EnumTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *EnumTypeWrapper<CONFIG>::CLASS_NAME = "EnumType";

template <typename CONFIG>
const uint32_t EnumTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> EnumTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> enum_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Inherit from "NamedType"
	enum_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(enum_type_function_template);
}

// EnumType() => EnumType
template <typename CONFIG>
void EnumTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
EnumTypeWrapper<CONFIG> *EnumTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::EnumType *enum_type)
{
	This *enum_type_wrapper = nodejs.template GetObjectWrapper<This>(enum_type);
	return enum_type_wrapper ? enum_type_wrapper : new EnumTypeWrapper<CONFIG>(nodejs, enum_type);
}

template <typename CONFIG>
EnumTypeWrapper<CONFIG>::EnumTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::EnumType *_enum_type, std::size_t size)
	: Super(_nodejs, _enum_type, size ? size : sizeof(*this))
	, enum_type(_enum_type)
{
}

template <typename CONFIG>
EnumTypeWrapper<CONFIG>::~EnumTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::EnumType *EnumTypeWrapper<CONFIG>::GetEnumType() const
{
	return enum_type;
}

template <typename CONFIG>
v8::Local<v8::Object> EnumTypeWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	
	v8::Local<v8::Object> enum_type_object = this->ThisObject();
	
	if(enum_type_object.IsEmpty())
	{
		enum_type_object = Base::template MakeObject<This>();
		
		v8::Local<v8::Object> prop_enumerators = v8::Object::New(this->GetIsolate());
		
		struct EnumTypeScanner
		{
			NodeJS<CONFIG>& nodejs;
			v8::Local<v8::Object>& prop_enumerators;
			
			EnumTypeScanner(NodeJS<CONFIG>& _nodejs, v8::Local<v8::Object>& _prop_enumerators) : nodejs(_nodejs), prop_enumerators(_prop_enumerators) {}
			
			bool Visit(const unisim::util::debug::Enumerator *enumerator)
			{
				EnumeratorWrapper<CONFIG> *enumerator_wrapper = EnumeratorWrapper<CONFIG>::Wrap(nodejs, enumerator);
				// add "enumerators.<enumerator-name>" read-only property
				prop_enumerators->DefineOwnProperty(
					nodejs.GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8(nodejs.GetIsolate(), enumerator->GetName(), v8::NewStringType::kInternalized).ToLocalChecked(),
					enumerator_wrapper->MakeObject(),
					v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
				).ToChecked();
				return false; // do not stop: continue with next enumerator
			}
		};
		
		EnumTypeScanner enum_type_scanner(this->nodejs, prop_enumerators);
		this->enum_type->Scan(enum_type_scanner);
		
		// add "enumerators" read-only property
		enum_type_object->DefineOwnProperty(
			this->GetIsolate()->GetCurrentContext(),
			v8::String::NewFromUtf8Literal(this->GetIsolate(), "enumerators", v8::NewStringType::kInternalized),
			prop_enumerators,
			v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
		).ToChecked();
	}
	
	return handle_scope.Escape(enum_type_object);
}

template <typename CONFIG>
void EnumTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/enum_type.h>
	;
}

//////////////////////////////// UnspecifiedTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *UnspecifiedTypeWrapper<CONFIG>::CLASS_NAME = "UnspecifiedType";

template <typename CONFIG>
const uint32_t UnspecifiedTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> UnspecifiedTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> unspecified_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Inherit from "Type"
	unspecified_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(unspecified_type_function_template);
}

// UnspecifiedType() => UnspecifiedType
template <typename CONFIG>
void UnspecifiedTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
UnspecifiedTypeWrapper<CONFIG> *UnspecifiedTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::UnspecifiedType *unspecified_type)
{
	This *unspecified_type_wrapper = nodejs.template GetObjectWrapper<This>(unspecified_type);
	return unspecified_type_wrapper ? unspecified_type_wrapper : new UnspecifiedTypeWrapper<CONFIG>(nodejs, unspecified_type);
}

template <typename CONFIG>
UnspecifiedTypeWrapper<CONFIG>::UnspecifiedTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::UnspecifiedType *_unspecified_type, std::size_t size)
	: Super(_nodejs, _unspecified_type, size ? size : sizeof(*this))
	, unspecified_type(_unspecified_type)
{
}

template <typename CONFIG>
UnspecifiedTypeWrapper<CONFIG>::~UnspecifiedTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::UnspecifiedType *UnspecifiedTypeWrapper<CONFIG>::GetUnspecifiedType() const
{
	return unspecified_type;
}

template <typename CONFIG>
void UnspecifiedTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/unspecified_type.h>
	;
}

//////////////////////////////// VolatileTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *VolatileTypeWrapper<CONFIG>::CLASS_NAME = "VolatileType";

template <typename CONFIG>
const uint32_t VolatileTypeWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> VolatileTypeWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> volatile_type_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = volatile_type_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "type", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetType>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "Type"
	volatile_type_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<Super>());
	
	return handle_scope.Escape(volatile_type_function_template);
}

// VolatileType() => VolatileType
template <typename CONFIG>
void VolatileTypeWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
VolatileTypeWrapper<CONFIG> *VolatileTypeWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::VolatileType *volatile_type)
{
	This *volatile_type_wrapper = nodejs.template GetObjectWrapper<This>(volatile_type);
	return volatile_type_wrapper ? volatile_type_wrapper : new VolatileTypeWrapper<CONFIG>(nodejs, volatile_type);
}

template <typename CONFIG>
VolatileTypeWrapper<CONFIG>::VolatileTypeWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::VolatileType *_volatile_type, std::size_t size)
	: Super(_nodejs, _volatile_type, size ? size : sizeof(*this))
	, volatile_type(_volatile_type)
{
}

template <typename CONFIG>
VolatileTypeWrapper<CONFIG>::~VolatileTypeWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::VolatileType *VolatileTypeWrapper<CONFIG>::GetVolatileType() const
{
	return volatile_type;
}

template <typename CONFIG>
void VolatileTypeWrapper<CONFIG>::GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(volatile_type)
	{
		const unisim::util::debug::Type *type = volatile_type->GetType();
		if(type)
		{
			info.GetReturnValue().Set(TypeWrapper<CONFIG>::MakeObjectFromType(this->nodejs, type));
		}
	}
}

template <typename CONFIG>
void VolatileTypeWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/volatile_type.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_TYPE_TCC__
