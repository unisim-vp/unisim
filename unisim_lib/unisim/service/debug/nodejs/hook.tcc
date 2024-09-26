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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_HOOK_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_HOOK_TCC__

#include <unisim/service/debug/nodejs/hook.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::nodejs::MakeInteger;

///////////////////////////////////////////////////////////////////////////////
//                                Definitions                                //
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////// HookWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *HookWrapper<CONFIG>::CLASS_NAME = "Hook";

template <typename CONFIG>
const uint32_t HookWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> HookWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> hook_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = hook_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "processor", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetProcessor>, 0 }
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
	v8::Local<v8::Template> prototype_template = hook_function_template->PrototypeTemplate();
	
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
	
	return handle_scope.Escape(hook_function_template);
}

// Hook() => Hook
template <typename CONFIG>
void HookWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
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
HookWrapper<CONFIG>::HookWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG> *_processor_wrapper, unisim::util::debug::Hook<ADDRESS> *_hook, std::size_t size)
	: Super(_nodejs, /* ptr */ 0, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, hook(_hook)
	, functions()
	, resolvers()
	, hook_set(false)
{
	if(hook) hook->Catch();
	if(processor_wrapper) processor_wrapper->Catch();
}

template <typename CONFIG>
HookWrapper<CONFIG>::~HookWrapper()
{
	Update(false);
	if(hook) hook->Release();
	if(processor_wrapper) processor_wrapper->Release();
}

template <typename CONFIG>
unisim::util::debug::Hook<typename CONFIG::ADDRESS> *HookWrapper<CONFIG>::GetHook() const
{
	return hook;
}

template <typename CONFIG>
void HookWrapper<CONFIG>::Trigger()
{
	if(this->Killed()) return;
	
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << "Hook::Trigger() of " << (*hook) << std::endl;
	}
	
	if(functions.empty() && resolvers.empty())
	{
		if(this->IsDebugging())
		{
			this->DebugInfoStream() << "No event listener" << std::endl;
		}
		return;
	}
	
	v8::Locker locker(this->GetIsolate());
	v8::Isolate::Scope isolateScope(this->GetIsolate());
	v8::HandleScope handle_scope(this->GetIsolate());
	v8::Local<v8::Context> context = this->GetContext();
	v8::Context::Scope context_scope(context);
	
	// beware of reentrancy
	typedef std::vector<v8::Local<v8::Function> > LocalFunctions;
	LocalFunctions local_functions;
	
	for(Functions::iterator it = functions.begin(); it != functions.end(); ++it)
	{
		v8::Global<v8::Function>& function = *it;
		local_functions.push_back(function.Get(this->GetIsolate()));
	}
	
	for(LocalFunctions::iterator it = local_functions.begin(); it != local_functions.end() && !this->Killed(); ++it)
	{
		v8::Local<v8::Function>& local_function = *it;
		v8::Local<v8::Value> recv = this->ThisObject(); // "this"
		v8::Local<v8::Value> result;
		if(this->IsDebugging())
		{
			this->DebugInfoStream() << "Calling Hook.on callback of " << (*hook) << std::endl;
		}
		if(!local_function->Call(context, recv, 0, (v8::Local<v8::Value> *) 0).ToLocal(&result))
		{
			this->nodejs.Kill();
		}
	}

	if(!resolvers.empty())
	{
		if(Update(!functions.empty()))
		{
			Resolve(v8::Undefined(this->GetIsolate()));
		}
		else
		{
			Reject(SetRemoveError());
		}
	}
}

template <typename CONFIG>
void HookWrapper<CONFIG>::GetProcessor(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(processor_wrapper)
	{
		info.GetReturnValue().Set(processor_wrapper->MakeObject());
	}
}

// Hook.on(callback : function)
// Hook.on() => Promise
template <typename CONFIG>
void HookWrapper<CONFIG>::On(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(!hook || !processor_wrapper) return;
	
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Context> context = args.GetIsolate()->GetCurrentContext();
	v8::Local<v8::Value> arg0 = args[0];
	
	if(!arg0->IsUndefined())
	{
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error("Hook.on(callback : function) expects a function for 'callback'"));
			return;
		}
		if(!Update(true))
		{
			this->Throw(SetRemoveError());
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		functions.resize(functions.size() + 1);
		functions.back().Reset(this->GetIsolate(), function);
		this->CatchObject(args.This()); // catch "this"
	}
	else
	{
		if(!Update(true))
		{
			Reject(SetRemoveError());
			return;
		}
		v8::Local<v8::Promise::Resolver> resolver;
		if(!v8::Promise::Resolver::New(context).ToLocal(&resolver))
		{
			this->DebugErrorStream() << "Can't create Javascript promise resolver" << std::endl;
			this->nodejs.Kill();
			return;
		}
		resolvers.resize(resolvers.size() + 1);
		resolvers.back().Reset(args.GetIsolate(), resolver);
		args.GetReturnValue().Set(resolver->GetPromise());
	}
}

