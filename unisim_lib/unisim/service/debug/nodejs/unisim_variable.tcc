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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_UNISIM_VARIABLE_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_UNISIM_VARIABLE_TCC__

#include <unisim/service/debug/nodejs/unisim_variable.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::nodejs::MakeInteger;
using unisim::util::nodejs::ToInt;
using unisim::util::nodejs::ToString;

//////////////////////////////// UnisimVariableWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *UnisimVariableWrapper<CONFIG>::CLASS_NAME = "UnisimVariable";

template <typename CONFIG>
const uint32_t UnisimVariableWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> UnisimVariableWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> unisim_variable_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = unisim_variable_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "name"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetName       >, 0 },
		{ "description", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetDescription>, 0 },
		{ "type"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetType       >, 0 },
		{ "dataType"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetDataType   >, 0 },
		{ "enum"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetEnum       >, 0 },
		{ "value"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetValue      >, unisim::util::nodejs::AccessorSetterCallback<This, &This::SetValue> }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	return handle_scope.Escape(unisim_variable_function_template);
}

// UnisimVariable() => UnisimVariable
template <typename CONFIG>
void UnisimVariableWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
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
UnisimVariableWrapper<CONFIG> *UnisimVariableWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, unisim::kernel::VariableBase *unisim_variable)
{
	This *unisim_variable_wrapper = nodejs.template GetObjectWrapper<This>(unisim_variable);
	return unisim_variable_wrapper ? unisim_variable_wrapper : new This(nodejs, unisim_variable);
}

template <typename CONFIG>
UnisimVariableWrapper<CONFIG>::UnisimVariableWrapper(NodeJS<CONFIG>& _nodejs, unisim::kernel::VariableBase *_unisim_variable, std::size_t size)
	: Super(_nodejs, _unisim_variable, size ? size : sizeof(*this))
	, unisim_variable(_unisim_variable)
{
}

template <typename CONFIG>
UnisimVariableWrapper<CONFIG>::~UnisimVariableWrapper()
{
}

template <typename CONFIG>
unisim::kernel::VariableBase *UnisimVariableWrapper<CONFIG>::GetUnisimVariable() const
{
	return unisim_variable;
}

template <typename CONFIG>
void UnisimVariableWrapper<CONFIG>::GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(unisim_variable) info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), unisim_variable->GetName()).ToLocalChecked());
}

template <typename CONFIG>
void UnisimVariableWrapper<CONFIG>::GetDescription(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(unisim_variable) info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), unisim_variable->GetDescription()).ToLocalChecked());
}

template <typename CONFIG>
void UnisimVariableWrapper<CONFIG>::GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(unisim_variable) info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), unisim_variable->GetTypeName()).ToLocalChecked());
}

template <typename CONFIG>
void UnisimVariableWrapper<CONFIG>::GetDataType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(unisim_variable) info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), unisim_variable->GetDataTypeName()).ToLocalChecked());
}

template <typename CONFIG>
void UnisimVariableWrapper<CONFIG>::GetEnum(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(unisim_variable)
	{
		struct EnumValueVisitor
		{
			const v8::PropertyCallbackInfo<v8::Value>& info;
			std::vector<v8::Local<v8::Value> > enum_values;
			
			EnumValueVisitor(const v8::PropertyCallbackInfo<v8::Value>& _info) : info(_info) {}
			
			bool Visit(const std::string& enum_value)
			{
				enum_values.push_back(v8::String::NewFromUtf8(info.GetIsolate(), enum_value.c_str()).ToLocalChecked().template As<v8::Value>());
				return false;
			}
		};
		
		EnumValueVisitor enum_value_visitor(info);
		unisim_variable->ScanEnumeratedValues(enum_value_visitor);
		
		info.GetReturnValue().Set(v8::Array::New(info.GetIsolate(), &enum_value_visitor.enum_values[0], enum_value_visitor.enum_values.size()));
	}
}

