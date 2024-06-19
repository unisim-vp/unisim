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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_STUB_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_STUB_TCC__

#include <unisim/service/debug/nodejs/stub.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

///////////////////////////////////////////////////////////////////////////////
//                                Definitions                                //
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////// StubWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *StubWrapper<CONFIG>::CLASS_NAME = "Stub";

template <typename CONFIG>
const uint32_t StubWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Global<v8::ObjectTemplate> StubWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
StubWrapper<CONFIG>::StubWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG>& _processor_wrapper, Stub<CONFIG> *_stub, std::size_t size)
	: Super(_nodejs, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, stub(_stub)
	, function()
	, stub_set(false)
	, shadow_object()
{
	_stub->Bind(this);
	stub->Catch();
}

template <typename CONFIG>
StubWrapper<CONFIG>::~StubWrapper()
{
	stub->Release();
}

template <typename CONFIG>
void StubWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void StubWrapper<CONFIG>::Finalize()
{
	shadow_object.Reset();
	Super::Finalize();
}

template <typename CONFIG>
void StubWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	// add "on" method
	object_template->Set(isolate, "on", unisim::util::nodejs::CreateFunctionTemplate<StubWrapper<CONFIG>, &StubWrapper<CONFIG>::On>(isolate));
	// add "removeListener" method
	object_template->Set(isolate, "removeListener", unisim::util::nodejs::CreateFunctionTemplate<StubWrapper<CONFIG>, &StubWrapper<CONFIG>::RemoveListener>(isolate));
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> StubWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
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
v8::Local<v8::Object> StubWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = Super::template MakeObject<T>(object_template);
	// add "name" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "name", v8::NewStringType::kInternalized),
		v8::String::NewFromUtf8(this->GetIsolate(), this->GetStub()->GetSubProgram()->GetName()).ToLocalChecked(),
		v8::ReadOnly
	).ToChecked();
	
	// keep object reference for setting "this" of callback
	shadow_object.Reset(this->GetIsolate(), object);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
v8::Local<v8::Object> StubWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object template
	v8::Local<v8::ObjectTemplate> object_template = MakeObjectTemplate(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = this->template MakeObject<This>(object_template);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
unisim::util::debug::Stub<typename CONFIG::ADDRESS> *StubWrapper<CONFIG>::GetStub() const
{
	return stub;
}

template <typename CONFIG>
void StubWrapper<CONFIG>::Trigger(typename unisim::util::debug::Stub<ADDRESS>::Parameters& parameters)
{
	if(this->Killed()) return;
	
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << "StubWrapper::Trigger() of " << (*stub) << std::endl;
	}
	
	if(!function.IsEmpty())
	{
		v8::Locker locker(this->GetIsolate());
		v8::Isolate::Scope isolateScope(this->GetIsolate());
		v8::HandleScope handle_scope(this->GetIsolate());
		v8::Local<v8::Function> local_function = function.Get(this->GetIsolate());
		v8::Local<v8::Context> context = this->GetContext();
		v8::Context::Scope context_scope(context);
		
		std::vector<v8::Local<v8::Value> > args(parameters.Length());
		for(unsigned int i = 0; i < parameters.Length(); ++i)
		{
			DataObjectWrapper<CONFIG> *data_object = new DataObjectWrapper<CONFIG>(this->nodejs, this->processor_wrapper, parameters[i]);
			v8::Local<v8::Object> arg = data_object->MakeObject();
			args[i] = arg;
		}

		v8::Local<v8::Value> recv = shadow_object.Get(this->GetIsolate()); // "this"
		v8::Local<v8::Value> result;
		if(this->IsDebugging())
		{
			this->DebugInfoStream() << "Calling " << CLASS_NAME << ".on callback of" << (*stub) << std::endl;
		}
		if(local_function->Call(context, recv, args.size(), &args[0]).ToLocal(&result))
		{
			if(!result->IsUndefined())
			{
				typename unisim::util::debug::Stub<ADDRESS>::ReturnValue& return_value = parameters.GetReturnValue();
				if(!return_value.IsUndefined())
				{
					DataObjectWrapper<CONFIG> data_object = DataObjectWrapper<CONFIG>(this->nodejs, this->processor_wrapper, return_value);
					data_object.Set(result);
				}
			}
		}
		else
		{
			this->nodejs.Kill();
		}
	}
}

// Stub.on(callback : function)
template <typename CONFIG>
void StubWrapper<CONFIG>::On(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << CLASS_NAME << ".on() of " << (*stub) << std::endl;
	}
	
	v8::Local<v8::Value> arg0 = args[0];
	if(!arg0->IsFunction())
	{
		this->Throw(this->Error(std::string(CLASS_NAME) + ".on() expects a function as argument"));
		return;
	}

	v8::Local<v8::Function> local_function = arg0.As<v8::Function>();
	function.Reset(this->GetIsolate(), local_function);
	if(!Update())
	{
		this->Throw(SetRemoveError());
	}
}

// Stub.removeListener(callback : function)
template <typename CONFIG>
void StubWrapper<CONFIG>::RemoveListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << CLASS_NAME << ".removeListener() of " << (*stub) << std::endl;
	}

	if(args.Length() != 1)
	{
		this->Throw(this->Error(std::string(CLASS_NAME) + ".removeListener() expects one argument"));
		return;
	}

	v8::HandleScope handle_scope(args.GetIsolate());
	
	v8::Local<v8::Value> arg0 = args[0];
	if(!arg0->IsFunction())
	{
		this->Throw(this->Error(std::string(CLASS_NAME) + ".removeListener() expects a function as argument"));
		return;
	}
	
	v8::Local<v8::Function> local_function = arg0.As<v8::Function>();
	if(function == local_function)
	{
		function.Reset();
	}
	
	if(!Update())
	{
		this->Throw(SetRemoveError());
	}
}

template <typename CONFIG>
v8::Local<v8::Value> StubWrapper<CONFIG>::SetRemoveError()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	std::ostringstream err_msg;
	err_msg << "Can't " << (stub_set ? "remove" : "set") << " " << (*stub);
	return this->Error(err_msg.str());
}

template <typename CONFIG>
bool StubWrapper<CONFIG>::Update()
{
	if(!function.IsEmpty() && !stub_set)
	{
		if(this->IsVerbose())
		{
			this->DebugInfoStream() << "Setting " << (*stub) << std::endl;
		}
		if(this->processor_wrapper->SetStub(stub))
		{
			stub_set = true;
			return true;
		}
		
		return false;
	}
	else if(function.IsEmpty() && stub_set)
	{
		if(this->IsVerbose())
		{
			this->DebugInfoStream() << "Removing " << (*stub) << std::endl;
		}
		if(this->processor_wrapper->RemoveStub(stub))
		{
			stub_set = false;
			return true;
		}
		
		return false;
	}
	
	return true;
}

//////////////////////////////////// Stub<> ////////////////////////////////////

template <typename CONFIG>
Stub<CONFIG>::Stub(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::SubProgram<ADDRESS> *_subprogram)
	: unisim::util::debug::Stub<ADDRESS>(_subprogram)
	, stub_wrapper(0)
{
}

template <typename CONFIG>
void Stub<CONFIG>::Bind(StubWrapper<CONFIG> *_stub_wrapper)
{
	stub_wrapper = _stub_wrapper;
}

template <typename CONFIG>
void Stub<CONFIG>::Run(typename unisim::util::debug::Stub<ADDRESS>::Parameters& parameters)
{
	stub_wrapper->Trigger(parameters);
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_STUB_TCC__
