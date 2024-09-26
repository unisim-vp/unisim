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
v8::Local<v8::FunctionTemplate> PointerWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> pointer_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the prototype template
	v8::Local<v8::Template> prototype_template = pointer_function_template->PrototypeTemplate();
	
	// Add methods
	struct { const char *method_name; v8::FunctionCallback callback; } methods_config[] =
	{
		{ "set"  , &unisim::util::nodejs::FunctionCallback<This, &This::Set  > },
		{ "get"  , &unisim::util::nodejs::FunctionCallback<This, &This::Get  > },
		{ "deref", &unisim::util::nodejs::FunctionCallback<This, &This::Deref> }
	};
	for(auto method_config : methods_config)
	{
		prototype_template->Set(isolate, method_config.method_name, v8::FunctionTemplate::New(isolate, method_config.callback));
	}
	
	return handle_scope.Escape(pointer_function_template);
}

// Pointer() => Pointer
template <typename CONFIG>
void PointerWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	ProcessorWrapper<CONFIG> *processor_wrapper = 0;
	unisim::util::debug::DataObjectRef<ADDRESS> pointer_data_object;
	PointerWrapper<CONFIG> *pointer_wrapper = new PointerWrapper<CONFIG>(nodejs, processor_wrapper, pointer_data_object);
	pointer_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
PointerWrapper<CONFIG>::PointerWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG> *_processor_wrapper, unisim::util::debug::DataObjectRef<ADDRESS> _pointer_data_object, std::size_t size)
	: Super(_nodejs, /* ptr */ 0, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, pointer_data_object(_pointer_data_object)
{
}

// Pointer.set(value: Number)
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

// Pointer.get() => Number
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

// Pointer.deref() => DataObject
template <typename CONFIG>
void PointerWrapper<CONFIG>::Deref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	DataObjectWrapper<CONFIG> *target_object_wrapper = new DataObjectWrapper<CONFIG>(this->nodejs, this->processor_wrapper, *pointer_data_object);
	args.GetReturnValue().Set(target_object_wrapper->MakeObject());
}

template <typename CONFIG>
void PointerWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/pointer.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_POINTER_TCC__
