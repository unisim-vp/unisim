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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_NODEJS_HH__
#define __UNISIM_SERVICE_DEBUG_NODEJS_NODEJS_HH__

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if HAVE_NODEJS

#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/subprogram_lookup.hh>
#include <unisim/service/interfaces/debug_processors.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/debug/nodejs/fwd.hh>
#include <unisim/service/debug/nodejs/source_code_location.hh>
#include <unisim/service/debug/nodejs/debug_event.hh>
#include <unisim/service/debug/nodejs/breakpoint.hh>
#include <unisim/service/debug/nodejs/watchpoint.hh>
#include <unisim/service/debug/nodejs/data_object.hh>
#include <unisim/service/debug/nodejs/pointer.hh>
#include <unisim/service/debug/nodejs/hook.hh>
#include <unisim/service/debug/nodejs/stub.hh>
#include <unisim/service/debug/nodejs/event_bridge.hh>
#include <unisim/service/debug/nodejs/processor.hh>
#include <unisim/service/debug/nodejs/register.hh>
#include <unisim/service/debug/nodejs/executable_binary_file.hh>
#include <unisim/service/debug/nodejs/stack_frame_info.hh>
#include <unisim/service/debug/nodejs/debug_symbol.hh>
#include <unisim/service/debug/nodejs/statement.hh>
#include <unisim/service/debug/nodejs/subprogram.hh>
#include <unisim/util/debug/type.hh>

#include <string>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

///////////////////////////////////////////////////////////////////////////////
//                               Declarations                                //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////// Helper functions ////////////////////////////////

bool ToMemoryAccessType(v8::Isolate *isolate, v8::Local<v8::Value> value, unisim::util::debug::MemoryAccessType& out);
bool ToMemoryType(v8::Isolate *isolate, v8::Local<v8::Value> value, unisim::util::debug::MemoryType& out);

template <typename ADDRESS>
bool ToScope(v8::Isolate *isolate, v8::Local<v8::Value> value, typename unisim::service::interfaces::StatementLookupBase::Scope& out);

////////////////////////////////// NodeJS<> ///////////////////////////////////

