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

using unisim::util::nodejs::ToInt;

//////////////////////////////// ProcessorWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *ProcessorWrapper<CONFIG>::CLASS_NAME = "Processor";

template <typename CONFIG>
const uint32_t ProcessorWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> ProcessorWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> processor_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = processor_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "id", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetId>, 0 }
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
	v8::Local<v8::Template> prototype_template = processor_function_template->PrototypeTemplate();
	
	// Add methods
	struct { const char *method_name; v8::FunctionCallback callback; } methods_config[] =
	{
		{ "stepInstruction"   , &unisim::util::nodejs::FunctionCallback<This, &This::StepInstruction   > },
		{ "nextInstruction"   , &unisim::util::nodejs::FunctionCallback<This, &This::NextInstruction   > },
		{ "step"              , &unisim::util::nodejs::FunctionCallback<This, &This::Step              > },
		{ "stepInto"          , &unisim::util::nodejs::FunctionCallback<This, &This::Step              > },
		{ "next"              , &unisim::util::nodejs::FunctionCallback<This, &This::Next              > },
		{ "stepOver"          , &unisim::util::nodejs::FunctionCallback<This, &This::Next              > },
		{ "finish"            , &unisim::util::nodejs::FunctionCallback<This, &This::Finish            > },
		{ "stepOut"           , &unisim::util::nodejs::FunctionCallback<This, &This::Finish            > },
		{ "returnFromFunction", &unisim::util::nodejs::FunctionCallback<This, &This::Return            > },
		{ "disasm"            , &unisim::util::nodejs::FunctionCallback<This, &This::Disasm            > },
		{ "readMemory"        , &unisim::util::nodejs::FunctionCallback<This, &This::ReadMemory        > },
		{ "writeMemory"       , &unisim::util::nodejs::FunctionCallback<This, &This::WriteMemory       > },
		{ "getStackFrameInfos", &unisim::util::nodejs::FunctionCallback<This, &This::GetStackFrameInfos> },
		{ "selectStackFrame"  , &unisim::util::nodejs::FunctionCallback<This, &This::SelectStackFrame  > },
		{ "getTime"           , &unisim::util::nodejs::FunctionCallback<This, &This::GetTime           > },
		{ "getDataObjectNames", &unisim::util::nodejs::FunctionCallback<This, &This::GetDataObjectNames> }
	};
	for(auto method_config : methods_config)
	{
		prototype_template->Set(isolate, method_config.method_name, v8::FunctionTemplate::New(isolate, method_config.callback));
	}
	
	return handle_scope.Escape(processor_function_template);
}

// Processor() => Processor
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
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
ProcessorWrapper<CONFIG> *ProcessorWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, unisim::service::interfaces::DebugProcessor<ADDRESS, TIME_TYPE> *processor)
{
	This *processor_wrapper = nodejs.template GetObjectWrapper<This>(processor);
	return processor_wrapper ? processor_wrapper : new This(nodejs, processor);
}

template <typename CONFIG>
ProcessorWrapper<CONFIG>::ProcessorWrapper(NodeJS<CONFIG>& _nodejs, unisim::service::interfaces::DebugProcessor<ADDRESS, TIME_TYPE> *_processor, std::size_t size)
	: Super(_nodejs, _processor, size ? size : sizeof(*this))
	, processor(_processor)
	, fetch_insn_event_bridge(0)
	, next_insn_event_bridge(0)
	, trap_event_bridge(0)
	, fetch_stmt_event_bridge(0)
	, next_stmt_event_bridge(0)
	, finish_event_bridge(0)
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
	Super::Finalize();
}

