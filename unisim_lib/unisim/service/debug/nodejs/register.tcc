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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_REGISTER_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_REGISTER_TCC__

#include <unisim/service/debug/nodejs/register.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::nodejs::MakeInteger;
using unisim::util::nodejs::ToInt;

//////////////////////////////// RegisterWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *RegisterWrapper<CONFIG>::CLASS_NAME = "Register";

template <typename CONFIG>
const uint32_t RegisterWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> RegisterWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> register_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = register_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "name"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetName       >, 0 },
		{ "description", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetDescription>, 0 },
		{ "size"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetSize       >, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Get the prototype template
	v8::Local<v8::Template> prototype_template = register_function_template->PrototypeTemplate();
	
	// Add methods
	struct { const char *method_name; v8::FunctionCallback callback; } methods_config[] =
	{
		{ "get", &unisim::util::nodejs::FunctionCallback<This, &This::Get> },
		{ "set", &unisim::util::nodejs::FunctionCallback<This, &This::Set> }
	};
	for(auto method_config : methods_config)
	{
		prototype_template->Set(isolate, method_config.method_name, v8::FunctionTemplate::New(isolate, method_config.callback));
	}
	
	return handle_scope.Escape(register_function_template);
}

// Register() => Register
template <typename CONFIG>
void RegisterWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
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
RegisterWrapper<CONFIG> *RegisterWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, unisim::service::interfaces::Register *reg)
{
	This *register_wrapper = nodejs.template GetObjectWrapper<This>(reg);
	return register_wrapper ? register_wrapper : new This(nodejs, reg);
}

template <typename CONFIG>
RegisterWrapper<CONFIG>::RegisterWrapper(NodeJS<CONFIG>& _nodejs, unisim::service::interfaces::Register *_reg, std::size_t size)
	: Super(_nodejs, _reg, size ? size : sizeof(*this))
	, reg(_reg)
{
}

template <typename CONFIG>
RegisterWrapper<CONFIG>::~RegisterWrapper()
{
}

template <typename CONFIG>
v8::Local<v8::Object> RegisterWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	
	v8::Local<v8::Object> register_object = this->ThisObject();
	
	if(register_object.IsEmpty())
	{
		register_object = Super::template MakePersistentObject<This>();
	
		v8::Local<v8::Object> prop_fields = v8::Object::New(this->GetIsolate());
		
		struct FieldScanner : unisim::service::interfaces::FieldScanner
		{
			NodeJS<CONFIG>& nodejs;
			v8::Local<v8::Object>& prop_fields;
			
			FieldScanner(NodeJS<CONFIG>& _nodejs, v8::Local<v8::Object>& _prop_fields) : nodejs(_nodejs), prop_fields(_prop_fields) {}
			
			virtual void Append(unisim::service::interfaces::Field* field)
			{
				FieldWrapper<CONFIG> *field_wrapper = FieldWrapper<CONFIG>::Wrap(nodejs, field);
				// add "fields.<fieldname>" property
				prop_fields->DefineOwnProperty(
					nodejs.GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8(nodejs.GetIsolate(), field->GetName(), v8::NewStringType::kInternalized).ToLocalChecked(),
					field_wrapper->MakeObject(),
					v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
				).ToChecked();
			}
		} field_scanner(this->nodejs, prop_fields);
		
		this->reg->ScanFields(field_scanner);
		
		// add "fields" property
		register_object->DefineOwnProperty(
			this->GetIsolate()->GetCurrentContext(),
			v8::String::NewFromUtf8Literal(this->GetIsolate(), "fields", v8::NewStringType::kInternalized),
			prop_fields,
			v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
		).ToChecked();
	}
	
	return handle_scope.Escape(register_object);
}

template <typename CONFIG>
unisim::service::interfaces::Register *RegisterWrapper<CONFIG>::GetRegister() const
{
	return reg;
}

template <typename CONFIG>
void RegisterWrapper<CONFIG>::GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), reg->GetName()).ToLocalChecked());
}

template <typename CONFIG>
void RegisterWrapper<CONFIG>::GetDescription(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), reg->GetDescription()).ToLocalChecked());
}

template <typename CONFIG>
void RegisterWrapper<CONFIG>::GetSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), reg->GetSize()));
}

// Register.get() => Number
template <typename CONFIG>
void RegisterWrapper<CONFIG>::Get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	bool is_signed = false;
	v8::Local<v8::Value> arg0 = args[0];
	if(!arg0->IsUndefined())
	{
		if(!arg0->IsObject())
		{
			this->Throw(this->TypeError(std::string(CLASS_NAME) + ".get([options : Object]) expects an object as 'options'"));
			return;
		}
		v8::Local<v8::Object> get_options = arg0.As<v8::Object>();
		is_signed = get_options->Get(
			args.GetIsolate()->GetCurrentContext(),
			v8::String::NewFromUtf8Literal(args.GetIsolate(), "signed")
		).ToLocalChecked()->ToBoolean(args.GetIsolate())->Value();
	}
	unsigned reg_size = reg->GetSize();
	uint8_t value[reg_size];
	reg->GetValue(value);
	args.GetReturnValue().Set(MakeInteger(args.GetIsolate(), value, reg_size, is_signed));
}

// Register.set(value: Number)
template <typename CONFIG>
void RegisterWrapper<CONFIG>::Set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0]; // value
	unsigned reg_size = reg->GetSize();
	uint8_t value[reg_size];
	if((!arg0->IsNumber() && !arg0->IsBigInt()) || !ToInt(args.GetIsolate(), arg0, value, reg_size))
	{
		this->Throw(this->TypeError(std::string(CLASS_NAME) + ".set(value: Number) expects a number for 'value'"));
		return;
	}
	reg->SetValue(value);
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_REGISTER_TCC__
