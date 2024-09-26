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
v8::Local<v8::FunctionTemplate> StubWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> stub_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = stub_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "processor" , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetProcessor> , 0 },
		{ "subprogram", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetSubProgram>, 0 }
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
	v8::Local<v8::Template> prototype_template = stub_function_template->PrototypeTemplate();
	
	// Add methods
	struct { const char *method_name; v8::FunctionCallback callback; } methods_config[] =
	{
		{ "on"            , &unisim::util::nodejs::FunctionCallback<This, &This::On            > },
		{ "removeListener", &unisim::util::nodejs::FunctionCallback<This, &This::RemoveListener> }
	};
	for(auto method_config : methods_config)
	{
		prototype_template->Set(isolate, method_config.method_name, v8::FunctionTemplate::New(isolate, method_config.callback));
	}
	
	return handle_scope.Escape(stub_function_template);
}

// Stub() => Stub
// Stub(processor: Processor, subprogram: SubProgram) => Stub
template <typename CONFIG>
void StubWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	ProcessorWrapper<CONFIG> *processor_wrapper = 0;
	Stub<CONFIG> *stub = 0;
	
	if(args.Length() != 0)
	{
		struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + "(processor: Processor, file : string, name : string)"; } };
		
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a Processor for 'processor'"));
			return;
		}
		v8::Local<v8::Value> arg1 = args[1];
		SubProgramWrapper<CONFIG> *subprogram_wrapper = 0;
		if(!arg1->IsObject() || !(subprogram_wrapper = SubProgramWrapper<CONFIG>::GetInstance(arg1)))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a SubProgram for 'subprogram'"));
			return;
		}
		
		stub = new Stub<CONFIG>(nodejs, subprogram_wrapper->GetSubProgram());
	}
	
	StubWrapper<CONFIG> *stub_wrapper = new StubWrapper<CONFIG>(nodejs, processor_wrapper, stub);
	stub_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
StubWrapper<CONFIG>::StubWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG> *_processor_wrapper, Stub<CONFIG> *_stub, std::size_t size)
	: Super(_nodejs, /* ptr */ 0, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, stub(_stub)
	, function()
	, stub_set(false)
{
	if(stub)
	{
		_stub->Bind(this);
		stub->Catch();
	}
	if(processor_wrapper) processor_wrapper->Catch();
}

template <typename CONFIG>
StubWrapper<CONFIG>::~StubWrapper()
{
	Update(false);
	if(stub) stub->Release();
	if(processor_wrapper) processor_wrapper->Release();
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

		v8::Local<v8::Value> recv = this->ThisObject(); // "this"
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

template <typename CONFIG>
void StubWrapper<CONFIG>::GetProcessor(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(processor_wrapper)
	{
		info.GetReturnValue().Set(processor_wrapper->MakeObject());
	}
}

template <typename CONFIG>
void StubWrapper<CONFIG>::GetSubProgram(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(stub)
	{
		const unisim::util::debug::SubProgram<ADDRESS> *subprogram = stub->GetSubProgram();
		if(subprogram)
		{
			SubProgramWrapper<CONFIG> *subprogram_wrapper = SubProgramWrapper<CONFIG>::Wrap(this->nodejs, subprogram);
			info.GetReturnValue().Set(subprogram_wrapper->MakeObject());
		}
	}
}

// Stub.on(callback : function)
template <typename CONFIG>
void StubWrapper<CONFIG>::On(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(!stub || !processor_wrapper) return;
	
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0];
	if(!arg0->IsFunction())
	{
		this->Throw(this->Error(std::string(CLASS_NAME) + ".on() expects a function for 'callback'"));
		return;
	}

	if(!Update(true))
	{
		this->Throw(SetRemoveError());
		return;
	}
	
	v8::Local<v8::Function> function = arg0.As<v8::Function>();
	this->function.Reset(this->GetIsolate(), function);
	this->CatchObject(args.This()); // catch "this"
}

// Stub.removeListener(callback : function)
template <typename CONFIG>
void StubWrapper<CONFIG>::RemoveListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(!stub || !processor_wrapper) return;
	
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0];
	if(!arg0->IsFunction())
	{
		this->Throw(this->Error(std::string(CLASS_NAME) + ".removeListener() expects a function for 'callback'"));
		return;
	}
	
	v8::Local<v8::Function> function = arg0.As<v8::Function>();
	if(this->function != function)
	{
		return;
	}
	
	if(!Update(false))
	{
		this->Throw(SetRemoveError());
		return;
	}
	
	this->function.Reset();
	this->ReleaseObject(); // release "this"
	
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
bool StubWrapper<CONFIG>::Update(bool has_listeners)
{
	if(!stub || !processor_wrapper) return false;
	
	if(has_listeners && !stub_set)
	{
		if(this->IsVerbose())
		{
			this->DebugInfoStream() << "Setting " << (*stub) << std::endl;
		}
		if((*this->processor_wrapper)->SetStub(stub))
		{
			stub_set = true;
			return true;
		}
		
		return false;
	}
	else if(!has_listeners && stub_set)
	{
		if(this->IsVerbose())
		{
			this->DebugInfoStream() << "Removing " << (*stub) << std::endl;
		}
		if((*this->processor_wrapper)->RemoveStub(stub))
		{
			stub_set = false;
			return true;
		}
		
		return false;
	}
	
	return true;
}

template <typename CONFIG>
void StubWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/stub.h>
	;
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