template <typename CONFIG>
v8::Local<v8::Object> ProcessorWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	
	v8::Local<v8::Object> processor_object = this->ThisObject();
	
	if(processor_object.IsEmpty())
	{
		processor_object = Super::template MakePersistentObject<This>();
	
		v8::Local<v8::Object> prop_registers = v8::Object::New(this->GetIsolate());
		
		struct RegisterScanner : unisim::service::interfaces::RegisterScanner
		{
			NodeJS<CONFIG>& nodejs;
			v8::Local<v8::Object>& prop_registers;
			
			RegisterScanner(NodeJS<CONFIG>& _nodejs, v8::Local<v8::Object>& _prop_registers) : nodejs(_nodejs), prop_registers(_prop_registers) {}
			
			virtual void Append(unisim::service::interfaces::Register* reg)
			{
				RegisterWrapper<CONFIG> *register_wrapper = RegisterWrapper<CONFIG>::Wrap(nodejs, reg);
				// add "registers.<regname>" property
				prop_registers->DefineOwnProperty(
					nodejs.GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8(nodejs.GetIsolate(), reg->GetName(), v8::NewStringType::kInternalized).ToLocalChecked(),
					register_wrapper->MakeObject(),
					v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
				).ToChecked();
			}
		} register_scanner(this->nodejs, prop_registers);
		
		this->processor->ScanRegisters(register_scanner);
		
		// add "registers" property
		processor_object->DefineOwnProperty(
			this->GetIsolate()->GetCurrentContext(),
			v8::String::NewFromUtf8Literal(this->GetIsolate(), "registers", v8::NewStringType::kInternalized),
			prop_registers,
			v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
		).ToChecked();
	}
	
	return handle_scope.Escape(processor_object);
}

template <typename CONFIG>
unisim::service::interfaces::DebugProcessor<typename CONFIG::ADDRESS, typename CONFIG::TIME_TYPE> *ProcessorWrapper<CONFIG>::GetProcessor() const
{
	return processor;
}

template <typename CONFIG>
void ProcessorWrapper<CONFIG>::GetId(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), processor->GetProcessorNumber()));
}

// Processor.stepInstruction(callback : function)
// Processor.stepInstruction() => Promise
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::StepInstruction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!fetch_insn_event_bridge) fetch_insn_event_bridge = new EventBridge<CONFIG>(this->nodejs, processor->CreateFetchInsnEvent());
	
	if(args.Length() > 0)
	{
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error(std::string(CLASS_NAME) + ".stepInstruction(callback : function) expects a function for 'callback'"));
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		if(!fetch_insn_event_bridge->AddListener(function, this->ThisObject()))
		{
			this->Throw(this->Error(std::string("Processor #" + ToString(processor->GetProcessorNumber()) + ": Can't step instruction")));
			return;
		}
	}
	else
	{
		v8::Local<v8::Promise> promise = fetch_insn_event_bridge->NewPromise();
		args.GetReturnValue().Set(promise);
	}
	this->nodejs.Continue();
}

// Processor.nextInstruction(callback : function)
// Processor.nextInstruction() => Promise
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::NextInstruction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!next_insn_event_bridge) next_insn_event_bridge = new EventBridge<CONFIG>(this->nodejs, processor->CreateNextInsnEvent());
	
	if(args.Length() > 0)
	{
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error(std::string(CLASS_NAME) + ".nextInstruction(callback : function) expects a function for 'callback'"));
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		if(!next_insn_event_bridge->AddListener(function, this->ThisObject()))
		{
			this->Throw(this->Error(std::string("Processor #" + ToString(processor->GetProcessorNumber()) + ": Can't step over instruction")));
			return;
		}
	}
	else
	{
		v8::Local<v8::Promise> promise = next_insn_event_bridge->NewPromise();
		args.GetReturnValue().Set(promise);
	}
	this->nodejs.Continue();
}

// Processor.step(callback : function)
// Processor.step() => Promise
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Step(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!fetch_stmt_event_bridge) fetch_stmt_event_bridge = new EventBridge<CONFIG>(this->nodejs, processor->CreateFetchStmtEvent());
	
	if(args.Length() > 0)
	{
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error(std::string(CLASS_NAME) + ".step(callback : function) expects a function for 'callback'"));
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		if(!fetch_stmt_event_bridge->AddListener(function, this->ThisObject()))
		{
			this->Throw(this->Error(std::string("Processor #" + ToString(processor->GetProcessorNumber()) + ": Can't step into")));
			return;
		}
	}
	else
	{
		v8::Local<v8::Promise> promise = fetch_stmt_event_bridge->NewPromise();
		args.GetReturnValue().Set(promise);
	}
	this->nodejs.Continue();
}

