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
#include <regex>

#if defined(HAVE_LIBEDIT)
#include <editline/readline.h>
#endif

#include <unisim/util/ostream/ostream.hh>
#include <unisim/service/debug/nodejs/source_code_location.tcc>
#include <unisim/service/debug/nodejs/debug_event.tcc>
#include <unisim/service/debug/nodejs/breakpoint.tcc>
#include <unisim/service/debug/nodejs/watchpoint.tcc>
#include <unisim/service/debug/nodejs/data_object.tcc>
#include <unisim/service/debug/nodejs/pointer.tcc>
#include <unisim/service/debug/nodejs/hook.tcc>
#include <unisim/service/debug/nodejs/stub.tcc>
#include <unisim/service/debug/nodejs/processor.tcc>
#include <unisim/service/debug/nodejs/register.tcc>
#include <unisim/service/debug/nodejs/executable_binary_file.tcc>
#include <unisim/service/debug/nodejs/stack_frame_info.tcc>
#include <unisim/service/debug/nodejs/debug_symbol.tcc>
#include <unisim/service/debug/nodejs/statement.tcc>
#include <unisim/service/debug/nodejs/subprogram.tcc>
#include <unisim/service/debug/nodejs/type.tcc>
#include <unisim/util/locate/locate.hh>

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

///////////////////////////// Helper functions ////////////////////////////////

template <typename ADDRESS>
bool ToScope(v8::Isolate *isolate, v8::Local<v8::Value> value, typename unisim::service::interfaces::StatementLookupBase::Scope& out)
{
	std::string str;
	if(!ToString(isolate, value, str)) return false;
	std::istringstream sstr(str);
	if(sstr >> out) return true; else return false;
}

////////////////////////////////// NodeJS<> ///////////////////////////////////

