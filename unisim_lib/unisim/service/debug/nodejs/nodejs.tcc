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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_NODEJS_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_NODEJS_TCC__

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if HAVE_NODEJS

#include <unisim/service/debug/nodejs/nodejs.hh>
#include <unisim/util/likely/likely.hh>
#include <limits>

#if defined(HAVE_LIBEDIT)
#include <editline/readline.h>
#endif

#include <unisim/util/ostream/ostream.hh>
#include <unisim/service/debug/nodejs/source_code_location.tcc>
#include <unisim/service/debug/nodejs/breakpoint.tcc>
#include <unisim/service/debug/nodejs/watchpoint.tcc>
#include <unisim/service/debug/nodejs/data_object.tcc>
#include <unisim/service/debug/nodejs/pointer.tcc>
#include <unisim/service/debug/nodejs/hook.tcc>
#include <unisim/service/debug/nodejs/stub.tcc>
#include <unisim/service/debug/nodejs/processor.tcc>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::nodejs::ToString;
using unisim::util::ostream::ToString;
using unisim::util::nodejs::ToInt;

///////////////////////////////////////////////////////////////////////////////
//                                Definitions                                //
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////// NodeJS<> ///////////////////////////////////

template <typename CONFIG>
NodeJS<CONFIG>::NodeJS(const char *_name, unisim::kernel::Object *_parent)
	: unisim::kernel::Object(_name, _parent, "this service implements a debugging front-end for the Node.js JavaScript runtime environment")
	, unisim::util::nodejs::NodeJS(unisim::kernel::Simulator::Instance()->GetExecutablePath())
	, unisim::kernel::Service<unisim::service::interfaces::DebugYielding>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugProcessors<ADDRESS, TIME_TYPE> >(_name, _parent)
	, debug_yielding_export("debug-yielding-export", this)
	, debug_yielding_request_import("debug-yielding-request-import", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, stmt_lookup_import("stmt-lookup-import", this)
	, debug_info_loading_import("debug-info-loading-import", this)
	, subprogram_lookup_import("subprogram-lookup-import", this)
	, debug_processors_import("processors-import", this)
	, logger(*this)
	, shell(false)
	, stop_simulation_at_exit(false)
	, program_counter_name("pc")
	, arguments_length(0)
	, param_arguments()
	, param_verbose("verbose", this, this->verbose, "Enable/Disable verbosity")
	, param_debug("debug", this, this->debug, "Enable/Disable debug (intended for developper)")
	, param_shell("shell", this, this->shell, "Enable/Disable interactive shell")
	, param_stop_simulation_at_exit("stop-simulation-at-exit", this, stop_simulation_at_exit, "Enable/Disable stopping simulation when Javascript script exits")
	, param_program_counter_name("program-counter-name", this, program_counter_name, "name of program counter")
	, param_filename("filename", this, this->filename, "Filename of Javascript script (.js) to run")
	, param_arguments_length("arguments-length", this, this->arguments_length, "Number of command line arguments to pass to Javascript script")
	, std_output_stream(&std::cout)
	, std_error_stream(&std::cerr)
	, prompt(std::string(_name) + "> ")
	, processor_wrappers()
	, trap(false)
	, cont(false)
	, cont_exec_resolvers()
{
	this->SetDebugInfoStream(logger.DebugInfoStream());
	this->SetDebugWarningStream(logger.DebugWarningStream());
	this->SetDebugErrorStream(logger.DebugErrorStream());
	
	param_arguments_length.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	this->arguments.resize(arguments_length);
	for(unsigned int i = 0; i < arguments_length; ++i)
	{
		std::stringstream param_argument_name, param_argument_desc;
		param_argument_name << "arguments[" << i << "]";
		param_argument_desc << "Javascript script command line argument #" << i;
		ParamArgument *param_argument = new ParamArgument(param_argument_name.str().c_str(), this, this->arguments[i], param_argument_desc.str().c_str());
		param_arguments.push_back(param_argument);
	}
}

template <typename CONFIG>
NodeJS<CONFIG>::~NodeJS()
{
	Cleanup();
	
	for(ParamArguments::iterator it = param_arguments.begin(); it != param_arguments.end(); ++it)
	{
		ParamArgument *param_argument = *it;
		delete param_argument;
	}
}

template <typename CONFIG>
void NodeJS<CONFIG>::Trap()
{
	trap = true;
}

template <typename CONFIG>
void NodeJS<CONFIG>::Interrupt()
{
	if(shell)
	{
		Trap();
	}
		
	if(debug_yielding_request_import)
	{
		debug_yielding_request_import->DebugYieldRequest();
	}
}

template <typename CONFIG>
bool NodeJS<CONFIG>::GetCommand(std::string& cmd)
{
	std::string line;
	int line_count = 0;
	
	while(!Killed())
	{
		if(!GetLine(line)) return !Killed() && (line_count != 0);
		if(!line_count++) cmd.clear();
		if((line.length() == 0) || (line.back() != '\\'))
		{
			cmd.append(line);
#if defined(HAVE_LIBEDIT)
			if(cmd.length() > 0) add_history(cmd.c_str());
#endif
			return true;
		}
		cmd.append(line, 0, line.length() - 1);
		line.clear();
	}
	
	return false;
}

template <typename CONFIG>
char **NodeJS<CONFIG>::Completion(char *text, int start, int end)
{
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, (rl_compentry_func_t *) &NodeJS<CONFIG>::CommandGenerator);
}

