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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_STACK_FRAME_INFO_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_STACK_FRAME_INFO_TCC__

#include <unisim/service/debug/nodejs/stack_frame_info.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::nodejs::MakeInteger;

//////////////////////////////// StackFrameInfoWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *StackFrameInfoWrapper<CONFIG>::CLASS_NAME = "StackFrameInfo";

template <typename CONFIG>
const uint32_t StackFrameInfoWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> StackFrameInfoWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> stack_frame_info_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = stack_frame_info_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "id"           , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetId           >, 0 },
		{ "address"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetAddress      >, 0 },
		{ "pc"           , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetPC           >, 0 },
		{ "returnAddress", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetReturnAddress>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	return handle_scope.Escape(stack_frame_info_function_template);
}

// StackFrameInfo() => StackFrameInfo
template <typename CONFIG>
void StackFrameInfoWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
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
StackFrameInfoWrapper<CONFIG>::StackFrameInfoWrapper(NodeJS<CONFIG>& _nodejs, const unisim::service::interfaces::StackFrameInfo<ADDRESS>& _stack_frame_info, std::size_t size)
	: Super(_nodejs, /* ptr */ 0, size ? size : sizeof(*this))
	, stack_frame_info(_stack_frame_info)
{
}

template <typename CONFIG>
StackFrameInfoWrapper<CONFIG>::~StackFrameInfoWrapper()
{
}

template <typename CONFIG>
const unisim::service::interfaces::StackFrameInfo<typename CONFIG::ADDRESS>& StackFrameInfoWrapper<CONFIG>::GetStackFrameInfo() const
{
	return stack_frame_info;
}

template <typename CONFIG>
void StackFrameInfoWrapper<CONFIG>::GetId(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), stack_frame_info.frame_num));
}

template <typename CONFIG>
void StackFrameInfoWrapper<CONFIG>::GetAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), stack_frame_info.addr));
}

template <typename CONFIG>
void StackFrameInfoWrapper<CONFIG>::GetPC(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), stack_frame_info.pc));
}

template <typename CONFIG>
void StackFrameInfoWrapper<CONFIG>::GetReturnAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), stack_frame_info.ret_addr));
}

template <typename CONFIG>
void StackFrameInfoWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/stack_frame_info.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_STACK_FRAME_INFO_TCC__