// Processor.next(callback : function)
// Processor.next() => Promise
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Next(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!next_stmt_event_bridge) next_stmt_event_bridge = new EventBridge<CONFIG>(this->nodejs, processor->CreateNextStmtEvent());
	
	if(args.Length() > 0)
	{
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error(std::string(CLASS_NAME) + ".next(callback : function) expects a function for 'callback'"));
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		if(!next_stmt_event_bridge->AddListener(function, this->ThisObject()))
		{
			this->Throw(this->Error(std::string("Processor #" + ToString(processor->GetProcessorNumber()) + ": Can't step over")));
			return;
		}
	}
	else
	{
		v8::Local<v8::Promise> promise = next_stmt_event_bridge->NewPromise();
		args.GetReturnValue().Set(promise);
	}
	this->nodejs.Continue();
}

// Processor.finish(callback : function)
// Processor.finish() => Promise
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Finish(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!finish_event_bridge) finish_event_bridge = new EventBridge<CONFIG>(this->nodejs, processor->CreateFinishEvent());
	
	if(args.Length() > 0)
	{
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsFunction())
		{
			this->Throw(this->Error(std::string(CLASS_NAME) + ".finish(callback : function) expects a function for 'callback'"));
			return;
		}
		v8::Local<v8::Function> function = arg0.As<v8::Function>();
		if(!finish_event_bridge->AddListener(function, this->ThisObject()))
		{
			this->Throw(this->Error(std::string("Processor #" + ToString(processor->GetProcessorNumber()) + ": Can't finish current current function")));
			return;
		}
	}
	else
	{
		v8::Local<v8::Promise> promise = finish_event_bridge->NewPromise();
		args.GetReturnValue().Set(promise);
	}
	this->nodejs.Continue();
}

// Processor.return([return_value : *])
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Return(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> return_value = args[0];
	if(!return_value->IsUndefined())
	{
		DataObjectWrapper<CONFIG> data_object = DataObjectWrapper<CONFIG>(this->nodejs, this, "$return_value");
		data_object.Set(return_value);
	}
	processor->UnwindStack();
}

// Processor.disasm(addr: Number) => string
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Disasm(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0]; // addr
	ADDRESS addr = 0;
	if((!arg0->IsNumber() && !arg0->IsBigInt()) || !ToInt(args.GetIsolate(), arg0, addr))
	{
		this->Throw(this->TypeError(std::string(CLASS_NAME) + ".disasm(addr: Number) expects a number for 'addr'"));
		return;
	}
	
	ADDRESS next_addr;
	std::string disasm = processor->Disasm(addr, next_addr);
	args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), disasm.c_str()).ToLocalChecked());
}

// Processor.readMemory(addr: Number, size: Number) => Buffer
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::ReadMemory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + ".readMemory(addr: Number, size: Number)"; } };
	v8::Local<v8::Value> arg0 = args[0]; // addr
	ADDRESS addr = 0;
	if((!arg0->IsNumber() && !arg0->IsBigInt()) || !ToInt(args.GetIsolate(), arg0, addr))
	{
		this->Throw(this->TypeError(Synopsis().str() + " expects a number for 'addr'"));
		return;
	}
	v8::Local<v8::Value> arg1 = args[1]; // size
	uint64_t size = 0;
	if((!arg1->IsNumber() && !arg1->IsBigInt()) || !ToInt(args.GetIsolate(), arg1, size))
	{
		this->Throw(this->TypeError(Synopsis().str() + " expects a number for 'size'"));
		return;
	}
	
	v8::Local<v8::Object> node_buffer = node::Buffer::New(args.GetIsolate(), size).ToLocalChecked();
	if(!processor->ReadMemory(addr, node::Buffer::Data(node_buffer), size))
	{
		this->Throw(this->Error(std::string("Processor #" + ToString(processor->GetProcessorNumber()) + ": Memory read failed")));
		return;
	}
	args.GetReturnValue().Set(node_buffer);
}

