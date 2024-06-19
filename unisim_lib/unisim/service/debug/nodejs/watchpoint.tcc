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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_WATCHPOINT_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_WATCHPOINT_TCC__

#include <unisim/service/debug/nodejs/watchpoint.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::nodejs::MakeInteger;

//////////////////////////////// WatchpointWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *WatchpointWrapper<CONFIG>::CLASS_NAME = "Watchpoint";

template <typename CONFIG>
const uint32_t WatchpointWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Global<v8::ObjectTemplate> WatchpointWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
WatchpointWrapper<CONFIG>::WatchpointWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG>& _processor_wrapper, unisim::util::debug::Watchpoint<ADDRESS> *_watchpoint, std::size_t size)
	: Super(_nodejs, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, watchpoint(_watchpoint)
	, event_bridge(_nodejs, _watchpoint, processor_wrapper.Recv())
	, shadow_object()
{
}

template <typename CONFIG>
WatchpointWrapper<CONFIG>::~WatchpointWrapper()
{
}

template <typename CONFIG>
void WatchpointWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void WatchpointWrapper<CONFIG>::Finalize()
{
	shadow_object.Reset();
	Super::Finalize();
}

template <typename CONFIG>
void WatchpointWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	// add "XXXX" method
	//object_template->Set(isolate, "xxxx", unisim::util::nodejs::CreateFunctionTemplate<WatchpointWrapper<CONFIG>, &WatchpointWrapper<CONFIG>::XXXX>(isolate));
	// add "enable" method
	object_template->Set(isolate, "enable", unisim::util::nodejs::CreateFunctionTemplate<WatchpointWrapper<CONFIG>, &WatchpointWrapper<CONFIG>::Enable>(isolate));
	// add "disable" method
	object_template->Set(isolate, "disable", unisim::util::nodejs::CreateFunctionTemplate<WatchpointWrapper<CONFIG>, &WatchpointWrapper<CONFIG>::Disable>(isolate));
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> WatchpointWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
{
	v8::EscapableHandleScope handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> object_template;
	
	if(cached_object_template.IsEmpty())
	{
		// create object template
		object_template = unisim::util::nodejs::CreateObjectTemplate(isolate);
		// fill object template
		FillObjectTemplate(isolate, object_template);
		// cache object template
		cached_object_template.Reset(isolate, object_template);
	}
	else
	{
		object_template = cached_object_template.Get(isolate);
	}
	
	return handle_scope.Escape(object_template);
}

template <typename CONFIG>
template <typename T>
v8::Local<v8::Object> WatchpointWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = Super::template MakeObject<T>(object_template);
	// add "yyyy" read-only property
	//object->DefineOwnProperty(
	//	this->GetIsolate()->GetCurrentContext(),
	//	v8::String::NewFromUtf8Literal(this->GetIsolate(), "yyyy", v8::NewStringType::kInternalized),
	//	v8::String::NewFromUtf8(this->GetIsolate(), "zzzz").ToLocalChecked(),
	//	v8::ReadOnly
	//).ToChecked();
	// add "mat" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "mat", v8::NewStringType::kInternalized),
		v8::String::NewFromUtf8(this->GetIsolate(), ToString(watchpoint->GetMemoryAccessType()).c_str()).ToLocalChecked(),
		v8::ReadOnly
 	).ToChecked();
	// add "mat" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "mt", v8::NewStringType::kInternalized),
		v8::String::NewFromUtf8(this->GetIsolate(), ToString(watchpoint->GetMemoryType()).c_str()).ToLocalChecked(),
		v8::ReadOnly
 	).ToChecked();
	// add "address" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "address", v8::NewStringType::kInternalized),
		MakeInteger(this->GetIsolate(), watchpoint->GetAddress()),
		v8::ReadOnly
 	).ToChecked();
	// add "size" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "size", v8::NewStringType::kInternalized),
		MakeInteger(this->GetIsolate(), watchpoint->GetSize()),
		v8::ReadOnly
 	).ToChecked();
	// add "overlook" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "overlook", v8::NewStringType::kInternalized),
		v8::Boolean::New(this->GetIsolate(), watchpoint->Overlooks()),
		v8::ReadOnly
 	).ToChecked();
	
	// keep object reference for setting "this" of callback
	shadow_object.Reset(this->GetIsolate(), object);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
v8::Local<v8::Object> WatchpointWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object template
	v8::Local<v8::ObjectTemplate> object_template = MakeObjectTemplate(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = this->template MakeObject<This>(object_template);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
unisim::util::debug::Watchpoint<typename CONFIG::ADDRESS> *WatchpointWrapper<CONFIG>::GetWatchpoint() const
{
	return watchpoint;
}

//template <typename CONFIG>
//void WatchpointWrapper<CONFIG>::XXXX(const v8::FunctionCallbackInfo<v8::Value>& args)
//{
//	v8::Local<v8::Value> recv = shadow_object.Get(this->GetIsolate()); // "this"
//}

template <typename CONFIG>
void WatchpointWrapper<CONFIG>::Enable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	event_bridge.Trap(true);
}

template <typename CONFIG>
void WatchpointWrapper<CONFIG>::Disable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	event_bridge.Trap(false);
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_WATCHPOINT_TCC__
