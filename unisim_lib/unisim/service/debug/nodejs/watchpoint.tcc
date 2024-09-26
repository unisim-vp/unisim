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
#include <unisim/util/ostream/ostream.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::ostream::ToString;
using unisim::util::nodejs::MakeInteger;

//////////////////////////////// WatchpointWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *WatchpointWrapper<CONFIG>::CLASS_NAME = "Watchpoint";

template <typename CONFIG>
const uint32_t WatchpointWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> WatchpointWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> watchpoint_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = watchpoint_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "memoryAccessType", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetMemoryAccessType>, 0 },
		{ "memoryType"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetMemoryType      >, 0 },
		{ "address"         , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetAddress         >, 0 },
		{ "size"            , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetSize            >, 0 },
		{ "overlook"        , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetOverlook        >, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "DebugEvent"
	watchpoint_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<DebugEventWrapper<CONFIG> >());
	
	return handle_scope.Escape(watchpoint_function_template);
}

// Watchpoint(processor: Processor, addr : Number, size: Number, [options: { [memoryType: string], [memoryAccessType: string], [overlook: boolean] }]) => Watchpoint
template <typename CONFIG>
void WatchpointWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	ProcessorWrapper<CONFIG> *processor_wrapper = 0;
	unisim::util::debug::Watchpoint<ADDRESS> *watchpoint = 0;
	
	if(args.Length() != 0)
	{
		struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + "(processor: Processor, addr : Number, size: Number, [options: { [memoryType: string], [memoryAccessType: string], [overlook: boolean] }])"; } };
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a Processor for 'processor'"));
			return;
		}
		v8::Local<v8::Value> arg1 = args[1]; // addr
		ADDRESS addr = 0;
		if((!arg1->IsNumber() && !arg1->IsBigInt()) || !ToInt(args.GetIsolate(), arg1, addr))
		{
			nodejs.Throw(nodejs.Error(Synopsis().str() + " expects a number for 'addr'"));
			return;
		}
		v8::Local<v8::Value> arg2 = args[2]; // size
		uint32_t size = 0;
		if((!arg2->IsNumber() && !arg2->IsBigInt()) || !ToInt(args.GetIsolate(), arg2, size))
		{
			nodejs.Throw(nodejs.Error(Synopsis().str() + " expects a number for 'size'"));
			return;
		}
		unisim::util::debug::MemoryAccessType memory_access_type = unisim::util::debug::MAT_WRITE;
		unisim::util::debug::MemoryType memory_type = unisim::util::debug::MT_DATA;
		bool overlook = true;
		if(args.Length() >= 4)
		{
			v8::Local<v8::Value> arg3 = args[3]; // options
			if(!arg3->IsObject())
			{
				nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects an object for 'options'"));
				return;
			}
			v8::Local<v8::Object> obj_options = arg3.As<v8::Object>();
			if(obj_options->HasOwnProperty(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(args.GetIsolate(), "memoryAccessType")).ToChecked())
			{
				v8::Local<v8::Value> prop_options_mat;
				if(!obj_options->Get(
						args.GetIsolate()->GetCurrentContext(),
						v8::String::NewFromUtf8Literal(args.GetIsolate(), "memoryAccessType")
					).ToLocal(&prop_options_mat) || !prop_options_mat->IsString() || !ToMemoryAccessType(args.GetIsolate(), prop_options_mat, memory_access_type))
				{
					nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a string for property 'options.memoryAccessType'"));
					return;
				}
			}
			if(obj_options->HasOwnProperty(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(args.GetIsolate(), "memoryType")).ToChecked())
			{
				v8::Local<v8::Value> prop_options_mt;
				if(!obj_options->Get(
						args.GetIsolate()->GetCurrentContext(),
						v8::String::NewFromUtf8Literal(args.GetIsolate(), "memoryType")
					).ToLocal(&prop_options_mt) || !prop_options_mt->IsString() || !ToMemoryType(args.GetIsolate(), prop_options_mt, memory_type))
				{
					nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a string for property 'options.memoryType'"));
					return;
				}
			}
			if(obj_options->HasOwnProperty(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(args.GetIsolate(), "overlook")).ToChecked())
			{
				v8::Local<v8::Value> prop_options_overlook;
				if(!obj_options->Get(
					args.GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8Literal(args.GetIsolate(), "overlook")
				).ToLocal(&prop_options_overlook))
				{
					nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a boolean for property 'options.overlook'"));
					return;
				}
				overlook = prop_options_overlook->ToBoolean(args.GetIsolate())->Value();
			}
		}
		
		watchpoint = processor_wrapper->GetProcessor()->CreateWatchpoint(memory_access_type, memory_type, addr, size, overlook);
	}
	
	WatchpointWrapper<CONFIG> *watchpoint_wrapper = new WatchpointWrapper<CONFIG>(nodejs, processor_wrapper, watchpoint);
	watchpoint_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
WatchpointWrapper<CONFIG>::WatchpointWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG> *_processor_wrapper, unisim::util::debug::Watchpoint<ADDRESS> *_watchpoint, std::size_t size)
	: Super(_nodejs, _processor_wrapper, _watchpoint, size ? size : sizeof(*this))
	, watchpoint(_watchpoint)
{
}

template <typename CONFIG>
WatchpointWrapper<CONFIG>::~WatchpointWrapper()
{
}

template <typename CONFIG>
unisim::util::debug::Watchpoint<typename CONFIG::ADDRESS> *WatchpointWrapper<CONFIG>::GetWatchpoint() const
{
	return watchpoint;
}

template <typename CONFIG>
void WatchpointWrapper<CONFIG>::GetMemoryAccessType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), ToString(watchpoint->GetMemoryAccessType()).c_str()).ToLocalChecked());
}

template <typename CONFIG>
void WatchpointWrapper<CONFIG>::GetMemoryType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), ToString(watchpoint->GetMemoryType()).c_str()).ToLocalChecked());
}

template <typename CONFIG>
void WatchpointWrapper<CONFIG>::GetAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), watchpoint->GetAddress()));
}

template <typename CONFIG>
void WatchpointWrapper<CONFIG>::GetSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), watchpoint->GetSize()));
}

template <typename CONFIG>
void WatchpointWrapper<CONFIG>::GetOverlook(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), watchpoint->Overlooks()));
}

template <typename CONFIG>
void WatchpointWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/watchpoint.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_WATCHPOINT_TCC__