// Hook.removeListener(callback : function)
template <typename CONFIG>
void HookWrapper<CONFIG>::RemoveListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(!hook || !processor_wrapper) return;
	
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0];
	
	if(!arg0->IsFunction())
	{
		this->Throw(this->Error("Hook.removeListener(callback : function) expects a function for 'callback'"));
		return;
	}
	
	if(!Update(!resolvers.empty()))
	{
		this->Throw(SetRemoveError());
		return;
	}
	
	v8::Local<v8::Function> func = arg0.As<v8::Function>();
	Functions::iterator it = functions.begin();
	while(it != functions.end())
	{
		v8::Global<v8::Function>& function = *it;
		if(function == func)
		{
			function.Reset();
			it = functions.erase(it);
		}
		else
		{
			++it;
		}
	}
	if(functions.empty())
	{
		this->ReleaseObject(); // release "this"
	}
}

template <typename CONFIG>
v8::Local<v8::Value> HookWrapper<CONFIG>::SetRemoveError()
{
	std::ostringstream err_msg;
	err_msg << "Can't " << (hook_set ? "remove" : "set") << " " << (*hook);
	return this->Error(err_msg.str());
}

template <typename CONFIG>
template <v8::Maybe<bool> (v8::Promise::Resolver::*SETTLE_METHOD)(v8::Local<v8::Context>, v8::Local<v8::Value>)>
void HookWrapper<CONFIG>::Settle(v8::Local<v8::Value> value)
{
	for(Resolvers::iterator it = resolvers.begin(); (it != resolvers.end()) && !this->Killed(); ++it)
	{
		v8::Global<v8::Promise::Resolver>& resolver = *it;
		if(!resolver.IsEmpty())
		{
			v8::Local<v8::Promise::Resolver> local_resolver = resolver.Get(this->GetIsolate());
			((*local_resolver)->*SETTLE_METHOD)(this->GetIsolate()->GetCurrentContext(), value).ToChecked();
		}
	}
	resolvers.clear();
}

template <typename CONFIG>
void HookWrapper<CONFIG>::Resolve(v8::Local<v8::Value> value)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << "Resolving Hook.on promise of " << (*hook) << std::endl;
	}
	Settle<&v8::Promise::Resolver::Resolve>(value);
}

template <typename CONFIG>
void HookWrapper<CONFIG>::Reject(v8::Local<v8::Value> value)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << "Rejecting Hook.on promise of " << (*hook) << std::endl;
	}
	Settle<&v8::Promise::Resolver::Reject>(value);
}

template <typename CONFIG>
bool HookWrapper<CONFIG>::Update(bool has_listeners)
{
	if(!hook || !processor_wrapper) return false;
	
	if(has_listeners && !hook_set)
	{
		if(this->IsVerbose())
		{
			this->DebugInfoStream() << "Setting " << (*hook) << std::endl;
		}
		if((*this->processor_wrapper)->SetHook(hook))
		{
			hook_set = true;
			return true;
		}
		
		return false;
	}
	else if(!has_listeners && hook_set)
	{
		if(this->IsVerbose())
		{
			this->DebugInfoStream() << "Removing " << (*hook) << std::endl;
		}
		if((*this->processor_wrapper)->RemoveHook(hook))
		{
			hook_set = false;
			return true;
		}
		
		return false;
	}
	
	return true;
}

template <typename CONFIG>
void HookWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/hook.h>
	;
}

///////////////////////////// AddressHookWrapper<> /////////////////////////////

template <typename CONFIG>
const char *AddressHookWrapper<CONFIG>::CLASS_NAME = "AddressHook";

template <typename CONFIG>
const uint32_t AddressHookWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> AddressHookWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> address_hook_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = address_hook_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "address", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetAddress>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "Hook"
	address_hook_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<HookWrapper<CONFIG> >());
	
	return handle_scope.Escape(address_hook_function_template);
}