template <typename CONFIG>
std::mutex NodeJS<CONFIG>::rl_mutex;

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
bool NodeJS<CONFIG>::GetLine(std::string& line)
{
#if defined(HAVE_LIBEDIT)
	struct ReadLineScope
	{
		rl_completion_func_t *old_rl_attempted_completion_function;
		const char * old_rl_basic_word_break_characters;
		
		ReadLineScope()
			: old_rl_attempted_completion_function(rl_attempted_completion_function)
			, old_rl_basic_word_break_characters(rl_basic_word_break_characters)
		{
			rl_attempted_completion_function = (rl_completion_func_t *) &NodeJS<CONFIG>::Completion;
			rl_basic_word_break_characters = " \t\n,.()[]:{}+-/*%&|;?\"'<=>";
		}
		
		~ReadLineScope()
		{
			rl_basic_word_break_characters = old_rl_basic_word_break_characters;
			rl_attempted_completion_function = old_rl_attempted_completion_function;
		}
	};
	
	std::unique_lock<std::mutex> lock(rl_mutex);
	{
		ReadLineScope rl_scope;
		
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
	}
	
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

template <typename CONFIG>
char **NodeJS<CONFIG>::Completion(char *text, int start, int end)
{
	rl_compentry_func_t *generator = 0;
	do
	{
		if(start)
		{
			std::string str(&rl_line_buffer[0], start);
			if(std::regex_match(str, std::regex("^help +$")))
			{
				generator = (rl_compentry_func_t *) &NodeJS<CONFIG>::HelpCompletionGenerator;
				break;
			}
			if(std::regex_match(str, std::regex("^.*new +$")))
			{
				generator = (rl_compentry_func_t *) &NodeJS<CONFIG>::ConstructorCompletionGenerator;
				break;
			}
			if(std::regex_match(str, std::regex("^.*processors\\[[0-9]+\\] *\\. *$")))
			{
				generator = (rl_compentry_func_t *) &NodeJS<CONFIG>::ProcessorCompletionGenerator;
				break;
			}
			if(std::regex_match(str, std::regex("^.*processors\\[[0-9]+\\] *\\. *registers *\\.[a-zA-Z_][a-zA-Z0-9_]* *\\. *$")))
			{
				generator = (rl_compentry_func_t *) &NodeJS<CONFIG>::RegisterCompletionGenerator;
				break;
			}
		}
		
		generator = (rl_compentry_func_t *) &NodeJS<CONFIG>::GlobalCompletionGenerator;
	}
	while(0);
	
	rl_attempted_completion_over = 1;
	rl_completion_append_character = 0;
	return rl_completion_matches(text, generator);
}

template <typename CONFIG>
char *NodeJS<CONFIG>::CompletionGenerator(char *text, int state, Commands& commands)
{
	static Commands::size_type index;
	static size_t len;
	
	if(!state)
	{
		index = 0;
		len = strlen(text);
	}

	while(index < commands.size())
	{
		const char *cmd = commands[index].c_str();
		index++;

		if(strncmp(cmd, text, len) == 0)
		{
			return strdup(cmd);
		}
	}

	return (char *) 0;
}

template <typename CONFIG>
char *NodeJS<CONFIG>::GlobalCompletionGenerator(char *text, int state)
{
	static Commands commands =
	{
		"continueExecution()", "quit()", "loadDebugInfo()", "getExecutableBinaryFiles()",
		"getSymbols()", "findSymbol(", "getStatements()", "findStatement(", "findStatements(",
		"findSubProgram(", "processors[0]", "console.log(", "help", "new"
	};
	return CompletionGenerator(text, state, commands);
}

template <typename CONFIG>
char *NodeJS<CONFIG>::HelpCompletionGenerator(char *text, int state)
{
	static Commands commands =
	{
		SourceCodeLocationWrapper<CONFIG>  ::CLASS_NAME, DebugEventWrapper<CONFIG>          ::CLASS_NAME,
		BreakpointWrapper<CONFIG>          ::CLASS_NAME, SourceCodeBreakpointWrapper<CONFIG>::CLASS_NAME,
		SubProgramBreakpointWrapper<CONFIG>::CLASS_NAME, WatchpointWrapper<CONFIG>          ::CLASS_NAME,
		DataObjectWrapper<CONFIG>          ::CLASS_NAME, PointerWrapper<CONFIG>             ::CLASS_NAME,
		HookWrapper<CONFIG>                ::CLASS_NAME, AddressHookWrapper<CONFIG>         ::CLASS_NAME,
		SourceCodeHookWrapper<CONFIG>      ::CLASS_NAME, SubProgramHookWrapper<CONFIG>      ::CLASS_NAME,
		StubWrapper<CONFIG>                ::CLASS_NAME, ProcessorWrapper<CONFIG>           ::CLASS_NAME,
		RegisterWrapper<CONFIG>            ::CLASS_NAME, ExecutableBinaryFileWrapper<CONFIG>::CLASS_NAME,
		StackFrameInfoWrapper<CONFIG>      ::CLASS_NAME, DebugSymbolWrapper<CONFIG>         ::CLASS_NAME,
		StatementWrapper<CONFIG>           ::CLASS_NAME, SubProgramWrapper<CONFIG>          ::CLASS_NAME,
		TypeWrapper<CONFIG>                ::CLASS_NAME, NamedTypeWrapper<CONFIG>           ::CLASS_NAME,
		BaseTypeWrapper<CONFIG>            ::CLASS_NAME, IntegerTypeWrapper<CONFIG>         ::CLASS_NAME,
		CharTypeWrapper<CONFIG>            ::CLASS_NAME, FloatingPointTypeWrapper<CONFIG>   ::CLASS_NAME,
		BooleanTypeWrapper<CONFIG>         ::CLASS_NAME, MemberWrapper<CONFIG>              ::CLASS_NAME,
		CompositeTypeWrapper<CONFIG>       ::CLASS_NAME, StructureTypeWrapper<CONFIG>       ::CLASS_NAME,
		UnionTypeWrapper<CONFIG>           ::CLASS_NAME, ClassTypeWrapper<CONFIG>           ::CLASS_NAME,
		InterfaceTypeWrapper<CONFIG>       ::CLASS_NAME, ArrayTypeWrapper<CONFIG>           ::CLASS_NAME,
		PointerTypeWrapper<CONFIG>         ::CLASS_NAME, TypedefWrapper<CONFIG>             ::CLASS_NAME,
		FormalParameterWrapper<CONFIG>     ::CLASS_NAME, FunctionTypeWrapper<CONFIG>        ::CLASS_NAME,
		ConstTypeWrapper<CONFIG>           ::CLASS_NAME, EnumeratorWrapper<CONFIG>          ::CLASS_NAME,
		EnumTypeWrapper<CONFIG>            ::CLASS_NAME, UnspecifiedTypeWrapper<CONFIG>     ::CLASS_NAME,
		VolatileTypeWrapper<CONFIG>        ::CLASS_NAME,
		"continueExecution", "quit", "loadDebugInfo", "getExecutableBinaryFiles", "getSymbols",
		"findSymbol", "getStatements", "findStatement", "findStatements", "findSubProgram", "processors"
	};
	return CompletionGenerator(text, state, commands);
}

template <typename CONFIG>
char *NodeJS<CONFIG>::ConstructorCompletionGenerator(char *text, int state)
{
	static Commands commands =
	{
		std::string(SourceCodeLocationWrapper<CONFIG>  ::CLASS_NAME) + "(",
		std::string(BreakpointWrapper<CONFIG>          ::CLASS_NAME) + "(",
		std::string(SourceCodeBreakpointWrapper<CONFIG>::CLASS_NAME) + "(",
		std::string(SubProgramBreakpointWrapper<CONFIG>::CLASS_NAME) + "(",
		std::string(WatchpointWrapper<CONFIG>          ::CLASS_NAME) + "(",
		std::string(DataObjectWrapper<CONFIG>          ::CLASS_NAME) + "(",
		std::string(AddressHookWrapper<CONFIG>         ::CLASS_NAME) + "(",
		std::string(SourceCodeHookWrapper<CONFIG>      ::CLASS_NAME) + "(",
		std::string(SubProgramHookWrapper<CONFIG>      ::CLASS_NAME) + "(",
		std::string(StubWrapper<CONFIG>                ::CLASS_NAME) + "(",
	};
	return CompletionGenerator(text, state, commands);
}

template <typename CONFIG>
char *NodeJS<CONFIG>::ProcessorCompletionGenerator(char *text, int state)
{
	static Commands commands =
	{
		"stepInstruction()", "nextInstruction()", "step()", "stepInto()", "next()", "stepOver()",
		"finish()", "stepOut()", "returnFromFunction()", "disasm(", "readMemory(", "writeMemory(",
		"getStackFrameInfos()", "selectStackFrame(", "getTime()", "getDataObjectNames()", "registers."
	};
	return CompletionGenerator(text, state, commands);
}

template <typename CONFIG>
char *NodeJS<CONFIG>::RegisterCompletionGenerator(char *text, int state)
{
	static Commands commands = { "get()", "set()", "name", "description", "size" };
	return CompletionGenerator(text, state, commands);
}

template <typename CONFIG>
void NodeJS<CONFIG>::Help(std::ostream& stream, const std::string& section)
{
	if(section.empty())
	{
		stream <<
#include <unisim/service/debug/nodejs/doc/index.h>
		;
		return;
	}
	
	if((section == "continueExecution"       ) ||
	   (section == "quit"                    ) ||
	   (section == "loadDebugInfo"           ) ||
	   (section == "getExecutableBinaryFiles") ||
	   (section == "getSymbols"              ) ||
	   (section == "findSymbol"              ) ||
	   (section == "getStatements"           ) ||
	   (section == "findStatement"           ) ||
	   (section == "findStatements"          ) ||
	   (section == "findSubProgram"          ) ||
	   (section == "processors"              ))
	{
		stream <<
#include <unisim/service/debug/nodejs/doc/global.h>
		;
		return;
	}
	
	if(section == SourceCodeLocationWrapper<CONFIG>  ::CLASS_NAME) { SourceCodeLocationWrapper<CONFIG>  ::Help(stream); return; }
	if(section == DebugEventWrapper<CONFIG>          ::CLASS_NAME) { DebugEventWrapper<CONFIG>          ::Help(stream); return; }
	if(section == BreakpointWrapper<CONFIG>          ::CLASS_NAME) { BreakpointWrapper<CONFIG>          ::Help(stream); return; }
	if(section == SourceCodeBreakpointWrapper<CONFIG>::CLASS_NAME) { SourceCodeBreakpointWrapper<CONFIG>::Help(stream); return; }
	if(section == SubProgramBreakpointWrapper<CONFIG>::CLASS_NAME) { SubProgramBreakpointWrapper<CONFIG>::Help(stream); return; }
	if(section == WatchpointWrapper<CONFIG>          ::CLASS_NAME) { WatchpointWrapper<CONFIG>          ::Help(stream); return; }
	if(section == DataObjectWrapper<CONFIG>          ::CLASS_NAME) { DataObjectWrapper<CONFIG>          ::Help(stream); return; }
	if(section == PointerWrapper<CONFIG>             ::CLASS_NAME) { PointerWrapper<CONFIG>             ::Help(stream); return; }
	if(section == HookWrapper<CONFIG>                ::CLASS_NAME) { HookWrapper<CONFIG>                ::Help(stream); return; }
	if(section == AddressHookWrapper<CONFIG>         ::CLASS_NAME) { AddressHookWrapper<CONFIG>         ::Help(stream); return; }
	if(section == SourceCodeHookWrapper<CONFIG>      ::CLASS_NAME) { SourceCodeHookWrapper<CONFIG>      ::Help(stream); return; }
	if(section == SubProgramHookWrapper<CONFIG>      ::CLASS_NAME) { SubProgramHookWrapper<CONFIG>      ::Help(stream); return; }
	if(section == StubWrapper<CONFIG>                ::CLASS_NAME) { StubWrapper<CONFIG>                ::Help(stream); return; }
	if(section == ProcessorWrapper<CONFIG>           ::CLASS_NAME) { ProcessorWrapper<CONFIG>           ::Help(stream); return; }
	if(section == RegisterWrapper<CONFIG>            ::CLASS_NAME) { RegisterWrapper<CONFIG>            ::Help(stream); return; }
	if(section == ExecutableBinaryFileWrapper<CONFIG>::CLASS_NAME) { ExecutableBinaryFileWrapper<CONFIG>::Help(stream); return; }
	if(section == StackFrameInfoWrapper<CONFIG>      ::CLASS_NAME) { StackFrameInfoWrapper<CONFIG>      ::Help(stream); return; }
	if(section == DebugSymbolWrapper<CONFIG>         ::CLASS_NAME) { DebugSymbolWrapper<CONFIG>         ::Help(stream); return; }
	if(section == StatementWrapper<CONFIG>           ::CLASS_NAME) { StatementWrapper<CONFIG>           ::Help(stream); return; }
	if(section == SubProgramWrapper<CONFIG>          ::CLASS_NAME) { SubProgramWrapper<CONFIG>          ::Help(stream); return; }
	if(section == TypeWrapper<CONFIG>                ::CLASS_NAME) { TypeWrapper<CONFIG>                ::Help(stream); return; }
	if(section == NamedTypeWrapper<CONFIG>           ::CLASS_NAME) { NamedTypeWrapper<CONFIG>           ::Help(stream); return; }
	if(section == BaseTypeWrapper<CONFIG>            ::CLASS_NAME) { BaseTypeWrapper<CONFIG>            ::Help(stream); return; }
	if(section == IntegerTypeWrapper<CONFIG>         ::CLASS_NAME) { IntegerTypeWrapper<CONFIG>         ::Help(stream); return; }
	if(section == CharTypeWrapper<CONFIG>            ::CLASS_NAME) { CharTypeWrapper<CONFIG>            ::Help(stream); return; }
	if(section == FloatingPointTypeWrapper<CONFIG>   ::CLASS_NAME) { FloatingPointTypeWrapper<CONFIG>   ::Help(stream); return; }
	if(section == BooleanTypeWrapper<CONFIG>         ::CLASS_NAME) { BooleanTypeWrapper<CONFIG>         ::Help(stream); return; }
	if(section == MemberWrapper<CONFIG>              ::CLASS_NAME) { MemberWrapper<CONFIG>              ::Help(stream); return; }
	if(section == CompositeTypeWrapper<CONFIG>       ::CLASS_NAME) { CompositeTypeWrapper<CONFIG>       ::Help(stream); return; }
	if(section == StructureTypeWrapper<CONFIG>       ::CLASS_NAME) { StructureTypeWrapper<CONFIG>       ::Help(stream); return; }
	if(section == UnionTypeWrapper<CONFIG>           ::CLASS_NAME) { UnionTypeWrapper<CONFIG>           ::Help(stream); return; }
	if(section == ClassTypeWrapper<CONFIG>           ::CLASS_NAME) { ClassTypeWrapper<CONFIG>           ::Help(stream); return; }
	if(section == InterfaceTypeWrapper<CONFIG>       ::CLASS_NAME) { InterfaceTypeWrapper<CONFIG>       ::Help(stream); return; }
	if(section == ArrayTypeWrapper<CONFIG>           ::CLASS_NAME) { ArrayTypeWrapper<CONFIG>           ::Help(stream); return; }
	if(section == PointerTypeWrapper<CONFIG>         ::CLASS_NAME) { PointerTypeWrapper<CONFIG>         ::Help(stream); return; }
	if(section == TypedefWrapper<CONFIG>             ::CLASS_NAME) { TypedefWrapper<CONFIG>             ::Help(stream); return; }
	if(section == FormalParameterWrapper<CONFIG>     ::CLASS_NAME) { FormalParameterWrapper<CONFIG>     ::Help(stream); return; }
	if(section == FunctionTypeWrapper<CONFIG>        ::CLASS_NAME) { FunctionTypeWrapper<CONFIG>        ::Help(stream); return; }
	if(section == ConstTypeWrapper<CONFIG>           ::CLASS_NAME) { ConstTypeWrapper<CONFIG>           ::Help(stream); return; }
	if(section == EnumeratorWrapper<CONFIG>          ::CLASS_NAME) { EnumeratorWrapper<CONFIG>          ::Help(stream); return; }
	if(section == EnumTypeWrapper<CONFIG>            ::CLASS_NAME) { EnumTypeWrapper<CONFIG>            ::Help(stream); return; }
	if(section == UnspecifiedTypeWrapper<CONFIG>     ::CLASS_NAME) { UnspecifiedTypeWrapper<CONFIG>     ::Help(stream); return; }
	if(section == VolatileTypeWrapper<CONFIG>        ::CLASS_NAME) { VolatileTypeWrapper<CONFIG>        ::Help(stream); return; }
	
	stream << "No help available in section " << section << std::endl << std::endl;
}

template <typename CONFIG>
bool NodeJS<CONFIG>::LocateFile(const std::string& filename, std::string& match_file_path, bool lazy_match)
{
	v8::HandleScope handle_scope(this->GetIsolate());
	v8::Local<v8::Object> global_object = this->GetContext()->Global();
	v8::Local<v8::String> prop_search_path = global_object->Get(
		this->GetContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "searchPath")
	).ToLocalChecked().template As<v8::String>();
	unisim::util::locate::LocateFileOptions options;
	if(!ToString(this->GetIsolate(), prop_search_path, options.search_path)) return false;
	options.shared_directory = unisim::kernel::Object::GetSimulator()->GetSharedDataDirectory();
	options.lazy_match = lazy_match;
	return unisim::util::locate::LocateFile(filename, match_file_path, options);
}

