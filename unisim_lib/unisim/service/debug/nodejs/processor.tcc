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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_PROCESSOR_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_PROCESSOR_TCC__

#include <unisim/service/debug/nodejs/processor.hh>
#include <unisim/util/debug/event.hh>
#include <unisim/util/debug/breakpoint.hh>
#include <unisim/util/debug/watchpoint.hh>
#include <unisim/util/debug/fetch_insn_event.hh>
#include <unisim/util/debug/fetch_stmt_event.hh>
#include <unisim/util/debug/commit_insn_event.hh>
#include <unisim/util/debug/trap_event.hh>
#include <unisim/util/debug/next_insn_event.hh>
#include <unisim/util/debug/next_stmt_event.hh>
#include <unisim/util/debug/finish_event.hh>
#include <unisim/util/debug/source_code_breakpoint.hh>
#include <unisim/util/debug/subprogram_breakpoint.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

//////////////////////////////// ProcessorWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *ProcessorWrapper<CONFIG>::CLASS_NAME = "Processor";

template <typename CONFIG>
const uint32_t ProcessorWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Global<v8::ObjectTemplate> ProcessorWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
ProcessorWrapper<CONFIG>::ProcessorWrapper(NodeJS<CONFIG>& _nodejs, unisim::service::interfaces::DebugProcessor<ADDRESS, TIME_TYPE> *_processor, unsigned _id, std::size_t size)
	: Super(_nodejs, size ? size : sizeof(*this))
	, processor(_processor)
	, id(_id)
	, fetch_insn_event_bridge(0)
	, next_insn_event_bridge(0)
	, trap_event_bridge(0)
	, fetch_stmt_event_bridge(0)
	, next_stmt_event_bridge(0)
	, finish_event_bridge(0)
	, shadow_object()
{
}

template <typename CONFIG>
ProcessorWrapper<CONFIG>::~ProcessorWrapper()
{
}

template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Finalize()
{
	if(fetch_insn_event_bridge) delete fetch_insn_event_bridge;
	if(next_insn_event_bridge) delete next_insn_event_bridge;
	if(trap_event_bridge) delete trap_event_bridge;
	if(fetch_stmt_event_bridge) delete fetch_stmt_event_bridge;
	if(next_stmt_event_bridge) delete next_stmt_event_bridge;
	if(finish_event_bridge) delete finish_event_bridge;
	shadow_object.Reset();
	Super::Finalize();
}

template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void ProcessorWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	// add "XXXX" method
	//object_template->Set(isolate, "xxxx", unisim::util::nodejs::CreateFunctionTemplate<ProcessorWrapper<CONFIG>, &ProcessorWrapper<CONFIG>::XXXX>(isolate));
	
	// Add some builtin functions
	object_template->Set(isolate, "stepInstruction", unisim::util::nodejs::CreateFunctionTemplate<ProcessorWrapper<CONFIG>, &ProcessorWrapper<CONFIG>::StepInstruction>(isolate));
	object_template->Set(isolate, "nextInstruction", unisim::util::nodejs::CreateFunctionTemplate<ProcessorWrapper<CONFIG>, &ProcessorWrapper<CONFIG>::NextInstruction>(isolate));
	object_template->Set(isolate, "step", unisim::util::nodejs::CreateFunctionTemplate<ProcessorWrapper<CONFIG>, &ProcessorWrapper<CONFIG>::Step>(isolate));
	object_template->Set(isolate, "stepInto", unisim::util::nodejs::CreateFunctionTemplate<ProcessorWrapper<CONFIG>, &ProcessorWrapper<CONFIG>::Step>(isolate));
	object_template->Set(isolate, "next", unisim::util::nodejs::CreateFunctionTemplate<ProcessorWrapper<CONFIG>, &ProcessorWrapper<CONFIG>::Next>(isolate));
	object_template->Set(isolate, "stepOver", unisim::util::nodejs::CreateFunctionTemplate<ProcessorWrapper<CONFIG>, &ProcessorWrapper<CONFIG>::Next>(isolate));
	object_template->Set(isolate, "finish", unisim::util::nodejs::CreateFunctionTemplate<ProcessorWrapper<CONFIG>, &ProcessorWrapper<CONFIG>::Finish>(isolate));
	object_template->Set(isolate, "stepOut", unisim::util::nodejs::CreateFunctionTemplate<ProcessorWrapper<CONFIG>, &ProcessorWrapper<CONFIG>::Finish>(isolate));
	object_template->Set(isolate, "returnFromFunction", unisim::util::nodejs::CreateFunctionTemplate<ProcessorWrapper<CONFIG>, &ProcessorWrapper<CONFIG>::Return>(isolate));
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> ProcessorWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
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
v8::Local<v8::Object> ProcessorWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
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
	// add "id" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "id", v8::NewStringType::kInternalized),
		v8::Integer::New(this->GetIsolate(), id),
		v8::ReadOnly
	).ToChecked();
	
	// keep object reference for setting "this" of callback
	shadow_object.Reset(this->GetIsolate(), object);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