// AddressHook() => AddressHook
// AddressHook(processor: Processor, addr : Number) => AddressHook
template <typename CONFIG>
void AddressHookWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	ProcessorWrapper<CONFIG> *processor_wrapper = 0;
	AddressHook<CONFIG> *address_hook = 0;
	if(args.Length() != 0)
	{
		struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + "(processor: Processor, addr : Number)"; } };
		
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a Processor for 'processor'"));
			return;
		}
		
		v8::Local<v8::Value> arg1 = args[1];
		
		ADDRESS addr = 0;
		if((!arg1->IsNumber() && !arg1->IsBigInt()) || !ToInt(args.GetIsolate(), arg1, addr))
		{
			nodejs.Throw(nodejs.Error(Synopsis().str() + " expects a number for 'addr'"));
			return;
		}
		
		address_hook = new AddressHook<CONFIG>(addr);
	}
	
	AddressHookWrapper<CONFIG> *address_hook_wrapper = new AddressHookWrapper<CONFIG>(nodejs, processor_wrapper, address_hook);
	address_hook_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
AddressHookWrapper<CONFIG>::AddressHookWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG> *processor_wrapper, AddressHook<CONFIG> *hook, std::size_t size)
	: Super(nodejs, processor_wrapper, hook, size ? size : sizeof(*this))
	, address_hook(hook)
{
	if(hook) hook->Bind(this);
}

// Accessor getter for "address"
template <typename CONFIG>
void AddressHookWrapper<CONFIG>::GetAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), address_hook->GetAddress()));
}

template <typename CONFIG>
void AddressHookWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/address_hook.h>
	;
}

/////////////////////////// SourceCodeHookWrapper<> ////////////////////////////

template <typename CONFIG>
const char *SourceCodeHookWrapper<CONFIG>::CLASS_NAME = "SourceCodeHook";

template <typename CONFIG>
const uint32_t SourceCodeHookWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> SourceCodeHookWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> source_code_hook_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = source_code_hook_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "file", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetFile>, 0 },
		{ "loc" , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetLoc> , 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "Hook"
	source_code_hook_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<HookWrapper<CONFIG> >());

	return handle_scope.Escape(source_code_hook_function_template);
}

// SourceCodeHook() => SourceCodeHook
// SourceCodeHook(processor: Processor, file : string, loc : (Object|string)) => SourceCodeHook
template <typename CONFIG>
void SourceCodeHookWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	ProcessorWrapper<CONFIG> *processor_wrapper = 0;
	SourceCodeHook<CONFIG> *source_code_hook = 0;
	if(args.Length() != 0)
	{
		struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + "(processor: Processor, file : string, loc : (Object|string))"; } };
	
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a Processor for 'processor'"));
			return;
		}
		v8::Local<v8::Value> arg1 = args[1];
		std::string file;
		if(arg1->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg1, file))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a string for 'file'"));
			return;
		}
		v8::Local<v8::Value> arg2 = args[2];
		unisim::util::debug::SourceCodeLocation source_code_location;
		if(!nodejs.ArgToSourceCodeLocation(arg2, Synopsis().str(), "loc", source_code_location)) return;

		source_code_hook = new SourceCodeHook<CONFIG>(source_code_location, nodejs.LocateFile(file));
	}

	SourceCodeHookWrapper<CONFIG> *source_code_hook_wrapper = new SourceCodeHookWrapper<CONFIG>(nodejs, processor_wrapper, source_code_hook);
	source_code_hook_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
SourceCodeHookWrapper<CONFIG>::SourceCodeHookWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG> *processor_wrapper, SourceCodeHook<CONFIG> *hook, std::size_t size)
	: Super(nodejs, processor_wrapper, hook, size ? size : sizeof(*this))
	, source_code_hook(hook)
{
	if(hook) hook->Bind(this);
}

// Accessor getter for "file"
template <typename CONFIG>
void SourceCodeHookWrapper<CONFIG>::GetFile(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), source_code_hook->GetFilename().c_str()).ToLocalChecked());
}

// Accessor getter for "loc"
template <typename CONFIG>
void SourceCodeHookWrapper<CONFIG>::GetLoc(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	SourceCodeLocationWrapper<CONFIG> *source_code_location_wrapper = new SourceCodeLocationWrapper<CONFIG>(this->nodejs, source_code_hook->GetSourceCodeLocation());
	info.GetReturnValue().Set(source_code_location_wrapper->MakeObject());
}