template <typename CONFIG>
char *NodeJS<CONFIG>::CommandGenerator(char *text, int state)
{
	static int index, len;
	static const char *commands[] =
	{
		"DataObject(",
		"AddressHook(",
		"SourceCodeHook(",
		"SubProgramHook(",
		"Stub(",
		"continueExecution(",
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
bool NodeJS<CONFIG>::GetLine(std::string& line)
{
#if defined(HAVE_LIBEDIT)
	rl_completion_func_t *old_rl_attempted_completion_function = rl_attempted_completion_function;
	rl_attempted_completion_function = (rl_completion_func_t *) &NodeJS<CONFIG>::Completion;
	
	char *line_read;
	do
	{
		if(std_output_stream != &std::cout)
		{
			(*std_output_stream) << prompt;
		}
		line_read = readline(prompt.c_str());
		if(Killed())
		{
			rl_attempted_completion_function = old_rl_attempted_completion_function;
			return false;
		}
		if(!line_read)
		{
			rl_redisplay(); // work around when terminal size changes
		}
	} while(!line_read);

	if(std_output_stream != &std::cout)
	{
		(*std_output_stream) << line_read << std::endl;
	}

	line = line_read;

	free(line_read);
	
	rl_attempted_completion_function = old_rl_attempted_completion_function;
#else
	if(std_output_stream != &std::cout)
	{
		std::cout << prompt;
	}
	(*std_output_stream) << prompt;
	getline(std::cin, line);
	if(std::cin.fail()) return false;
	if(Killed()) return false;
	if(std_output_stream != &std::cout)
	{
		(*std_output_stream) << line << std::endl;
	}
#endif
	return true;
}

// unisim::service::interfaces::DebugYielding
template <typename CONFIG>
void NodeJS<CONFIG>::DebugYield()
{
	if(Killed()) return;
	
	this->YieldToNodeJS();
	
	if(likely(!shell || !trap))
	{
		return;
	}
	
	// REPL
	do
	{
		if(Killed()) return;
		
		std::string cmd;
		if(!GetCommand(cmd))
		{
			this->Stop(0);
			return;
		}
		
		trap = false;
		cont = false;
		{
			v8::Locker locker(GetIsolate());
			v8::Isolate::Scope isolate_scope(GetIsolate());
			v8::HandleScope handle_scope(isolate);
			v8::Local<v8::Context> context = this->GetContext();
			v8::Context::Scope context_scope(context);

			v8::Local<v8::Value> result = this->Execute(cmd, "<shell>");
			if(!result.IsEmpty())
			{
				if(result->IsPromise())
				{
					v8::Local<v8::Promise> promise = result.As<v8::Promise>();
					if(promise->State() == v8::Promise::kPending)
					{
						(*std_output_stream) << "Pending promise" << std::endl;
						v8::Local<v8::Function> resolve = CreateFunction<NodeJS<CONFIG>, &NodeJS<CONFIG>::Resolve>();
						v8::Local<v8::Function> reject = CreateFunction<NodeJS<CONFIG>, &NodeJS<CONFIG>::Reject>();
						promise = promise->Then(context, resolve, reject).ToLocalChecked();
						result.Clear();
					}
					else
					{
						result = promise->Result();
						if(promise->State() == v8::Promise::kFulfilled)
						{
							(*std_output_stream) << "Fullfilled promise: ";
						}
						else // if(promise->State() == v8::Promise::kRejected)
						{
							(*std_output_stream) << "Rejected promise: ";
						}
					}
				}
				
				if(!result.IsEmpty())
				{
					std::string result_str;
					if(ToString(isolate, result, result_str))
					{
						(*std_output_stream) << result_str << std::endl;
					}
					else
					{
						(*std_output_stream) << "<unprintable result>" << std::endl;
					}
				}
			}
		}
		
		this->YieldToNodeJS();
	}
	while(!cont);
}

// unisim::service::interfaces::DebugEventListener<ADDRESS>
template <typename CONFIG>
void NodeJS<CONFIG>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
	struct Bad {};
	throw Bad();
}

// unisim::kernel::Object
template <typename CONFIG>
bool NodeJS<CONFIG>::BeginSetup()
{
	return true;
}

// unisim::kernel::Object
template <typename CONFIG>
bool NodeJS<CONFIG>::EndSetup()
{
	if(!this->Initialize()) return false;

	Interrupt();
	
	YieldToNodeJS();
	
	return true;
}

template <typename CONFIG>
void NodeJS<CONFIG>::SigInt()
{
	Interrupt();
}

template <typename CONFIG>
void NodeJS<CONFIG>::Kill()
{
	unisim::kernel::Object::Kill();
	unisim::util::nodejs::NodeJS::Kill();
}

template <typename CONFIG>
bool NodeJS<CONFIG>::Killed() const
{
	return unisim::kernel::Object::Killed() || unisim::util::nodejs::NodeJS::Killed();
}

template <typename CONFIG>
void NodeJS<CONFIG>::Cleanup()
{
	for(typename Resolvers::iterator it = cont_exec_resolvers.begin(); it != cont_exec_resolvers.end(); ++it)
	{
		v8::Global<v8::Promise::Resolver>& resolver = *it;
		resolver.Reset();
	}
	
	Super::Cleanup();
	
	SourceCodeLocationWrapper<CONFIG>::Cleanup();
	BreakpointWrapper<CONFIG>::Cleanup();
	SourceCodeBreakpointWrapper<CONFIG>::Cleanup();
	SubProgramBreakpointWrapper<CONFIG>::Cleanup();
	WatchpointWrapper<CONFIG>::Cleanup();
	DataObjectWrapper<CONFIG>::Cleanup();
	PointerWrapper<CONFIG>::Cleanup();
	AddressHookWrapper<CONFIG>::Cleanup();
	SourceCodeHookWrapper<CONFIG>::Cleanup();
	SubProgramHookWrapper<CONFIG>::Cleanup();
	StubWrapper<CONFIG>::Cleanup();
	ProcessorWrapper<CONFIG>::Cleanup();
}

template <typename CONFIG>
void NodeJS<CONFIG>::AtExit()
{
	Cleanup();
	
	if(stop_simulation_at_exit)
	{
		Stop(this->exit_code, /* asynchronous */ true);
	}
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> NodeJS<CONFIG>::CreateGlobalObjectTemplate()
{
	// Create a template for the global object
	v8::Local<v8::ObjectTemplate> global_object_template = Super::CreateGlobalObjectTemplate();
	// Add constructor functions for builtin objects
	global_object_template->Set(isolate, SourceCodeLocationWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::SourceCodeLocationCtor>());
	global_object_template->Set(isolate, BreakpointWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::BreakpointCtor>());
	global_object_template->Set(isolate, SourceCodeBreakpointWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::SourceCodeBreakpointCtor>());
	global_object_template->Set(isolate, SubProgramBreakpointWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::SubProgramBreakpointCtor>());
	global_object_template->Set(isolate, WatchpointWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::WatchpointCtor>());
	global_object_template->Set(isolate, DataObjectWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::DataObjectCtor>());
	global_object_template->Set(isolate, PointerWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::PointerCtor>());
	global_object_template->Set(isolate, AddressHookWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::AddressHookCtor>());
	global_object_template->Set(isolate, SourceCodeHookWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::SourceCodeHookCtor>());
	global_object_template->Set(isolate, SubProgramHookWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::SubProgramHookCtor>());
	global_object_template->Set(isolate, StubWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::StubCtor>());
	global_object_template->Set(isolate, ProcessorWrapper<CONFIG>::CLASS_NAME, CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::ProcessorCtor>());
	// Add some builtin functions
	global_object_template->Set(isolate, "continueExecution", CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::ContinueExecution>());
	global_object_template->Set(isolate, "quit", CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::Quit>());
	return global_object_template;
}

template <typename CONFIG>
void NodeJS<CONFIG>::BeforeExecution()
{
	// Creates processor array
	v8::HandleScope handle_scope(GetIsolate());
	v8::Local<v8::Object> global_object = GetContext()->Global();

	unsigned int prc_count = debug_processors_import->DebugGetProcessorCount();
	processor_wrappers.resize(prc_count);
	
	for(unsigned int prc_num = 0; prc_num < prc_count; ++prc_num)
	{
		processor_wrappers[prc_num] = new ProcessorWrapper<CONFIG>(*this, debug_processors_import->DebugGetProcessor(prc_num), prc_num);
	}
	
	std::vector<v8::Local<v8::Value> > processor_objects;
	for(typename ProcessorWrappers::iterator it = processor_wrappers.begin(); it != processor_wrappers.end(); ++it)
	{
		ProcessorWrapper<CONFIG> *processor_wrapper = *it;
		processor_objects.push_back(processor_wrapper->MakeObject().template As<v8::Value>());
	}
	
	global_object->Set(
		GetContext(),
		v8::String::NewFromUtf8Literal(GetIsolate(), "processors"),
		v8::Array::New(GetIsolate(), &processor_objects[0], processor_objects.size())
	).ToChecked();

	CreateIsInstanceOf<SourceCodeLocationWrapper<CONFIG> >();
	CreateIsInstanceOf<BreakpointWrapper<CONFIG> >();
	CreateIsInstanceOf<SourceCodeBreakpointWrapper<CONFIG> >();
	CreateIsInstanceOf<SubProgramBreakpointWrapper<CONFIG> >();
	CreateIsInstanceOf<WatchpointWrapper<CONFIG> >();
	CreateIsInstanceOf<DataObjectWrapper<CONFIG> >();
	CreateIsInstanceOf<PointerWrapper<CONFIG> >();
	CreateIsInstanceOf<AddressHookWrapper<CONFIG> >();
	CreateIsInstanceOf<SourceCodeHookWrapper<CONFIG> >();
	CreateIsInstanceOf<SubProgramHookWrapper<CONFIG> >();
	CreateIsInstanceOf<StubWrapper<CONFIG> >();
	CreateIsInstanceOf<ProcessorWrapper<CONFIG> >();
}

// SourceCodeLocation(source_code_filename: string, lineno: Number, [colno: Number]) => SourceCodeLocation
// SourceCodeLocation(loc: string) => SourceCodeLocation
template <typename CONFIG>
void NodeJS<CONFIG>::SourceCodeLocationCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = SourceCodeLocationWrapper<CONFIG>::CLASS_NAME;
	const char *params = (args.Length() > 1) ? "(source_code_filename: string, lineno: Number, [colno: Number])" : "(loc: string)";
	v8::HandleScope handle_scope(args.GetIsolate());
	unisim::util::debug::SourceCodeLocation source_code_location;
	if(args.Length() > 1)
	{
		v8::Local<v8::Value> arg0 = args[0]; // source_code_filename
		std::string source_code_filename;
		if(!arg0->IsString() || !ToString(args.GetIsolate(), arg0, source_code_filename))
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects a string for 'source_code_filename'"));
			return;
		}
		v8::Local<v8::Value> arg1 = args[1]; // lineno
		if(!arg1->IsNumber())
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for 'lineno'"));
			return;
		}
		int64_t lineno = arg1->ToInteger(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Value();
		if((lineno < 1) || (lineno > std::numeric_limits<unsigned>::max()))
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for 'lineno'"));
			return;
		}
		int64_t colno = 0;
		if(args.Length() >= 3)
		{
			v8::Local<v8::Value> arg2 = args[2]; // colno
			if(!arg2->IsNumber())
			{
				this->Throw(this->TypeError(std::string(class_name) + params + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property 'loc.colno'"));
				return;
			}
			colno = arg2->ToInteger(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Value();
			if((colno < 1) || (colno > std::numeric_limits<unsigned>::max()))
			{
				this->Throw(this->RangeError(std::string(class_name) + params + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property 'loc.colno'"));
				return;
			}
		}
		
		if(this->debug)
		{
			logger << DebugInfo << class_name << "('" << source_code_filename << "', " << lineno << ", " << colno << ")" << EndDebugInfo;
		}
		
		source_code_location = unisim::util::debug::SourceCodeLocation(source_code_filename, lineno, colno);
	}
	else
	{
		v8::Local<v8::Value> arg0 = args[0]; // loc
		std::string loc;
		if(arg0->IsString() && ToString(args.GetIsolate(), arg0, loc))
		{
			if(this->debug)
			{
				logger << DebugInfo << class_name << "('" << loc << "'" << ")" << EndDebugInfo;
			}

			if(!source_code_location.Parse(loc))
			{
				std::ostringstream err_msg;
				err_msg << "'" << loc << "' is not a source code location";
				Throw(RangeError(err_msg.str()));
				return;
			}
		}
		else
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects a string for 'loc'"));
			return;
		}
	}
	
	SourceCodeLocationWrapper<CONFIG> *source_code_location_wrapper = new SourceCodeLocationWrapper<CONFIG>(*this, source_code_location);
	args.GetReturnValue().Set(source_code_location_wrapper->MakeObject());
}

// Breakpoint(processor: Processor, addr : Number) => Breakpoint
template <typename CONFIG>
void NodeJS<CONFIG>::BreakpointCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = BreakpointWrapper<CONFIG>::CLASS_NAME;
	const char *params = "(processor: Processor, addr : Number)";
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0];
	ProcessorWrapper<CONFIG> *processor_wrapper;
	if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a Processor for 'processor'"));
		return;
	}
	v8::Local<v8::Value> arg1 = args[1];
	ADDRESS addr = 0;
	if(!ToInt(args.GetIsolate(), arg1, addr))
	{
		this->Throw(this->Error(std::string(class_name) + params + " expects something convertible to a number for 'addr'"));
		return;
	}
	
	unisim::util::debug::Breakpoint<ADDRESS> *breakpoint = processor_wrapper->GetProcessor()->CreateBreakpoint(addr);
	if(this->debug)
	{
		logger << DebugInfo << class_name << "(" << ProcessorWrapper<CONFIG>::CLASS_NAME << "(" << processor_wrapper->GetId() << "), 0x" << std::hex << addr << std::dec << ")" << EndDebugInfo;
	}
	
	BreakpointWrapper<CONFIG> *breakpoint_wrapper = new BreakpointWrapper<CONFIG>(*this, *processor_wrapper, breakpoint);
	args.GetReturnValue().Set(breakpoint_wrapper->MakeObject());
}