template <typename CONFIG>
struct NodeJS
	: unisim::util::nodejs::NodeJS
	, unisim::kernel::Service<unisim::service::interfaces::DebugYielding>
	, unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>
	, unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::StatementLookup<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>
	, unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::DebugProcessors<typename CONFIG::ADDRESS, typename CONFIG::TIME_TYPE> >
{
	typedef unisim::util::nodejs::NodeJS Super;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	
	// exports
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding>                        debug_yielding_export;
	
	// imports
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYieldingRequest>                 debug_yielding_request_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >          debug_event_trigger_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >          symbol_table_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::StatementLookup<ADDRESS> >            stmt_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugInfoLoading>                     debug_info_loading_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SubProgramLookup<ADDRESS> >           subprogram_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugProcessors<ADDRESS, TIME_TYPE> > debug_processors_import;
	
	NodeJS(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~NodeJS();

	// unisim::service::interfaces::DebugYielding
	virtual void DebugYield();
	
	// unisim::kernel::Object
	virtual bool BeginSetup();
	virtual bool EndSetup();
	virtual bool SigInt();
	virtual void Kill();
	bool Killed() const;
	
	virtual void AtExit();
protected:

	virtual v8::Local<v8::ObjectTemplate> CreateGlobalObjectTemplate();
	virtual void BeforeExecution();

	v8::Isolate *GetIsolate() const { return Super::GetIsolate(); }
	v8::Local<v8::Context> GetContext() { return Super::GetContext(); }

private:
	friend struct SourceCodeLocationWrapper<CONFIG>;
	friend struct DebugEventWrapper<CONFIG>;
	friend struct BreakpointWrapper<CONFIG>;
	friend struct SourceCodeBreakpointWrapper<CONFIG>;
	friend struct SubProgramBreakpointWrapper<CONFIG>;
	friend struct WatchpointWrapper<CONFIG>;
	friend struct DataObjectWrapper<CONFIG>;
	friend struct PointerWrapper<CONFIG>;
	friend struct HookWrapper<CONFIG>;
	friend struct AddressHookWrapper<CONFIG>;
	friend struct SourceCodeHookWrapper<CONFIG>;
	friend struct SubProgramHookWrapper<CONFIG>;
	friend struct AddressHook<CONFIG>;
	friend struct SourceCodeHook<CONFIG>;
	friend struct SubProgramHook<CONFIG>;
	friend struct StubWrapper<CONFIG>;
	friend struct Stub<CONFIG>;
	friend struct EventBridge<CONFIG>;
	friend struct ProcessorWrapper<CONFIG>;
	friend struct RegisterWrapper<CONFIG>;
	friend struct ExecutableBinaryFileWrapper<CONFIG>;
	friend struct StackFrameInfoWrapper<CONFIG>;
	friend struct DebugSymbolWrapper<CONFIG>;
	friend struct SubProgramWrapper<CONFIG>;
	friend struct StatementWrapper<CONFIG>;
	friend struct TypeWrapper<CONFIG>;
	friend struct NamedTypeWrapper<CONFIG>;
	friend struct BaseTypeWrapper<CONFIG>;
	friend struct IntegerTypeWrapper<CONFIG>;
	friend struct CharTypeWrapper<CONFIG>;
	friend struct FloatingPointTypeWrapper<CONFIG>;
	friend struct BooleanTypeWrapper<CONFIG>;
	friend struct MemberWrapper<CONFIG>;
	friend struct CompositeTypeWrapper<CONFIG>;
	friend struct StructureTypeWrapper<CONFIG>;
	friend struct UnionTypeWrapper<CONFIG>;
	friend struct ClassTypeWrapper<CONFIG>;
	friend struct InterfaceTypeWrapper<CONFIG>;
	friend struct ArrayTypeWrapper<CONFIG>;
	friend struct PointerTypeWrapper<CONFIG>;
	friend struct TypedefWrapper<CONFIG>;
	friend struct FormalParameterWrapper<CONFIG>;
	friend struct FunctionTypeWrapper<CONFIG>;
	friend struct ConstTypeWrapper<CONFIG>;
	friend struct EnumeratorWrapper<CONFIG>;
	friend struct EnumTypeWrapper<CONFIG>;
	friend struct UnspecifiedTypeWrapper<CONFIG>;
	friend struct VolatileTypeWrapper<CONFIG>;
	
	unisim::kernel::logger::Logger logger;
	bool shell;
	bool stop_simulation_at_exit;
	std::string program_counter_name;
	unsigned int arguments_length;
	typedef unisim::kernel::variable::Parameter<std::string> ParamArgument;
	typedef std::vector<ParamArgument *> ParamArguments;
	ParamArguments param_arguments;
	
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Parameter<bool> param_debug;
	unisim::kernel::variable::Parameter<bool> param_shell;
	unisim::kernel::variable::Parameter<bool> param_stop_simulation_at_exit;
	unisim::kernel::variable::Parameter<std::string> param_program_counter_name;
	unisim::kernel::variable::Parameter<std::string> param_filename;
	unisim::kernel::variable::Parameter<unsigned int> param_arguments_length;
	
	std::ostream *std_output_stream;
	std::ostream *std_error_stream;
	std::string prompt;
	
	bool trap;
	bool cont;
	
	typedef std::vector<v8::Global<v8::Promise::Resolver> > Resolvers;
	Resolvers cont_exec_resolvers;
	
	void Interrupt();
	void Trap();
	
	// Readline stuff
	static std::mutex rl_mutex;
	typedef std::vector<std::string> Commands;

	bool GetCommand(std::string& cmd);
	bool GetLine(std::string& line);
	static char **Completion(char *text, int start, int end);
	static char *CompletionGenerator(char *text, int state, Commands& commands);
	static char *GlobalCompletionGenerator(char *text, int state);
	static char *HelpCompletionGenerator(char *text, int state);
	static char *ConstructorCompletionGenerator(char *text, int state);
	static char *ProcessorCompletionGenerator(char *text, int state);
	static char *RegisterCompletionGenerator(char *text, int state);
	static void Help(std::ostream& stream, const std::string& section = std::string());
	
	bool LocateFile(const std::string& file_path, std::string& match_file_path, bool lazy_match = false);
	std::string LocateFile(const std::string& filename, bool lazy_match = false);

	void Cleanup();
	
	// Global functions
	void ContinueExecution(const v8::FunctionCallbackInfo<v8::Value>& args);
	void Quit(const v8::FunctionCallbackInfo<v8::Value>& args);
	void LoadDebugInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
	void GetExecutableBinaryFiles(const v8::FunctionCallbackInfo<v8::Value>& args);
	void GetSymbols(const v8::FunctionCallbackInfo<v8::Value>& args);
	void FindSymbol(const v8::FunctionCallbackInfo<v8::Value>& args);
	void GetStatements(const v8::FunctionCallbackInfo<v8::Value>& args);
	void FindStatement(const v8::FunctionCallbackInfo<v8::Value>& args);
	void FindStatements(const v8::FunctionCallbackInfo<v8::Value>& args);
	void FindSubProgram(const v8::FunctionCallbackInfo<v8::Value>& args);
	
	bool ArgToSourceCodeLocation(v8::Local<v8::Value> arg, const std::string& err_msg_context, const char *arg_name, unisim::util::debug::SourceCodeLocation& source_code_location);
	
	void Resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
	void Reject(const v8::FunctionCallbackInfo<v8::Value>& args);
	
	void Continue();
	void ResolveContinue(v8::Local<v8::Value> value);
};

/////////////////////////////// ObjectWrapper<> ////////////////////////////////

template <typename CONFIG>
struct ObjectWrapper : unisim::util::nodejs::ObjectWrapper
{
	typedef unisim::util::nodejs::ObjectWrapper Super;
	typedef typename CONFIG::ADDRESS ADDRESS;
	ObjectWrapper(NodeJS<CONFIG>& nodejs, const void *ptr, std::size_t size = 0);
protected:
	NodeJS<CONFIG>& nodejs;
};

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // HAVE_NODEJS

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_NODEJS_HH__