template <typename CONFIG>
std::string NodeJS<CONFIG>::LocateFile(const std::string& filename, bool lazy_match)
{
	std::string match_file_path;
	return LocateFile(filename, match_file_path, lazy_match) ? match_file_path : filename;
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
		
		if(cmd == "help")
		{
			Help(std::cout);
			continue;
		}
		
		std::smatch m;
		if(std::regex_match(cmd, m, std::regex("^help +(.*)$")) && (m.size() >= 2))
		{
			Help(std::cout, m.str(1));
			continue;
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
bool NodeJS<CONFIG>::SigInt()
{
	if(!this->IsRunning()) return false;
	
	Interrupt();
	
	return true; // handled
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
	cont_exec_resolvers.clear();
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
	
	// Create and register constructor function templates
	v8::Local<v8::FunctionTemplate> source_code_location_function_template = SourceCodeLocationWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<SourceCodeLocationWrapper<CONFIG> >(source_code_location_function_template);
	
	v8::Local<v8::FunctionTemplate> hook_function_template = HookWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<HookWrapper<CONFIG> >(hook_function_template);
	
	v8::Local<v8::FunctionTemplate> address_hook_function_template = AddressHookWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<AddressHookWrapper<CONFIG> >(address_hook_function_template);
	
	v8::Local<v8::FunctionTemplate> source_code_hook_function_template = SourceCodeHookWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<SourceCodeHookWrapper<CONFIG> >(source_code_hook_function_template);
	
	v8::Local<v8::FunctionTemplate> subprogram_hook_function_template = SubProgramHookWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<SubProgramHookWrapper<CONFIG> >(subprogram_hook_function_template);
	
	v8::Local<v8::FunctionTemplate> debug_event_function_template = DebugEventWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<DebugEventWrapper<CONFIG> >(debug_event_function_template);
	
	v8::Local<v8::FunctionTemplate> breakpoint_function_template = BreakpointWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<BreakpointWrapper<CONFIG> >(breakpoint_function_template);

	v8::Local<v8::FunctionTemplate> subprogram_breakpoint_function_template = SubProgramBreakpointWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<SubProgramBreakpointWrapper<CONFIG> >(subprogram_breakpoint_function_template);

	v8::Local<v8::FunctionTemplate> source_code_breakpoint_function_template = SourceCodeBreakpointWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<SourceCodeBreakpointWrapper<CONFIG> >(source_code_breakpoint_function_template);

	v8::Local<v8::FunctionTemplate> watchpoint_function_template = WatchpointWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<WatchpointWrapper<CONFIG> >(watchpoint_function_template);
	
	v8::Local<v8::FunctionTemplate> data_object_function_template = DataObjectWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<DataObjectWrapper<CONFIG> >(data_object_function_template);
	
	v8::Local<v8::FunctionTemplate> pointer_function_template = PointerWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<PointerWrapper<CONFIG> >(pointer_function_template);
	
	v8::Local<v8::FunctionTemplate> stub_function_template = StubWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<StubWrapper<CONFIG> >(stub_function_template);
	
	v8::Local<v8::FunctionTemplate> processor_function_template = ProcessorWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<ProcessorWrapper<CONFIG> >(processor_function_template);
	
	v8::Local<v8::FunctionTemplate> register_function_template = RegisterWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<RegisterWrapper<CONFIG> >(register_function_template);

	v8::Local<v8::FunctionTemplate> executable_binary_file_function_template = ExecutableBinaryFileWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<ExecutableBinaryFileWrapper<CONFIG> >(executable_binary_file_function_template);
	
	v8::Local<v8::FunctionTemplate> stack_frame_info_function_template = StackFrameInfoWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<StackFrameInfoWrapper<CONFIG> >(stack_frame_info_function_template);
	
	v8::Local<v8::FunctionTemplate> debug_symbol_function_template = DebugSymbolWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<DebugSymbolWrapper<CONFIG> >(debug_symbol_function_template);
	
	v8::Local<v8::FunctionTemplate> statement_function_template = StatementWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<StatementWrapper<CONFIG> >(statement_function_template);

	v8::Local<v8::FunctionTemplate> subprogram_function_template = SubProgramWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<SubProgramWrapper<CONFIG> >(subprogram_function_template);
	
	v8::Local<v8::FunctionTemplate> type_function_template = TypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<TypeWrapper<CONFIG> >(type_function_template);
	
	v8::Local<v8::FunctionTemplate> named_type_function_template = NamedTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<NamedTypeWrapper<CONFIG> >(named_type_function_template);
	
	v8::Local<v8::FunctionTemplate> base_type_function_template = BaseTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<BaseTypeWrapper<CONFIG> >(base_type_function_template);
	
	v8::Local<v8::FunctionTemplate> integer_type_function_template = IntegerTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<IntegerTypeWrapper<CONFIG> >(integer_type_function_template);
	
	v8::Local<v8::FunctionTemplate> char_type_function_template = CharTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<CharTypeWrapper<CONFIG> >(char_type_function_template);
	
	v8::Local<v8::FunctionTemplate> floating_point_type_function_template = FloatingPointTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<FloatingPointTypeWrapper<CONFIG> >(floating_point_type_function_template);
	
	v8::Local<v8::FunctionTemplate> boolean_type_function_template = BooleanTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<BooleanTypeWrapper<CONFIG> >(boolean_type_function_template);
	
	v8::Local<v8::FunctionTemplate> member_function_template = MemberWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<MemberWrapper<CONFIG> >(member_function_template);
	
	v8::Local<v8::FunctionTemplate> composite_type_function_template = CompositeTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<CompositeTypeWrapper<CONFIG> >(composite_type_function_template);
	
	v8::Local<v8::FunctionTemplate> structure_type_function_template = StructureTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<StructureTypeWrapper<CONFIG> >(structure_type_function_template);
	
	v8::Local<v8::FunctionTemplate> union_type_function_template = UnionTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<UnionTypeWrapper<CONFIG> >(union_type_function_template);
	
	v8::Local<v8::FunctionTemplate> class_type_function_template = ClassTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<ClassTypeWrapper<CONFIG> >(class_type_function_template);
	
	v8::Local<v8::FunctionTemplate> interface_type_function_template = InterfaceTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<InterfaceTypeWrapper<CONFIG> >(interface_type_function_template);
	
	v8::Local<v8::FunctionTemplate> array_type_function_template = ArrayTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<ArrayTypeWrapper<CONFIG> >(array_type_function_template);
	
	v8::Local<v8::FunctionTemplate> pointer_type_function_template = PointerTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<PointerTypeWrapper<CONFIG> >(pointer_type_function_template);
	
	v8::Local<v8::FunctionTemplate> typedef_function_template = TypedefWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<TypedefWrapper<CONFIG> >(typedef_function_template);
	
	v8::Local<v8::FunctionTemplate> formal_parameter_function_template = FormalParameterWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<FormalParameterWrapper<CONFIG> >(formal_parameter_function_template);
	
	v8::Local<v8::FunctionTemplate> function_type_function_template = FunctionTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<FunctionTypeWrapper<CONFIG> >(function_type_function_template);
	
	v8::Local<v8::FunctionTemplate> const_type_function_template = ConstTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<ConstTypeWrapper<CONFIG> >(const_type_function_template);
	
	v8::Local<v8::FunctionTemplate> enumerator_function_template = EnumeratorWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<EnumeratorWrapper<CONFIG> >(enumerator_function_template);
	
	v8::Local<v8::FunctionTemplate> enum_type_function_template = EnumTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<EnumTypeWrapper<CONFIG> >(enum_type_function_template);
	
	v8::Local<v8::FunctionTemplate> unspecified_type_function_template = UnspecifiedTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<UnspecifiedTypeWrapper<CONFIG> >(unspecified_type_function_template);
	
	v8::Local<v8::FunctionTemplate> volatile_type_function_template = VolatileTypeWrapper<CONFIG>::CreateFunctionTemplate(*this);
	this->template RegisterCtorFunctionTemplate<VolatileTypeWrapper<CONFIG> >(volatile_type_function_template);
	
	// Add constructor functions for builtin objects
	global_object_template->Set(isolate, SourceCodeLocationWrapper<CONFIG>  ::CLASS_NAME, source_code_location_function_template  , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, DebugEventWrapper<CONFIG>          ::CLASS_NAME, debug_event_function_template           , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, BreakpointWrapper<CONFIG>          ::CLASS_NAME, breakpoint_function_template            , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, SourceCodeBreakpointWrapper<CONFIG>::CLASS_NAME, source_code_breakpoint_function_template, v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, SubProgramBreakpointWrapper<CONFIG>::CLASS_NAME, subprogram_breakpoint_function_template , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, WatchpointWrapper<CONFIG>          ::CLASS_NAME, watchpoint_function_template            , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, DataObjectWrapper<CONFIG>          ::CLASS_NAME, data_object_function_template           , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, PointerWrapper<CONFIG>             ::CLASS_NAME, pointer_function_template               , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, HookWrapper<CONFIG>                ::CLASS_NAME, hook_function_template                  , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, AddressHookWrapper<CONFIG>         ::CLASS_NAME, address_hook_function_template          , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, SourceCodeHookWrapper<CONFIG>      ::CLASS_NAME, source_code_hook_function_template      , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, SubProgramHookWrapper<CONFIG>      ::CLASS_NAME, subprogram_hook_function_template       , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, StubWrapper<CONFIG>                ::CLASS_NAME, stub_function_template                  , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, ProcessorWrapper<CONFIG>           ::CLASS_NAME, processor_function_template             , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, RegisterWrapper<CONFIG>            ::CLASS_NAME, register_function_template              , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, ExecutableBinaryFileWrapper<CONFIG>::CLASS_NAME, executable_binary_file_function_template, v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, StackFrameInfoWrapper<CONFIG>      ::CLASS_NAME, stack_frame_info_function_template      , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, DebugSymbolWrapper<CONFIG>         ::CLASS_NAME, debug_symbol_function_template          , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, StatementWrapper<CONFIG>           ::CLASS_NAME, statement_function_template             , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, SubProgramWrapper<CONFIG>          ::CLASS_NAME, subprogram_function_template            , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, TypeWrapper<CONFIG>                ::CLASS_NAME, type_function_template                  , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, NamedTypeWrapper<CONFIG>           ::CLASS_NAME, named_type_function_template            , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, BaseTypeWrapper<CONFIG>            ::CLASS_NAME, base_type_function_template             , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, IntegerTypeWrapper<CONFIG>         ::CLASS_NAME, integer_type_function_template          , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, CharTypeWrapper<CONFIG>            ::CLASS_NAME, char_type_function_template             , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, FloatingPointTypeWrapper<CONFIG>   ::CLASS_NAME, floating_point_type_function_template   , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, BooleanTypeWrapper<CONFIG>         ::CLASS_NAME, boolean_type_function_template          , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, MemberWrapper<CONFIG>              ::CLASS_NAME, member_function_template                , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, CompositeTypeWrapper<CONFIG>       ::CLASS_NAME, composite_type_function_template        , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, StructureTypeWrapper<CONFIG>       ::CLASS_NAME, structure_type_function_template        , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, UnionTypeWrapper<CONFIG>           ::CLASS_NAME, union_type_function_template            , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, ClassTypeWrapper<CONFIG>           ::CLASS_NAME, class_type_function_template            , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, InterfaceTypeWrapper<CONFIG>       ::CLASS_NAME, interface_type_function_template        , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, ArrayTypeWrapper<CONFIG>           ::CLASS_NAME, array_type_function_template            , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, PointerTypeWrapper<CONFIG>         ::CLASS_NAME, pointer_type_function_template          , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, TypedefWrapper<CONFIG>             ::CLASS_NAME, typedef_function_template               , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, FormalParameterWrapper<CONFIG>     ::CLASS_NAME, formal_parameter_function_template      , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, FunctionTypeWrapper<CONFIG>        ::CLASS_NAME, function_type_function_template         , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, ConstTypeWrapper<CONFIG>           ::CLASS_NAME, const_type_function_template            , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, EnumeratorWrapper<CONFIG>          ::CLASS_NAME, enumerator_function_template            , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, EnumTypeWrapper<CONFIG>            ::CLASS_NAME, enum_type_function_template             , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, UnspecifiedTypeWrapper<CONFIG>     ::CLASS_NAME, unspecified_type_function_template      , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, VolatileTypeWrapper<CONFIG>        ::CLASS_NAME, volatile_type_function_template         , v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));

	// Add some builtin functions
	global_object_template->Set(isolate, "continueExecution"       , CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::ContinueExecution       >(), v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, "quit"                    , CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::Quit                    >(), v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, "loadDebugInfo"           , CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::LoadDebugInfo           >(), v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, "getExecutableBinaryFiles", CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::GetExecutableBinaryFiles>(), v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, "getSymbols"              , CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::GetSymbols              >(), v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, "findSymbol"              , CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::FindSymbol              >(), v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, "getStatements"           , CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::GetStatements           >(), v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, "findStatement"           , CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::FindStatement           >(), v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, "findStatements"          , CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::FindStatements          >(), v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	global_object_template->Set(isolate, "findSubProgram"          , CreateFunctionTemplate<NodeJS<CONFIG>, &NodeJS<CONFIG>::FindSubProgram          >(), v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
	
	return global_object_template;
}

template <typename CONFIG>
void NodeJS<CONFIG>::BeforeExecution()
{
	v8::HandleScope handle_scope(GetIsolate());
	v8::Local<v8::Object> global_object = GetContext()->Global();

	// Create searchPath
	global_object->DefineOwnProperty(
		GetContext(),
		v8::String::NewFromUtf8Literal(GetIsolate(), "searchPath"),
		v8::String::NewFromUtf8Literal(GetIsolate(), ""),
		v8::PropertyAttribute(v8::DontDelete)
	).ToChecked();
	
	// Create processor array (read-only, don't delete)
	std::vector<v8::Local<v8::Value> > processor_objects;
	for(unsigned int prc_num = 0, prc_count = debug_processors_import->DebugGetProcessorCount(); prc_num < prc_count; ++prc_num)
	{
		ProcessorWrapper<CONFIG> *processor_wrapper = ProcessorWrapper<CONFIG>::Wrap(*this, debug_processors_import->DebugGetProcessor(prc_num));
		processor_objects.push_back(processor_wrapper->MakeObject().template As<v8::Value>());
	}
	
	global_object->DefineOwnProperty(
		GetContext(),
		v8::String::NewFromUtf8Literal(GetIsolate(), "processors"),
		v8::Array::New(GetIsolate(), &processor_objects[0], processor_objects.size()),
		v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
	).ToChecked();

	// Create functions like Foo.isFoo()
	CreateIsInstanceOf<SourceCodeLocationWrapper<CONFIG> >();
	CreateIsInstanceOf<DebugEventWrapper<CONFIG> >();
	CreateIsInstanceOf<BreakpointWrapper<CONFIG> >();
	CreateIsInstanceOf<SourceCodeBreakpointWrapper<CONFIG> >();
	CreateIsInstanceOf<SubProgramBreakpointWrapper<CONFIG> >();
	CreateIsInstanceOf<WatchpointWrapper<CONFIG> >();
	CreateIsInstanceOf<DataObjectWrapper<CONFIG> >();
	CreateIsInstanceOf<PointerWrapper<CONFIG> >();
	CreateIsInstanceOf<HookWrapper<CONFIG> >();
	CreateIsInstanceOf<AddressHookWrapper<CONFIG> >();
	CreateIsInstanceOf<SourceCodeHookWrapper<CONFIG> >();
	CreateIsInstanceOf<SubProgramHookWrapper<CONFIG> >();
	CreateIsInstanceOf<StubWrapper<CONFIG> >();
	CreateIsInstanceOf<ProcessorWrapper<CONFIG> >();
	CreateIsInstanceOf<RegisterWrapper<CONFIG> >();
	CreateIsInstanceOf<ExecutableBinaryFileWrapper<CONFIG> >();
	CreateIsInstanceOf<StackFrameInfoWrapper<CONFIG> >();
	CreateIsInstanceOf<DebugSymbolWrapper<CONFIG> >();
	CreateIsInstanceOf<StatementWrapper<CONFIG> >();
	CreateIsInstanceOf<SubProgramWrapper<CONFIG> >();
	CreateIsInstanceOf<TypeWrapper<CONFIG> >();
	CreateIsInstanceOf<NamedTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<BaseTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<IntegerTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<CharTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<FloatingPointTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<BooleanTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<MemberWrapper<CONFIG> >();
	CreateIsInstanceOf<CompositeTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<StructureTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<UnionTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<ClassTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<InterfaceTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<ArrayTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<PointerTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<TypedefWrapper<CONFIG> >();
	CreateIsInstanceOf<FormalParameterWrapper<CONFIG> >();
	CreateIsInstanceOf<FunctionTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<ConstTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<EnumeratorWrapper<CONFIG> >();
	CreateIsInstanceOf<EnumTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<UnspecifiedTypeWrapper<CONFIG> >();
	CreateIsInstanceOf<VolatileTypeWrapper<CONFIG> >();
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

// loadDebugInfo(file: string)
template <typename CONFIG>
void NodeJS<CONFIG>::LoadDebugInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0]; // file
	std::string file;
	if(!arg0->IsString() || !ToString(args.GetIsolate(), arg0, file))
	{
		this->Throw(this->TypeError("loadDebugInfo(filename: string) expects a string for 'file'"));
		return;
	}
	
	std::string match_file_path;
	if(LocateFile(file, match_file_path))
	{
		if(!debug_info_loading_import->LoadDebugInfo(match_file_path.c_str()))
		{
			Throw(Error("No symbols loaded"));
		}
	}
	else
	{
		Throw(Error(std::string("Can't locate \"") + file + "\""));
	}
}

// getExecutableBinaryFiles() => Array of ExecutableBinaryFile
template <typename CONFIG>
void NodeJS<CONFIG>::GetExecutableBinaryFiles(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	// Scan all executable binary files for which debug infos have been loaded, wrap each C++ objects
	struct ExecutableBinaryFileScanner : unisim::service::interfaces::ExecutableBinaryFileScanner
	{
		NodeJS<CONFIG>& nodejs;
		std::vector<v8::Local<v8::Value> > executable_binary_files_objects;
		
		ExecutableBinaryFileScanner(NodeJS<CONFIG>& _nodejs) : nodejs(_nodejs), executable_binary_files_objects() {}
		
		virtual void Append(unisim::service::interfaces::ExecutableBinaryFile *executable_binary_file)
		{
			ExecutableBinaryFileWrapper<CONFIG> *exec_bin_file_wrapper = ExecutableBinaryFileWrapper<CONFIG>::Wrap(nodejs, executable_binary_file);
			executable_binary_files_objects.push_back(exec_bin_file_wrapper->MakeObject().template As<v8::Value>());
		}
	};
	
	ExecutableBinaryFileScanner executable_binary_file_scanner(*this);
	debug_info_loading_import->ScanExecutableBinaryFiles(executable_binary_file_scanner);
	
	// Create an array
	v8::Local<v8::Array> executable_binary_files_array = v8::Array::New(
		GetIsolate(),
		&executable_binary_file_scanner.executable_binary_files_objects[0],
		executable_binary_file_scanner.executable_binary_files_objects.size()
	);
	
	args.GetReturnValue().Set(executable_binary_files_array);
}

// getSymbols() => Array of DebugSymbol
template <typename CONFIG>
void NodeJS<CONFIG>::GetSymbols(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	struct SymbolTableScanner : unisim::service::interfaces::SymbolTableScanner<ADDRESS>
	{
		NodeJS<CONFIG>& nodejs;
		std::vector<v8::Local<v8::Value> > symbol_objects;
		
		SymbolTableScanner(NodeJS<CONFIG>& _nodejs) : nodejs(_nodejs), symbol_objects() {}
		
		virtual void Append(const unisim::util::debug::Symbol<ADDRESS> *symbol)
		{
			DebugSymbolWrapper<CONFIG> *symbol_wrapper = DebugSymbolWrapper<CONFIG>::Wrap(nodejs, symbol);
			symbol_objects.push_back(symbol_wrapper->MakeObject().template As<v8::Value>());
		}
	};
	
	SymbolTableScanner symbol_table_scanner(*this);
	symbol_table_lookup_import->ScanSymbols(symbol_table_scanner);
	
	v8::Local<v8::Array> symbol_array = v8::Array::New(GetIsolate(), &symbol_table_scanner.symbol_objects[0], symbol_table_scanner.symbol_objects.size());
	
	args.GetReturnValue().Set(symbol_array);
}

// findSymbol((name: string | address: Number), [type: string]) => DebugSymbol
template <typename CONFIG>
void NodeJS<CONFIG>::FindSymbol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	struct Synopsis { std::string str() const { return std::string("findSymbol((name: string | address: Number), [type: string])"); } };
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0]; // address
	bool by_addr = false;
	std::string name;
	ADDRESS address = 0;
	if(arg0->IsNumber() || arg0->IsBigInt())
	{
		if(!ToInt(args.GetIsolate(), arg0, address))
		{
			this->Throw(this->TypeError(Synopsis().str() + " expects a number for 'address'"));
			return;
		}
		by_addr = true;
	}
	else if(arg0->IsString())
	{
		if(!ToString(args.GetIsolate(), arg0, name))
		{
			Throw(TypeError(Synopsis().str() + " expects a string for 'name'"));
			return;
		}
	}
	else
	{
		Throw(TypeError(Synopsis().str() + " expects either a number for 'address' or a string for 'name'"));
		return;
	}
	const unisim::util::debug::Symbol<ADDRESS> *symbol = 0;
	if(args.Length() >= 2)
	{
		v8::Local<v8::Value> arg1 = args[1]; // type
		std::string type_name;
		if(!arg1->IsString() || !ToString(args.GetIsolate(), arg1, type_name))
		{
			this->Throw(this->TypeError(Synopsis().str() + " expects a string for 'type'"));
			return;
		}
		typename unisim::util::debug::SymbolBase::Type type;
		struct ValidTypes
		{
			std::string str() const
			{
				std::string s;
				for(typename unisim::util::debug::SymbolBase::Type type : unisim::util::debug::SymbolBase::Types)
				{
					if(!s.empty()) s += ", ";
					s += "'";
					s += ToString(type);
					s += "'";
				}
				return s;
			}
		};
		std::istringstream type_sstr(type_name);
		if(!(type_sstr >> type))
		{
			this->Throw(this->TypeError(Synopsis().str() + " expects a valid string (one of " + ValidTypes().str() + ") for 'type'"));
			return;
		}
		
		symbol = by_addr ? symbol_table_lookup_import->FindSymbolByAddr(address, type)
		                 : symbol_table_lookup_import->FindSymbolByName(name.c_str(), type);
	}
	else
	{
		symbol = by_addr ? symbol_table_lookup_import->FindSymbolByAddr(address)
		                 : symbol_table_lookup_import->FindSymbolByName(name.c_str());
	}
	
	if(symbol)
	{
		DebugSymbolWrapper<CONFIG> *symbol_wrapper = DebugSymbolWrapper<CONFIG>::Wrap(*this, symbol);
		args.GetReturnValue().Set(symbol_wrapper->MakeObject());
	}
}