// SourceCodeBreakpoint(processor: Processor, file : string, loc : (Object|string)) => SourceCodeBreakpoint
template <typename CONFIG>
void NodeJS<CONFIG>::SourceCodeBreakpointCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = SourceCodeBreakpointWrapper<CONFIG>::CLASS_NAME;
	const char *params = "(processor: Processor, file : string, loc : (Object|string))";
	v8::HandleScope handle_scope(args.GetIsolate());
	
	v8::Local<v8::Value> arg0 = args[0]; // processor
	ProcessorWrapper<CONFIG> *processor_wrapper;
	if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a Processor for 'processor'"));
		return;
	}
	v8::Local<v8::Value> arg1 = args[1]; // file
	std::string file;
	if(arg1->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg1, file))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a string for 'file'"));
		return;
	}
	unisim::util::debug::SourceCodeLocation source_code_location;
	v8::Local<v8::Value> arg2 = args[2]; // loc
	if(arg2->IsObject())
	{
		v8::Local<v8::Object> obj_loc = arg2.As<v8::Object>();
		std::string source_code_filename;
		v8::Local<v8::Value> prop_source_code_filename;
		if(!obj_loc->Get(
				args.GetIsolate()->GetCurrentContext(),
				v8::String::NewFromUtf8Literal(args.GetIsolate(), "source_code_filename")
			).ToLocal(&prop_source_code_filename) || !prop_source_code_filename->IsString() || !ToString(args.GetIsolate(), prop_source_code_filename, source_code_filename))
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects a string for property 'loc.source_code_filename'"));
			return;
		}
		v8::Local<v8::Value> prop_lineno;
		if(!obj_loc->Get(
				args.GetIsolate()->GetCurrentContext(),
				v8::String::NewFromUtf8Literal(args.GetIsolate(), "lineno")
			).ToLocal(&prop_lineno) || !prop_lineno->IsNumber())
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property 'loc.lineno'"));
			return;
		}
		int64_t lineno = prop_lineno->ToInteger(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Value();
		if((lineno < 1) || (lineno > std::numeric_limits<unsigned>::max()))
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property 'loc.lineno'"));
			return;
		}
		int64_t colno = 0;
		if(obj_loc->HasOwnProperty(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(args.GetIsolate(), "colno")).ToChecked())
		{
			v8::Local<v8::Value> prop_colno;
			if(obj_loc->Get(
					args.GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8Literal(args.GetIsolate(), "colno")
				).ToLocal(&prop_colno) && prop_colno->IsNumber())
			{
				colno = prop_colno->ToInteger(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Value();
				if((colno < 1) || (colno > std::numeric_limits<unsigned>::max()))
				{
					this->Throw(this->TypeError(std::string(class_name) + params + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property 'loc.colno'"));
					return;
				}
			}
		}
		
		if(this->debug)
		{
			logger << DebugInfo << class_name << "(" << ProcessorWrapper<CONFIG>::CLASS_NAME << "(" << processor_wrapper->GetId() << "), '" << file << "'" << ", { source_code_filename: '" << source_code_filename << "', lineno: " << lineno << ", colno: " << colno << "})" << EndDebugInfo;
		}
		
		source_code_location = unisim::util::debug::SourceCodeLocation(source_code_filename, lineno, colno);
	}
	else
	{
		std::string loc;
		if(arg2->IsString() && ToString(args.GetIsolate(), arg2, loc))
		{
			if(this->debug)
			{
				logger << DebugInfo << class_name << "(" << ProcessorWrapper<CONFIG>::CLASS_NAME << "(" << processor_wrapper->GetId() << "), '" << file << "'" << ",'" << loc << "'" << ")" << EndDebugInfo;
			}

			if(!source_code_location.Parse(loc))
			{
				std::ostringstream err_msg;
				err_msg << "'" << loc << "' is not a source code location";
				Throw(RangeError(err_msg.str()));
				return;
			}
		}
		else
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects an object or a string for 'loc'"));
			return;
		}
	}
	
	unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *source_code_breakpoint = processor_wrapper->GetProcessor()->CreateSourceCodeBreakpoint(source_code_location, file);
	SourceCodeBreakpointWrapper<CONFIG> *source_code_breakpoint_wrapper = new SourceCodeBreakpointWrapper<CONFIG>(*this, *processor_wrapper, source_code_breakpoint);
	args.GetReturnValue().Set(source_code_breakpoint_wrapper->MakeObject());
}