template <typename CONFIG>
void UnisimVariableWrapper<CONFIG>::GetValue(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(unisim_variable)
	{
		switch(unisim_variable->GetDataType())
		{
			case unisim::kernel::VariableBase::DT_BOOL      : info.GetReturnValue().Set(v8::Boolean::New(info.GetIsolate(), (bool) *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_SCHAR     : info.GetReturnValue().Set(MakeInteger(info.GetIsolate(), (signed char)        *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_SHORT     : info.GetReturnValue().Set(MakeInteger(info.GetIsolate(), (short)              *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_INT       : info.GetReturnValue().Set(MakeInteger(info.GetIsolate(), (int)                *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_LONG      : info.GetReturnValue().Set(MakeInteger(info.GetIsolate(), (long)               *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_LONG_LONG : info.GetReturnValue().Set(MakeInteger(info.GetIsolate(), (long long)          *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_UCHAR     : info.GetReturnValue().Set(MakeInteger(info.GetIsolate(), (unsigned char)      *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_USHORT    : info.GetReturnValue().Set(MakeInteger(info.GetIsolate(), (unsigned short)     *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_UINT      : info.GetReturnValue().Set(MakeInteger(info.GetIsolate(), (unsigned int)       *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_ULONG     : info.GetReturnValue().Set(MakeInteger(info.GetIsolate(), (unsigned long)      *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_ULONG_LONG: info.GetReturnValue().Set(MakeInteger(info.GetIsolate(), (unsigned long long) *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_FLOAT     : info.GetReturnValue().Set(v8::Number::New(info.GetIsolate(), (float)  *unisim_variable)); break;
			case unisim::kernel::VariableBase::DT_DOUBLE    : info.GetReturnValue().Set(v8::Number::New(info.GetIsolate(), (double) *unisim_variable)); break;
			default                                         : info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), ((std::string) *unisim_variable).c_str()).ToLocalChecked()); break;
		}
	}
}

template <typename CONFIG>
void UnisimVariableWrapper<CONFIG>::SetValue(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	if(unisim_variable)
	{
		switch(unisim_variable->GetDataType())
		{
			case unisim::kernel::VariableBase::DT_BOOL      : (*unisim_variable) = value->ToBoolean(info.GetIsolate())->Value(); break;
			case unisim::kernel::VariableBase::DT_SCHAR     : { signed char        _value = 0; if(ToInt(info.GetIsolate(), value, _value)) { (*unisim_variable) = _value; }; break; }
			case unisim::kernel::VariableBase::DT_SHORT     : { short              _value = 0; if(ToInt(info.GetIsolate(), value, _value)) { (*unisim_variable) = _value; }; break; }
			case unisim::kernel::VariableBase::DT_INT       : { int                _value = 0; if(ToInt(info.GetIsolate(), value, _value)) { (*unisim_variable) = _value; }; break; }
			case unisim::kernel::VariableBase::DT_LONG      : { long               _value = 0; if(ToInt(info.GetIsolate(), value, _value)) { (*unisim_variable) = _value; }; break; }
			case unisim::kernel::VariableBase::DT_LONG_LONG : { long long          _value = 0; if(ToInt(info.GetIsolate(), value, _value)) { (*unisim_variable) = _value; }; break; }
			case unisim::kernel::VariableBase::DT_UCHAR     : { unsigned char      _value = 0; if(ToInt(info.GetIsolate(), value, _value)) { (*unisim_variable) = _value; }; break; }
			case unisim::kernel::VariableBase::DT_USHORT    : { unsigned short     _value = 0; if(ToInt(info.GetIsolate(), value, _value)) { (*unisim_variable) = _value; }; break; }
			case unisim::kernel::VariableBase::DT_UINT      : { unsigned int       _value = 0; if(ToInt(info.GetIsolate(), value, _value)) { (*unisim_variable) = _value; }; break; }
			case unisim::kernel::VariableBase::DT_ULONG     : { unsigned long      _value = 0; if(ToInt(info.GetIsolate(), value, _value)) { (*unisim_variable) = _value; }; break; }
			case unisim::kernel::VariableBase::DT_ULONG_LONG: { unsigned long long _value = 0; if(ToInt(info.GetIsolate(), value, _value)) { (*unisim_variable) = _value; }; break; }
			case unisim::kernel::VariableBase::DT_FLOAT     :
			case unisim::kernel::VariableBase::DT_DOUBLE    : (*unisim_variable) = value->ToNumber(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Value(); break;
			default                                         : { std::string _value; if(ToString(info.GetIsolate(), value, _value)) { (*unisim_variable) = _value.c_str(); }; break; }
		}
	}
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_UNISIM_VARIABLE_TCC__