// Processor.writeMemory(addr: Number, buffer: Buffer)
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::WriteMemory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + ".writeMemory(addr: Number, buffer: Buffer)"; } };
	v8::Local<v8::Value> arg0 = args[0]; // addr
	ADDRESS addr = 0;
	if((!arg0->IsNumber() && !arg0->IsBigInt()) || !ToInt(args.GetIsolate(), arg0, addr))
	{
		this->Throw(this->TypeError(Synopsis().str() + " expects a number for 'addr'"));
		return;
	}
	v8::Local<v8::Value> arg1 = args[1]; // buffer
	if(!node::Buffer::HasInstance(arg1))
	{
		this->Throw(this->TypeError(Synopsis().str() + " expects a Buffer for 'buffer'"));
		return;
	}
	v8::Local<v8::Object> node_buffer = arg1.As<v8::Object>();
	if(!processor->WriteMemory(addr, node::Buffer::Data(node_buffer), node::Buffer::Length(node_buffer)))
	{
		this->Throw(this->Error(std::string("Processor #" + ToString(processor->GetProcessorNumber()) + ": Memory read failed")));
		return;
	}
}

// Processor.getStackFrameInfos([maxStackFrames: Number]) => Array of StackFrameInfo
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::GetStackFrameInfos(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0]; // max_stack_frames
	int64_t max_stack_frames = 0;
	if(args.Length() > 0)
	{
		if((!arg0->IsNumber() && !arg0->IsBigInt()) || !ToInt(args.GetIsolate(), arg0, max_stack_frames))
		{
			this->Throw(this->TypeError(std::string(CLASS_NAME) + ".getStackFrameInfos([maxStackFrames: Number]) expects an integer for 'maxStackFrames'"));
			return;
		}
	}
	if(max_stack_frames < 0) max_stack_frames = 0;
	
	struct StackFrameInfoScanner : unisim::service::interfaces::StackFrameInfoScanner<ADDRESS>
	{
		NodeJS<CONFIG>& nodejs;
		std::vector<v8::Local<v8::Value> > stack_frame_info_objects;
		
		StackFrameInfoScanner(NodeJS<CONFIG>& _nodejs) : nodejs(_nodejs), stack_frame_info_objects() {}
		
		virtual void Append(const unisim::service::interfaces::StackFrameInfo<ADDRESS>& stack_frame_info)
		{
			StackFrameInfoWrapper<CONFIG> *stack_frame_info_wrapper = new StackFrameInfoWrapper<CONFIG>(this->nodejs, stack_frame_info);
			stack_frame_info_objects.push_back(stack_frame_info_wrapper->MakeObject());
		}
	};
	
	StackFrameInfoScanner stack_frame_info_scanner(this->nodejs);
	processor->ScanStackFrameInfos(stack_frame_info_scanner);
	
	v8::Local<v8::Array> stack_frame_infos = v8::Array::New(this->GetIsolate(), &stack_frame_info_scanner.stack_frame_info_objects[0], stack_frame_info_scanner.stack_frame_info_objects.size());
	
	args.GetReturnValue().Set(stack_frame_infos);
}

// Processor.selectStackFrame(frameId: Number)
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::SelectStackFrame(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0]; // frameId
	int64_t frame_id = 0;
	if((!arg0->IsNumber() && !arg0->IsBigInt()) || !ToInt(args.GetIsolate(), arg0, frame_id))
	{
		this->Throw(this->TypeError(std::string(CLASS_NAME) + ".selectStackFrame(frameId: Number) expects an integer for 'frameId'"));
		return;
	}
	if(frame_id < 0) frame_id = 0;
	
	if(!processor->SelectStackFrame(frame_id))
	{
		this->Throw(this->Error(std::string("Processor #" + ToString(processor->GetProcessorNumber()) + ": Can't select Frame #" + ToString(frame_id))));
		return;
	}
}