// SubProgramBreakpoint(processor: Processor, file : string, name : string) => SubProgramBreakpoint
template <typename CONFIG>
void NodeJS<CONFIG>::SubProgramBreakpointCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = SubProgramHookWrapper<CONFIG>::CLASS_NAME;
	const char *params = "(processor: Processor, file : string, name : string)";
	v8::HandleScope handle_scope(args.GetIsolate());
	
	v8::Local<v8::Value> arg0 = args[0];
	ProcessorWrapper<CONFIG> *processor_wrapper;
	if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a Processor for 'processor'"));
		return;
	}
	v8::Local<v8::Value> arg1 = args[1];
	std::string file;
	if(arg1->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg1, file))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a string for 'file'"));
		return;
	}
	v8::Local<v8::Value> arg2 = args[2];
	std::string name;
	if(arg2->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg2, name))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a string for 'name'"));
		return;
	}
	if(this->debug)
	{
		logger << DebugInfo << class_name << "(" << ProcessorWrapper<CONFIG>::CLASS_NAME << "(" << processor_wrapper->GetId() << "), '" << file << "'" << ",'" << name << "'" << ")" << EndDebugInfo;
	}
	
	const unisim::util::debug::SubProgram<ADDRESS> *subprogram = this->subprogram_lookup_import->FindSubProgram(name.c_str(), file.c_str());
	
	if(!subprogram)
	{
		std::ostringstream err_msg;
		err_msg << "Can't find subprogram '" << name << "' in File '" << file << "'";
		Throw(RangeError(err_msg.str()));
		return;
	}
	
	unisim::util::debug::SubProgramBreakpoint<ADDRESS> *subprogram_breakpoint = processor_wrapper->GetProcessor()->CreateSubProgramBreakpoint(subprogram);
	
	SubProgramBreakpointWrapper<CONFIG> *subprogram_breakpoint_wrapper = new SubProgramBreakpointWrapper<CONFIG>(*this, *processor_wrapper, subprogram_breakpoint);
	
	args.GetReturnValue().Set(subprogram_breakpoint_wrapper->MakeObject());
}

