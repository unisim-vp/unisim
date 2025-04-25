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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_FIELD_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_FIELD_TCC__

#include <unisim/service/debug/nodejs/field.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

//////////////////////////////// FieldWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *FieldWrapper<CONFIG>::CLASS_NAME = "Field";

template <typename CONFIG>
const uint32_t FieldWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> FieldWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> field_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = field_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "name"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetName       >, 0 },
		{ "description", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetDescription>, 0 },
		{ "bitOffset"  , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetBitOffset  >, 0 },
		{ "bitWidth"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetBitWidth   >, 0 }
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
	v8::Local<v8::Template> prototype_template = field_function_template->PrototypeTemplate();
	
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
	
	return handle_scope.Escape(field_function_template);
}

// Field() => Field
template <typename CONFIG>
void FieldWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
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
FieldWrapper<CONFIG> *FieldWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, unisim::service::interfaces::Field *field)
{
	This *field_wrapper = nodejs.template GetObjectWrapper<This>(field);
	return field_wrapper ? field_wrapper : new This(nodejs, field);
}

template <typename CONFIG>
FieldWrapper<CONFIG>::FieldWrapper(NodeJS<CONFIG>& _nodejs, unisim::service::interfaces::Field *_field, std::size_t size)
	: Super(_nodejs, _field, size ? size : sizeof(*this))
	, field(_field)
{
}

template <typename CONFIG>
FieldWrapper<CONFIG>::~FieldWrapper()
{
}

template <typename CONFIG>
unisim::service::interfaces::Field *FieldWrapper<CONFIG>::GetField() const
{
	return field;
}

template <typename CONFIG>
void FieldWrapper<CONFIG>::GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), field->GetName()).ToLocalChecked());
}

template <typename CONFIG>
void FieldWrapper<CONFIG>::GetDescription(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), field->GetDescription()).ToLocalChecked());
}

template <typename CONFIG>
void FieldWrapper<CONFIG>::GetBitOffset(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), field->GetBitOffset()));
}

template <typename CONFIG>
void FieldWrapper<CONFIG>::GetBitWidth(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), field->GetBitWidth()));
}

// Field.get() => Number
template <typename CONFIG>
void FieldWrapper<CONFIG>::Get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	uint64_t value = field->GetValue();
	args.GetReturnValue().Set(MakeInteger(this->GetIsolate(), value));
}

// Field.set(value: Number)
template <typename CONFIG>
void FieldWrapper<CONFIG>::Set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0]; // value
	uint64_t value = 0;
	if((!arg0->IsNumber() && !arg0->IsBigInt()) || !ToInt(args.GetIsolate(), arg0, value))
	{
		this->Throw(this->TypeError(std::string(CLASS_NAME) + ".set(value: Number) expects a number for 'value'"));
		return;
	}
	field->SetValue(value);
}


} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_FIELD_TCC__
