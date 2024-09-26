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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_DEBUG_EVENT_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_DEBUG_EVENT_TCC__

#include <unisim/service/debug/nodejs/debug_event.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

//////////////////////////////// DebugEventWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *DebugEventWrapper<CONFIG>::CLASS_NAME = "DebugEvent";

template <typename CONFIG>
const uint32_t DebugEventWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> DebugEventWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> debug_event_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);

	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = debug_event_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "processor", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetProcessor>, 0                                                                    },
		{ "enable"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetEnable>   , unisim::util::nodejs::AccessorSetterCallback<This, &This::SetEnable> }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	return handle_scope.Escape(debug_event_function_template);
}

// DebugEvent() => DebugEvent
template <typename CONFIG>
void DebugEventWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
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
DebugEventWrapper<CONFIG>::DebugEventWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG> *_processor_wrapper, unisim::util::debug::Event<ADDRESS> *_event, std::size_t size)
	: Super(_nodejs, /* ptr */ 0, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, event(_event)
	, event_bridge(_nodejs, _event)
{
}

template <typename CONFIG>
DebugEventWrapper<CONFIG>::~DebugEventWrapper()
{
}

template <typename CONFIG>
unisim::util::debug::Event<typename CONFIG::ADDRESS> *DebugEventWrapper<CONFIG>::GetEvent() const
{
	return event;
}

template <typename CONFIG>
void DebugEventWrapper<CONFIG>::GetProcessor(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(processor_wrapper)
	{
		info.GetReturnValue().Set(processor_wrapper->MakeObject());
	}
}

template <typename CONFIG>
void DebugEventWrapper<CONFIG>::GetEnable(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), event_bridge.Trap()));
}

template <typename CONFIG>
void DebugEventWrapper<CONFIG>::SetEnable(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	event_bridge.Trap(value->ToBoolean(this->GetIsolate())->Value());
}

template <typename CONFIG>
void DebugEventWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/debug_event.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_DEBUG_EVENT_TCC__