// Processor.getTime() => string
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::GetTime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	std::ostringstream time_sstr;
	const TIME_TYPE& time = processor->DebugGetTime();
	time_sstr << time;
	
	args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), time_sstr.str().c_str()).ToLocalChecked());
}

// getDataObjectNames([scope: string]) => Array of string
template <typename CONFIG>
void ProcessorWrapper<CONFIG>::GetDataObjectNames(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	struct Synopsis { std::string str() const { return std::string("getDataObjectNames([scope: string])"); } };
	v8::HandleScope handle_scope(args.GetIsolate());
	typename unisim::service::interfaces::DataObjectLookupBase::Scope scope = unisim::service::interfaces::DataObjectLookupBase::SCOPE_BOTH_GLOBAL_AND_LOCAL;
	if(args.Length() >= 1)
	{
		struct ValidScopes
		{
			std::string str() const
			{
				std::string s;
				for(typename unisim::service::interfaces::DataObjectLookupBase::Scope scope : unisim::service::interfaces::DataObjectLookupBase::Scopes)
				{
					if(!s.empty()) s += ", ";
					s += "'";
					s += ToString(scope);
					s += "'";
				}
				return s;
			}
		};
		v8::Local<v8::Value> arg0 = args[0]; // scope
		std::string scope_name;
		if(!arg0->IsString() || !ToString(args.GetIsolate(), arg0, scope_name))
		{
			this->Throw(this->TypeError(Synopsis().str() + " expects a string for 'scope'"));
			return;
		}
		std::istringstream scope_sstr(scope_name);
		if(!(scope_sstr >> scope))
		{
			this->Throw(this->TypeError(Synopsis().str() + " expects a valid string (one of " + ValidScopes().str() + ") for 'scope'"));
			return;
		}
	}
	struct DataObjectNameScanner : unisim::service::interfaces::DataObjectNameScanner
	{
		v8::Isolate *isolate;
		std::vector<v8::Local<v8::Value> > data_object_name_strings;
		
		DataObjectNameScanner(v8::Isolate *_isolate) : isolate(_isolate), data_object_name_strings() {}
		
		virtual void Append(const char *data_object_name)
		{
			data_object_name_strings.push_back(v8::String::NewFromUtf8(isolate, data_object_name).ToLocalChecked());
		}
	};
	
	DataObjectNameScanner data_object_name_scanner(args.GetIsolate());
	processor->ScanDataObjectNames(data_object_name_scanner, scope);
	
	v8::Local<v8::Array> data_object_names_array = v8::Array::New(args.GetIsolate(), &data_object_name_scanner.data_object_name_strings[0], data_object_name_scanner.data_object_name_strings.size());
	
	args.GetReturnValue().Set(data_object_names_array);
}

template <typename CONFIG>
char *ProcessorWrapper<CONFIG>::CommandGenerator(char *text, int state)
{
	std::cerr << "CommandGenerator: text=\"" << (text ? text : "nullptr") << "\", state=" << state << std::endl;
	static int index, len;
	static const char *commands[] =
	{
		"stepInstruction(",
		"nextInstruction(",
		"step(",
		"stepInto(",
		"next(",
		"stepOver(",
		"finish(",
		"stepOut(",
		"returnFromFunction(",
		"disasm(",
		"readMemory(",
		"writeMemory(",
		"getStackFrameInfos(",
		"selectStackFrame(",
		"getTime(",
		"getDataObjectNames(",
		0
	};

	if(!state)
	{
		index = 0;
		len = strlen(text);
	}

	const char *cmd;
	while((cmd = commands[index]) != 0)
	{
		index++;

		if(strncmp(cmd, text, len) == 0)
		{
			return strdup(cmd);
		}
	}

	return (char *) 0;
}

template <typename CONFIG>
void ProcessorWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/processor.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_PROCESSOR_TCC__