v8::Local<v8::Object> ProcessorWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object template
	v8::Local<v8::ObjectTemplate> object_template = MakeObjectTemplate(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = this->template MakeObject<This>(object_template);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
unisim::service::interfaces::DebugProcessor<typename CONFIG::ADDRESS, typename CONFIG::TIME_TYPE> *ProcessorWrapper<CONFIG>::GetProcessor() const
{
	return processor;
}

//template <typename CONFIG>
//void ProcessorWrapper<CONFIG>::XXXX(const v8::FunctionCallbackInfo<v8::Value>& args)
//{
//	v8::Local<v8::Value> recv = shadow_object.Get(this->GetIsolate()); // "this"
//}

// Processor.stepInstruction(callback : function)
// Processor.stepInstruction() => Promise
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::StepInstruction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << CLASS_NAME << ".stepInstruction" << std::endl;
	}
	
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!fetch_insn_event_bridge) fetch_insn_event_bridge = new EventBridge<CONFIG>(this->nodejs, processor->CreateFetchInsnEvent(), v8::Global<v8::Object>(this->GetIsolate(), shadow_object));
	
	v8::Local<v8::Value> arg0 = args[0];
	if(!arg0->IsUndefined())
	{
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error(std::string(CLASS_NAME) + ".stepInstruction(callback : function) expects a function for 'callback'"));
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		if(!fetch_insn_event_bridge->AddListener(function)) return;
	}
	else
	{
		v8::Local<v8::Promise> promise = fetch_insn_event_bridge->NewPromise();
		if(promise->IsUndefined()) return;
		args.GetReturnValue().Set(promise);
	}
	this->nodejs.Continue();
}

// Processor.nextInstruction(callback : function)
// Processor.nextInstruction() => Promise
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::NextInstruction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << CLASS_NAME << ".nextInstruction" << std::endl;
	}
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!next_insn_event_bridge) next_insn_event_bridge = new EventBridge<CONFIG>(this->nodejs, processor->CreateNextInsnEvent(), v8::Global<v8::Object>(this->GetIsolate(), shadow_object));
	
	v8::Local<v8::Value> arg0 = args[0];
	if(!arg0->IsUndefined())
	{
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error(std::string(CLASS_NAME) + ".nextInstruction(callback : function) expects a function for 'callback'"));
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		if(!next_insn_event_bridge->AddListener(function)) return;
	}
	else
	{
		v8::Local<v8::Promise> promise = next_insn_event_bridge->NewPromise();
		if(promise->IsUndefined()) return;
		args.GetReturnValue().Set(promise);
	}
	this->nodejs.Continue();
}

// Processor.step(callback : function)
// Processor.step() => Promise
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Step(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << CLASS_NAME << ".step" << std::endl;
	}
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!fetch_stmt_event_bridge) fetch_stmt_event_bridge = new EventBridge<CONFIG>(this->nodejs, processor->CreateFetchStmtEvent(), v8::Global<v8::Object>(this->GetIsolate(), shadow_object));
	
	v8::Local<v8::Value> arg0 = args[0];
	if(!arg0->IsUndefined())
	{
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error(std::string(CLASS_NAME) + ".step(callback : function) expects a function for 'callback'"));
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		if(!fetch_stmt_event_bridge->AddListener(function)) return;
	}
	else
	{
		v8::Local<v8::Promise> promise = fetch_stmt_event_bridge->NewPromise();
		if(promise->IsUndefined()) return;
		args.GetReturnValue().Set(promise);
	}
	this->nodejs.Continue();
}

// Processor.next(callback : function)
// Processor.next() => Promise
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Next(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << CLASS_NAME << ".next" << std::endl;
	}
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!next_stmt_event_bridge) next_stmt_event_bridge = new EventBridge<CONFIG>(this->nodejs, processor->CreateNextStmtEvent(), v8::Global<v8::Object>(this->GetIsolate(), shadow_object));
	
	v8::Local<v8::Value> arg0 = args[0];
	if(!arg0->IsUndefined())
	{
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error(std::string(CLASS_NAME) + ".next(callback : function) expects a function for 'callback'"));
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		if(!next_stmt_event_bridge->AddListener(function)) return;
	}
	else
	{
		v8::Local<v8::Promise> promise = next_stmt_event_bridge->NewPromise();
		if(promise->IsUndefined()) return;
		args.GetReturnValue().Set(promise);
	}
	this->nodejs.Continue();
}

// Processor.finish(callback : function)
// Processor.finish() => Promise
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Finish(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << CLASS_NAME << ".finish" << std::endl;
	}
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!finish_event_bridge) finish_event_bridge = new EventBridge<CONFIG>(this->nodejs, processor->CreateFinishEvent(), v8::Global<v8::Object>(this->GetIsolate(), shadow_object));
	
	v8::Local<v8::Value> arg0 = args[0];
	if(!arg0->IsUndefined())
	{
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error(std::string(CLASS_NAME) + ".finish(callback : function) expects a function for 'callback'"));
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		if(!finish_event_bridge->AddListener(function)) return;
	}
	else
	{
		v8::Local<v8::Promise> promise = finish_event_bridge->NewPromise();
		if(promise->IsUndefined()) return;
		args.GetReturnValue().Set(promise);
	}
	this->nodejs.Continue();
}

// Processor.return()
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Return(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(this->IsDebugging())
	{
		this->DebugInfoStream() << CLASS_NAME << ".return" << std::endl;
	}
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> return_value = args[0];
	if(!return_value->IsUndefined())
	{
		DataObjectWrapper<CONFIG> data_object = DataObjectWrapper<CONFIG>(this->nodejs, *this, "$return_value");
		data_object.Set(return_value);
	}
	processor->UnwindStack();
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_PROCESSOR_TCC__