// Watchpoint(processor: Processor, addr : Number, size: Number, [options: { [mt: string], [mat: string], [overlooks: boolean] }]) => Watchpoint
template <typename CONFIG>
void NodeJS<CONFIG>::WatchpointCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = WatchpointWrapper<CONFIG>::CLASS_NAME;
	const char *params = "(processor: Processor, addr : Number, size: Number, [options: { [mt: string], [mat: string], [overlooks: boolean] }])";
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0];
	ProcessorWrapper<CONFIG> *processor_wrapper;
	if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a Processor for 'processor'"));
		return;
	}
	v8::Local<v8::Value> arg1 = args[1]; // addr
	ADDRESS addr = 0;
	if(!ToInt(args.GetIsolate(), arg1, addr))
	{
		this->Throw(this->Error(std::string(class_name) + params + " expects a number for 'addr'"));
		return;
	}
	v8::Local<v8::Value> arg2 = args[2]; // size
	uint32_t size = 0;
	if(!ToInt(args.GetIsolate(), arg2, size))
	{
		this->Throw(this->Error(std::string(class_name) + params + " expects a number for 'size'"));
		return;
	}
	unisim::util::debug::MemoryAccessType mat = unisim::util::debug::MAT_WRITE;
	unisim::util::debug::MemoryType mt = unisim::util::debug::MT_DATA;
	bool overlook = true;
	if(args.Length() >= 4)
	{
		v8::Local<v8::Value> arg3 = args[3]; // options
		if(!arg3->IsObject())
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects an object for 'options'"));
			return;
		}
		v8::Local<v8::Object> obj_options = arg3.As<v8::Object>();
		if(obj_options->HasOwnProperty(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(args.GetIsolate(), "mat")).ToChecked())
		{
			v8::Local<v8::Value> prop_options_mat;
			if(!obj_options->Get(
					args.GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8Literal(args.GetIsolate(), "mat")
				).ToLocal(&prop_options_mat) || !prop_options_mat->IsString() || !ToMemoryAccessType(args.GetIsolate(), prop_options_mat, mat))
			{
				this->Throw(this->TypeError(std::string(class_name) + params + " expects a string for property 'options.mat'"));
				return;
			}
		}
		if(obj_options->HasOwnProperty(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(args.GetIsolate(), "mt")).ToChecked())
		{
			v8::Local<v8::Value> prop_options_mt;
			if(!obj_options->Get(
					args.GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8Literal(args.GetIsolate(), "mt")
				).ToLocal(&prop_options_mt) || !prop_options_mt->IsString() || !ToMemoryType(args.GetIsolate(), prop_options_mt, mt))
			{
				this->Throw(this->TypeError(std::string(class_name) + params + " expects a string for property 'options.mt'"));
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
				this->Throw(this->TypeError(std::string(class_name) + params + " expects a boolean for property 'options.overlook'"));
				return;
			}
			overlook = prop_options_overlook->ToBoolean(args.GetIsolate())->Value();
		}
	}
	
	unisim::util::debug::Watchpoint<ADDRESS> *watchpoint = processor_wrapper->GetProcessor()->CreateWatchpoint(mat, mt, addr, size, overlook);
	if(this->debug)
	{
		logger << DebugInfo << class_name << "(" << ProcessorWrapper<CONFIG>::CLASS_NAME << "(" << processor_wrapper->GetId() << "), 0x" << std::hex << addr << std::dec << ", " << size << ", { mat: '" << ToString(mat) << "', mt: '" << ToString(mt) << "', overlook:" << (overlook ? "true" : "false") << " })" << EndDebugInfo;
	}
	
	WatchpointWrapper<CONFIG> *watchpoint_wrapper = new WatchpointWrapper<CONFIG>(*this, *processor_wrapper, watchpoint);
	args.GetReturnValue().Set(watchpoint_wrapper->MakeObject());
}