// getStatements([file: string]) => Array of Statement
template <typename CONFIG>
void NodeJS<CONFIG>::GetStatements(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	std::string file;
	const char *filename = 0;
	if(args.Length() >= 1)
	{
		v8::Local<v8::Value> arg0 = args[0]; // file
		std::string file;
		if(arg0->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg0, file))
		{
			Throw(TypeError("getStatements([file: string]) expects a string for 'file'"));
			return;
		}
		std::string real_file;
		filename = (LocateFile(file, real_file) ? real_file : file).c_str();
	}
	struct StatementScanner : unisim::service::interfaces::StatementScanner<ADDRESS>
	{
		NodeJS<CONFIG>& nodejs;
		std::vector<v8::Local<v8::Value> > statement_objects;
		
		StatementScanner(NodeJS<CONFIG>& _nodejs) : nodejs(_nodejs), statement_objects() {}
		
		virtual void Append(const unisim::util::debug::Statement<ADDRESS> *stmt)
		{
			StatementWrapper<CONFIG> *statement_wrapper = StatementWrapper<CONFIG>::Wrap(nodejs, stmt);
			statement_objects.push_back(statement_wrapper->MakeObject().template As<v8::Value>());
		}
	};
	
	StatementScanner stmt_scanner(*this);
	stmt_lookup_import->ScanStatements(stmt_scanner, filename);

	v8::Local<v8::Array> statement_array = v8::Array::New(GetIsolate(), &stmt_scanner.statement_objects[0], stmt_scanner.statement_objects.size());
	
	args.GetReturnValue().Set(statement_array);
}