template <typename CONFIG>
void SourceCodeHookWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/processor.h>
	;
}

/////////////////////////// SubProgramHookWrapper<> ////////////////////////////

template <typename CONFIG>
const char *SubProgramHookWrapper<CONFIG>::CLASS_NAME = "SubProgramHook";

template <typename CONFIG>
const uint32_t SubProgramHookWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> SubProgramHookWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> subprogram_hook_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = subprogram_hook_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
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
	
	// Inherit from "Hook"
	subprogram_hook_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<HookWrapper<CONFIG> >());
	
	return handle_scope.Escape(subprogram_hook_function_template);
}

// SubProgramHook() => SubProgramHook
// SubProgramHook(processor: Processor, subprogram: SubProgram) => SubProgramHook
template <typename CONFIG>
void SubProgramHookWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	ProcessorWrapper<CONFIG> *processor_wrapper = 0;
	SubProgramHook<CONFIG> *subprogram_hook = 0;
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
		
		subprogram_hook = new SubProgramHook<CONFIG>(subprogram_wrapper->GetSubProgram());
	}
	
	SubProgramHookWrapper<CONFIG> *subprogram_hook_wrapper = new SubProgramHookWrapper<CONFIG>(nodejs, processor_wrapper, subprogram_hook);
	
	subprogram_hook_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
SubProgramHookWrapper<CONFIG>::SubProgramHookWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG> *processor_wrapper, SubProgramHook<CONFIG> *hook, std::size_t size)
	: Super(nodejs, processor_wrapper, hook, size ? size : sizeof(*this))
	, subprogram_hook(hook)
{
	hook->Bind(this);
}

// Accessor getter for "subprogram"
template <typename CONFIG>
void SubProgramHookWrapper<CONFIG>::GetSubProgram(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram_hook)
	{
		const unisim::util::debug::SubProgram<ADDRESS> *subprogram = subprogram_hook->GetSubProgram();
		if(subprogram)
		{
			SubProgramWrapper<CONFIG> *subprogram_wrapper = SubProgramWrapper<CONFIG>::Wrap(this->nodejs, subprogram);
			info.GetReturnValue().Set(subprogram_wrapper->MakeObject());
		}
	}
}

template <typename CONFIG>
void SubProgramHookWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/subprogram_hook.h>
	;
}

//////////////////////////////// AddressHook<> /////////////////////////////////

template <typename CONFIG>
AddressHook<CONFIG>::AddressHook(ADDRESS _addr)
	: unisim::util::debug::AddressHook<ADDRESS>(_addr)
	, hook_wrapper(0)
{
}

template <typename CONFIG>
void AddressHook<CONFIG>::Run()
{
	if(hook_wrapper) hook_wrapper->Trigger();
}

template <typename CONFIG>
void AddressHook<CONFIG>::Bind(AddressHookWrapper<CONFIG> *_hook_wrapper)
{
	hook_wrapper = _hook_wrapper;
}

/////////////////////////////// SourceCodeHook<> ///////////////////////////////

template <typename CONFIG>
SourceCodeHook<CONFIG>::SourceCodeHook(const unisim::util::debug::SourceCodeLocation& _source_code_location, const std::string& _filename)
	: unisim::util::debug::SourceCodeHook<ADDRESS>(_source_code_location, _filename)
	, hook_wrapper(0)
{
}

template <typename CONFIG>
void SourceCodeHook<CONFIG>::Run()
{
	if(hook_wrapper) hook_wrapper->Trigger();
}

template <typename CONFIG>
void SourceCodeHook<CONFIG>::Bind(SourceCodeHookWrapper<CONFIG> *_hook_wrapper)
{
	hook_wrapper = _hook_wrapper;
}

/////////////////////////////// SubProgramHook<> ///////////////////////////////

template <typename CONFIG>
SubProgramHook<CONFIG>::SubProgramHook(const unisim::util::debug::SubProgram<ADDRESS> *_subprogram)
	: unisim::util::debug::SubProgramHook<ADDRESS>(_subprogram)
	, hook_wrapper(0)
{
}

template <typename CONFIG>
void SubProgramHook<CONFIG>::Run()
{
	if(hook_wrapper) hook_wrapper->Trigger();
}

template <typename CONFIG>
void SubProgramHook<CONFIG>::Bind(SubProgramHookWrapper<CONFIG> *_hook_wrapper)
{
	hook_wrapper = _hook_wrapper;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_HOOK_TCC__