// DataObject(processor: Processor, expr : string) => DataObject
template <typename CONFIG>
void NodeJS<CONFIG>::DataObjectCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = DataObjectWrapper<CONFIG>::CLASS_NAME;
	const char *params = "(processor: Processor, expr : string)";
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0];
	ProcessorWrapper<CONFIG> *processor_wrapper;
	if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a Processor for 'processor'"));
		return;
	}
	v8::Local<v8::Value> arg1 = args[1];
	std::string expr;
	if(arg1->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg1, expr))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a string for 'expr'"));
		return;
	}
	if(this->debug)
	{
		logger << DebugInfo << class_name << "(" << ProcessorWrapper<CONFIG>::CLASS_NAME << "(" << processor_wrapper->GetId() << "), '" << expr << "'" << ")" << EndDebugInfo;
	}
	
	DataObjectWrapper<CONFIG> *data_object = new DataObjectWrapper<CONFIG>(*this, *processor_wrapper, expr);
	args.GetReturnValue().Set(data_object->MakeObject());
}

// Pointer() shall not be called
template <typename CONFIG>
void NodeJS<CONFIG>::PointerCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = PointerWrapper<CONFIG>::CLASS_NAME;
	Throw(Error(std::string(class_name) + " is not constructible"));
}

// AddressHook(processor: Processor, addr : Number) => AddressHook
template <typename CONFIG>
void NodeJS<CONFIG>::AddressHookCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = AddressHookWrapper<CONFIG>::CLASS_NAME;
	const char *params = "(processor: Processor, addr : Number)";
	v8::HandleScope handle_scope(args.GetIsolate());
	
	v8::Local<v8::Value> arg0 = args[0];
	ProcessorWrapper<CONFIG> *processor_wrapper;
	if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a Processor for 'processor'"));
		return;
	}
	
	v8::Local<v8::Value> arg1 = args[1];
	
	ADDRESS addr = 0;
	if(!ToInt(args.GetIsolate(), arg1, addr))
	{
		Throw(Error(std::string(class_name) + params + " expects a number for 'addr'"));
		return;
	}
	
	if(this->debug)
	{
		logger << DebugInfo << class_name << "(0x" << std::hex << addr << std::dec << ")" << EndDebugInfo;
	}

	AddressHook<CONFIG> *address_hook = new AddressHook<CONFIG>(addr);
	
	AddressHookWrapper<CONFIG> *address_hook_wrapper = new AddressHookWrapper<CONFIG>(*this, *processor_wrapper, address_hook);
	
	args.GetReturnValue().Set(address_hook_wrapper->MakeObject());
}

