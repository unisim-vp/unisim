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
v8::Global<v8::ObjectTemplate> HookWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
HookWrapper<CONFIG>::HookWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG>& _processor_wrapper, unisim::util::debug::Hook<ADDRESS> *_hook, std::size_t size)
	: Super(_nodejs, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, hook(_hook)
	, functions()
	, resolvers()
	, hook_set(false)
	, shadow_object()
{
	hook->Catch();
}

template <typename CONFIG>
HookWrapper<CONFIG>::~HookWrapper()
{
	Update(false);
	hook->Release();
}

template <typename CONFIG>
void HookWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void HookWrapper<CONFIG>::Finalize()
{
	shadow_object.Reset();
	Super::Finalize();
}

template <typename CONFIG>
void HookWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	// add "on" method
	object_template->Set(isolate, "on", unisim::util::nodejs::CreateFunctionTemplate<HookWrapper<CONFIG>, &HookWrapper<CONFIG>::On>(isolate));
	// add "removeListener" method
	object_template->Set(isolate, "removeListener", unisim::util::nodejs::CreateFunctionTemplate<HookWrapper<CONFIG>, &HookWrapper<CONFIG>::RemoveListener>(isolate));
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> HookWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
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
v8::Local<v8::Object> HookWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = Super::template MakeObject<T>(object_template);
	// keep object reference for setting "this" of callback
	shadow_object.Reset(this->GetIsolate(), object);
	
	return handle_scope.Escape(object);
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
		v8::Local<v8::Value> recv = shadow_object.Get(this->GetIsolate()); // "this"
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

// Hook.on(callback : function)
// Hook.on() => Promise
template <typename CONFIG>
void HookWrapper<CONFIG>::On(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << "Hook.on of " << (*hook) << std::endl;
	}
	
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
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		functions.resize(functions.size() + 1);
		functions.back().Reset(this->GetIsolate(), function);
		if(!Update(true))
		{
			this->Throw(SetRemoveError());
		}
	}
	else
	{
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
		if(!Update(true))
		{
			Reject(SetRemoveError());
		}
	}
}

// Hook.removeListener(callback : function)
template <typename CONFIG>
void HookWrapper<CONFIG>::RemoveListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << "Hook.removeListener(callback : function) of " << (*hook) << std::endl;
	}

	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0];
	
	if(!arg0->IsFunction())
	{
		this->Throw(this->Error("Hook.removeListener(callback : function) expects a function for 'callback'"));
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
	
	if(!Update(!functions.empty() || !resolvers.empty()))
	{
		this->Throw(SetRemoveError());
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
	if(has_listeners && !hook_set)
	{
		if(this->IsVerbose())
		{
			this->DebugInfoStream() << "Setting " << (*hook) << std::endl;
		}
		if(this->processor_wrapper->SetHook(hook))
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
		if(this->processor_wrapper->RemoveHook(hook))
		{
			hook_set = false;
			return true;
		}
		
		return false;
	}
	
	return true;
}

///////////////////////////// AddressHookWrapper<> /////////////////////////////

template <typename CONFIG>
const char *AddressHookWrapper<CONFIG>::CLASS_NAME = "AddressHook";

template <typename CONFIG>
const uint32_t AddressHookWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Global<v8::ObjectTemplate> AddressHookWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
AddressHookWrapper<CONFIG>::AddressHookWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG>& processor_wrapper, AddressHook<CONFIG> *hook, std::size_t size)
	: Super(nodejs, processor_wrapper, hook, size ? size : sizeof(*this))
	, address_hook(hook)
{
	hook->Bind(this);
}

template <typename CONFIG>
void AddressHookWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void AddressHookWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	Super::FillObjectTemplate(isolate, object_template);
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> AddressHookWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
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
v8::Local<v8::Object> AddressHookWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = Super::template MakeObject<T>(object_template);
	// add "addr" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "addr", v8::NewStringType::kInternalized),
		MakeInteger(this->GetIsolate(), address_hook->GetAddress()),
		v8::ReadOnly
 	).ToChecked();
	return handle_scope.Escape(object);
}

template <typename CONFIG>
v8::Local<v8::Object> AddressHookWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object template
	v8::Local<v8::ObjectTemplate> object_template = MakeObjectTemplate(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = this->template MakeObject<This>(object_template);
	
	return handle_scope.Escape(object);
}

/////////////////////////// SourceCodeHookWrapper<> ////////////////////////////

template <typename CONFIG>
const char *SourceCodeHookWrapper<CONFIG>::CLASS_NAME = "SourceCodeHook";

template <typename CONFIG>
const uint32_t SourceCodeHookWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Global<v8::ObjectTemplate> SourceCodeHookWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
SourceCodeHookWrapper<CONFIG>::SourceCodeHookWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG>& processor_wrapper, SourceCodeHook<CONFIG> *hook, std::size_t size)
	: Super(nodejs, processor_wrapper, hook, size ? size : sizeof(*this))
	, source_code_hook(hook)
{
	hook->Bind(this);
}

template <typename CONFIG>
void SourceCodeHookWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void SourceCodeHookWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	Super::FillObjectTemplate(isolate, object_template);
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> SourceCodeHookWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
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
v8::Local<v8::Object> SourceCodeHookWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = Super::template MakeObject<T>(object_template);
	// add "file" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "file", v8::NewStringType::kInternalized),
		v8::String::NewFromUtf8(this->GetIsolate(), source_code_hook->GetFilename().c_str()).ToLocalChecked(),
		v8::ReadOnly
 	).ToChecked();
	
	// add "loc" read-only property
	std::ostringstream loc;
	loc << source_code_hook->GetSourceCodeLocation();
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "loc", v8::NewStringType::kInternalized),
		v8::String::NewFromUtf8(this->GetIsolate(), loc.str().c_str()).ToLocalChecked(),
		v8::ReadOnly
 	).ToChecked();
	return handle_scope.Escape(object);
}

template <typename CONFIG>
v8::Local<v8::Object> SourceCodeHookWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object template
	v8::Local<v8::ObjectTemplate> object_template = MakeObjectTemplate(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = this->template MakeObject<This>(object_template);
	
	return handle_scope.Escape(object);
}

/////////////////////////// SubProgramHookWrapper<> ////////////////////////////

template <typename CONFIG>
const char *SubProgramHookWrapper<CONFIG>::CLASS_NAME = "SubProgramHook";

template <typename CONFIG>
const uint32_t SubProgramHookWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Global<v8::ObjectTemplate> SubProgramHookWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
SubProgramHookWrapper<CONFIG>::SubProgramHookWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG>& processor_wrapper, SubProgramHook<CONFIG> *hook, std::size_t size)
	: Super(nodejs, processor_wrapper, hook, size ? size : sizeof(*this))
	, subprogram_hook(hook)
{
	hook->Bind(this);
}

template <typename CONFIG>
void SubProgramHookWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void SubProgramHookWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	Super::FillObjectTemplate(isolate, object_template);
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> SubProgramHookWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
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
v8::Local<v8::Object> SubProgramHookWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = Super::template MakeObject<T>(object_template);
	// add "name" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "name", v8::NewStringType::kInternalized),
		v8::String::NewFromUtf8(this->GetIsolate(), subprogram_hook->GetSubProgram()->GetName()).ToLocalChecked(),
		v8::ReadOnly
 	).ToChecked();
	return handle_scope.Escape(object);
}

template <typename CONFIG>
v8::Local<v8::Object> SubProgramHookWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object template
	v8::Local<v8::ObjectTemplate> object_template = MakeObjectTemplate(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = this->template MakeObject<This>(object_template);
	
	return handle_scope.Escape(object);
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
