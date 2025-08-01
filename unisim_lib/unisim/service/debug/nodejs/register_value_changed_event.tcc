/*
 *  Copyright (c) 2025,
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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_REGISTER_VALUE_CHANGED_EVENT_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_REGISTER_VALUE_CHANGED_EVENT_TCC__

#include <unisim/service/debug/nodejs/register_value_changed_event.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

//////////////////////////////// RegisterValueChangedEventWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *RegisterValueChangedEventWrapper<CONFIG>::CLASS_NAME = "RegisterValueChangedEvent";

template <typename CONFIG>
const uint32_t RegisterValueChangedEventWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> RegisterValueChangedEventWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> register_value_changed_event_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = register_value_changed_event_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "registerName", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetRegisterName>, 0 }
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
	v8::Local<v8::Template> prototype_template = register_value_changed_event_function_template->PrototypeTemplate();
	
	// Add methods
	struct { const char *method_name; v8::FunctionCallback callback; } methods_config[] =
	{
		{ "on"            , &unisim::util::nodejs::FunctionCallback<This, &This::On            > },
		{ "removeListener", &unisim::util::nodejs::FunctionCallback<This, &This::RemoveListener> },
		{ "toString"      , &unisim::util::nodejs::FunctionCallback<This, &This::ToString      > }
	};
	for(auto method_config : methods_config)
	{
		prototype_template->Set(isolate, method_config.method_name, v8::FunctionTemplate::New(isolate, method_config.callback));
	}
	
	return handle_scope.Escape(register_value_changed_event_function_template);
}

// RegisterValueChangedEvent() => RegisterValueChangedEvent
// RegisterValueChangedEvent(processor: Processor, reg_name : string) => RegisterValueChangedEvent
template <typename CONFIG>
void RegisterValueChangedEventWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	ProcessorWrapper<CONFIG> *processor_wrapper = 0;
	unisim::util::debug::RegisterValueChangedEvent *reg_val_changed_event = 0;
	if(args.Length() != 0)
	{
		struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + "(processor: Processor, reg_name : string)"; } };
		
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a Processor for 'processor'"));
			return;
		}
		
		v8::Local<v8::Value> arg1 = args[1];
		
		std::string reg_name;
		if(arg1->IsNullOrUndefined() || !unisim::util::nodejs::ToString(args.GetIsolate(), arg1, reg_name))
		{
			nodejs.Throw(nodejs.Error(Synopsis().str() + " expects a string for 'reg_name'"));
			return;
		}
		
		reg_val_changed_event = processor_wrapper->GetProcessor()->CreateRegisterValueChangedEvent(reg_name.c_str());
	}
	
	RegisterValueChangedEventWrapper<CONFIG> *reg_val_changed_event_wrapper = new RegisterValueChangedEventWrapper<CONFIG>(nodejs, reg_val_changed_event);
	reg_val_changed_event_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
RegisterValueChangedEventWrapper<CONFIG>::RegisterValueChangedEventWrapper(NodeJS<CONFIG>& _nodejs, unisim::util::debug::RegisterValueChangedEvent *_reg_val_changed_event, std::size_t size)
	: Super(_nodejs, /* ptr */ 0, size ? size : sizeof(*this))
	, reg_val_changed_event(_reg_val_changed_event)
	, event_bridge(_nodejs, reg_val_changed_event)
{
}

template <typename CONFIG>
RegisterValueChangedEventWrapper<CONFIG>::~RegisterValueChangedEventWrapper()
{
}

template <typename CONFIG>
unisim::util::debug::RegisterValueChangedEvent *RegisterValueChangedEventWrapper<CONFIG>::GetRegisterValueChangedEvent() const
{
	return reg_val_changed_event;
}

template <typename CONFIG>
void RegisterValueChangedEventWrapper<CONFIG>::GetRegisterName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(reg_val_changed_event) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), reg_val_changed_event->GetRegister()->GetName()).ToLocalChecked());
}

template <typename CONFIG>
void RegisterValueChangedEventWrapper<CONFIG>::On(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(!reg_val_changed_event) return;
	
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0];
	
	if(!arg0->IsUndefined())
	{
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error("RegisterValueChangedEvent.on(callback : function) expects a function for 'callback'"));
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		if(!event_bridge.AddListener(function)) return;
		this->CatchObject(args.This()); // catch "this"
	}
	else
	{
		bool status = false;
		v8::Local<v8::Promise> promise = event_bridge.NewPromise(status);
		args.GetReturnValue().Set(promise);
	}
}

template <typename CONFIG>
void RegisterValueChangedEventWrapper<CONFIG>::RemoveListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(!reg_val_changed_event) return;
	
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0];
	
	if(!arg0->IsFunction())
	{
		this->Throw(this->Error("RegisterValueChangedEvent.removeListener(callback : function) expects a function for 'callback'"));
		return;
	}
	
	v8::Local<v8::Function> function = arg0.As<v8::Function>();
	if(!event_bridge.RemoveListener(function)) return;
	if(!event_bridge.HasListeners()) this->ReleaseObject(); // release "this"
}

template <typename CONFIG>
void RegisterValueChangedEventWrapper<CONFIG>::ToString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(reg_val_changed_event) args.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), unisim::util::ostream::ToString(*reg_val_changed_event).c_str()).ToLocalChecked());
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_REGISTER_VALUE_CHANGED_EVENT_TCC__