// SourceCodeHook(processor: Processor, file : string, loc : (Object|string)) => SourceCodeHook
template <typename CONFIG>
void NodeJS<CONFIG>::SourceCodeHookCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = SourceCodeHookWrapper<CONFIG>::CLASS_NAME;
	const char *params = "(processor: Processor, file : string, loc : (Object|string))";
	v8::HandleScope handle_scope(args.GetIsolate());
	
	v8::Local<v8::Value> arg0 = args[0];
	ProcessorWrapper<CONFIG> *processor_wrapper;
	if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a Processor for 'processor'"));
		return;
	}
	v8::Local<v8::Value> arg1 = args[1];
	std::string file;
	if(arg1->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg1, file))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a string for 'file'"));
		return;
	}
	unisim::util::debug::SourceCodeLocation source_code_location;
	v8::Local<v8::Value> arg2 = args[2];
	if(arg2->IsObject())
	{
		v8::Local<v8::Object> obj_loc = arg2.As<v8::Object>();
		std::string source_code_filename;
		v8::Local<v8::Value> prop_source_code_filename;
		if(!obj_loc->Get(
				args.GetIsolate()->GetCurrentContext(),
				v8::String::NewFromUtf8Literal(args.GetIsolate(), "source_code_filename")
			).ToLocal(&prop_source_code_filename) || !prop_source_code_filename->IsString() || !ToString(args.GetIsolate(), prop_source_code_filename, source_code_filename))
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects a string for property 'loc.source_code_filename'"));
			return;
		}
		v8::Local<v8::Value> prop_lineno;
		if(!obj_loc->Get(
				args.GetIsolate()->GetCurrentContext(),
				v8::String::NewFromUtf8Literal(args.GetIsolate(), "lineno")
			).ToLocal(&prop_lineno) || !prop_lineno->IsNumber())
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property 'loc.lineno'"));
			return;
		}
		int64_t lineno = prop_lineno->ToInteger(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Value();
		if((lineno < 1) || (lineno > std::numeric_limits<unsigned>::max()))
		{
			this->Throw(this->RangeError(std::string(class_name) + params + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property 'loc.lineno'"));
			return;
		}
		int64_t colno = 0;
		v8::Local<v8::Value> prop_colno;
		if(obj_loc->Get(
				args.GetIsolate()->GetCurrentContext(),
				v8::String::NewFromUtf8Literal(args.GetIsolate(), "colno")
			).ToLocal(&prop_colno) && prop_colno->IsNumber())
		{
			colno = prop_colno->ToInteger(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Value();
			if((colno < 1) || (colno > std::numeric_limits<unsigned>::max()))
			{
				this->Throw(this->TypeError(std::string(class_name) + params + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property 'loc.colno'"));
				return;
			}
		}
		
		if(this->debug)
		{
			logger << DebugInfo << class_name << "(" << ProcessorWrapper<CONFIG>::CLASS_NAME << "(" << processor_wrapper->GetId() << "), '" << file << "'" << ", { source_code_filename: '" << source_code_filename << "', lineno: " << lineno << ", colno: " << colno << "})" << EndDebugInfo;
		}
		
		source_code_location = unisim::util::debug::SourceCodeLocation(source_code_filename, lineno, colno);
	}
	else
	{
		std::string loc;
		if(arg2->IsString() && ToString(args.GetIsolate(), arg2, loc))
		{
			if(this->debug)
			{
				logger << DebugInfo << class_name << "(" << ProcessorWrapper<CONFIG>::CLASS_NAME << "(" << processor_wrapper->GetId() << "), '" << file << "'" << ",'" << loc << "'" << ")" << EndDebugInfo;
			}

			if(!source_code_location.Parse(loc))
			{
				std::ostringstream err_msg;
				err_msg << "'" << loc << "' is not a source code location";
				Throw(RangeError(err_msg.str()));
				return;
			}
		}
		else
		{
			this->Throw(this->TypeError(std::string(class_name) + params + " expects an object or a string for 'loc'"));
			return;
		}
	}

	SourceCodeHook<CONFIG> *source_code_hook = new SourceCodeHook<CONFIG>(source_code_location, file);
	
	SourceCodeHookWrapper<CONFIG> *source_code_hook_wrapper = new SourceCodeHookWrapper<CONFIG>(*this, *processor_wrapper, source_code_hook);
	
	args.GetReturnValue().Set(source_code_hook_wrapper->MakeObject());
}

// SubProgramHook(processor: Processor, file : string, name : string) => SubProgramHook
template <typename CONFIG>
void NodeJS<CONFIG>::SubProgramHookCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = SubProgramHookWrapper<CONFIG>::CLASS_NAME;
	const char *params = "(processor: Processor, file : string, name : string)";
	v8::HandleScope handle_scope(args.GetIsolate());
	
	v8::Local<v8::Value> arg0 = args[0];
	ProcessorWrapper<CONFIG> *processor_wrapper;
	if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a Processor for 'processor'"));
		return;
	}
	v8::Local<v8::Value> arg1 = args[1];
	std::string file;
	if(arg1->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg1, file))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a string for 'file'"));
		return;
	}
	v8::Local<v8::Value> arg2 = args[2];
	std::string name;
	if(arg2->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg2, name))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a string for 'loc'"));
		return;
	}
	if(this->debug)
	{
		logger << DebugInfo << class_name << "(" << ProcessorWrapper<CONFIG>::CLASS_NAME << "(" << processor_wrapper->GetId() << "), '" << file << "'" << ",'" << name << "'" << ")" << EndDebugInfo;
	}
	
	const unisim::util::debug::SubProgram<ADDRESS> *subprogram = this->subprogram_lookup_import->FindSubProgram(name.c_str(), file.c_str());
	
	if(!subprogram)
	{
		std::ostringstream err_msg;
		err_msg << "Can't find subprogram '" << name << "' in File '" << file << "'";
		Throw(RangeError(err_msg.str()));
		return;
	}
	
	SubProgramHook<CONFIG> *subprogram_hook = new SubProgramHook<CONFIG>(subprogram);
	
	SubProgramHookWrapper<CONFIG> *subprogram_hook_wrapper = new SubProgramHookWrapper<CONFIG>(*this, *processor_wrapper, subprogram_hook);
	
	args.GetReturnValue().Set(subprogram_hook_wrapper->MakeObject());
}