// findStatement((address : Number | loc : (Object|string)), [options: Object]) => Statement
template <typename CONFIG>
void NodeJS<CONFIG>::FindStatement(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	struct Synopsis { std::string str() const { return std::string("findStatement((address : Number | loc : (Object|string)), [options: Object])"); } };
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0]; // address or loc
	bool by_addr = false;
	ADDRESS address = 0;
	unisim::util::debug::SourceCodeLocation source_code_location;
	if(arg0->IsObject() || arg0->IsString())
	{
		if(!ArgToSourceCodeLocation(arg0, Synopsis().str(), "loc", source_code_location)) return;
	}
	else if(arg0->IsNumber() || arg0->IsBigInt())
	{
		if(!ToInt(args.GetIsolate(), arg0, address))
		{
			this->Throw(this->TypeError(Synopsis().str() + " expects a number for 'address'"));
			return;
		}
		by_addr = true;
	}
	else
	{
		this->Throw(this->TypeError(Synopsis().str() + " expects either a number for 'address', an object or a string for 'loc'"));
		return;
	}
	std::string file;
	typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope = unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_EXACT_STMT;
	if(args.Length() >= 2)
	{
		v8::Local<v8::Value> arg1 = args[1]; // options
		if(!arg1->IsObject())
		{
			this->Throw(this->TypeError(Synopsis().str() + " expects an object for 'options'"));
			return;
		}
		v8::Local<v8::Object> obj_options = arg1.As<v8::Object>();
		if(obj_options->HasOwnProperty(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(args.GetIsolate(), "file")).ToChecked())
		{
			v8::Local<v8::Value> prop_options_file;
			if(!obj_options->Get(
			   args.GetIsolate()->GetCurrentContext(),
			   v8::String::NewFromUtf8Literal(args.GetIsolate(), "file")
			  ).ToLocal(&prop_options_file) || !prop_options_file->IsString() || !ToString(args.GetIsolate(), prop_options_file, file))
			{
				this->Throw(this->TypeError(Synopsis().str() + " expects a string for property 'options.file'"));
				return;
			}
		}
		if(by_addr)
		{
			if(obj_options->HasOwnProperty(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(args.GetIsolate(), "scope")).ToChecked())
			{
				struct ValidScopes
				{
					std::string str() const
					{
						std::string s;
						for(typename unisim::service::interfaces::StatementLookupBase::Scope scope : unisim::service::interfaces::StatementLookupBase::Scopes)
						{
							if(!s.empty()) s += ", ";
							s += "'";
							s += ToString(scope);
							s += "'";
						}
						return s;
					}
				};
				v8::Local<v8::Value> prop_options_scope;
				if(!obj_options->Get(
					args.GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8Literal(args.GetIsolate(), "scope")
					).ToLocal(&prop_options_scope) || !prop_options_scope->IsString() || !ToScope<ADDRESS>(args.GetIsolate(), prop_options_scope, scope))
				{
					this->Throw(this->TypeError(Synopsis().str() + " expects a valid string (one of " + ValidScopes().str() + ") for property 'options.scope'"));
					return;
				}
			}
		}
	}

	const unisim::util::debug::Statement<ADDRESS> *stmt = by_addr ? stmt_lookup_import->FindStatement(address, LocateFile(file), scope)
	                                                              : stmt_lookup_import->FindStatement(source_code_location, LocateFile(file));
	
	if(stmt)
	{
		StatementWrapper<CONFIG> *statement_wrapper = StatementWrapper<CONFIG>::Wrap(*this, stmt);
		args.GetReturnValue().Set(statement_wrapper->MakeObject());
	}
}

