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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_POINTER_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_POINTER_TCC__

#include <unisim/service/debug/nodejs/pointer.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::nodejs::ToInt;

///////////////////////////////////////////////////////////////////////////////
//                                Definitions                                //
///////////////////////////////////////////////////////////////////////////////

////////////////////////////// PointerWrapper<> ////////////////////////////////

template <typename CONFIG>
const char *PointerWrapper<CONFIG>::CLASS_NAME = "Pointer";

template <typename CONFIG>
const uint32_t PointerWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Global<v8::ObjectTemplate> PointerWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
PointerWrapper<CONFIG>::PointerWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG>& _processor_wrapper, unisim::util::debug::DataObjectRef<ADDRESS> _pointer_data_object)
	: Super(_nodejs)
	, shadow_object()
	, processor_wrapper(_processor_wrapper)
	, pointer_data_object(_pointer_data_object)
{
}

template <typename CONFIG>
void PointerWrapper<CONFIG>::Set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0];
	ADDRESS addr = 0;
	
	DataObjectWrapper<CONFIG> *source_object_wrapper = DataObjectWrapper<CONFIG>::GetInstance(arg0);
	
	if(source_object_wrapper)
	{
		const unisim::util::debug::DataObjectRef<ADDRESS>& source_data_object = source_object_wrapper->GetDataObject();
		addr = source_data_object;
	}
	else if(!ToInt(args.GetIsolate(), arg0, addr))
	{
		this->Throw(this->TypeError("not an address"));
		return;
	}
	
	pointer_data_object = addr;
}

template <typename CONFIG>
void PointerWrapper<CONFIG>::Get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	ADDRESS addr = pointer_data_object;
	v8::Local<v8::Value> address;
	if(std::numeric_limits<ADDRESS>::max() > std::numeric_limits<uint32_t>::max())
	{
		address = v8::BigInt::NewFromUnsigned(this->GetIsolate(), addr);
	}
	else
	{
		address = v8::Integer::NewFromUnsigned(this->GetIsolate(), uint32_t(addr));
	}
	args.GetReturnValue().Set(address);
}

template <typename CONFIG>
void PointerWrapper<CONFIG>::Deref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	DataObjectWrapper<CONFIG> *target_object_wrapper = new DataObjectWrapper<CONFIG>(this->nodejs, this->processor_wrapper, *pointer_data_object);
	args.GetReturnValue().Set(target_object_wrapper->MakeObject());
}

template <typename CONFIG>
void PointerWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void PointerWrapper<CONFIG>::Finalize()
{
	shadow_object.Reset();
	Super::Finalize();
}

template <typename CONFIG>
void PointerWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	// add "set" method
	object_template->Set(isolate, "set", unisim::util::nodejs::CreateFunctionTemplate<PointerWrapper<CONFIG>, &PointerWrapper<CONFIG>::Set>(isolate));
	// add "get" method
	object_template->Set(isolate, "get", unisim::util::nodejs::CreateFunctionTemplate<PointerWrapper<CONFIG>, &PointerWrapper<CONFIG>::Get>(isolate));
	// add "deref" method
	object_template->Set(isolate, "deref", unisim::util::nodejs::CreateFunctionTemplate<PointerWrapper<CONFIG>, &PointerWrapper<CONFIG>::Deref>(isolate));
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> PointerWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
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
v8::Local<v8::Object> PointerWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = Super::template MakeObject<T>(object_template);
	// keep object reference for setting "this" of callback
	shadow_object.Reset(this->GetIsolate(), object);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
v8::Local<v8::Object> PointerWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object template
	v8::Local<v8::ObjectTemplate> object_template = MakeObjectTemplate(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = this->template MakeObject<This>(object_template);
	
	return handle_scope.Escape(object);
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_POINTER_TCC__