// Stub(processor: Processor, file : string, name : string) => Stub
template <typename CONFIG>
void NodeJS<CONFIG>::StubCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = StubWrapper<CONFIG>::CLASS_NAME;
	const char *params = "(processor: Processor, file : string, name : string)";
	v8::HandleScope handle_scope(args.GetIsolate());
	
	v8::Local<v8::Value> arg0 = args[0];
	ProcessorWrapper<CONFIG> *processor_wrapper;
	if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a Processor for 'processor'"));
		return;
	}
	v8::Local<v8::Value> arg1 = args[1];
	std::string file;
	if(arg1->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg1, file))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a string for 'file'"));
		return;
	}
	v8::Local<v8::Value> arg2 = args[2];
	std::string name;
	if(arg2->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg2, name))
	{
		Throw(TypeError(std::string(class_name) + params + " expects a string for 'name'"));
		return;
	}
	if(this->debug)
	{
		logger << DebugInfo << class_name << "(" << ProcessorWrapper<CONFIG>::CLASS_NAME << "(" << processor_wrapper->GetId() << "), '" << file << "'" << ",'" << name << "'" << ")" << EndDebugInfo;
	}
	
	const unisim::util::debug::SubProgram<ADDRESS> *subprogram = this->subprogram_lookup_import->FindSubProgram(name.c_str(), file.c_str());
	
	if(!subprogram)
	{
		std::ostringstream err_msg;
		err_msg << "Can't find subprogram '" << name << "' in File '" << file << "'";
		Throw(RangeError(err_msg.str()));
		return;
	}
	
	Stub<CONFIG> *stub = new Stub<CONFIG>(*this, subprogram);
	
	StubWrapper<CONFIG> *stub_wrapper = new StubWrapper<CONFIG>(*this, *processor_wrapper, stub);
	
	args.GetReturnValue().Set(stub_wrapper->MakeObject());
}

// Processor(id : integer) => Processor
template <typename CONFIG>
void NodeJS<CONFIG>::ProcessorCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	const char *class_name = ProcessorWrapper<CONFIG>::CLASS_NAME;
	const char *params = "(id : integer)";
	v8::HandleScope handle_scope(args.GetIsolate());
	
	v8::Local<v8::Value> arg0 = args[0];
	v8::Local<v8::Integer> integer;
	if(arg0->IsUndefined() || !arg0->ToInteger(args.GetIsolate()->GetCurrentContext()).ToLocal(&integer))
	{
		Throw(TypeError(std::string(class_name) + params + " expects an integer for 'id'"));
		return;
	}
	int64_t id = integer->Value();
	
	if((id >= 0) && (uint64_t(id) < processor_wrappers.size()))
	{
		args.GetReturnValue().Set(processor_wrappers[id]->MakeObject());
	}
}

// continueExecution() => Promise
template <typename CONFIG>
void NodeJS<CONFIG>::ContinueExecution(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Context> context = args.GetIsolate()->GetCurrentContext();
	
	v8::Local<v8::Promise::Resolver> resolver;
	if(!v8::Promise::Resolver::New(context).ToLocal(&resolver))
	{
		DebugErrorStream() << "Can't create Javascript promise resolver" << std::endl;
		Kill();
		return;
	}
	cont_exec_resolvers.resize(cont_exec_resolvers.size() + 1);
	cont_exec_resolvers.back().Reset(args.GetIsolate(), resolver);
	args.GetReturnValue().Set(resolver->GetPromise());

	Continue();
}

// quit()
template <typename CONFIG>
void NodeJS<CONFIG>::Quit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	Kill();
	this->Stop(0, /* asynchronous */ true);
}

template <typename CONFIG>
void NodeJS<CONFIG>::Resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> result = args[0];
	(*std_output_stream) << "Fullfilled promise: ";
	std::string result_str;
	if(ToString(isolate, result, result_str))
	{
		(*std_output_stream) << result_str << std::endl;
	}
	else
	{
		(*std_output_stream) << "<unprintable result>" << std::endl;
	}
	Trap();
}

template <typename CONFIG>
void NodeJS<CONFIG>::Reject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> result = args[0];
	(*std_output_stream) << "Rejected promise: ";
	std::string result_str;
	if(ToString(isolate, result, result_str))
	{
		(*std_output_stream) << result_str << std::endl;
	}
	else
	{
		(*std_output_stream) << "<unprintable result>" << std::endl;
	}
	Trap();
}

template <typename CONFIG>
void NodeJS<CONFIG>::Continue()
{
	cont = true;
}

template <typename CONFIG>
void NodeJS<CONFIG>::ResolveContinue(v8::Local<v8::Value> value)
{
	for(typename Resolvers::iterator it = cont_exec_resolvers.begin(); it != cont_exec_resolvers.end(); ++it)
	{
		v8::Global<v8::Promise::Resolver>& resolver = *it;
		if(!resolver.IsEmpty())
		{
			v8::Local<v8::Promise::Resolver> local_resolver = resolver.Get(this->GetIsolate());
			local_resolver->Resolve(this->GetIsolate()->GetCurrentContext(), value).ToChecked();
		}
	}
}

/////////////////////////////// ObjectWrapper<> ////////////////////////////////

template <typename CONFIG>
ObjectWrapper<CONFIG>::ObjectWrapper(NodeJS<CONFIG>& _nodejs, std::size_t size)
	: Super(_nodejs, size ? size : sizeof(*this))
	, nodejs(_nodejs)
{
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // HAVE_NODEJS

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_NODEJS_TCC__