// findStatements((addr : Number | loc : (Object|string)), [options: Object]) => Array of Statement
template <typename CONFIG>
void NodeJS<CONFIG>::FindStatements(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	struct Synopsis { std::string str() const { return std::string("findStatements((addr : Number | loc : (Object|string)), [options: Object])"); } };
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0]; // addr or loc
	bool by_addr = false;
	ADDRESS addr = 0;
	unisim::util::debug::SourceCodeLocation source_code_location;
	if(arg0->IsObject() || arg0->IsString())
	{
		if(!ArgToSourceCodeLocation(arg0, Synopsis().str(), "loc", source_code_location)) return;
	}
	else if(arg0->IsNumber() || arg0->IsBigInt())
	{
		if(!ToInt(args.GetIsolate(), arg0, addr))
		{
			this->Throw(this->TypeError(Synopsis().str() + " expects a number for 'addr'"));
			return;
		}
		by_addr = true;
	}
	else
	{
		this->Throw(this->TypeError(Synopsis().str() + " expects either a number for 'addr', an object or a string for 'loc'"));
		return;
	}
	std::string file;
	if(args.Length() >= 2)
	{
		v8::Local<v8::Value> arg1 = args[1]; // options
		if(!arg1->IsObject())
		{
			this->Throw(this->TypeError(Synopsis().str() + " expects an object for 'options'"));
			return;
		}
		v8::Local<v8::Object> obj_options = arg1.As<v8::Object>();
		if(obj_options->HasOwnProperty(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(args.GetIsolate(), "file")).ToChecked())
		{
			v8::Local<v8::Value> prop_options_file;
			if(!obj_options->Get(
			   args.GetIsolate()->GetCurrentContext(),
			   v8::String::NewFromUtf8Literal(args.GetIsolate(), "file")
			  ).ToLocal(&prop_options_file) || !prop_options_file->IsString() || !ToString(args.GetIsolate(), prop_options_file, file))
			{
				this->Throw(this->TypeError(Synopsis().str() + " expects a string for property 'options.file'"));
				return;
			}
		}
	}
	
	struct StatementScanner : unisim::service::interfaces::StatementScanner<ADDRESS>
	{
		NodeJS<CONFIG>& nodejs;
		std::vector<v8::Local<v8::Value> > statement_objects;
		
		StatementScanner(NodeJS<CONFIG>& _nodejs) : nodejs(_nodejs), statement_objects() {}
		
		virtual void Append(const unisim::util::debug::Statement<ADDRESS> *stmt)
		{
			StatementWrapper<CONFIG> *statement_wrapper = StatementWrapper<CONFIG>::Wrap(nodejs, stmt);
			statement_objects.push_back(statement_wrapper->MakeObject().template As<v8::Value>());
		}
	};
	
	StatementScanner stmt_scanner(*this);
	
	if(by_addr)
	{
		stmt_lookup_import->FindStatements(stmt_scanner, addr, LocateFile(file));
	}
	else
	{
		stmt_lookup_import->FindStatements(stmt_scanner, source_code_location, LocateFile(file));
	}
	
	v8::Local<v8::Array> statement_array = v8::Array::New(GetIsolate(), &stmt_scanner.statement_objects[0], stmt_scanner.statement_objects.size());
	
	args.GetReturnValue().Set(statement_array);
}

// findSubProgram((name: string | address: Number), [options: Object]) => SubProgram
template <typename CONFIG>
void NodeJS<CONFIG>::FindSubProgram(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	struct Synopsis { std::string str() const { return std::string("findSubProgram((name: string | address: Number), [options: Object])"); } };
	v8::HandleScope handle_scope(args.GetIsolate());
	v8::Local<v8::Value> arg0 = args[0]; // name or address
	bool by_addr = false;
	std::string name;
	ADDRESS address = 0;
	std::string file;
	std::string compilation_unit_name;
	if(arg0->IsNumber() || arg0->IsBigInt())
	{
		if(!ToInt(args.GetIsolate(), arg0, address))
		{
			this->Throw(this->TypeError(Synopsis().str() + " expects a number for 'address'"));
			return;
		}
		by_addr = true;
	}
	else if(arg0->IsString())
	{
		if(!ToString(args.GetIsolate(), arg0, name))
		{
			Throw(TypeError(Synopsis().str() + " expects a string for 'name'"));
			return;
		}
	}
	else
	{
		Throw(TypeError(Synopsis().str() + " expects either a number for 'address' or a string for 'name'"));
		return;
	}
	if(args.Length() >= 2)
	{
		v8::Local<v8::Value> arg1 = args[1]; // options
		if(!arg1->IsObject())
		{
			this->Throw(this->TypeError(Synopsis().str() + " expects an object for 'options'"));
			return;
		}
		v8::Local<v8::Object> obj_options = arg1.As<v8::Object>();
		if(obj_options->HasOwnProperty(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(args.GetIsolate(), "file")).ToChecked())
		{
			v8::Local<v8::Value> prop_options_file;
			if(!obj_options->Get(
			   args.GetIsolate()->GetCurrentContext(),
			   v8::String::NewFromUtf8Literal(args.GetIsolate(), "file")
			  ).ToLocal(&prop_options_file) || !prop_options_file->IsString() || !ToString(args.GetIsolate(), prop_options_file, file))
			{
				this->Throw(this->TypeError(Synopsis().str() + " expects a string for property 'options.file'"));
				return;
			}
		}
		if(!by_addr)
		{
			if(obj_options->HasOwnProperty(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(args.GetIsolate(), "compilationUnit")).ToChecked())
			{
				v8::Local<v8::Value> prop_options_compilation_unit;
				if(!obj_options->Get(
					args.GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8Literal(args.GetIsolate(), "compilationUnit")
					).ToLocal(&prop_options_compilation_unit) || !prop_options_compilation_unit->IsString() || !ToString(args.GetIsolate(), prop_options_compilation_unit, compilation_unit_name))
				{
					this->Throw(this->TypeError(Synopsis().str() + " expects a string for property 'options.compilationUnit'"));
					return;
				}
			}
		}
	}
	
	const unisim::util::debug::SubProgram<ADDRESS> *subprogram = by_addr ? subprogram_lookup_import->FindSubProgram(address, LocateFile(file))
	                                                                     : subprogram_lookup_import->FindSubProgram(name, LocateFile(file), compilation_unit_name);
	
	if(subprogram)
	{
		SubProgramWrapper<CONFIG> *subprogram_wrapper = SubProgramWrapper<CONFIG>::Wrap(*this, subprogram);
		args.GetReturnValue().Set(subprogram_wrapper->MakeObject());
	}
}

// Convert a function argument to a SourceCodeLocation (duck typing)
template <typename CONFIG>
bool NodeJS<CONFIG>::ArgToSourceCodeLocation(v8::Local<v8::Value> arg, const std::string& err_msg_context, const char *arg_name, unisim::util::debug::SourceCodeLocation& source_code_location)
{
	if(arg->IsObject())
	{
		v8::Local<v8::Object> obj_loc = arg.As<v8::Object>();
		std::string source_code_filename;
		v8::Local<v8::Value> prop_source_code_file;
		if(!obj_loc->Get(
				this->GetIsolate()->GetCurrentContext(),
				v8::String::NewFromUtf8Literal(this->GetIsolate(), "sourceCodeFile")
			).ToLocal(&prop_source_code_file) || !prop_source_code_file->IsString() || !ToString(this->GetIsolate(), prop_source_code_file, source_code_filename))
		{
			this->Throw(this->TypeError(err_msg_context + " expects a string for property '" + arg_name + ".sourceCodeFile'"));
			return false;
		}
		v8::Local<v8::Value> prop_lineno;
		int64_t lineno = 0;
		if(!obj_loc->Get(
				this->GetIsolate()->GetCurrentContext(),
				v8::String::NewFromUtf8Literal(this->GetIsolate(), "lineno")
			).ToLocal(&prop_lineno) || (!prop_lineno->IsNumber() && !prop_lineno->IsBigInt()) || !ToInt(this->GetIsolate(), prop_lineno, lineno) || (lineno < 1) || (lineno > std::numeric_limits<unsigned>::max()))
		{
			this->Throw(this->TypeError(err_msg_context + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property '" + arg_name + ".lineno'"));
			return false;
		}
		int64_t colno = 0;
		if(obj_loc->HasOwnProperty(this->GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8Literal(this->GetIsolate(), "colno")).ToChecked())
		{
			v8::Local<v8::Value> prop_colno;
			if(!obj_loc->Get(
					this->GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8Literal(this->GetIsolate(), "colno")
				).ToLocal(&prop_colno) || (!prop_colno->IsNumber() && !prop_colno->IsBigInt()) || !ToInt(this->GetIsolate(), prop_colno, colno) || (colno < 1) || (colno > std::numeric_limits<unsigned>::max()))
			{
					this->Throw(this->TypeError(err_msg_context + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property '" + arg_name + ".colno'"));
					return false;
			}
		}
		
		source_code_location = unisim::util::debug::SourceCodeLocation(source_code_filename, lineno, colno);
	}
	else
	{
		std::string loc;
		if(arg->IsString() && ToString(this->GetIsolate(), arg, loc))
		{
			if(!source_code_location.Parse(loc))
			{
				this->Throw(RangeError(std::string("'" + loc + "' is not a source code location")));
				return false;
			}
		}
		else
		{
			this->Throw(this->TypeError(err_msg_context + " expects an object or a string for '" + arg_name + "'"));
			return false;
		}
	}
	
	return true;
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
ObjectWrapper<CONFIG>::ObjectWrapper(NodeJS<CONFIG>& _nodejs, const void *ptr, std::size_t size)
	: Super(_nodejs, ptr, size ? size : sizeof(*this))
	, nodejs(_nodejs)
{
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // HAVE_NODEJS

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_NODEJS_TCC__
